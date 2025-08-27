#include <zephyr/kernel.h>
#include "loggers.h"
#include "bt_service.h"
#include "ble_adv.h"
#include "msg_que.h"
#include "blec_acc_service.h"

#define LOG_TAG "BT_SVC"
K_MSGQ_DEFINE(bt_svc_msgq, sizeof(msg_t *), 10, 4);

typedef struct {
    uint8_t msg_id;
    void (*handler)(msg_t *msg);
} msg_handler_t;

msg_handler_t handler[] = {

};

static void conn_listener(uint8_t status, uint8_t reason) {
    BEGIN();
    log_i("status: %u, reason: %u", status, reason);
    END();
}

int bt_service_start(void) {
    BEGIN();
    set_conn_listener(conn_listener);
    start_bluetooth();
    END();
    return 0;
}

int bt_service_init(void) {
    BEGIN();
    END();
    return 0;
}

int insert_bt_svc_msgq(void *msg) {
    BEGIN();
    int ret = k_msgq_put(&bt_svc_msgq, (msg_t *) msg, K_NO_WAIT);
    if (ret != 0) {
        log_e("insert failed, ret:%d", ret);
    }
    END();
    return ret;
}

void bt_service_th_cb(void *arg1, void *arg2, void *arg3) {
    BEGIN();
    while (1) {
        msg_t msg = {0, };
        int ret = k_msgq_get(&bt_svc_msgq, &msg, K_FOREVER);
        if (ret == 0) {
           // handler[msg.id].handler(&msg);
            k_free(msg.data);
        }
    }
    END();
}

void send_data_to_bt(const uint8_t *msg, uint16_t len) {
    BEGIN();
    send_data(msg, len);
    END();
}