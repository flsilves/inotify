#ifndef INOTIFY_LISTENER_H
#define INOTIFY_LISTENER_H

#define BUF_LEN (1000 * (sizeof(struct inotify_event) + NAME_MAX + 1))

class Listener {

public:
    Listener(string folder);



private:

    char buffer[BUF_LEN];
    Timer clock;

};




