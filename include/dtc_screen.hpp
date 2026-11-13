#pragma once
#include <Arduino.h>
#include <lvgl.h>
#include <Timber.h>
#include "hud_ui.h"
#include "displays/pins.h"

/* ---------- DTC DATABASE & DECODER ---------- */
struct DtcEntry {
    const char *code;
    const char *title;
};

static const DtcEntry DTC_DATABASE[] = {
    {"P0000", "No Fault Detected"},
    {"P0010", "Camshaft Position Actuator Circuit (B1)"},
    {"P0011", "Cam Timing Over-Advanced (B1)"},
    {"P0012", "Cam Timing Over-Retarded (B1)"},
    {"P0016", "Crank/Cam Timing Alignment (B1)"},
    {"P0030", "O2 Heater Control Circuit (B1S1)"},
    {"P0036", "O2 Heater Control Circuit (B1S2)"},
    {"P0045", "Turbo Boost Control Solenoid Circuit"},
    {"P0087", "Fuel Rail/System Pressure Too Low"},
    {"P0088", "Fuel Rail/System Pressure Too High"},
    {"P0093", "Fuel System Major Leak Detected"},
    {"P0100", "Mass Air Flow (MAF) Circuit"},
    {"P0101", "MAF Sensor Range / Performance"},
    {"P0102", "MAF Sensor Circuit Low Input"},
    {"P0103", "MAF Sensor Circuit High Input"},
    {"P0105", "MAP Sensor Circuit Malfunction"},
    {"P0106", "MAP Sensor Range / Performance"},
    {"P0107", "MAP Sensor Circuit Low Input"},
    {"P0108", "MAP Sensor Circuit High Input"},
    {"P0110", "Intake Air Temp (IAT) Circuit"},
    {"P0111", "IAT Sensor Range / Performance"},
    {"P0112", "IAT Sensor Circuit Low Input"},
    {"P0113", "IAT Sensor Circuit High Input"},
    {"P0115", "Engine Coolant Temp (ECT) Circuit"},
    {"P0116", "ECT Sensor Range / Performance"},
    {"P0117", "ECT Sensor Circuit Low Input"},
    {"P0118", "ECT Sensor Circuit High Input"},
    {"P0120", "Throttle Position Sensor A Circuit"},
    {"P0121", "Throttle Position Sensor A Range"},
    {"P0122", "Throttle Position Sensor A Low"},
    {"P0123", "Throttle Position Sensor A High"},
    {"P0128", "Coolant Temp Below Thermostat"},
    {"P0130", "O2 Sensor Circuit (B1S1)"},
    {"P0131", "O2 Sensor Circuit Low Volts (B1S1)"},
    {"P0132", "O2 Sensor Circuit High Volts (B1S1)"},
    {"P0133", "O2 Sensor Slow Response (B1S1)"},
    {"P0134", "O2 Sensor No Activity (B1S1)"},
    {"P0135", "O2 Sensor Heater Malfunction (B1S1)"},
    {"P0136", "O2 Sensor Circuit (B1S2)"},
    {"P0141", "O2 Sensor Heater Malfunction (B1S2)"},
    {"P0171", "System Too Lean (Bank 1)"},
    {"P0172", "System Too Rich (Bank 1)"},
    {"P0180", "Fuel Temperature Sensor A Circuit"},
    {"P0190", "Fuel Rail Pressure Sensor Circuit"},
    {"P0191", "Fuel Rail Pressure Sensor Range"},
    {"P0192", "Fuel Rail Pressure Sensor Low"},
    {"P0193", "Fuel Rail Pressure Sensor High"},
    {"P0200", "Injector Circuit Malfunction"},
    {"P0201", "Cylinder 1 Injector Circuit"},
    {"P0202", "Cylinder 2 Injector Circuit"},
    {"P0203", "Cylinder 3 Injector Circuit"},
    {"P0204", "Cylinder 4 Injector Circuit"},
    {"P0234", "Turbocharger Engine Overboost"},
    {"P0236", "Turbo Boost Sensor A Range/Perf"},
    {"P0237", "Turbo Boost Sensor A Low Input"},
    {"P0238", "Turbo Boost Sensor A High Input"},
    {"P0299", "Turbocharger Underboost Condition"},
    {"P0300", "Random / Multiple Cylinder Misfire"},
    {"P0301", "Cylinder 1 Misfire Detected"},
    {"P0302", "Cylinder 2 Misfire Detected"},
    {"P0303", "Cylinder 3 Misfire Detected"},
    {"P0304", "Cylinder 4 Misfire Detected"},
    {"P0325", "Knock Sensor 1 Circuit (Bank 1)"},
    {"P0335", "Crankshaft Position Sensor A Circuit"},
    {"P0336", "Crank Position Sensor Range/Perf"},
    {"P0340", "Camshaft Position Sensor A Circuit"},
    {"P0341", "Cam Position Sensor Range/Perf"},
    {"P0380", "Glow Plug / Heater Circuit A"},
    {"P0381", "Glow Plug Indicator Circuit"},
    {"P0400", "Exhaust Gas Recirc (EGR) Flow"},
    {"P0401", "EGR Flow Insufficient Detected"},
    {"P0402", "EGR Flow Excessive Detected"},
    {"P0403", "EGR Control Circuit Malfunction"},
    {"P0404", "EGR Circuit Range / Performance"},
    {"P0420", "Catalyst Efficiency Below Threshold"},
    {"P0470", "Exhaust Pressure Sensor Malfunction"},
    {"P0471", "Exhaust Pressure Sensor Range"},
    {"P0500", "Vehicle Speed Sensor (VSS) Malfunc"},
    {"P0505", "Idle Control System Malfunction"},
    {"P0520", "Engine Oil Pressure Sensor Circuit"},
    {"P0562", "System Voltage Low"},
    {"P0563", "System Voltage High"},
    {"P0606", "ECM / PCM Processor Fault"},
    {"P0670", "Glow Plug Module Control Circuit"},
    {"P0700", "Transmission Control System Malf"},
    {"P2002", "DPF Efficiency Below Threshold"},
    {"P2101", "Throttle Actuator Motor Perf"},
    {"P2452", "DPF Pressure Sensor A Circuit"},
    {"P2453", "DPF Pressure Sensor A Range"},
    {"P2463", "DPF Soot Accumulation"},
    {"U0001", "High Speed CAN Communication Bus"},
    {"U0100", "Lost Comm With ECM / PCM"},
    {"U0101", "Lost Comm With TCM"},
    {"U0121", "Lost Comm With ABS Control Module"},
    {"U0155", "Lost Comm With Instrument Panel (IPC)"}
};

