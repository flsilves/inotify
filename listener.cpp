#include "listener.h"

Listener::Listener(string &watch_path, ConcurrentSet* p_backlog_input) {
    p_backlog = p_backlog_input;
    unreadEvents = 0;
    numEventsRead = 0;
    addWatch(watch_path);
    auditClock.start();
}

void Listener::addWatch(string &watch_path) {
    inotifyFD = inotify_init();
    folderPath = watch_path;

    if (inotifyFD < 0) {
        perror("ERROR: create_inotify_instances -> inotify_init() ");
        exit(EXIT_FAILURE);
    }

    watchDescriptor = inotify_add_watch(inotifyFD, watch_path.c_str(), INOTIFY_EVENTS);

    if (watchDescriptor < 0) {
        fprintf(stderr, "ERROR: create_inotify_instances -> inotify_add_watch(%d, %s, %d) ", inotifyFD,
                watch_path.c_str(), INOTIFY_EVENTS);
        perror("");
        exit(EXIT_FAILURE);
    }

    debug("Watching %s using watchDescriptor %d\n", watch_path.c_str(), watchDescriptor);
}

int Listener::readEvents() {
    char *buffer_pointer;

    timeout.tv_sec = SELECT_TIMEOUT; // Set timeout
    timeout.tv_usec = 0;

    int selectRetval = select(inotifyFD + 1, &rfds, NULL, NULL, &timeout); // After select() don't rely on &rfds and &timeout values.

    if (selectRetval) {

        int numEventsRead = read(inotifyFD, eventsBuffer, BUF_LEN);

        if (numEventsRead < 0) {
            perror("ERROR: folder_listener -> read() ");
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
        char *buffer_pointer;
        for(buffer_pointer = eventsBuffer; buffer_pointer < eventsBuffer + numEventsRead) {
            event = (struct inotify_event *) buffer_pointer;
            processEvent(event, folderPath + "/" +  event->name);
            buffer_pointer += sizeof(struct inotify_event) + event->len;
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




