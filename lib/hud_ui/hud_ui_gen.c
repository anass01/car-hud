/**
 * @file hud_ui_gen.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "hud_ui.h"

#if defined(LV_USE_XML) && LV_USE_XML
#endif /* LV_USE_XML */

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void check_font(lv_font_t ** font, const char * name);

/**********************
 *  STATIC VARIABLES
 **********************/

static uint32_t hud_ui_target = HUD_UI_COMPILE_TARGET;

/*----------------
 * Translations
 *----------------*/

/**********************
 *  GLOBAL VARIABLES
 **********************/

/*--------------------
 *  Permanent screens
 *-------------------*/

/*----------------
 * Fonts
 *----------------*/

lv_font_t * roboto_regular_24;
extern lv_font_t roboto_regular_24_data;
lv_font_t * roboto_bold_40;
extern lv_font_t roboto_bold_40_data;
lv_font_t * roboto_bold_150;
extern lv_font_t roboto_bold_150_data;

/*----------------
 * Images
 *----------------*/

/* Targets: any */
const void * rotation = NULL;
extern const void * rotation_data;
const void * brightness = NULL;
extern const void * brightness_data;
const void * hud = NULL;
extern const void * hud_data;
const void * warning = NULL;
extern const void * warning_data;
const void * disconnect = NULL;
extern const void * disconnect_data;
const void * back = NULL;
extern const void * back_data;
const void * restart = NULL;
extern const void * restart_data;
const void * turbo = NULL;
extern const void * turbo_data;

/*----------------
 * Global styles
 *----------------*/

lv_style_t style_dark;

/*----------------
 * Subjects
 *----------------*/

