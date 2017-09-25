#include "listener.h"


Listener::Listener(string &watchPath, ConcurrentBacklog *p_backlog_input) {
    p_backlog = p_backlog_input;
    unprocessedEvents = 0;
    addWatch(watchPath);
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

    FD_ZERO(&selectReadDescriptor);
    FD_SET(inotifyFD, &selectReadDescriptor);
    timeout.tv_sec = SELECT_TIMEOUT;
    timeout.tv_usec = 0;

    int selectRetval = select(inotifyFD + 1, &selectReadDescriptor, NULL, NULL, &timeout);

    if (selectRetval) {
        unprocessedEvents = read(inotifyFD, eventsBuffer, EVENTS_BUFFER_LENGTH);
        debug("Inotify listener: Read %ld bytes from inotify fd\n", (long) unprocessedEvents);
        if (unprocessedEvents < 0) {
            perror("ERROR: threadReaderLoop -> read() ");
        }

    } else if (selectRetval == SELECT_ERROR) {
        unprocessedEvents = 0;
        debug("Select error \n");
    } else if (selectRetval == SELECT_TIMEOUT) {
        unprocessedEvents = 0;
        debug("Read timeout \n");
    }
}

void Listener::processBuffer() {
    if (unprocessedEvents > 0) {
        char *bufferPointer;
        for (bufferPointer = eventsBuffer; bufferPointer < eventsBuffer + unprocessedEvents;) {
            event = (struct inotify_event *) bufferPointer;
            processEvent(event, folderPath + "/" + event->name);
            bufferPointer += sizeof(struct inotify_event) + event->len;
        }
    }
    //p_backlog->print();
}

void Listener::processEvent(struct inotify_event *event, string filePath) {
    if (event->mask & IN_DELETE) {
        p_backlog->erase(filePath);
    } else if (event->mask & IN_CLOSE_WRITE) {
        p_backlog->push(filePath);
    }
}




