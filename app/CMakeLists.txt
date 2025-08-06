cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(app)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/bt_service/inc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/battery/inc)

if(CONFIG_BT_SERVICE)
add_subdirectory(bt_service)
endif()

if(CONFIG_BATTERY)
add_subdirectory(battery)
endif()

target_sources(app PRIVATE 
    src/main.c
)