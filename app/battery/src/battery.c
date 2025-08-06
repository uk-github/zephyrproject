#include <zephyr/kernel.h>
#include <unistd.h>
#include "battery.h"
#include "loggers.h"

#include "bt_service.h"

#define LOG_TAG "BATT"

int battery_init(void) {
    BEGIN();
    END();
    return 0;
}

int battery_start(void) {
    BEGIN();
    END();
    return 0;
}

void battery_th_cb(void *arg1, void *arg2, void *arg3) {
    BEGIN();
    while (1) {
        LOG_INFO("inside thread");
        uint8_t data[] = {1,2,3,4};
        send_data_to_bt(data, sizeof(data));
        k_sleep(K_SECONDS(60));
    }
    END();
}