static inline const char *dtc_get_title(const char *code)
{
    for (size_t i = 0; i < sizeof(DTC_DATABASE) / sizeof(DTC_DATABASE[0]); i++)
    {
        if (strcmp(code, DTC_DATABASE[i].code) == 0)
        {
            return DTC_DATABASE[i].title;
        }
    }

    if (code[0] == 'P')
    {
        if (code[1] == '0')
        {
            switch (code[2])
            {
            case '1': return "Fuel / Air Metering Fault";
            case '2': return "Fuel Injector / Boost Fault";
            case '3': return "Ignition / Misfire / Glow Fault";
            case '4': return "Auxiliary Emissions / EGR / DPF";
            case '5': return "Vehicle Speed / Idle Control";
            case '6': return "Computer / Output Circuits";
            case '7':
            case '8': return "Transmission System Fault";
            default:  return "Powertrain System Fault";
            }
        }
        else
        {
            return "Manufacturer Powertrain Fault";
        }
    }
    else if (code[0] == 'C') return "Chassis / ABS / Brake Fault";
    else if (code[0] == 'B') return "Body / Lighting System Fault";
    else if (code[0] == 'U') return "Network / CAN Bus Fault";
    return "Diagnostic Trouble Code";
}

/* ---------- DTC SCREEN STATE & OBJECTS ---------- */
struct DtcItem {
    char code[8];
    const char *title;
};

static constexpr size_t MAX_STORED_DTCS = 16;

static DtcItem s_dtc_list[MAX_STORED_DTCS];
static uint8_t s_dtc_count = 0;
static bool s_dtc_scanned = false;
static bool s_engine_off = true;

// Public accessors
inline bool dtc_is_scanned() { return s_dtc_scanned; }
inline uint8_t dtc_get_count() { return s_dtc_count; }
inline void dtc_reset_codes() { s_dtc_count = 0; }

