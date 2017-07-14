CC=g++
CFLAGS=-I.
DEPS = inotify.h
OBJ = inotify.o
TARGET = inotify_runner

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm *.o $(TARGET)
