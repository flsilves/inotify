#include "inotify.h"

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))
#define INOTIFY_EVENTS (IN_MOVED_FROM | IN_MOVED_TO | IN_CLOSE_WRITE)


#define AUDIT_TIMEOUT 3.0
#define SELECT_TIMEOUT 1

using namespace std;

int enable_debug = 1;

set<string> file_list;
mutex cv_m;
condition_variable cv;

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

void read_arguments(const int &argc, const char *argv[], int &number_of_threads, char *folder_path) {

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
}

void create_inotify_instances(const char* watch_path, int &inotify_fd) {
    
    int watch_descriptor;
    inotify_fd = inotify_init(); /* Create inotify instance */

    if (inotify_fd == -1) {
        printf("FATAL: inotify_init - errno:%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    watch_descriptor = inotify_add_watch(inotify_fd, watch_path, INOTIFY_EVENTS);
    
    if (watch_descriptor == -1) {
        printf("FATAL: inotify_add_watch: - errno:%s\n", strerror(errno));
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

    while(1) {

        struct timeval timeout;
        timeout.tv_sec = SELECT_TIMEOUT; // Set timeout to 2.0 seconds
        timeout.tv_usec = 0;
     
        debug("Timer: %f \n", timer.elapsedSeconds());
        debug("Inotify listener: waiting for events\n");

        FD_ZERO(&rfds);
        FD_SET(inotify_fd, &rfds);  

        int retval = select(inotify_fd + 1, &rfds, NULL, NULL, &timeout); // After select() don't rely on &rfds and &timeout values.
        
        if (retval) {

            num_read = read(inotify_fd, buf, BUF_LEN);
            if (num_read == 0) {
                printf("FATAL: read() from inotify fd returned 0! - errno:%d\n", strerror(errno));
                exit(EXIT_FAILURE);
            }

            if (num_read == -1) {
                printf("FATAL: read rom inotify fd returned -1! - errno:$d\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            
            debug("Inotify listener: Aquiring lock\n");
            unique_lock<mutex> lk(cv_m);

            debug("Inotify listener: Read %ld bytes from inotify fd\n", (long) num_read);

            for (buffer_pointer = buf; buffer_pointer < buf + num_read;   ) {
                
                event = (struct inotify_event *) buffer_pointer;
                file_list.insert(event->name);            
                buffer_pointer += sizeof (struct inotify_event) +event->len;
            
            }
        }
        else if(retval == -1) {
            debug("Select error \n");
        }
        else if(retval == 0) {
            debug("Read timeout \n");
        }

        if(timer.elapsedSeconds() > AUDIT_TIMEOUT) {
            audit_folder(folder_path);
            timer.restart();
        }

        debug("Inotify listener: notifying\n");
        cv.notify_one();
        cout << file_list << endl;
    }
}

void consume_files() {   

    while(1) {   
        unique_lock<mutex> lk(cv_m); 
        debug("Consumer thread waiting... \n");

        cv.wait(lk, []{return file_list.size() !=0 ;}); // is it needed cv.notify_one to process the files?? 

        debug("Consumer thread: backlog size: %d\n",file_list.size());
        auto it = file_list.begin();

        while(it != file_list.end()) {
            file_list.erase(it++); // concurrency issues with producer thread ??
            debug("Consumer thread: Removed item from list\n");               
            usleep(1000000);
        }
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
           if(stat(file, &statbuf) == -1) { 
               printf("stat error - errno:%d\n", strerror(errno));
               return errno;
           }

           free(file);

           if(S_ISREG(statbuf.st_mode)) { // check if file isn't a directory and has right permissions
               file_list.insert(ent->d_name);
           }

        }        
        closedir (dir);      

    } else {            
        printf("opendir error - errno%d\n", strerror(errno));
        return EXIT_FAILURE;
    }
}

char* concat(const char *s1, const char *s2) {

    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char *result = (char*)malloc(len1+len2+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    memcpy(result, s1, len1);
    memcpy(result+len1, s2, len2+1);//+1 to copy the null-terminator
    return result;
}

template <typename T> // Debug print for std::set<T>
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