// UI Object pointers
static lv_obj_t *s_scr_dtc = nullptr;
static lv_obj_t *s_cont_list = nullptr;
static lv_obj_t *s_badge_engine = nullptr;
static lv_obj_t *s_label_engine = nullptr;
static lv_obj_t *s_label_status = nullptr;
static lv_obj_t *s_btn_read = nullptr;
static lv_obj_t *s_btn_clear = nullptr;
static lv_obj_t *s_label_clear = nullptr;

// External request trigger flags (handled by main OBD loop)
extern volatile uint8_t dtc_request_flag; // 0 = idle, 1 = read, 2 = clear

/* ---------- FORWARD DECLARATIONS ---------- */
inline void dtc_render_list();
inline void dtc_update_engine_ui();

extern lv_obj_t *dashboard_screen;
static lv_obj_t *s_return_scr = nullptr;

inline void dtc_set_return_screen(lv_obj_t *return_screen)
{
    s_return_scr = return_screen;
}

/* ---------- CALLBACKS ---------- */
static void on_dtc_back_cb(lv_event_t *e)
{
    lv_indev_t *indev = lv_indev_active();
    if (indev) lv_indev_wait_release(indev);
    lv_obj_t *target_scr = s_return_scr ? s_return_scr : dashboard_screen;
    if (target_scr)
    {
        lv_screen_load_anim(target_scr, LV_SCREEN_LOAD_ANIM_MOVE_RIGHT, 300, 0, false);
    }
}

static void on_dtc_read_clicked(lv_event_t *e)
{
    if (s_label_status)
    {
        lv_label_set_text(s_label_status, "Requesting Mode 03 (DTCs)...");
    }
    dtc_request_flag = 1; // Trigger OBD Read
}

static void on_dtc_clear_clicked(lv_event_t *e)
{
    if (!s_engine_off)
    {
        if (s_label_status)
        {
            lv_label_set_text(s_label_status, "Engine running! Stop engine to clear.");
        }
        return;
    }

    if (s_label_status)
    {
        lv_label_set_text(s_label_status, "Sending Clear DTC (Mode 04)...");
    }
    dtc_request_flag = 2; // Trigger OBD Clear
}

/* ---------- BUILD & BIND DTC SCREEN ---------- */
inline void dtc_screen_init(lv_obj_t *scr, lv_obj_t *return_screen)
{
    s_scr_dtc = scr;
    s_return_scr = return_screen;

    s_cont_list = lv_obj_find_by_name(s_scr_dtc, "dtc_list_cont");
    s_badge_engine = lv_obj_find_by_name(s_scr_dtc, "dtc_engine_badge");
    s_label_engine = lv_obj_find_by_name(s_scr_dtc, "dtc_engine_label");
    s_label_status = lv_obj_find_by_name(s_scr_dtc, "dtc_status");
    s_btn_read = lv_obj_find_by_name(s_scr_dtc, "dtc_btn_read");
    s_btn_clear = lv_obj_find_by_name(s_scr_dtc, "dtc_btn_clear");
    s_label_clear = lv_obj_find_by_name(s_scr_dtc, "dtc_clear_label");
    lv_obj_t *btn_back = lv_obj_find_by_name(s_scr_dtc, "dtc_back");

    if (btn_back)
    {
        lv_obj_add_event_cb(btn_back, on_dtc_back_cb, LV_EVENT_CLICKED, NULL);
    }

    if (s_btn_read)
    {
        lv_obj_add_event_cb(s_btn_read, on_dtc_read_clicked, LV_EVENT_CLICKED, NULL);
    }

    if (s_btn_clear)
    {
        lv_obj_add_event_cb(s_btn_clear, on_dtc_clear_clicked, LV_EVENT_CLICKED, NULL);
    }

    // Horizontal swipe gesture on DTC screen: swipe right returns to dashboard
    lv_obj_add_event_cb(s_scr_dtc, [](lv_event_t *e) {
        if (lv_event_get_code(e) == LV_EVENT_GESTURE) {
            lv_indev_t *indev = lv_indev_active();
            if (indev) {
                lv_dir_t dir = lv_indev_get_gesture_dir(indev);
                if (dir == LV_DIR_RIGHT) {
                    lv_indev_wait_release(indev);
                    Timber.i("[UI] DTC swipe right -> returning to dashboard");
                    lv_obj_t *target = s_return_scr ? s_return_scr : dashboard_screen;
                    if (target) {
                        lv_screen_load_anim(target, LV_SCREEN_LOAD_ANIM_MOVE_RIGHT, 300, 0, false);
                    }
                }
            }
        }
    }, LV_EVENT_GESTURE, NULL);

    if (s_cont_list)
    {
        lv_obj_set_scroll_dir(s_cont_list, LV_DIR_VER);
        lv_obj_add_flag(s_cont_list, LV_OBJ_FLAG_GESTURE_BUBBLE);
    }

    // Initial render
    dtc_update_engine_ui();
    dtc_render_list();
}

