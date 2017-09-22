#ifndef INOTIFY_LISTENER_H
#define INOTIFY_LISTENER_H

#define EVENTS_BUFFER_LENGTH (1000 * (sizeof(struct inotify_event) + NAME_MAX + 1))
#define INOTIFY_EVENTS (IN_DELETE | IN_CLOSE_WRITE)

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

using namespace std;

class Listener {

public:
    Listener(string &watchPath, ConcurrentSet *p_backlog_input);

    void readEvents();

    void processBuffer();

private:
    Timer auditClock;
    ConcurrentSet *p_backlog;
    ssize_t unprocessedEvents;
    char eventsBuffer[EVENTS_BUFFER_LENGTH];
    struct timeval timeout;
    struct inotify_event *event;
    fd_set selectReadDescriptor;
    int inotifyFD, watchDescriptor, numEventsRead;
    string folderPath;

    void addWatch(string &watchPath);

    void processEvent(struct inotify_event *event, string filePath);
};


#endif


