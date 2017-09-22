#include "listener.h"

Listener::Listener(string &watch_path, ConcurrentSet* p_backlog_input) {
    unreadEvents = 0;
    numEventsRead = 0;
    addWatch(watch_path);
    audit_clock.start();
}

void Listener::addWatch(string &watch_path) {
    inotify_fd = inotify_init();
    folder_path = watch_path;

    if (inotify_fd < 0) {
        perror("ERROR: create_inotify_instances -> inotify_init() ");
        exit(EXIT_FAILURE);
    }

    watch_descriptor = inotify_add_watch(inotify_fd, watch_path.c_str(), INOTIFY_EVENTS);

    if (watch_descriptor < 0) {
        fprintf(stderr, "ERROR: create_inotify_instances -> inotify_add_watch(%d, %s, %d) ", inotify_fd,
                watch_path.c_str(), INOTIFY_EVENTS);
        perror("");
        exit(EXIT_FAILURE);
    }

    debug("Watching %s using watch_descriptor %d\n", watch_path.c_str(), watch_descriptor);
}

int Listener::readEvents() {
    char *buffer_pointer;

    timeout.tv_sec = SELECT_TIMEOUT; // Set timeout
    timeout.tv_usec = 0;

    int selectRetval = select(inotify_fd + 1, &rfds, NULL, NULL, &timeout); // After select() don't rely on &rfds and &timeout values.

    if (selectRetval) {

        int numEventsRead = read(inotify_fd, events_buffer, BUF_LEN);

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
        for(buffer_pointer = events_buffer; buffer_pointer < events_buffer + numEventsRead) {
            event = (struct inotify_event *) buffer_pointer;
            processEvent(event)
            buffer_pointer += sizeof(struct inotify_event) + event->len;
        }


    }
}

void process_event(struct inotify_event *event, string &file_path) {
    if (event->mask & IN_DELETE) {
        file_list.erase(file_path);
    } else if (event->mask & IN_CLOSE_WRITE) {
        file_list.push(file_path);
    }
}




