#pragma once
#include "loggers.h"

typedef enum {
    SENS_ACC, 
    SENS_GYRO,
    SENS_IR,
    SENS_MAX
} sens_type_t;

int sens_mgr_init(void);
int sens_mgr_start(void);