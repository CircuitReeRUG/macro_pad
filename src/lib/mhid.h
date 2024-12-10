#pragma once
#include <pico/stdlib.h>

#include "bsp/board_api.h"
#include "tllist.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include "mhid_config.h"

#ifndef POLL_INTERVAL
#define POLL_INTERVAL 10
#endif

#define COMPARE_REPORTS(a, b) (a.mod_key == b.mod_key && memcmp(a.data, b.data, 6) == 0)
#define COPY_REPORTS(a, b) (memcpy(&(a), &(b), sizeof(hid_report)))
#define RESET_ALARM(a) (a.fired = false, a.set = false)

typedef tll(uint16_t) encoder_queue_t;

enum key_state {
    MP_KEY_PRESSED_OR_RELEASED,
    MP_KEY_REPEATING
};

typedef struct hid_report {
    bool valid;
    uint8_t mod_key;
    uint8_t data[6];
} hid_report;

typedef struct alarm_state {
    bool fired;
    bool set;
    alarm_id_t id;
} alarm_state;

typedef struct repeat_info {
    uint8_t state;
    uint8_t key;
}repeat_info;

typedef struct mhid_state {
    alarm_state alarm;
    repeat_info repeat;
} mhid_state;

typedef hid_report (*get_key_fn)(void);
typedef encoder_queue_t* (*get_enc_fn)(void);
typedef void (*set_dpy_fn)(int8_t keycode);

typedef struct macropad_options {
    get_key_fn get_keycode_function;
    get_enc_fn get_enc;
    set_dpy_fn set_display_function;
} macropad_options;

void hid_task(macropad_options options, mhid_state* state);
