#include "inotify.h"
#include "backlog.h"
#include "listener.h"

using namespace std;

int event_count = 0;
bool enable_debug = true;

ConcurrentSet fileList;

int main(const int argc, const char *argv[]) {

    int numberOfThreads = 100;
    char folder_path_ch[PATH_MAX];

    readArguments(argc, argv, numberOfThreads, folder_path_ch);
    debug("input arguments: PATH:[%s]  NUMBER_OF_THREADS:[%d]\n", folder_path_ch, numberOfThreads);

    string folderPath = string(folder_path_ch);
    cout << folderPath << endl;

    thread thread1(threadReaderLoop, folderPath);
    thread thread2(threadConsumerLoop);

    thread1.join();
    thread2.join();

    exit(EXIT_SUCCESS);
}

void readArguments(const int &argc, const char **argv, int &numberOfThreads,
                   char *folderPath) { // TODO - number_of_threads not used

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


void threadReaderLoop(string &folderPath) {

    Listener ListenerInstance(folderPath, &fileList);

    while (true) {
        ListenerInstance.readEvents();
        ListenerInstance.processBuffer();
    }
}



void threadConsumerLoop() {

    while (true) {
        debug("Consumer thread - Retrieving File... \n");
        string file_to_delete = fileList.pop();
        deleteFile(file_to_delete);
    }
}

void deleteFile(string &filePath) {
    if (remove(filePath.c_str()) != 0) {
        fprintf(stderr, "ERROR: threadConsumerLoop -> remove(%s) ", filePath.c_str());
        perror("");
    }
}

int auditFolder(string &folderPath) {

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
                return errno;
            }

            if (S_ISREG(statbuf.st_mode)) { // check if file isn't a directory and has right permissions
                fileList.push(filePath.c_str());
            }

        }
        closedir(dir);

    } else {
        fprintf(stderr, "ERROR - auditFolder -> opendir(%s) ", folderPath.c_str());
        perror("");
        return EXIT_FAILURE;
    }
}


template<typename T>
ostream &operator<<(ostream &os, const set<T> &v) {

    os << "** DEBUG **    SET: [";
    for (typename set<T>::const_iterator it = v.begin(); it != v.end(); ++it) {
        os << " " << *it << ",";
    }
    os << "]";
    return os;
}

void debug(const char *format, ...) {
    va_list args;
    va_start(args, format);

    if (enable_debug) {
        printf("** DEBUG **    ");
        vprintf(format, args);
    }
    va_end(args);
}

