CC=g++ 
CFLAGS=-ggdb -I. 
DEPS = inotify.h
OBJ = inotify.o
TARGET = inotify_runner
LIBS = -lpthread

%.o: %.cc $(DEPS)
	$(CC)-c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm *.o $(TARGET)
