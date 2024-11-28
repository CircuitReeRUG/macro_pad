#pragma once

#include <hardware/i2c.h>
#include <hardware/pio.h>
#include <pico/multicore.h>
#include <pico/stdlib.h>

#include "config.h"
#include "encoder.h"
#include "hardware/timer.h"
#include "hid_helpers.h"
#include "mhid.h"
#include "ssd1306.h"

static ssd1306_t oled_display;

static void setup_i2c() {
    i2c_init(I2C_INSTANCE(1), 400 * 1000);

    gpio_set_function(SCREEN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(SCREEN_SCL, GPIO_FUNC_I2C);

    gpio_pull_up(SCREEN_SDA);
    gpio_pull_up(SCREEN_SCL);
}

static void setup_kb_gpio() {
    for (uint8_t i = 0; i < MATRIX_COLS; i++) {
        gpio_init(col_pins[i]);
        gpio_set_dir(col_pins[i], GPIO_IN);
        gpio_pull_up(col_pins[i]);
    }

    for (uint8_t j = 0; j < MATRIX_ROWS; j++) {
        gpio_init(row_pins[j]);
        gpio_set_dir(row_pins[j], GPIO_OUT);
        gpio_put(row_pins[j], 1);
    }
}

static inline void scan_matrix(int8_t *arr, int8_t *cnt);

static inline void scan_matrix_backup(int8_t *arr, int8_t *cnt) {
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        gpio_put(row_pins[i], 0);
        sleep_us(1);

        for (uint8_t j = 0; j < MATRIX_COLS; j++) {
            if (gpio_get(col_pins[j]) == 0 && *cnt < 2) {
                arr[*cnt] = (int8_t)(i * MATRIX_COLS + j);
                (*cnt)++;
            }
        }

        gpio_put(row_pins[i], 1);
    }
}

static inline hid_report parse_keys(int8_t keys[2], int8_t cnt) {
    hid_report report = {.valid = true};

    for (int8_t i = 0; i < cnt; i++) {

        if (query(keys[i]).type) {
            report.mod_key = query(keys[i]).mod_key;

        } else {
            memcpy(report.data, query(keys[i]).keys, 6);
        }
    }

    return report;
}

static inline hid_report get_keycode() {
    int8_t keys[2] = {0};
    hid_report report = {.valid = false};
    uint32_t start = to_ms_since_boot(get_absolute_time());

    while (to_ms_since_boot(get_absolute_time()) - start < DEBOUNCE_DELAY) {
        int8_t current_keys[2] = {0};
        int8_t cnt = 0;
        scan_matrix(current_keys, &cnt);

        if (memcmp(keys, current_keys, 2) != 0 && cnt > 0) {
            memcpy(keys, current_keys, 2);
            report = parse_keys(keys, cnt);
            start = to_ms_since_boot(get_absolute_time());
        }
    }

    return report;
}

static inline void set_display_function(int8_t keycode) {
    if (keycode == -1) return;

    char ch = keycode_to_char((char)keycode, false);

    if (ch) {
        ssd1306_clear(&oled_display);
        char str[2] = {ch, '\0'};
        ssd1306_draw_string(&oled_display, 0, 0, 7, str);
        ssd1306_show(&oled_display);
    }
}

static inline int32_t get_enc() {
    int8_t diff = get_enc_pos_diff();
    if (diff > 0) {
        return encoder.increment;  // increment
    } else if (diff < 0) {
        return encoder.decrement;  // decrement
    } else if (get_enc_btn_state()) {
        return encoder.button;  // button
    }
    return -1;
}

static inline void init_keypad() {
    board_init();
    tud_init(BOARD_TUD_RHPORT);

    if (board_init_after_tusb) {
        board_init_after_tusb();
    }

    // Keyboard and encoder setup
    setup_kb_gpio();
    setup_enc();
}

static inline void init_display() {
    // Display setup
    setup_i2c();
    ssd1306_init(&oled_display, 128, 64, 0x3C, I2C_INSTANCE(1));
    ssd1306_clear(&oled_display);
    ssd1306_show(&oled_display);
}

static void update_macropad(macropad_options options) {
    options.get_enc = get_enc;

    hid_task(options);
}

