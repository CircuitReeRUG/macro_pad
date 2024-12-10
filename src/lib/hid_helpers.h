#pragma once
#include <pico/stdlib.h>

#include "tusb.h"

typedef void (*char_callback_t)(uint8_t chr);

char keycode_to_char(uint8_t keycode, bool shift);
bool char_to_keycode(char chr, uint8_t *keycode, uint8_t *modifier);

extern uint8_t const keycode_to_ascii[128][2];
extern uint8_t const ascii_to_keycode[128][2];
