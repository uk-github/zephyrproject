include_directories(${CMAKE_SOURCE_DIR}/bt_service/inc)

target_sources(app PRIVATE
    src/bt_service.c
    src/ble_adv.c
    src/blec_acc_service.c
)