#include "mhid.h"

#include "tllist.h"

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

static void send_hid_report(hid_report report) {
    if (!tud_hid_ready()) return;

    static bool has_keyboard_key = false;

    //TODO: implement manual key repeat
    if (report.valid) {
        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, report.mod_key, report.data); // will trigger repeat delay if next key is valid
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
        while (!tud_hid_ready()); // wait for the report to be sent and send a null report to not enter key repeat mode
        tud_hid_report(REPORT_ID_CONSUMER_CONTROL, NULL, 0);
        has_consumer_key = true;
    } else {
        if (has_consumer_key) tud_hid_report(REPORT_ID_CONSUMER_CONTROL, NULL, 0);
        has_consumer_key = false;
    }
}

void hid_task(macropad_options options) {
    static uint32_t start_ms = 0;

    if (board_millis() - start_ms < POLL_INTERVAL) return;  // not enough time
    start_ms += POLL_INTERVAL;

    hid_report const report = options.get_keycode_function ? options.get_keycode_function() : (hid_report){0};
    encoder_queue_t* enc = options.get_enc ? options.get_enc() : &(encoder_queue_t)tll_init();

    if (tud_suspended()) {
        tud_remote_wakeup();
    } else {
        send_hid_report(report);
        send_enc_hid_report(enc);
        if (options.set_display_function) options.set_display_function((int8_t)report.data[0]);
    }
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
                               hid_report_type_t report_type, uint8_t* buffer,
                               uint16_t reqlen) {
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)reqlen;
    return 0;
}

void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {
    (void)itf;
    (void)report_id;
    (void)report_type;
}

void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len) {
    (void)instance;
    (void)report;
    (void)len;
}