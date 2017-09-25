#ifndef INOTIFY_H
#define INOTIFY_H

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

#define AUDIT_TIMEOUT 2.0

using namespace std;

void readArguments(const int &argc, const char **argv, int &numberOfThreads, char *folderPath);

void debug(const char *format, ...);

void deleteFile(string &filePath);

void threadConsumerLoop();

void threadReaderLoop(string folderPath);

int auditFolder(string &folderPath);

void threadAuditFolder(string folderPath);


#endif

