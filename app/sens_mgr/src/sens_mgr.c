#include "sens_mgr.h"
#include "msg_que.h"
#define LOG_TAG     "SENS_M"

typedef enum {
    CMD_SENS_DATA,
    CMD_MAX
} msg_cmd_t;

K_MSGQ_DEFINE(sens_mgr_msgq, sizeof(msg_t *), 10, 4);
sys_slist_t slist[SENS_MAX];
static struct k_mutex slist_mtx;
static struct k_mutex sens_mgr_mtx;

void handle_sensor_data(uint8_t id,  void *data, uint16_t len) {
    BEGIN();
    sens_handler_t *handle;
    k_mutex_lock(&sens_mgr_mtx, K_FOREVER);
    SYS_SLIST_FOR_EACH_CONTAINER(&slist[id], handle, node) {
        if (handle->data_cb) {
            handle->data_cb(id, data, len);
        }
    }
    k_mutex_unlock(&sens_mgr_mtx);
    END();
}

int insert_sens_mgr_msgq(void *msg) {
    BEGIN();
    k_mutex_lock(&sens_mgr_mtx, K_FOREVER);
    int ret = k_msgq_put(&sens_mgr_msgq, (msg_t *) msg, K_NO_WAIT);
    if (ret != 0) {
        log_e("insert failed, ret:%d", ret);
    }
    k_mutex_unlock(&sens_mgr_mtx);
    END();
    return ret;
}

void sens_mgr_th_cb(void *arg1, void *arg2, void *arg3) {
    BEGIN();
    while (1) {
        msg_t *msg = NULL;
        int ret = k_msgq_get(&sens_mgr_msgq, msg, K_FOREVER);
        if (ret == 0) {
            switch (msg->cmd) {
                case CMD_SENS_DATA:{
                    handle_sensor_data(msg->id, msg->data, msg->len);
                }   break;
                default:
                    break;
            }
            k_free(msg->data);
            k_free(msg);
        }
    }
    END();
}




int reg_sensor(sens_type_t type, sens_handler_t *handle) {
    BEGIN();
    int ret = -1;
    do {
        k_mutex_lock(&slist_mtx, K_FOREVER);
       if (sys_slist_find(&slist[type], &handle->node, NULL)) {
            break;
        }
        if (sys_slist_is_empty(&slist[type])) {
            //register_sensor();
            ret = 0;
        }
    } while (0);
    
    sys_slist_append(&slist[type], &handle->node);
    k_mutex_unlock(&slist_mtx);
    END();
    return ret;
}

int unreg_sensor(sens_type_t type, sens_handler_t *handle) {
    BEGIN();
    int ret = -1;
    k_mutex_lock(&slist_mtx, K_FOREVER);
    do {
       if (sys_slist_find_and_remove(&slist[type], &handle->node)) {
            if (sys_slist_is_empty(&slist[type])) {
                // unregister sensor
                ret = 0;
            }
            handle->status_cb(0, 0);
        }
       
    } while (0);
    k_mutex_unlock(&slist_mtx);
    END();
    return ret;
}

int unreg_sensor_all() {
    BEGIN();
    int ret = 0;
    sens_handler_t *handle, *tmp;
    k_mutex_lock(&slist_mtx, K_FOREVER);
    for (int i = 0; i < SENS_MAX; i++) {
        SYS_SLIST_FOR_EACH_CONTAINER_SAFE(&slist[i], handle, tmp, node) {
            if (handle->status_cb) {
                handle->status_cb(0, 0);
            }
            sys_slist_remove(&slist[i], NULL, &handle->node);
        }
    }
    k_mutex_unlock(&slist_mtx);
    return ret;
    END();
}

int sens_mgr_init(void) {
    BEGIN();
    k_mutex_init(&slist_mtx);
    k_mutex_init(&sens_mgr_mtx);
    for (int i = 0; i < SENS_MAX; i++) {
        sys_slist_init(&slist[i]);
    }

    END();
    return 0;
}

int sens_mgr_start(void) {
    BEGIN();
    END();
    return 0;
}

void func_from_new_monitor() {
    return;
}