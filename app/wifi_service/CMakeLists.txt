include_directories(${CMAKE_SOURCE_DIR}/wifi_service/inc)

FILE(GLOB src_files src/*.c)
target_sources(app PRIVATE ${src_files})