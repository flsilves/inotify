CC=g++  
CFLAGS=-ggdb -I. 
DEPS = inotify.h myTimer.h
OBJ = inotify.o myTimer.o
TARGET = inotify_runner
LIBS = -lpthread

%.o: %.cpp $(DEPS)
	$(CC)-c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm *.o $(TARGET)
