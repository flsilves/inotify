#include "inotify.h"
#include "backlog.h"

#define BUF_LEN (1000 * (sizeof(struct inotify_event) + NAME_MAX + 1))  // BUFFER for inotify reader
#define INOTIFY_EVENTS (IN_DELETE | IN_CLOSE_WRITE)   // Relevant inotify events to watch

#define AUDIT_TIMEOUT 10.0
#define SELECT_TIMEOUT 3.0

/*TODO use string and if char* is needed use c_str() */

using namespace std;

// Global variables
int event_count = 0;       // number of inotify events catched
bool enable_debug = true;  // enable/disable debug

concurrent_set file_list;  

int main(const int argc, const char *argv[]) { 

    int inotify_fd, number_of_threads = 100;
    char folder_path[PATH_MAX];

    read_arguments(argc, argv, number_of_threads, folder_path);
    debug("input arguments: PATH:[%s]  NUMBER_OF_THREADS:[%d]\n", folder_path, number_of_threads);

    create_inotify_instances(folder_path, inotify_fd);

    thread thread1(folder_listener, inotify_fd, folder_path);
    thread thread2(consume_files);

    thread1.join();
    thread2.join();

    exit(EXIT_SUCCESS);
}

void read_arguments(const int &argc, const char *argv[], int &number_of_threads, char *folder_path) { // TODO - number_of_threads not used

    struct stat statbuf; 
    number_of_threads = 2; // default number of threads

    if ( argc <=1 || argc >=4 || strcmp(argv[1], "--help") == 0) {
        printf("USAGE: %s <pathname> <#threads>\n", argv[0]);
        exit(1);
    }
    realpath(argv[1], folder_path);
    debug("Validating folder: %s\n", folder_path);

    if(stat(folder_path, &statbuf) == -1) { 
        printf("Argument is not a valid folder path:\n");
        exit(EXIT_FAILURE);
    }

    if(!S_ISDIR(statbuf.st_mode)) {      
        printf("Argument is not a valid folder path:\n");
        exit(EXIT_FAILURE);
    }

    if(argc == 3) {
        number_of_threads = atoi(argv[2]);
    }

    strcat(folder_path, "/");
}

void create_inotify_instances(const char* watch_path, int &inotify_fd) {
    
    int watch_descriptor;
    inotify_fd = inotify_init(); /* Create inotify instance */

    if (inotify_fd < 0) {
        perror("ERROR: create_inotify_instances -> inotify_init() ");
        exit(EXIT_FAILURE);
    }

    watch_descriptor = inotify_add_watch(inotify_fd, watch_path, INOTIFY_EVENTS);
    
    if (watch_descriptor < 0) {
        fprintf(stderr, "ERROR: create_inotify_instances -> inotify_add_watch(%d, %s, %d) ", inotify_fd, watch_path, INOTIFY_EVENTS); perror("");
        exit(EXIT_FAILURE);
    }
    
    debug("Watching %s using watch_descriptor %d\n", watch_path, watch_descriptor);    
}


void folder_listener(const int inotify_fd, const char *folder_path) {

    Timer timer;
    timer.start();

    ssize_t num_read;
    char buf[BUF_LEN];
    char *buffer_pointer;
    struct inotify_event *event;   
    fd_set rfds;

    while(true) {

        struct timeval timeout;
        timeout.tv_sec = SELECT_TIMEOUT; // Set timeout
        timeout.tv_usec = 0;
     
        //debug("Timer: %f \n", timer.elapsedSeconds());
        //debug("Inotify listener: waiting for events\n");

        FD_ZERO(&rfds);
        FD_SET(inotify_fd, &rfds);  

        int select_retval = select(inotify_fd + 1, &rfds, NULL, NULL, &timeout); // After select() don't rely on &rfds and &timeout values.
        
        if (select_retval) {

            num_read = read(inotify_fd, buf, BUF_LEN);
            if (num_read < 0) {
                perror("ERROR: folder_listener -> read() ");
                exit(EXIT_FAILURE);
            }
           
            debug("Inotify listener: Aquiring lock\n");


            debug("Inotify listener: Read %ld bytes from inotify fd\n", (long) num_read);

            for (buffer_pointer = buf; buffer_pointer < buf + num_read;   ) {               
                event = (struct inotify_event *) buffer_pointer;
                //debug("Inotify event: File name[%s] watch_descriptor[%d] \n", event->name, event->wd);
                event_count++;
                string file_path = string(folder_path) + "/" + event->name;
                process_event(event, file_path);     
                buffer_pointer += sizeof (struct inotify_event) +event->len;           
            }

        }
        else if(select_retval == -1) {
            debug("Select error \n");
        }
        else if(select_retval == 0) {
            debug("Read timeout \n");
        }

        if(timer.elapsedSeconds() > AUDIT_TIMEOUT) {
            debug("Auditing Folder: %s\n", folder_path);
            audit_folder(folder_path);
            timer.restart();
        }
    }
}

void process_event(struct inotify_event *event, string& file_path)
{
    if(event->mask & IN_DELETE) {
        file_list.erase(file_path);
    }
    else if(event->mask & IN_CLOSE_WRITE) {
        file_list.push(file_path);     
    }
}

void consume_files() {  

    while(true) {   
        debug("Consumer thread - Retrieving File... \n");
        string file_to_delete = file_list.pop();
        delete_file(file_to_delete);        
    }
}

void delete_file(std::string &file_absolute_path)
{
   char* file_path = new char[file_absolute_path.length() + 1];
   strcpy(file_path, file_absolute_path.c_str());

   if(remove(file_path) != 0) {
        fprintf(stderr, "ERROR: consume_files -> remove(%s) ", file_path); perror("");    
   }
}

int audit_folder(const char* folder) {

    DIR *dir = 0;
    struct dirent *ent = 0;
    struct stat statbuf;  

    if ( (dir = opendir (folder)) != NULL) {   
        while ((ent = readdir (dir)) != NULL) {

           if( ent->d_name[0] == '.') { // ignore all files that start with '.' 
               continue;
           }

           char* file = concat(folder, ent->d_name);
           if(stat(file, &statbuf) != 0) {              
               fprintf(stderr, "ERROR: audit folder -> stat(%s) ", file); perror("");
               return errno;
           }

           free(file);

           if(S_ISREG(statbuf.st_mode)) { // check if file isn't a directory and has right permissions
               file_list.push(string(folder) + "/" + ent->d_name); 
           }

        }        
        closedir (dir);      

    } else {    
        fprintf(stderr, "ERROR - audit_folder -> opendir(%s) ", folder ); perror("");     
        return EXIT_FAILURE;
    }
}

char* concat(const char *s1, const char *s2) {

    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char *ptr = (char*)malloc(len1 + len2 + 1);// +1 for the zero-terminator

    if(ptr == NULL) {
        fprintf(stderr, "ERROR - concat() -> malloc()"); perror("");  
    }

    memcpy(ptr, s1, len1);
    memcpy(ptr+len1, s2, len2 + 1);// +1 to copy the null-terminator
    return ptr;
}

template <typename T> // Debug print for set<T>
ostream& operator << (ostream& os, const set<T>& v) {

    os << "** DEBUG **    SET: [";
    for (typename set<T>::const_iterator it = v.begin(); it != v.end(); ++it)
    {
        os << " " << *it << ","; 
    }
    os << "]";
    return os;
}

void debug(const char *format, ...) {
  va_list args;
  va_start(args, format);

  if(enable_debug) {
    printf("** DEBUG **    ");
    vprintf(format, args);
  }
  va_end(args);
}

