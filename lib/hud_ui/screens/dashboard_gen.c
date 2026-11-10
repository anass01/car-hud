/**
 * @file dashboard_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "dashboard_gen.h"
#include "../hud_ui.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/***********************
 *  STATIC VARIABLES
 **********************/

/***********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * dashboard_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_rpm_track;
    static lv_style_t style_rpm_indicator;
    static lv_style_t style_consumption_track;
    static lv_style_t style_consumption_indicator;
    static lv_style_t style_turbo_track;
    static lv_style_t style_turbo_indicator;
    static lv_style_t style_thin_frame;
    static lv_style_t style_top_accent;
    static lv_style_t style_no_indicator;
    static lv_style_t style_knob;
    static lv_style_t style_dot;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_rpm_track);
        lv_style_init(&style_rpm_indicator);
        lv_style_init(&style_consumption_track);
        lv_style_init(&style_consumption_indicator);
        lv_style_init(&style_turbo_track);
        lv_style_init(&style_turbo_indicator);
        lv_style_init(&style_thin_frame);
        lv_style_init(&style_top_accent);
        lv_style_init(&style_no_indicator);
        lv_style_init(&style_knob);
        lv_style_init(&style_dot);

        lv_style_set_arc_width(&style_rpm_track, 14);
        lv_style_set_arc_rounded(&style_rpm_track, false);
        lv_style_set_arc_color(&style_rpm_track, lv_color_hex(0x343844));
        lv_style_set_arc_width(&style_rpm_indicator, 14);
        lv_style_set_arc_rounded(&style_rpm_indicator, false);
        lv_style_set_arc_color(&style_rpm_indicator, lv_color_hex(0xffffff));
        lv_style_set_arc_opa(&style_rpm_indicator, 255);
        lv_style_set_arc_width(&style_consumption_track, 14);
        lv_style_set_arc_rounded(&style_consumption_track, false);
        lv_style_set_arc_color(&style_consumption_track, lv_color_hex(0x343844));
        lv_style_set_arc_width(&style_consumption_indicator, 14);
        lv_style_set_arc_rounded(&style_consumption_indicator, false);
        lv_style_set_arc_color(&style_consumption_indicator, lv_color_hex(0xdd2222));
        lv_style_set_arc_opa(&style_consumption_indicator, 255);
        lv_style_set_arc_width(&style_turbo_track, 8);
        lv_style_set_arc_rounded(&style_turbo_track, false);
        lv_style_set_arc_color(&style_turbo_track, lv_color_hex(0x282c36));
        lv_style_set_arc_width(&style_turbo_indicator, 8);
        lv_style_set_arc_rounded(&style_turbo_indicator, false);
        lv_style_set_arc_color(&style_turbo_indicator, lv_color_hex(0xb84a22));
        lv_style_set_arc_opa(&style_turbo_indicator, 255);
        lv_style_set_arc_width(&style_thin_frame, 2);
        lv_style_set_arc_rounded(&style_thin_frame, false);
        lv_style_set_arc_color(&style_thin_frame, lv_color_hex(0x2e333f));
        lv_style_set_arc_width(&style_top_accent, 2);
        lv_style_set_arc_rounded(&style_top_accent, false);
        lv_style_set_arc_color(&style_top_accent, lv_color_hex(0x3b4150));
        lv_style_set_arc_width(&style_no_indicator, 0);
        lv_style_set_arc_opa(&style_no_indicator, 0);
        lv_style_set_bg_opa(&style_knob, 0);
        lv_style_set_border_width(&style_knob, 0);
        lv_style_set_width(&style_knob, 0);
        lv_style_set_height(&style_knob, 0);
        lv_style_set_width(&style_dot, 8);
        lv_style_set_height(&style_dot, 8);
        lv_style_set_radius(&style_dot, 4);
        lv_style_set_bg_color(&style_dot, lv_color_hex(0xffffff));
        lv_style_set_bg_opa(&style_dot, 255);
        lv_style_set_border_width(&style_dot, 0);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HUD_UI_CHECK_COMPILE_TARGET(HUD_UI_TARGET_ALL)
    if (hud_ui_check_target(HUD_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "dashboard_#");

        lv_obj_add_style(lv_obj_0, &style_dark, 0);
        lv_obj_t * lv_arc_0 = lv_arc_create(lv_obj_0);
        lv_arc_set_bg_start_angle(lv_arc_0, 20);
        lv_arc_set_bg_end_angle(lv_arc_0, 55);
        lv_obj_set_align(lv_arc_0, LV_ALIGN_CENTER);
        lv_obj_set_width(lv_arc_0, 396);
        lv_obj_set_height(lv_arc_0, 396);
        lv_arc_set_min_value(lv_arc_0, 0);
        lv_arc_set_max_value(lv_arc_0, 35);
        lv_arc_bind_value(lv_arc_0, &fuel_consumption);
        lv_obj_set_flag(lv_arc_0, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(lv_arc_0, &style_consumption_track, 0);
        lv_obj_add_style(lv_arc_0, &style_consumption_indicator, LV_PART_INDICATOR);
        lv_obj_add_style(lv_arc_0, &style_knob, LV_PART_KNOB);

        lv_obj_t * lv_arc_1 = lv_arc_create(lv_obj_0);
        lv_arc_set_bg_start_angle(lv_arc_1, 145);
        lv_arc_set_bg_end_angle(lv_arc_1, 35);
        lv_obj_set_align(lv_arc_1, LV_ALIGN_CENTER);
        lv_obj_set_width(lv_arc_1, 396);
        lv_obj_set_height(lv_arc_1, 396);
        lv_arc_set_min_value(lv_arc_1, 0);
        lv_arc_set_max_value(lv_arc_1, 5000);
        lv_arc_bind_value(lv_arc_1, &engine_rpm);
        lv_obj_set_flag(lv_arc_1, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(lv_arc_1, &style_rpm_track, 0);
        lv_obj_add_style(lv_arc_1, &style_rpm_indicator, LV_PART_INDICATOR);
        lv_obj_add_style(lv_arc_1, &style_knob, LV_PART_KNOB);

        lv_obj_t * lv_arc_2 = lv_arc_create(lv_obj_0);
        lv_arc_set_bg_start_angle(lv_arc_2, 155);
        lv_arc_set_bg_end_angle(lv_arc_2, 205);
        lv_arc_set_start_angle(lv_arc_2, 0);
        lv_arc_set_end_angle(lv_arc_2, 0);
        lv_obj_set_align(lv_arc_2, LV_ALIGN_CENTER);
        lv_obj_set_width(lv_arc_2, 364);
        lv_obj_set_height(lv_arc_2, 364);
        lv_obj_set_flag(lv_arc_2, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(lv_arc_2, &style_thin_frame, 0);
        lv_obj_add_style(lv_arc_2, &style_no_indicator, LV_PART_INDICATOR);
        lv_obj_add_style(lv_arc_2, &style_knob, LV_PART_KNOB);

        lv_obj_t * lv_arc_3 = lv_arc_create(lv_obj_0);
        lv_arc_set_bg_start_angle(lv_arc_3, 155);
        lv_arc_set_bg_end_angle(lv_arc_3, 205);
        lv_arc_set_start_angle(lv_arc_3, 0);
        lv_arc_set_end_angle(lv_arc_3, 0);
        lv_obj_set_align(lv_arc_3, LV_ALIGN_CENTER);
        lv_obj_set_width(lv_arc_3, 348);
        lv_obj_set_height(lv_arc_3, 348);
        lv_obj_set_flag(lv_arc_3, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(lv_arc_3, &style_thin_frame, 0);
        lv_obj_add_style(lv_arc_3, &style_no_indicator, LV_PART_INDICATOR);
        lv_obj_add_style(lv_arc_3, &style_knob, LV_PART_KNOB);

        lv_obj_t * lv_arc_4 = lv_arc_create(lv_obj_0);
        lv_arc_set_bg_start_angle(lv_arc_4, 335);
        lv_arc_set_bg_end_angle(lv_arc_4, 25);
        lv_arc_set_start_angle(lv_arc_4, 0);
        lv_arc_set_end_angle(lv_arc_4, 0);
        lv_obj_set_align(lv_arc_4, LV_ALIGN_CENTER);
        lv_obj_set_width(lv_arc_4, 364);
        lv_obj_set_height(lv_arc_4, 364);
        lv_obj_set_flag(lv_arc_4, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(lv_arc_4, &style_thin_frame, 0);
        lv_obj_add_style(lv_arc_4, &style_no_indicator, LV_PART_INDICATOR);
        lv_obj_add_style(lv_arc_4, &style_knob, LV_PART_KNOB);

        lv_obj_t * lv_arc_5 = lv_arc_create(lv_obj_0);
        lv_arc_set_bg_start_angle(lv_arc_5, 335);
        lv_arc_set_bg_end_angle(lv_arc_5, 25);
        lv_arc_set_start_angle(lv_arc_5, 0);
        lv_arc_set_end_angle(lv_arc_5, 0);
        lv_obj_set_align(lv_arc_5, LV_ALIGN_CENTER);
        lv_obj_set_width(lv_arc_5, 348);
        lv_obj_set_height(lv_arc_5, 348);
        lv_obj_set_flag(lv_arc_5, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(lv_arc_5, &style_thin_frame, 0);
        lv_obj_add_style(lv_arc_5, &style_no_indicator, LV_PART_INDICATOR);
        lv_obj_add_style(lv_arc_5, &style_knob, LV_PART_KNOB);

        lv_obj_t * lv_arc_6 = lv_arc_create(lv_obj_0);
        lv_arc_set_bg_start_angle(lv_arc_6, 230);
        lv_arc_set_bg_end_angle(lv_arc_6, 310);
        lv_arc_set_start_angle(lv_arc_6, 0);
        lv_arc_set_end_angle(lv_arc_6, 0);
        lv_obj_set_align(lv_arc_6, LV_ALIGN_CENTER);
        lv_obj_set_width(lv_arc_6, 340);
        lv_obj_set_height(lv_arc_6, 340);
        lv_obj_set_flag(lv_arc_6, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(lv_arc_6, &style_top_accent, 0);
        lv_obj_add_style(lv_arc_6, &style_no_indicator, LV_PART_INDICATOR);
        lv_obj_add_style(lv_arc_6, &style_knob, LV_PART_KNOB);

        lv_obj_t * lv_obj_1 = lv_obj_create(lv_obj_0);
        lv_obj_set_align(lv_obj_1, LV_ALIGN_TOP_MID);
        lv_obj_set_y(lv_obj_1, 24);
        lv_obj_add_style(lv_obj_1, &style_dot, 0);

        lv_obj_t * lv_arc_7 = lv_arc_create(lv_obj_0);
        lv_arc_set_bg_start_angle(lv_arc_7, 65);
        lv_arc_set_bg_end_angle(lv_arc_7, 115);
        lv_obj_set_align(lv_arc_7, LV_ALIGN_CENTER);
        lv_obj_set_width(lv_arc_7, 400);
        lv_obj_set_height(lv_arc_7, 400);
        lv_arc_set_min_value(lv_arc_7, 0);
        lv_arc_set_max_value(lv_arc_7, 250);
        lv_arc_bind_value(lv_arc_7, &turbo_boost);
        lv_obj_set_flag(lv_arc_7, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(lv_arc_7, &style_turbo_track, 0);
        lv_obj_add_style(lv_arc_7, &style_turbo_indicator, LV_PART_INDICATOR);
        lv_obj_add_style(lv_arc_7, &style_knob, LV_PART_KNOB);

        lv_obj_t * lv_label_0 = lv_label_create(lv_obj_0);
        lv_label_bind_text(lv_label_0, &speed_str, NULL);
        lv_obj_set_align(lv_label_0, LV_ALIGN_TOP_MID);
        lv_obj_set_y(lv_label_0, 52);
        lv_obj_set_style_text_font(lv_label_0, roboto_bold_40, 0);
        lv_obj_set_style_text_color(lv_label_0, lv_color_hex(0xffffff), 0);

        lv_obj_t * lv_label_1 = lv_label_create(lv_obj_0);
        lv_label_set_text(lv_label_1, "km/h");
        lv_obj_set_align(lv_label_1, LV_ALIGN_TOP_MID);
        lv_obj_set_y(lv_label_1, 92);
        lv_obj_set_style_text_font(lv_label_1, roboto_regular_24, 0);
        lv_obj_set_style_text_color(lv_label_1, lv_color_hex(0x7a8394), 0);

        lv_obj_t * lv_label_2 = lv_label_create(lv_obj_0);
        lv_label_bind_text(lv_label_2, &coolant_temp_str, NULL);
        lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);
        lv_obj_set_y(lv_label_2, -18);
        lv_obj_set_style_text_font(lv_label_2, roboto_bold_150, 0);
        lv_obj_set_style_text_color(lv_label_2, lv_color_hex(0xffffff), 0);
        lv_obj_set_x(lv_label_2, -6);

        lv_obj_t * lv_label_3 = lv_label_create(lv_obj_0);
        lv_label_set_text(lv_label_3, "°C");
        lv_obj_set_align(lv_label_3, LV_ALIGN_CENTER);
        lv_obj_set_x(lv_label_3, 86);
        lv_obj_set_y(lv_label_3, -64);
        lv_obj_set_style_text_font(lv_label_3, roboto_regular_24, 0);
        lv_obj_set_style_text_color(lv_label_3, lv_color_hex(0x7a8394), 0);

        lv_obj_t * lv_label_4 = lv_label_create(lv_obj_0);
        lv_label_bind_text(lv_label_4, &engine_rpm_str, NULL);
        lv_obj_set_align(lv_label_4, LV_ALIGN_TOP_MID);
        lv_obj_set_y(lv_label_4, 246);
        lv_obj_set_style_text_font(lv_label_4, roboto_bold_40, 0);
        lv_obj_set_style_text_color(lv_label_4, lv_color_hex(0xffffff), 0);

        lv_obj_t * lv_label_5 = lv_label_create(lv_obj_0);
        lv_label_set_text(lv_label_5, "Rpm");
        lv_obj_set_align(lv_label_5, LV_ALIGN_TOP_MID);
        lv_obj_set_y(lv_label_5, 282);
        lv_obj_set_style_text_font(lv_label_5, roboto_regular_24, 0);
        lv_obj_set_style_text_color(lv_label_5, lv_color_hex(0x7a8394), 0);

        lv_obj_t * lv_image_0 = lv_image_create(lv_obj_0);
        lv_image_set_src(lv_image_0, turbo);
        lv_obj_set_align(lv_image_0, LV_ALIGN_CENTER);
        lv_obj_set_x(lv_image_0, -68);
        lv_obj_set_y(lv_image_0, 120);

        lv_obj_t * lv_label_6 = lv_label_create(lv_obj_0);
        lv_label_bind_text(lv_label_6, &turbo_boost_str, NULL);
        lv_obj_set_align(lv_label_6, LV_ALIGN_CENTER);
        lv_obj_set_x(lv_label_6, -10);
        lv_obj_set_y(lv_label_6, 120);
        lv_obj_set_style_text_font(lv_label_6, roboto_regular_24, 0);
        lv_obj_set_style_text_color(lv_label_6, lv_color_hex(0xb84a22), 0);

        lv_obj_t * lv_label_7 = lv_label_create(lv_obj_0);
        lv_label_bind_text(lv_label_7, &intake_temp_str, NULL);
        lv_obj_set_align(lv_label_7, LV_ALIGN_CENTER);
        lv_obj_set_x(lv_label_7, 54);
        lv_obj_set_y(lv_label_7, 120);
        lv_obj_set_style_text_font(lv_label_7, roboto_regular_24, 0);
        lv_obj_set_style_text_color(lv_label_7, lv_color_hex(0x7a8394), 0);

        lv_obj_t * lv_label_8 = lv_label_create(lv_obj_0);
        lv_label_bind_text(lv_label_8, &g_force_str, NULL);
        lv_obj_set_align(lv_label_8, LV_ALIGN_CENTER);
        lv_obj_set_x(lv_label_8, -123);
        lv_obj_set_y(lv_label_8, -12);
        lv_obj_set_style_text_font(lv_label_8, roboto_regular_24, 0);
        lv_obj_set_style_text_color(lv_label_8, lv_color_hex(0xffffff), 0);

        lv_obj_t * lv_label_9 = lv_label_create(lv_obj_0);
        lv_label_bind_text(lv_label_9, &fuel_consumption_val_str, NULL);
        lv_obj_set_align(lv_label_9, LV_ALIGN_CENTER);
        lv_obj_set_x(lv_label_9, 118);
        lv_obj_set_y(lv_label_9, -10);
        lv_obj_set_style_text_font(lv_label_9, roboto_bold_40, 0);
        lv_obj_set_style_text_color(lv_label_9, lv_color_hex(0xffffff), 0);

        lv_obj_t * lv_label_10 = lv_label_create(lv_obj_0);
        lv_label_bind_text(lv_label_10, &fuel_consumption_unit_str, NULL);
        lv_obj_set_align(lv_label_10, LV_ALIGN_CENTER);
        lv_obj_set_x(lv_label_10, 118);
        lv_obj_set_y(lv_label_10, 24);
        lv_obj_set_style_text_font(lv_label_10, roboto_regular_24, 0);
        lv_obj_set_style_text_color(lv_label_10, lv_color_hex(0x7a8394), 0);

        lv_obj_t * lv_image_1 = lv_image_create(lv_obj_0);
        lv_image_set_src(lv_image_1, warning);
        lv_obj_set_align(lv_image_1, LV_ALIGN_TOP_LEFT);
        lv_obj_set_x(lv_image_1, 55);
        lv_obj_set_y(lv_image_1, 35);
        lv_obj_bind_flag_if_eq(lv_image_1, &can_error, LV_OBJ_FLAG_HIDDEN, 0);

        lv_obj_t * lv_image_2 = lv_image_create(lv_obj_0);
        lv_image_set_src(lv_image_2, disconnect);
        lv_obj_set_align(lv_image_2, LV_ALIGN_TOP_RIGHT);
        lv_obj_set_x(lv_image_2, -55);
        lv_obj_set_y(lv_image_2, 35);
        lv_obj_bind_flag_if_eq(lv_image_2, &con_error, LV_OBJ_FLAG_HIDDEN, 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

