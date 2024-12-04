#include "main.h"

/*
 * SET DISPLAY
 *
 * This function is responsible for updating what appears on the display
 * every time a key gets pressed!
 *
 * Check lib/display.h for a full list of available functions
 */
void set_display(int8_t keycode) {
    // if nothing is pressed, skip
    if (keycode == -1) return;

    // get the character for the current keycode
    char ch = keycode_to_char((char)keycode, false);

    if (ch) {
        // Draw something to the screen!

        // example code:
        display_clear(&oled_display);
        char str[2] = {ch, '\0'};
        display_draw_string(&oled_display, 0, 0, 7, str);
        display_show(&oled_display);
    }
}

/*
 * MATRIX SCAN CODE (double for loop)
 *
 * arr:    An array of detected pressed buttons.
 *         Coordinates are stored in format: x * MATRIX_COLS + y
 *
 * cnt:    Amount of stored values. Increment every time you add a pressed key.
 */
void scan_matrix(int8_t *arr, int8_t *cnt) {
    // fill in code, ask for help if you don't know!
    scan_matrix_backup(arr, cnt);
}

void core1_entry() {
    while (1) {
        // run tud_task on a different core
        tud_task();
    }
}

/* MAIN FUNCTION */
int main() {
    // start everything up
    init_keypad();
    init_display();

    sleep_ms(100);

    check_reset() ? reset_usb_boot(0, 0) : 0;

    multicore_launch_core1(core1_entry);
    while (1) {
        // poll the usb
        // tud_task();
        // run tud_task on a different core
        // update the macropad stuff
        // tud_task();
        update_macropad((macropad_options){
            .get_keycode_function = get_keycode,
            .get_enc = get_enc,
            .set_display_function = set_display});
    }
}
