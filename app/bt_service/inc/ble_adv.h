#pragma once

typedef struct {
    void (*conn_noti_cb) (uint8_t status, uint8_t reason);
} ble_conn_notifer_t;

int set_conn_listener(ble_conn_notifer_t *notifier);
int start_bluetooth(void);