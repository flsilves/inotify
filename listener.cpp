#include "listener.h"

Listener::Listener(string &watchPath, ConcurrentSet* p_backlog_input) {
    p_backlog = p_backlog_input;
    unreadEvents = 0;
    numEventsRead = 0;
    addWatch(watchPath);
    auditClock.start();
}

void Listener::addWatch(string &watchPath) {
    inotifyFD = inotify_init();
    folderPath = watchPath;

    if (inotifyFD < 0) {
        perror("ERROR: create_inotify_instances -> inotify_init() ");
        exit(EXIT_FAILURE);
    }

    watchDescriptor = inotify_add_watch(inotifyFD, watchPath.c_str(), INOTIFY_EVENTS);

    if (watchDescriptor < 0) {
        fprintf(stderr, "ERROR: create_inotify_instances -> inotify_add_watch(%d, %s, %d) ", inotifyFD,
                watchPath.c_str(), INOTIFY_EVENTS);
        perror("");
        exit(EXIT_FAILURE);
    }

    debug("Watching %s using watchDescriptor %d\n", watchPath.c_str(), watchDescriptor);
}

void Listener::readEvents() {
    char *bufferPointer;

    timeout.tv_sec = SELECT_TIMEOUT; // Set timeout
    timeout.tv_usec = 0;

    int selectRetval = select(inotifyFD + 1, &selectReadDescriptor, NULL, NULL, &timeout); // After select() don't rely on &selectReadDescriptor and &timeout values.

    if (selectRetval) {

        int numEventsRead = read(inotifyFD, eventsBuffer, EVENTS_BUFFER_LENGTH);

        if (numEventsRead < 0) {
            perror("ERROR: threadReaderLoop -> read() ");
        }

    } else if (selectRetval == SELECT_ERROR) {
        numEventsRead = 0;
        debug("Select error \n");
    } else if (selectRetval == SELECT_TIMEOUT) {
        numEventsRead = 0;
        debug("Read timeout \n");
    }
}

void Listener::processBuffer()
{
    if(numEventsRead > 0)
    {
        char *bufferPointer;
        for(bufferPointer = eventsBuffer; bufferPointer < eventsBuffer + numEventsRead;) {
            event = (struct inotify_event *) bufferPointer;
            processEvent(event, folderPath + "/" +  event->name);
            bufferPointer += sizeof(struct inotify_event) + event->len;
        }
    }
}

void Listener::processEvent(struct inotify_event *event, string filePath) {

    if (event->mask & IN_DELETE) {
        p_backlog->erase(filePath);
    } else if (event->mask & IN_CLOSE_WRITE) {
        p_backlog->push(filePath);
    }
}




