#pragma once
#include <pico/stdlib.h>

#include "bsp/board_api.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include "tllist.h"

#define POLL_INTERVAL 10

typedef tll(uint16_t) encoder_queue_t;
/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum {
    BLINK_NOT_MOUNTED = 250,
    BLINK_MOUNTED = 1000,
    BLINK_SUSPENDED = 2500,
};

typedef struct hid_report {
    bool valid;
    uint8_t mod_key;
    uint8_t data[6];
} hid_report;

typedef hid_report (*get_key_fn)(void);
typedef encoder_queue_t* (*get_enc_fn)(void);
typedef void (*set_dpy_fn)(int8_t keycode);

typedef struct macropad_options {
    get_key_fn get_keycode_function;
    get_enc_fn get_enc;
    set_dpy_fn set_display_function;
} macropad_options;


void hid_task(macropad_options options);
