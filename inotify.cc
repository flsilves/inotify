#include "inotify.h"

#include <sys/inotify.h>
#include <limits.h> // NAME_MAX
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

int main(int argc, char *argv[])
{
    int inotifyFd, wd, j;
    char buf[BUF_LEN];
    ssize_t numRead;
    char *p;
    struct inotify_event *event;

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
    {
        printf("%s pathname... \n", argv[0]);
        exit(1);
    }

    inotifyFd = inotify_init(); /* Create inotify instance */
    if (inotifyFd == -1)
    {
        printf("inotify_init");
        exit(EXIT_FAILURE);
    }


    for (j = 1; j < argc; j++)
    {
        wd = inotify_add_watch(inotifyFd, argv[j], IN_ALL_EVENTS);
        if (wd == -1)
        {
            printf("inotify_add_watch");
            exit(EXIT_FAILURE);
        }
        printf("Watching %s using wd %d\n", argv[j], wd);
    }

    while(1) // Read Event forever
    {
        numRead = read(inotifyFd, buf, BUF_LEN);

        if (numRead == 0)
        {
            printf("FATAL: read() from inotify fd returned 0!");
            exit(EXIT_FAILURE);
        }
        if (numRead == -1)
        {
            printf("read");
            exit(EXIT_FAILURE);
        }

        printf("Read %ld bytes from inotify fd\n", (long) numRead);

        for (p = buf; p < buf + numRead;)         /* Process all of the events in buffer returned by read() */
        {
            event = (struct inotify_event *) p;
            displayInotifyEvent(event);
            p += sizeof (struct inotify_event) +event->len;
        }
    }
    exit(EXIT_SUCCESS);
}
