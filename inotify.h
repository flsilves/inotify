#ifndef INOTIFY_H
#define INOTIFY_H

#include <stdlib.h>
#include <stdio.h>
#include <linux/limits.h>

#include <condition_variable>
#include <thread>
#include <mutex>
#include <iostream>
#include <sys/inotify.h>
#include <dirent.h> 
#include <stdio.h> 
#include <iostream>
#include <limits.h> 
#include <thread>
#include <unistd.h>
#include <string.h>
#include <algorithm>
#include <set>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include "myTimer.h"
#include <stdarg.h>  

using namespace std;


void read_arguments(const int &argc, const char *argv[], int &number_of_threads, char *folder_path);


int audit_folder(const char *folder);
void debug(const char *format, ...);

void consume_files();
void folder_listener(const int inotify_fd, const char *folder_path);

char* concat(const char *s1, const char *s2);
template <class T> std::ostream& operator<< (std::ostream& os, const std::set<T>& v);
void create_inotify_instances(const char* watch_path, int &inotify_fd);


static void displayInotifyEvent(struct inotify_event *i) {
    
    printf(" wd =%2d; ", i->wd);
    if (i->cookie > 0) {
        printf("cookie =%4d; ", i->cookie);
    }

    printf("mask = "); // use DELETE_SELF 
    if (i->mask & IN_ACCESS) printf("IN_ACCESS ");
    if (i->mask & IN_ATTRIB) printf("IN_ATTRIB ");
    if (i->mask & IN_CLOSE_NOWRITE) printf("IN_CLOSE_NOWRITE ");
    if (i->mask & IN_CLOSE_WRITE) printf("IN_CLOSE_WRITE ");
    if (i->mask & IN_CREATE) printf("IN_CREATE ");
    if (i->mask & IN_DELETE) printf("IN_DELETE ");
    if (i->mask & IN_DELETE_SELF) printf("IN_DELETE_SELF ");
    if (i->mask & IN_IGNORED) printf("IN_IGNORED ");
    if (i->mask & IN_ISDIR) printf("IN_ISDIR ");
    if (i->mask & IN_MODIFY) printf("IN_MODIFY ");
    if (i->mask & IN_MOVE_SELF) printf("IN_MOVE_SELF ");
    if (i->mask & IN_MOVED_FROM) printf("IN_MOVED_FROM ");
    if (i->mask & IN_MOVED_TO) printf("IN_MOVED_TO ");
    if (i->mask & IN_OPEN) printf("IN_OPEN ");
    if (i->mask & IN_Q_OVERFLOW) printf("IN_Q_OVERFLOW ");
    if (i->mask & IN_UNMOUNT) printf("IN_UNMOUNT ");
    
    printf("\n");
    if (i->len > 0) {
        printf(" name = %s\n", i->name);
    }
}


#endif /* INOTIFY_H */

