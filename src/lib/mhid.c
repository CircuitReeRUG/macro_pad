#include "mhid.h"

#include "tllist.h"

static void send_hid_report(hid_report report) {
    if (!tud_hid_ready()) return;

    static bool has_keyboard_key = false;

    if (report.valid) {
        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, report.mod_key, report.data);  // Will trigger repeat delay if next key is valid
#if defined(MP_CUSTOM_REPEAT)
        while (!tud_hid_ready());
        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
#endif
        has_keyboard_key = true;
    } else {
        if (has_keyboard_key) tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
        has_keyboard_key = false;
    }
}

static void send_enc_hid_report(encoder_queue_t* enc) {
    if (!tud_hid_ready()) return;

    static bool has_consumer_key = false;

    if (tll_length(*enc) > 0) {
        uint16_t event = tll_pop_front(*enc);
        tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &event, sizeof(event));
        while (!tud_hid_ready() && event != HID_USAGE_CONSUMER_PLAY_PAUSE);  // Wait for the report to be sent (TODO: Generalize this)
        tud_hid_report(REPORT_ID_CONSUMER_CONTROL, NULL, 0);                 // Send a null report to prevent key repeat mode
        has_consumer_key = true;
    } else {
        if (has_consumer_key) tud_hid_report(REPORT_ID_CONSUMER_CONTROL, NULL, 0);
        has_consumer_key = false;
    }
}

#if defined(MP_CUSTOM_REPEAT)
static int64_t alarm_callback(__unused alarm_id_t id, void* user_data) {
    alarm_state* state = (alarm_state*)user_data;
    bool* fired = &(state->fired);
    *fired = true;
    return 0;
}

static void handle_repeat(mhid_state* state) {
    switch (state->repeat.state) {
        case MP_KEY_PRESSED_OR_RELEASED:
            if (!state->alarm.set) break;
            cancel_alarm(state->alarm.id);
            RESET_ALARM(state->alarm);
            break;

        case MP_KEY_REPEATING:
            if (state->alarm.set) break;
            state->alarm.id = add_alarm_in_ms(MP_REPEAT_DELAY, alarm_callback, &(state->alarm), false);
            state->alarm.set = true;
            break;

        default:
            __unreachable();
    }
}

#endif

void hid_task(macropad_options options, mhid_state* state) {
    static uint32_t start_ms = 0;
    static hid_report old_hid_report = {0};

    if (board_millis() - start_ms < POLL_INTERVAL) return;  // Not enough time
    start_ms += POLL_INTERVAL;

    hid_report const report = options.get_keycode_function ? options.get_keycode_function() : (hid_report){0};
    encoder_queue_t* enc = options.get_enc ? options.get_enc() : &(encoder_queue_t)tll_init();

    if (tud_suspended()) {
        tud_remote_wakeup();
    } else {

#if defined(MP_CUSTOM_REPEAT)
        state->repeat.state = COMPARE_REPORTS(old_hid_report, report) ? MP_KEY_REPEATING : MP_KEY_PRESSED_OR_RELEASED;
        handle_repeat(state);
#endif

        if (!state->alarm.set || state->alarm.fired) send_hid_report(report);
        send_enc_hid_report(enc);
        if (options.set_display_function) options.set_display_function((int8_t)report.data[0]);
    }

    COPY_REPORTS(old_hid_report, report);
}

uint16_t tud_hid_get_report_cb(__unused uint8_t instance, __unused uint8_t report_id,
                               __unused hid_report_type_t report_type, __unused uint8_t* buffer,
                               __unused uint16_t reqlen) {
    return 0;
}

void tud_hid_set_report_cb(__unused uint8_t itf, __unused uint8_t report_id,
                           __unused hid_report_type_t report_type, __unused uint8_t const* buffer,
                           __unused uint16_t bufsize) {}

void tud_hid_report_complete_cb(__unused uint8_t instance, __unused uint8_t const* report, __unused uint16_t len) {}
