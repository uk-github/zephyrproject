#pragma once
#include "loggers.h"

typedef enum {
    SENS_ACC, 
    SENS_GYRO,
    SENS_IR,
    SENS_MAX
} sens_type_t;

typedef struct {
    sens_type_t type;
    void (*status_cb)(int status, int reason);
    void (*data_cb)(sens_type_t type, void *data, int len);
    sys_snode_t node;
} sens_handler_t;

int reg_sensor(sens_type_t type, sens_handler_t *handle);
int unreg_sensor(sens_type_t type, sens_handler_t *handle);
int unreg_sensor_all();

void sens_mgr_th_cb(void *arg1, void *arg2, void *arg3);

int sens_mgr_init(void);
int sens_mgr_start(void);