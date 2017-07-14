#include "inotify.h"

#include <iostream>
#include <limits.h> 
#include <thread>
#include <unistd.h>
#include <string.h>
#include <algorithm>
#include <set>
#include <sys/inotify.h>

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

using namespace std;

set<string> file_list;

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
    thread1.join();
    //thread2.join();

    exit(EXIT_SUCCESS);
}

void folder_listener(int inotify_instance) {

    ssize_t num_read;
    char buf[BUF_LEN];
    char *buffer_pointer;
    struct inotify_event *event;


    while(1) {
        num_read = read(inotify_instance, buf, BUF_LEN);

        if (num_read == 0) {
            printf("FATAL: read() from inotify fd returned 0!");
            exit(EXIT_FAILURE);
        }

        if (num_read == -1) {
            printf("FATAL: read rom inotify fd returned -1!");
            exit(EXIT_FAILURE);
        }

        printf("Read %ld bytes from inotify fd\n", (long) num_read);

        for (buffer_pointer = buf; buffer_pointer < buf + num_read;   ) {
            event = (struct inotify_event *) buffer_pointer;
            //displayInotifyEvent(event);
            //cout << std::this_thread::get_id() << endl; 

            file_list.insert(event->name);
            print_container(file_list);
            buffer_pointer += sizeof (struct inotify_event) +event->len;
        }
    }
}

template <class T> void print_container(T list) {
    auto it = list.begin();  

    cout << "LIST: [";
    while(it != list.end())
        cout << ' ' << *it++;
    
    cout << "]" << endl;
}