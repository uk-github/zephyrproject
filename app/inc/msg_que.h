#pragma once
#include <zephyr/kernel.h>

typedef struct __attribute__((packed)) {
    uint8_t msq_type;
    uint8_t msq_id;
    uint16_t len;
    uint8_t *data;
} msg_t;