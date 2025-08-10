#pragma once


typedef void (*conn_noti_cb) (uint8_t status, uint8_t reason);

int set_conn_listener(conn_noti_cb notifier);
int start_bluetooth(void);