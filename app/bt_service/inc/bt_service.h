#pragma once

int bt_service_init(void);
int bt_service_start(void);

void bt_service_th_cb(void *arg1, void *arg2, void *arg3);
int insert_bt_svc_msgq(void *msg);
void send_data_to_bt(const uint8_t *msg, uint16_t len);