lv_subject_t engine_rpm;
lv_subject_t engine_rpm_str;
lv_subject_t coolant_temp;
lv_subject_t coolant_temp_str;
lv_subject_t speed;
lv_subject_t speed_str;
lv_subject_t fuel_consumption;
lv_subject_t fuel_consumption_str;
lv_subject_t fuel_consumption_val_str;
lv_subject_t fuel_consumption_unit_str;
lv_subject_t turbo_boost;
lv_subject_t turbo_boost_str;
lv_subject_t intake_temp;
lv_subject_t intake_temp_str;
lv_subject_t g_force_str;
lv_subject_t can_error;
lv_subject_t con_error;
lv_subject_t settings_brightness;
lv_subject_t settings_rotation;
lv_subject_t settings_hud;
lv_subject_t settings_restart;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void hud_ui_init_gen(const char * asset_path)
{

    /*----------------
     * Fonts
     *----------------*/

    /* Targets: any */

    #if HUD_UI_CHECK_COMPILE_TARGET(HUD_UI_TARGET_ALL)
    if (hud_ui_check_target(HUD_UI_TARGET_ALL)) {
        if (!roboto_regular_24) {
            /* roboto_regular_24 */
            /* get font 'roboto_regular_24' from a C array */
            roboto_regular_24 = &roboto_regular_24_data;

        }
        if (!roboto_bold_40) {
            /* roboto_bold_40 */
            /* get font 'roboto_bold_40' from a C array */
            roboto_bold_40 = &roboto_bold_40_data;

        }
        if (!roboto_bold_150) {
            /* roboto_bold_150 */
            /* get font 'roboto_bold_150' from a C array */
            roboto_bold_150 = &roboto_bold_150_data;

        }
    }
    #endif

    /*----------------
     * Images
     *----------------*/

    /* Targets: any */
    #if HUD_UI_CHECK_COMPILE_TARGET(HUD_UI_TARGET_ALL)
    if (hud_ui_check_target(HUD_UI_TARGET_ALL)) {
        /* rotation */
        if (!rotation) {
            rotation = &rotation_data;
        }
        /* brightness */
        if (!brightness) {
            brightness = &brightness_data;
        }
        /* hud */
        if (!hud) {
            hud = &hud_data;
        }
        /* warning */
        if (!warning) {
            warning = &warning_data;
        }
        /* disconnect */
        if (!disconnect) {
            disconnect = &disconnect_data;
        }
        /* back */
        if (!back) {
            back = &back_data;
        }
        /* restart */
        if (!restart) {
            restart = &restart_data;
        }
        /* turbo */
        if (!turbo) {
            turbo = &turbo_data;
        }
    }
    #endif

    /*----------------
     * Global styles
     *----------------*/

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_dark);

        lv_style_set_bg_color(&style_dark, lv_color_hex(0x1f222a));
        lv_style_set_bg_opa(&style_dark, 255);
        lv_style_set_text_color(&style_dark, lv_color_hex(0xffffff));
        lv_style_set_text_font(&style_dark, roboto_regular_24);

        style_inited = true;
    }

    /*----------------
     * Subjects
     *----------------*/
    lv_subject_init_int(&engine_rpm, 0);
    static char engine_rpm_str_buf[UI_SUBJECT_STRING_LENGTH];
    static char engine_rpm_str_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&engine_rpm_str, engine_rpm_str_buf, engine_rpm_str_prev_buf,
                           UI_SUBJECT_STRING_LENGTH, "--");
    lv_subject_init_int(&coolant_temp, 0);
    static char coolant_temp_str_buf[UI_SUBJECT_STRING_LENGTH];
    static char coolant_temp_str_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&coolant_temp_str, coolant_temp_str_buf, coolant_temp_str_prev_buf,
                           UI_SUBJECT_STRING_LENGTH, "--");
    lv_subject_init_int(&speed, 0);
    static char speed_str_buf[UI_SUBJECT_STRING_LENGTH];
    static char speed_str_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&speed_str, speed_str_buf, speed_str_prev_buf,
                           UI_SUBJECT_STRING_LENGTH, "--");
    lv_subject_init_int(&fuel_consumption, 0);
    lv_subject_set_min_value_int(&fuel_consumption, 0);
    lv_subject_set_max_value_int(&fuel_consumption, 35);
    static char fuel_consumption_str_buf[UI_SUBJECT_STRING_LENGTH];
    static char fuel_consumption_str_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&fuel_consumption_str, fuel_consumption_str_buf, fuel_consumption_str_prev_buf,
                           UI_SUBJECT_STRING_LENGTH, "-- L/100km");
    static char fuel_consumption_val_str_buf[UI_SUBJECT_STRING_LENGTH];
    static char fuel_consumption_val_str_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&fuel_consumption_val_str, fuel_consumption_val_str_buf, fuel_consumption_val_str_prev_buf,
                           UI_SUBJECT_STRING_LENGTH, "--");
    static char fuel_consumption_unit_str_buf[UI_SUBJECT_STRING_LENGTH];
    static char fuel_consumption_unit_str_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&fuel_consumption_unit_str, fuel_consumption_unit_str_buf, fuel_consumption_unit_str_prev_buf,
                           UI_SUBJECT_STRING_LENGTH, "L/100km");
    lv_subject_init_int(&turbo_boost, 0);
    lv_subject_set_min_value_int(&turbo_boost, 0);
    lv_subject_set_max_value_int(&turbo_boost, 250);
    static char turbo_boost_str_buf[UI_SUBJECT_STRING_LENGTH];
    static char turbo_boost_str_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&turbo_boost_str, turbo_boost_str_buf, turbo_boost_str_prev_buf,
                           UI_SUBJECT_STRING_LENGTH, "-- bar");
    lv_subject_init_int(&intake_temp, 0);
    static char intake_temp_str_buf[UI_SUBJECT_STRING_LENGTH];
    static char intake_temp_str_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&intake_temp_str, intake_temp_str_buf, intake_temp_str_prev_buf,
                           UI_SUBJECT_STRING_LENGTH, "--°C");
    static char g_force_str_buf[UI_SUBJECT_STRING_LENGTH];
    static char g_force_str_prev_buf[UI_SUBJECT_STRING_LENGTH];
    lv_subject_init_string(&g_force_str, g_force_str_buf, g_force_str_prev_buf,
                           UI_SUBJECT_STRING_LENGTH, "0.00 G");
    lv_subject_init_int(&can_error, 0);
    lv_subject_set_min_value_int(&can_error, 0);
    lv_subject_set_max_value_int(&can_error, 1);
    lv_subject_init_int(&con_error, 1);
    lv_subject_set_min_value_int(&con_error, 0);
    lv_subject_set_max_value_int(&con_error, 1);
    lv_subject_init_int(&settings_brightness, 127);
    lv_subject_set_min_value_int(&settings_brightness, 10);
    lv_subject_set_max_value_int(&settings_brightness, 255);
    lv_subject_init_int(&settings_rotation, 0);
    lv_subject_set_min_value_int(&settings_rotation, 0);
    lv_subject_set_max_value_int(&settings_rotation, 3);
    lv_subject_init_int(&settings_hud, 0);
    lv_subject_set_min_value_int(&settings_hud, 0);
    lv_subject_set_max_value_int(&settings_hud, 3);
    lv_subject_init_int(&settings_restart, 0);
    lv_subject_set_min_value_int(&settings_restart, 0);
    lv_subject_set_max_value_int(&settings_restart, 1);

    /*----------------
     * Translations
     *----------------*/

