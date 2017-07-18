
#include "inotify.h"

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

using namespace std;

set<string> file_list;

mutex cv_m;
condition_variable cv;

int main(int argc, char *argv[]) { 

    int inotify_instance, watch_descriptor;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        printf("%s pathname... \n", argv[0]);
        exit(1);
    }

    inotify_instance = inotify_init(); /* Create inotify instance */

    if (inotify_instance == -1) {
        printf("FATAL: inotify_init");
        exit(EXIT_FAILURE);
    }

    for (int j = 1; j < argc; j++) {
        watch_descriptor = inotify_add_watch(inotify_instance, argv[j], IN_MOVED_FROM | IN_MOVED_TO | IN_CLOSE_WRITE);
        
        if (watch_descriptor == -1) {
            printf("FATAL: inotify_add_watch");
            exit(EXIT_FAILURE);
        }
        
        printf("Watching %s using watch_descriptor %d\n", argv[j], watch_descriptor);
    }

    thread thread1(folder_listener, inotify_instance);
    thread thread2(consume_files);

    thread1.join();
    thread2.join();

    exit(EXIT_SUCCESS);
}

void folder_listener(int inotify_instance) {

    ssize_t num_read;
    char buf[BUF_LEN];
    char *buffer_pointer;
    struct inotify_event *event;
    getFileNameList();

    while(1) {

        cout << "Inotify listener: waiting for events" << endl;
        num_read = read(inotify_instance, buf, BUF_LEN);

        if (num_read == 0) {
            printf("FATAL: read() from inotify fd returned 0!");
            exit(EXIT_FAILURE);
        }

        if (num_read == -1) {
            printf("FATAL: read rom inotify fd returned -1!");
            exit(EXIT_FAILURE);
        }
        
        printf("Inotify listener: Aquiring lock\n");
        unique_lock<mutex> lk(cv_m);
        printf("Inotify listener: Read %ld bytes from inotify fd\n", (long) num_read);

        for (buffer_pointer = buf; buffer_pointer < buf + num_read;   ) {
            
            event = (struct inotify_event *) buffer_pointer;
            file_list.insert(event->name);            
            buffer_pointer += sizeof (struct inotify_event) +event->len;
        
        }
        printf("Inotify listener: notifying\n");
        cv.notify_one();
        cout << file_list << endl;
    }
}

void consume_files(){
    
    while(1)
    {   
        unique_lock<mutex> lk(cv_m);
        cout << "Consumer thread waiting... \n";
        cv.wait(lk, []{return file_list.size() !=0 ;});

        cout << "Consumer thread: backlog size " << file_list.size() << endl;
        auto it = file_list.begin();
        while(it != file_list.end())
        {
                file_list.erase(it++);
                cout << "Consumer thread: Removed item from list" << endl;               
                usleep(1000000);
       }
    }
}


template <class T> 
ostream& operator << (ostream& os, const set<T>& v) {
    os << "[";
    for (typename set<T>::const_iterator ii = v.begin(); ii != v.end(); ++ii)
    {
        os << " " << *ii;
    }
    os << "]";
    return os;
}

int getFileNameList() {

    DIR *dir = 0;
    struct dirent *ent = 0;
    struct stat statbuf;

    const char* path = "/home/frs/inotify";

    if ( (dir = opendir (path)) != NULL) {   
        while ((ent = readdir (dir)) != NULL) {

           if( ent->d_name[0] == '.') { // ignore all files that start with '.' 
               continue;
           }

           if(stat(ent->d_name, &statbuf) == -1) {
               return errno;
           }

           if(S_ISREG(statbuf.st_mode)) {
               printf ("%s\n", ent->d_name);
           }

        }
        
        closedir (dir);       
    } else {            
        return EXIT_FAILURE;
    }
}