inline lv_obj_t *dtc_create_screen(lv_obj_t *return_screen = nullptr)
{
    lv_obj_t *scr = dtc_create();
    dtc_screen_init(scr, return_screen);
    return scr;
}

inline lv_obj_t *dtc_create(lv_obj_t *return_screen)
{
    return dtc_create_screen(return_screen);
}

/* ---------- RENDER DTC LIST ---------- */
inline void dtc_render_list()
{
    if (!s_cont_list) return;

    lv_obj_clean(s_cont_list);

    if (!s_dtc_scanned)
    {
        lv_obj_t *card = lv_obj_create(s_cont_list);
        lv_obj_set_size(card, lv_pct(100), 120);
        lv_obj_set_style_bg_color(card, lv_color_hex(0x1a1e28), 0);
        lv_obj_set_style_border_color(card, lv_color_hex(0x2b3242), 0);
        lv_obj_set_style_border_width(card, 1, 0);
        lv_obj_set_style_radius(card, 8, 0);
        lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t *msg = lv_label_create(card);
        lv_label_set_text(msg, LV_SYMBOL_EYE_OPEN "\nReady to Scan\nTap 'Read' to query OBD");
        lv_obj_set_style_text_align(msg, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(msg, lv_color_hex(0xa4b0be), 0);
        lv_obj_center(msg);
        return;
    }

    if (s_dtc_count == 0)
    {
        lv_obj_t *card = lv_obj_create(s_cont_list);
        lv_obj_set_size(card, lv_pct(100), 120);
        lv_obj_set_style_bg_color(card, lv_color_hex(0x13271f), 0);
        lv_obj_set_style_border_color(card, lv_color_hex(0x27ae60), 0);
        lv_obj_set_style_border_width(card, 1, 0);
        lv_obj_set_style_radius(card, 8, 0);
        lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t *msg = lv_label_create(card);
        lv_label_set_text(msg, LV_SYMBOL_OK "\nNO FAULT CODES\nECU reports 0 stored DTCs");
        lv_obj_set_style_text_align(msg, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(msg, lv_color_hex(0x2ecc71), 0);
        lv_obj_center(msg);
        return;
    }

    // Render each detected DTC
    for (uint8_t i = 0; i < s_dtc_count; i++)
    {
        lv_obj_t *card = lv_obj_create(s_cont_list);
        lv_obj_set_size(card, lv_pct(100), 62);
        lv_obj_set_style_bg_color(card, lv_color_hex(0x1c212c), 0);
        lv_obj_set_style_border_color(card, lv_color_hex(0x384155), 0);
        lv_obj_set_style_border_width(card, 1, 0);
        lv_obj_set_style_radius(card, 8, 0);
        lv_obj_set_style_pad_all(card, 6, 0);
        lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);

        // DTC Code (Bold Amber)
        lv_obj_t *lbl_code = lv_label_create(card);
        lv_label_set_text(lbl_code, s_dtc_list[i].code);
        lv_obj_set_style_text_font(lbl_code, roboto_regular_24, 0);
        lv_obj_set_style_text_color(lbl_code, lv_color_hex(0xf39c12), 0);
        lv_obj_set_pos(lbl_code, 4, 0);

        // Decoded Short Title
        lv_obj_t *lbl_desc = lv_label_create(card);
        lv_label_set_text(lbl_desc, s_dtc_list[i].title);
        lv_obj_set_style_text_color(lbl_desc, lv_color_hex(0xdfe4ea), 0);
        lv_obj_set_pos(lbl_desc, 4, 30);
    }
}

/* ---------- UPDATE ENGINE STATE & CLEAR BUTTON ---------- */
inline void dtc_update_engine_ui()
{
    if (!s_badge_engine || !s_label_engine || !s_btn_clear || !s_label_clear) return;

    if (s_engine_off)
    {
        // Engine is OFF -> Clear is allowed
        lv_obj_set_style_bg_color(s_badge_engine, lv_color_hex(0x102b1c), 0);
        lv_obj_set_style_border_color(s_badge_engine, lv_color_hex(0x27ae60), 0);
        lv_obj_set_style_border_width(s_badge_engine, 1, 0);
        lv_label_set_text(s_label_engine, LV_SYMBOL_OK " Engine OFF  (Clear Allowed)");
        lv_obj_set_style_text_color(s_label_engine, lv_color_hex(0x2ecc71), 0);

        // Clear button active Red
        lv_obj_set_style_bg_color(s_btn_clear, lv_color_hex(0x8a2318), 0);
        lv_obj_set_style_border_color(s_btn_clear, lv_color_hex(0xb8382a), 0);
        lv_label_set_text(s_label_clear, LV_SYMBOL_TRASH " Clear DTC");
        lv_obj_set_style_text_color(s_label_clear, lv_color_hex(0xffffff), 0);
    }
    else
    {
        // Engine is RUNNING -> Clear is locked for safety
        lv_obj_set_style_bg_color(s_badge_engine, lv_color_hex(0x331e13), 0);
        lv_obj_set_style_border_color(s_badge_engine, lv_color_hex(0xe67e22), 0);
        lv_obj_set_style_border_width(s_badge_engine, 1, 0);
        lv_label_set_text(s_label_engine, LV_SYMBOL_WARNING " Engine RUNNING  (Clear Locked)");
        lv_obj_set_style_text_color(s_label_engine, lv_color_hex(0xf39c12), 0);

        // Clear button locked Grey
        lv_obj_set_style_bg_color(s_btn_clear, lv_color_hex(0x2b303c), 0);
        lv_obj_set_style_border_color(s_btn_clear, lv_color_hex(0x3e4657), 0);
        lv_label_set_text(s_label_clear, LV_SYMBOL_MINUS " Clear (Locked)");
        lv_obj_set_style_text_color(s_label_clear, lv_color_hex(0x718093), 0);
    }
}

inline void dtc_set_engine_state(bool is_engine_off)
{
    if (s_engine_off != is_engine_off)
    {
        s_engine_off = is_engine_off;
        dtc_update_engine_ui();
    }
}

/* ---------- DTC DATA SETTERS (CALLED FROM OBD PARSER) ---------- */
inline void dtc_clear_all_codes()
{
    s_dtc_count = 0;
    s_dtc_scanned = true;
    dtc_render_list();
    if (s_label_status)
    {
        lv_label_set_text(s_label_status, "DTCs cleared! Querying to confirm...");
    }
}

inline void dtc_add_code(const char *code)
{
    if (s_dtc_count >= MAX_STORED_DTCS) return;

    // Check for duplicate
    for (uint8_t i = 0; i < s_dtc_count; i++)
    {
        if (strcmp(s_dtc_list[i].code, code) == 0) return;
    }

    strncpy(s_dtc_list[s_dtc_count].code, code, sizeof(s_dtc_list[s_dtc_count].code) - 1);
    s_dtc_list[s_dtc_count].code[sizeof(s_dtc_list[s_dtc_count].code) - 1] = '\0';
    s_dtc_list[s_dtc_count].title = dtc_get_title(code);
    s_dtc_count++;
}

inline void dtc_finalize_scan()
{
    s_dtc_scanned = true;
    dtc_render_list();
    if (s_label_status)
    {
        if (s_dtc_count == 0)
        {
            lv_label_set_text(s_label_status, "ECU reports: 0 trouble codes found.");
        }
        else
        {
            char buf[64];
            snprintf(buf, sizeof(buf), "Scan complete: %d DTC fault(s) detected.", s_dtc_count);
            lv_label_set_text(s_label_status, buf);
        }
    }
}

inline void dtc_set_status_text(const char *text)
{
    if (s_label_status)
    {
        lv_label_set_text(s_label_status, text);
    }
}

