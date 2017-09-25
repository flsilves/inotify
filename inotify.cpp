#include "inotify.h"

using namespace std;

bool enableDebug = false;

ConcurrentBacklog fileList;

int main(const int argc, const char *argv[]) {
    int numberOfThreads = 100;
    char folderPathC[PATH_MAX];

    readArguments(argc, argv, numberOfThreads, folderPathC);
    debug("input arguments: PATH:[%s]  NUMBER_OF_THREADS:[%d]\n", folderPathC, numberOfThreads);

    string folderPath = string(folderPathC);
    cout << folderPath << endl;

    pthread_t thread1, thread2, thread3;

    pthread_create(&thread1, NULL, threadReaderLoop, &folderPath);
    pthread_create(&thread2, NULL, threadConsumerLoop, NULL);
    pthread_create(&thread3, NULL, threadAuditFolder, &folderPath);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);

    exit(EXIT_SUCCESS);
}


void *threadReaderLoop(void* arg) {

    string *folderPath = static_cast<string* > (arg);

    Listener ListenerInstance(*folderPath, &fileList);

    while (true) {
        ListenerInstance.readEvents();
        ListenerInstance.processBacklog();
    }
}

void *threadConsumerLoop(void *) {

    while (true) {
        debug("Consumer thread - Retrieving File... \n");
        if (fileList.size() != 0) {
            string file_to_delete = fileList.pop();
            deleteFile(file_to_delete);
        }
    }
}

void *threadAuditFolder(void* arg) {

    string *folderPath = static_cast<string* > (arg);
    while (true) {
        sleep(AUDIT_TIMEOUT);
        auditFolder(*folderPath);
    }
}

void deleteFile(string &filePath) {
    if (remove(filePath.c_str()) != 0) {
        fprintf(stderr, "ERROR: threadConsumerLoop -> remove(%s) ", filePath.c_str());
        perror("");
    }
}

void auditFolder(string &folderPath) {

    DIR *dir = 0;
    struct dirent *ent = 0;
    struct stat statbuf;

    if ((dir = opendir(folderPath.c_str())) != NULL) {

        while ((ent = readdir(dir)) != NULL) {

            if (ent->d_name[0] == '.') { // ignore all files that start with '.'
                continue;
            }
            string filePath = folderPath + ent->d_name;

            if (stat(filePath.c_str(), &statbuf) != 0) {
                fprintf(stderr, "ERROR: audit folder -> stat(%s) ", filePath.c_str());
                perror("");
                return;
            }

            if (S_ISREG(statbuf.st_mode)) { // check if file isn't a directory and has right permissions
                fileList.push(filePath.c_str());
            }
        }
        closedir(dir);

    } else {
        fprintf(stderr, "ERROR - auditFolder -> opendir(%s) ", folderPath.c_str());
        perror("");
        return;
    }
}

void readArguments(const int &argc, const char **argv, int &numberOfThreads,
                   char *folderPath) {

    struct stat statbuf;
    numberOfThreads = 2;

    if (argc <= 1 || argc >= 4 || strcmp(argv[1], "--help") == 0) {
        printf("USAGE: %s <pathname> <#threads>\n", argv[0]);
        exit(1);
    }

    realpath(argv[1], folderPath);
    debug("Validating folder: %s\n", folderPath);

    if (stat(folderPath, &statbuf) == -1) {
        printf("Argument is not a valid folder path:\n");
        exit(EXIT_FAILURE);
    }

    if (!S_ISDIR(statbuf.st_mode)) {
        printf("Argument is not a valid folder path:\n");
        exit(EXIT_FAILURE);
    }

    if (argc == 3) {
        numberOfThreads = atoi(argv[2]);
    }

    strcat(folderPath, "/");
}


void debug(const char *format, ...) {
    va_list args;
    va_start(args, format);

    if (enableDebug) {
        printf("** DEBUG **    ");
        vprintf(format, args);
    }
    va_end(args);
}

