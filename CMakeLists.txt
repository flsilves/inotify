cmake_minimum_required(VERSION 3.8)
project(inotify)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
    backlog.h
    inotify.cc
    inotify.h
    myTimer.h)

include_directories(.)

add_executable(inotify ${SOURCE_FILES})