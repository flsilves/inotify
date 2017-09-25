cmake_minimum_required(VERSION 3.8)
project(inotify)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
    listener.cpp
    listener.h
    backlog.cpp
    backlog.h
    exception.h
    mutex.cpp
    mutex.h
    inotify.cpp
    inotify.h)

include_directories(.)

add_executable(inotify ${SOURCE_FILES})
