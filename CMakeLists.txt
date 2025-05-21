cmake_minimum_required(VERSION 3.30)
project(aps_os_realtime)

set(CMAKE_CXX_STANDARD 14)

include_directories(aps_os_realtime)

add_executable(aps_os_realtime
    aps_os_realtime/defs.h
    aps_os_realtime/events.cpp
    aps_os_realtime/global.cpp
    aps_os_realtime/os.cpp
    aps_os_realtime/resource.cpp
    aps_os_realtime/rtos_api.h
    aps_os_realtime/sys.h
    aps_os_realtime/test.cpp
        aps_os_realtime/task.cpp
)
