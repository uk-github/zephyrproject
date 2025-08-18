#pragma once
#include <zephyr/kernel.h>

typedef struct __attribute__((packed)) {
    uint8_t cmd;
    uint8_t id;
    uint16_t len;
    uint8_t *data;
} msg_t;