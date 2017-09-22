#ifndef INOTIFY_H
#define INOTIFY_H

#include "myTimer.h"
#include "backlog.h"

#include <cstdlib>
#include <iostream>
#include <set>
#include <thread>
#include <mutex>

#include <dirent.h>
#include <cstdarg>
#include <cstring>

#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>

#define EVENTS_BUFFER_LENGTH (1000 * (sizeof(struct inotify_event) + NAME_MAX + 1))  // BUFFER for inotify reader
#define INOTIFY_EVENTS (IN_DELETE | IN_CLOSE_WRITE)                     // Relevant inotify events to watch

#define AUDIT_TIMEOUT 10.0
#define SELECT_TIMEOUT 3.0


using namespace std;

void readArguments(const int &argc, const char **argv, int &numberOfThreads, char *folderPath);

void debug(const char *format, ...);
void deleteFile(string &filePath);
void threadConsumerLoop();
void threadReaderLoop(string &folderPath);

int audit_folder(string &folder);
template<class T> std::ostream &operator<<(std::ostream &os, const std::set<T> &v);

static void displayInotifyEvent(struct inotify_event *i) {

    printf(" wd =%2d; ", i->wd);
    if (i->cookie > 0) {
        printf("cookie =%4d; ", i->cookie);
    }

    printf("mask = "); // use DELETE_SELF, study IN_MOVED_TO
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

#endif // INOTIFY_H

