#pragma once
#include <pico/stdlib.h>

#include "tusb.h"
#include "tllist.h"

#define query(key) keymap[key / MATRIX_COLS][key % MATRIX_COLS]

typedef struct EncoderControls {
    uint16_t increment;
    uint16_t decrement;
    uint16_t button;
} EncoderControls;

typedef struct Key {
    bool type;
    union {
        uint8_t mod_key;
        uint8_t keys[6];
    };
}Key;