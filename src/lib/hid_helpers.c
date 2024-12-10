#include "hid_helpers.h"

// Initialize the mapping arrays using the macros from "tusb.h"
uint8_t const ascii_to_keycode[128][2] = {HID_ASCII_TO_KEYCODE};
uint8_t const keycode_to_ascii[128][2] = {HID_KEYCODE_TO_ASCII};

/**
 * @brief Convert an ASCII character to HID keycode and modifier.
 *
 * @param chr The ASCII character to convert.
 * @param keycode Pointer to store the HID keycode.
 * @param modifier Pointer to store the HID modifier (e.g., shift).
 * @return true if successful, false if character is unsupported.
 */
bool char_to_keycode(char chr, uint8_t *keycode, uint8_t *modifier) {
    uint8_t chr_uint = (uint8_t)chr;

    // Check if the character is within the valid ASCII range
    if (chr_uint >= 128) {
        return false;
    }

    // Get the shift flag and keycode from the mapping array
    uint8_t shift = ascii_to_keycode[chr_uint][0];
    uint8_t kc = ascii_to_keycode[chr_uint][1];

    if (kc == 0) {
        // Unsupported character
        return false;
    }

    // Set modifier and keycode
    *modifier = shift ? KEYBOARD_MODIFIER_LEFTSHIFT : 0;
    *keycode = kc;

    return true;
}

/**
 * @brief Convert a TinyUSB keycode and shift flag to an ASCII character.
 *
 * @param keycode The TinyUSB HID keycode to convert.
 * @param shift Indicates whether the shift modifier is active.
 * @return The corresponding ASCII character, or 0 if unsupported.
 */
char keycode_to_char(uint8_t keycode, bool shift) {
    if (keycode >= 128) return 0;
    return keycode_to_ascii[keycode][shift ? 1 : 0];
}
