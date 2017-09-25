CC=clang++  
CFLAGS=-ggdb -I. -std=c++11 
DEPS = inotify.h backlog.h listener.h exception.h
OBJ = inotify.o backlog.o listener.o
TARGET = inotify_runner
LIBS = -lpthread

%.o: %.cpp $(DEPS)
	$(CC)-c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm *.o $(TARGET)
