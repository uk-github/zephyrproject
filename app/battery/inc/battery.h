#pragma once

typedef struct __attribute__((packed)) {
    uint8_t bat_level;
    uint8_t chg_status;
    uint8_t temp_c;
    uint8_t bat_full;
} batt_info_t;

int battery_init(void);
int battery_start(void);

void battery_th_cb(void *arg1, void *arg2, void *arg3);