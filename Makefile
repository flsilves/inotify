CC=g++
CFLAGS=-I.
DEPS = inotify.h
OBJ = inotify.o
TARGET = inotify_runner
LIBS = -lpthread

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm *.o $(TARGET)
