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
    batt_info_t bat = {
        .bat_full = 0,
        .bat_level = 10,
        .chg_status = 1,
        .temp_c = 35
    };
    while (1) {
        log_d("inside thread");
        if (bat.chg_status == 1)  bat.bat_level += 1;
        else bat.bat_level -= 1;

        if (bat.bat_level == 100) {
            bat.bat_full = 1;
            bat.chg_status = 0;
        } else if(bat.bat_level == 0) {
            bat.chg_status = 1;
        } else bat.bat_full = 0;

        send_data_to_bt((const uint8_t *) &bat, sizeof(bat));
        k_sleep(K_SECONDS(60));
    }
    END();
}
