#pragma once


int battery_init(void);
int battery_start(void);

void battery_th_cb(void *arg1, void *arg2, void *arg3);