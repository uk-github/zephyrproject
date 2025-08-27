
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <stdio.h>
#include "loggers.h"
#include "msg_que.h"

#if CONFIG_BT_SERVICE
#include "bt_service.h"
#endif

#if CONFIG_BATTERY
#include "battery.h"
#endif

#if CONFIG_SENS_MGR
#include "sens_mgr.h"
#endif

#if CONFIG_WIFI_SERVICE_ENABLED
#include "wifi_service.h"
#endif

#define LOG_TAG "MAIN"

#define STACK_NAME(name)    name##_stack 
#define CB_NAME(name)       name##_th_cb
#define TH_NAME(name)       name##_th
#define INIT_TH_TABLE(name) { .stack_area = STACK_NAME(name), .stack_size = 1024, .cb = CB_NAME(name) }

#define INIT_FUNC(name)     name##_init
#define START_FUNC(name)    name##_start
#define INIT_MODULE(name)   { .init = INIT_FUNC(name), .start = START_FUNC(name) }

#define ARY_SZ(ARY)         sizeof(ARY) / sizeof(ARY[0])


K_MSGQ_DEFINE(main_msgq, sizeof(msg_t *), 10, 4);

typedef struct {
    k_tid_t tid;
    struct k_thread th;
    void *stack_area;
    size_t stack_size;
    void (*cb)(void *arg1, void *arg2, void *arg3);
} thread_info_t;

typedef struct __attribute__((packed)) {
    int (*init) (void);
    int (*start) (void);
} module_info_t;

#if CONFIG_BT_SERVICE
    K_THREAD_STACK_DEFINE(STACK_NAME(bt_service), 1024);
#endif
#if CONFIG_BATTERY
    K_THREAD_STACK_DEFINE(STACK_NAME(battery), 1024);
#endif

#if CONFIG_SENS_MGR
    K_THREAD_STACK_DEFINE(STACK_NAME(sens_mgr), 1024);
#endif

thread_info_t th_tble[] = {
#if CONFIG_BT_SERVICE
    INIT_TH_TABLE(bt_service),
#endif
#if CONFIG_BATTERY
    INIT_TH_TABLE(battery),
#endif

#if CONFIG_SENS_MGR
    INIT_TH_TABLE(sens_mgr),
#endif
};

module_info_t module_info[] = {
#if CONFIG_BT_SERVICE
    INIT_MODULE(bt_service),
#endif
#if CONFIG_BATTERY
    INIT_MODULE(battery),
#endif
#if CONFIG_SENS_MGR
    INIT_MODULE(sens_mgr),
#endif
#if CONFIG_WIFI_SERVICE_ENABLED
    INIT_MODULE(wifi_service),
#endif    
};

int main(void) {
    BEGIN();
    for (int i = 0; i < ARY_SZ(module_info); i++) {
        if (module_info[i].init) module_info[i].init();
    }

    for (int i = 0; i < ARY_SZ(th_tble); i++) {
        th_tble[i].tid = k_thread_create(&th_tble[i].th,
                         th_tble[i].stack_area, th_tble[i].stack_size, 
                         th_tble[i].cb, NULL, NULL, NULL, 
                         5, 0, K_NO_WAIT);
    }
    
    for (int i = 0; i < ARY_SZ(module_info); i++) {
        if (module_info[i].start) module_info[i].start();
    }

    while (1) {
        msg_t msg;
        if (k_msgq_get(&main_msgq, &msg, K_FOREVER) == 0) {
            log_d("Received message of type %d with ID %d", msg.cmd, msg.id);
        }
        k_sleep(K_MSEC(1000));
        log_d("Main th is running");
    }
    END();  
    return 0;
}