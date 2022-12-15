cmake_minimum_required(VERSION 3.22)
project(lab1 C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "-Wall -Werror -g")

add_executable(rwLock
        main.c reader_first.c reader_first.h writer_first.c writer_first.h rw_lock.c rw_lock.h duration.c duration.h)

add_executable(lock
        lock.c reader_first.c reader_first.h writer_first.c writer_first.h rw_lock.c rw_lock.h duration.c duration.h)
