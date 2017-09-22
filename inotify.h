#ifndef INOTIFY_H
#define INOTIFY_H

#include "myTimer.h"
#include "backlog.h"
#include "listener.h"

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


#define AUDIT_TIMEOUT 10.0


using namespace std;

void readArguments(const int &argc, const char **argv, int &numberOfThreads, char *folderPath);

void debug(const char *format, ...);

void deleteFile(string &filePath);

void threadConsumerLoop();

void threadReaderLoop(string folderPath);

int auditFolder(string &folderPath);


template<class T>
std::ostream &operator<<(std::ostream &os, const std::set<T> &v);

static void displayInotifyEvent(struct inotify_event *iEvent) {

    printf(" wd =%2d; ", iEvent->wd);
    if (iEvent->cookie > 0) {
        printf("cookie =%4d; ", iEvent->cookie);
    }

    printf("mask = "); // use DELETE_SELF, study IN_MOVED_TO
    if (iEvent->mask & IN_ACCESS) printf("IN_ACCESS ");
    if (iEvent->mask & IN_ATTRIB) printf("IN_ATTRIB ");
    if (iEvent->mask & IN_CLOSE_NOWRITE) printf("IN_CLOSE_NOWRITE ");
    if (iEvent->mask & IN_CLOSE_WRITE) printf("IN_CLOSE_WRITE ");
    if (iEvent->mask & IN_CREATE) printf("IN_CREATE ");
    if (iEvent->mask & IN_DELETE) printf("IN_DELETE ");
    if (iEvent->mask & IN_DELETE_SELF) printf("IN_DELETE_SELF ");
    if (iEvent->mask & IN_IGNORED) printf("IN_IGNORED ");
    if (iEvent->mask & IN_ISDIR) printf("IN_ISDIR ");
    if (iEvent->mask & IN_MODIFY) printf("IN_MODIFY ");
    if (iEvent->mask & IN_MOVE_SELF) printf("IN_MOVE_SELF ");
    if (iEvent->mask & IN_MOVED_FROM) printf("IN_MOVED_FROM ");
    if (iEvent->mask & IN_MOVED_TO) printf("IN_MOVED_TO ");
    if (iEvent->mask & IN_OPEN) printf("IN_OPEN ");
    if (iEvent->mask & IN_Q_OVERFLOW) printf("IN_Q_OVERFLOW ");
    if (iEvent->mask & IN_UNMOUNT) printf("IN_UNMOUNT ");

    printf("\n");
    if (iEvent->len > 0) {
        printf(" name = %s\n", iEvent->name);
    }
}

#endif

