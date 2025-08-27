#include <zephyr/kernel.h>
#include <zephyr/types.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/printk.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/services/bas.h>
#include <zephyr/bluetooth/services/hrs.h>
#include <zephyr/bluetooth/direction.h>
#include <zephyr/logging/log.h>

#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include "loggers.h"
#include "ble_adv.h"

#define LOG_TAG "BLE_ADV"
#define MAX_NOTIFIEES 5

typedef struct __attribute__((packed)) {
    uint16_t     company_code; 
    uint8_t      color;
    uint8_t      charging;
    uint8_t      battery_level;
    uint8_t      flag;
} bt_adv_manuf_data_t;

static void bt_adv_work_cb(struct k_work *work);
K_WORK_DEFINE(bt_adv_work, bt_adv_work_cb);

static unsigned char url_data[] = {0x17,'/','/','a','c','a','d','e','m','y','.',
                                 'n','o','r','d','i','c','s','e','m','i','.',
                                 'c','o','m'};

static const bt_adv_manuf_data_t manuf_data = {
    .company_code   = sys_cpu_to_le16(0x02E5),                  // The Linux Foundation
    .color          = 0x01,                                     // Example color value
    .charging       = 0x01,                                     // Example charging status
    .battery_level  = 0x64,                                     // Example battery level (100%)
    .flag           = 0x00                                      // Example flag
};

const struct bt_le_adv_param le_adv_param = {
    .options = (BT_LE_ADV_OPT_CONN | BT_LE_ADV_OPT_USE_IDENTITY), 
    .interval_min = (0x0030), 
    .interval_max = (0x0060), 
    .peer = NULL, 
};

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL)),
	BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
    BT_DATA(BT_DATA_MANUFACTURER_DATA, &manuf_data, sizeof(manuf_data)),
};

static const struct bt_data sd[] = {
    BT_DATA(BT_DATA_URI, url_data, sizeof(url_data)),
};

conn_noti_cb notify_conn_status;
int set_conn_listener(conn_noti_cb notifier) {
    BEGIN();
    notify_conn_status = notifier;
    END();
    return 0;
}

static void connected(struct bt_conn *conn, uint8_t err) {
    BEGIN();
	if (err) {
		log_e("Connection failed err 0x%02x %s", err, bt_hci_err_to_str(err));
        return;
	}
    if (notify_conn_status) notify_conn_status(0, err); // Notify success
    END();
}

static void disconnected(struct bt_conn *conn, uint8_t reason) {
    BEGIN();
	log_i("Disconnected, reason 0x%02x %s", reason, bt_hci_err_to_str(reason));
    if (notify_conn_status) notify_conn_status(1, reason);
    END();
}

static void recycled() {
	BEGIN();
    k_work_submit(&bt_adv_work);
    END();
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
    .recycled = recycled,
};

static void start_advertise() {
	BEGIN();
	int err = bt_le_adv_start(&le_adv_param, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
	if (err) {
		log_e("Advertising failed to start (err %d)", err);
		return;
	}
	END();
}
 
static void bt_adv_work_cb(struct k_work *work) {
    BEGIN();
	start_advertise();
    END();
}

static void bt_enable_cb(int err) {
    BEGIN();
	if (err) {
		log_e("BT enable failed (err %d)", err);
		return;
	}
    bt_addr_le_t addr;
    err = bt_addr_le_from_str("FF:EE:DD:CC:BB:AA", "random", &addr);
    if (err) {
        log_e("Invalid BT address (err %d)", err);
    }

    err = bt_id_create(&addr, NULL);
    if (err < 0) {
        log_e("Creating new ID failed (err %d)", err);
    }

	k_work_submit(&bt_adv_work);
    END();
}

static void auth_passkey_display(struct bt_conn *conn, uint32_t passkey) {
    BEGIN();
	char addr[BT_ADDR_LE_STR_LEN];
	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
	log_i("Passkey for %s: %06u", addr, passkey);
    END();
}

static void auth_cancel(struct bt_conn *conn) {
    BEGIN();
	char addr[BT_ADDR_LE_STR_LEN];
	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
	log_i("Pairing cancelled: %s", addr);
    END();
}

static void pairing_complete(struct bt_conn *conn, bool bonded) {
    BEGIN();
	log_i("Pairing Complete");
    END();
}

static void pairing_failed(struct bt_conn *conn, enum bt_security_err reason) {
    BEGIN();
	log_i("Pairing Failed (%d). Disconnecting.", reason);
	bt_conn_disconnect(conn, BT_HCI_ERR_AUTH_FAIL);
    END();
}

static struct bt_conn_auth_cb auth_cb_display = {
	.passkey_display = auth_passkey_display,
	.passkey_entry = NULL,
	.cancel = auth_cancel,
};

static struct bt_conn_auth_info_cb auth_cb_info = {
	.pairing_complete = pairing_complete,
	.pairing_failed = pairing_failed,
};

int start_bluetooth(void) {
    BEGIN();
    k_work_init(&bt_adv_work, bt_adv_work_cb);
	if (bt_enable(bt_enable_cb)) {
		log_e("BT enable failed");
	}
    bt_conn_auth_cb_register(&auth_cb_display);
	bt_conn_auth_info_cb_register(&auth_cb_info);
    END();
	return 0;
}