#ifndef INOTIFY_LISTENER_H
#define INOTIFY_LISTENER_H

#define BUF_LEN (1000 * (sizeof(struct inotify_event) + NAME_MAX + 1))

#include <cstring>
#include "myTimer.h"
#include "backlog.h"
#include "inotify.h"

#include <cstdlib>
#include <iostream>
#include <set>
#include <thread>
#include <mutex>

#include <dirent.h>
#include <cstdarg>
#include <cstring>
#define SELECT_TIMEOUT 0
#define SELECT_ERROR -1
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>

class Listener {

public:
    Listener(string &folder, ConcurrentSet* p_backlog_input);
    int readEvents();

    void processBuffer();



private:

    Timer auditClock;
    ConcurrentSet* p_backlog;
    int unreadEvents;
    char eventsBuffer[BUF_LEN];
    struct timeval timeout;
    fd_set rfds;
    struct inotify_event *event;
    int inotifyFD, watchDescriptor, numEventsRead;
    string folderPath;

    void addWatch(string &watch_path);
    void processEvent(struct inotify_event *event, string filePath);


};





