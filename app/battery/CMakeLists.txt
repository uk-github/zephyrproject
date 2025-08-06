include_directories(${CMAKE_SOURCE_DIR}/battery/inc)

target_sources(app PRIVATE
    src/battery.c
)