#if defined(LV_USE_XML) && LV_USE_XML
    /* Register widgets */

    /* Check all fonts / default if needed. This prevents fonts that are used in one target but
       defined in another from causing assertion failures during rendering of the Preview. */
    check_font(&roboto_regular_24, "roboto_regular_24");
    check_font(&roboto_bold_40, "roboto_bold_40");
    check_font(&roboto_bold_150, "roboto_bold_150");

    /* Register fonts */
    lv_xml_register_font(NULL, "roboto_regular_24", roboto_regular_24);
    lv_xml_register_font(NULL, "roboto_bold_40", roboto_bold_40);
    lv_xml_register_font(NULL, "roboto_bold_150", roboto_bold_150);

    /* Register subjects */
    lv_xml_register_subject(NULL, "engine_rpm", &engine_rpm);
    lv_xml_register_subject(NULL, "engine_rpm_str", &engine_rpm_str);
    lv_xml_register_subject(NULL, "coolant_temp", &coolant_temp);
    lv_xml_register_subject(NULL, "coolant_temp_str", &coolant_temp_str);
    lv_xml_register_subject(NULL, "speed", &speed);
    lv_xml_register_subject(NULL, "speed_str", &speed_str);
    lv_xml_register_subject(NULL, "fuel_consumption", &fuel_consumption);
    lv_xml_register_subject(NULL, "fuel_consumption_str", &fuel_consumption_str);
    lv_xml_register_subject(NULL, "fuel_consumption_val_str", &fuel_consumption_val_str);
    lv_xml_register_subject(NULL, "fuel_consumption_unit_str", &fuel_consumption_unit_str);
    lv_xml_register_subject(NULL, "turbo_boost", &turbo_boost);
    lv_xml_register_subject(NULL, "turbo_boost_str", &turbo_boost_str);
    lv_xml_register_subject(NULL, "intake_temp", &intake_temp);
    lv_xml_register_subject(NULL, "intake_temp_str", &intake_temp_str);
    lv_xml_register_subject(NULL, "g_force_str", &g_force_str);
    lv_xml_register_subject(NULL, "can_error", &can_error);
    lv_xml_register_subject(NULL, "con_error", &con_error);
    lv_xml_register_subject(NULL, "settings_brightness", &settings_brightness);
    lv_xml_register_subject(NULL, "settings_rotation", &settings_rotation);
    lv_xml_register_subject(NULL, "settings_hud", &settings_hud);
    lv_xml_register_subject(NULL, "settings_restart", &settings_restart);

    /* Register callbacks */

    /* Register easing callbacks */
#endif

    /* Register all the global assets so that they won't be created again when globals.xml is parsed.
     * While running in the editor skip this step to update the preview when the XML changes */
#if defined(LV_USE_XML) && LV_USE_XML && !defined(LV_EDITOR_PREVIEW)
    /* Register images */
    lv_xml_register_image(NULL, "rotation", rotation);
    lv_xml_register_image(NULL, "brightness", brightness);
    lv_xml_register_image(NULL, "hud", hud);
    lv_xml_register_image(NULL, "warning", warning);
    lv_xml_register_image(NULL, "disconnect", disconnect);
    lv_xml_register_image(NULL, "back", back);
    lv_xml_register_image(NULL, "restart", restart);
    lv_xml_register_image(NULL, "turbo", turbo);
#endif

#if !defined(LV_USE_XML) || LV_USE_XML == 0
    /*--------------------
     *  Permanent screens
     *-------------------*/
    /* If XML is enabled it's assumed that the permanent screens are created
     * manually from XML using lv_xml_create() */
#endif
}

void hud_ui_set_target(uint32_t target)
{
    hud_ui_target = target;
}

uint32_t hud_ui_get_target(void)
{
    return hud_ui_target;
}

bool hud_ui_check_target(uint32_t target)
{
    return (hud_ui_target & target) ? true : false;
}

/* Callbacks */

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void check_font(lv_font_t ** font, const char * name)
{
    if (!(*font)) {
        *font = (lv_font_t *)LV_FONT_DEFAULT;
        LV_LOG_WARN("font `%s` was not set. Using `LV_FONT_DEFAULT` instead", name);
    }
}