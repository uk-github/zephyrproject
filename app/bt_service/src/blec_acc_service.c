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

static char acc_sve_value[20] = "Hello BLE";

static ssize_t read_my_char(struct bt_conn *conn,
                            const struct bt_gatt_attr *attr,
                            void *buf, uint16_t len, uint16_t offset) {
    BEGIN();
    const char *value = "this is esp32s3";
    int ret = bt_gatt_attr_read(conn, attr, buf, len, offset, value, strlen(value));
    LOG_INFO("sent data: %s", value);
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
    LOG_INFO("received data: %s", acc_sve_value);
    END();
    return len;
}

static uint16_t notify_enabled;
static void ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value) {
    BEGIN();
    notify_enabled = value;
    LOG_INFO("CCCD changed: %s", notify_enabled ? "enabled" : "disabled");
    END();
}

BT_GATT_SERVICE_DEFINE(acc_serivce,
    BT_GATT_PRIMARY_SERVICE(&acc_serivce_uuid),
    BT_GATT_CHARACTERISTIC(&acc_serivce_char_uuid.uuid,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_NOTIFY | BT_GATT_CHRC_INDICATE,
                           BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           read_my_char, write_my_char, &acc_sve_value),
    BT_GATT_CCC(ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

void send_data(const uint8_t *msg, uint16_t len) {
    BEGIN();
    if (notify_enabled) {
        bt_gatt_notify(NULL, &acc_serivce.attrs[2], msg, len);
        LOG_INFO("Sent to central: %s", msg);
    } else {
        LOG_WARN("not enabled");
    }
    END();
}