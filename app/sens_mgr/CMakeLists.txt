include_directories(${CMAKE_SOURCE_DIR}/sens_mgr/inc)

FILE(GLOB src_files src/*.c)
target_sources(app PRIVATE ${src_files})