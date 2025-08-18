include_directories(${CMAKE_SOURCE_DIR}/shell_cmd/inc)

FILE(GLOB src_files src/*.c)
target_sources(app PRIVATE ${src_files})