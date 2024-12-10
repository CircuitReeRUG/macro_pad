#define DEBOUNCE_DELAY 10
#define MATRIX_ROWS 4
#define MATRIX_COLS 3
#define RESET_BOOTSEL_KEY 2

static const uint col_pins[MATRIX_COLS] = {3, 4, 5};
static const uint row_pins[MATRIX_ROWS] = {6, 7, 8, 9};

static const Key keymap[MATRIX_ROWS][MATRIX_COLS] = {
    {
        {true, .mod_key = KEYBOARD_MODIFIER_LEFTGUI},
        {false, .keys = {HID_KEY_A, HID_KEY_B, HID_KEY_C, HID_KEY_D, HID_KEY_E, HID_KEY_F}},
        {false, .keys = {HID_KEY_SHIFT_LEFT, HID_KEY_S}},
    },
    {
        {true, .mod_key = KEYBOARD_MODIFIER_LEFTCTRL},
        {false, .keys = {HID_KEY_GUI_LEFT, HID_KEY_ARROW_RIGHT}},
        {false, .keys = {HID_KEY_GUI_LEFT, HID_KEY_ARROW_LEFT}},
    },
    {
        {true, .mod_key = KEYBOARD_MODIFIER_LEFTALT},
        {false, .keys = {HID_KEY_Y, HID_KEY_Z, HID_KEY_1, HID_KEY_2, HID_KEY_3, HID_KEY_4}},
        {false, .keys = {HID_KEY_5, HID_KEY_6, HID_KEY_7, HID_KEY_8, HID_KEY_9, HID_KEY_0}},
    },
    {
        {true, .mod_key = KEYBOARD_MODIFIER_LEFTGUI},
        {false, .keys = {HID_KEY_T, HID_KEY_U, HID_KEY_V, HID_KEY_W, HID_KEY_X, HID_KEY_Y}},
        {false, .keys = {HID_KEY_Z, HID_KEY_1, HID_KEY_2, HID_KEY_3, HID_KEY_4, HID_KEY_5}},
    },
};

// Simple encoder example
static const EncoderControls encoder = {
    HID_USAGE_CONSUMER_VOLUME_INCREMENT, // Spinning clockwise
    HID_USAGE_CONSUMER_VOLUME_DECREMENT, // Spinning counter-clockwise
    HID_USAGE_CONSUMER_PLAY_PAUSE // Pressing the encoder
};