#include <zephyr/kernel.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/sys/printk.h>
#include <string.h>

#include "blec_acc_service.h"
#include "loggers.h"
#include "bt_service.h"
#include "ble_adv.h"
#include "msg_que.h"

#define LOG_TAG "BLEC_ACC"

static struct bt_uuid_128 acc_serivce_uuid = BT_UUID_INIT_128(
    BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x123456789abc));

static struct bt_uuid_128 acc_serivce_char_uuid = BT_UUID_INIT_128(
    BT_UUID_128_ENCODE(0xabcdef01, 0x2345, 0x6789, 0xabcd, 0xef0123456789));

static struct bt_uuid_128 acc_service_c_uuid = BT_UUID_INIT_128(
    BT_UUID_128_ENCODE(0xd062b97d, 0xe392, 0x42d0, 0x8ed8, 0x42cbf5a492b0));

static char acc_sve_value[20] = "Hello BLE";

static ssize_t read_my_char(struct bt_conn *conn,
                            const struct bt_gatt_attr *attr,
                            void *buf, uint16_t len, uint16_t offset) {
    BEGIN();
    const char *value = "this is esp32s3";
    int ret = bt_gatt_attr_read(conn, attr, buf, len, offset, value, strlen(value));
    log_i("sent data: %s", value);
    END();
    return ret;
}

static ssize_t write_my_char(struct bt_conn *conn,
                             const struct bt_gatt_attr *attr,
                             const void *buf, uint16_t len, uint16_t offset,
                             uint8_t flags) {
    BEGIN();
    const char *data = (char *) buf;
    memcpy(acc_sve_value, data + offset, len);
    log_i("received data: %s", acc_sve_value);
    END();
    return len;
}

static uint16_t notify_enabled;
static uint16_t indication_enabled;

static void ccc_notify_cb(const struct bt_gatt_attr *attr, uint16_t value) {
    BEGIN();
    notify_enabled = value;
    log_i("CCCD changed: %s", notify_enabled ? "enabled" : "disabled");
    END();
}

static void ccc_indication_cb(const struct bt_gatt_attr *attr, uint16_t value) {
    BEGIN();
    indication_enabled = value;
    log_i("CCCD changed: %s", notify_enabled ? "enabled" : "disabled");
    END();
}

BT_GATT_SERVICE_DEFINE(acc_serivce,
    BT_GATT_PRIMARY_SERVICE(&acc_serivce_uuid),
    BT_GATT_CHARACTERISTIC(&acc_serivce_char_uuid.uuid,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           read_my_char, write_my_char, &acc_sve_value),
    BT_GATT_CCC(ccc_notify_cb, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

    BT_GATT_CHARACTERISTIC(&acc_service_c_uuid.uuid,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_INDICATE,
                           BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           read_my_char, write_my_char, &acc_sve_value),
    BT_GATT_CCC(ccc_indication_cb, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

void send_data(const uint8_t *msg, uint16_t len) {
    BEGIN();
    if (notify_enabled) {
        bt_gatt_notify(NULL, &acc_serivce.attrs[2], msg, len);
        log_i("Sent to central: %s", msg);
    } else {
        log_w("not enabled");
    }
    END();
}

void send_with_ack(const uint8_t *msg, uint16_t len) {
    BEGIN();
    END();
}