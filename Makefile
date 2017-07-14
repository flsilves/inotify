CC=g++
CFLAGS=-I.
DEPS = inotify.h
OBJ = inotify.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

inotify_runner: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
