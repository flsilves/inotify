
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
        print_container();
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


void print_container() { //TODO: override <operator
    auto it = file_list.begin();  

    cout << "LIST: [";
    while(it != file_list.end())
        cout << ' ' << *it++;
    
    cout << "]" << endl;
}