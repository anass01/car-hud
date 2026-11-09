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

    static lv_style_t style_dashboard;
    static lv_style_t style_arc;
    static lv_style_t style_arc_indicator;
    static lv_style_t style_knob;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_dashboard);
        lv_style_init(&style_arc);
        lv_style_init(&style_arc_indicator);
        lv_style_init(&style_knob);

        lv_style_set_text_font(&style_dashboard, roboto_bold_40);
        lv_style_set_arc_width(&style_arc, 3);
        lv_style_set_arc_rounded(&style_arc, false);
        lv_style_set_arc_color(&style_arc, lv_color_hex(0xffffff));
        lv_style_set_arc_width(&style_arc_indicator, 30);
        lv_style_set_arc_rounded(&style_arc_indicator, false);
        lv_style_set_arc_color(&style_arc_indicator, lv_color_hex(0xffffff));
        lv_style_set_bg_opa(&style_knob, 0);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HUD_UI_CHECK_COMPILE_TARGET(HUD_UI_TARGET_ALL)
    if (hud_ui_check_target(HUD_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "dashboard_#");

        lv_obj_add_style(lv_obj_0, &style_dark, 0);
        lv_obj_add_style(lv_obj_0, &style_dashboard, 0);
        lv_obj_t * lv_arc_0 = lv_arc_create(lv_obj_0);
        lv_obj_set_align(lv_arc_0, LV_ALIGN_TOP_MID);
        lv_obj_set_width(lv_arc_0, 410);
        lv_obj_set_height(lv_arc_0, 410);
        lv_arc_set_min_value(lv_arc_0, 0);
        lv_arc_set_max_value(lv_arc_0, 5000);
        lv_arc_bind_value(lv_arc_0, &engine_rpm);
        lv_obj_set_flag(lv_arc_0, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(lv_arc_0, &style_arc, 0);
        lv_obj_add_style(lv_arc_0, &style_arc_indicator, LV_PART_INDICATOR);
        lv_obj_add_style(lv_arc_0, &style_knob, LV_PART_KNOB);

        lv_obj_t * lv_arc_1 = lv_arc_create(lv_obj_0);
        lv_arc_set_bg_start_angle(lv_arc_1, 55);
        lv_arc_set_bg_end_angle(lv_arc_1, 125);
        lv_obj_set_align(lv_arc_1, LV_ALIGN_TOP_MID);
        lv_obj_set_width(lv_arc_1, 410);
        lv_obj_set_height(lv_arc_1, 410);
        lv_arc_set_min_value(lv_arc_1, 0);
        lv_arc_set_max_value(lv_arc_1, 45);
        lv_arc_bind_value(lv_arc_1, &fuel_capacity);
        lv_obj_set_flag(lv_arc_1, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(lv_arc_1, &style_arc, 0);
        lv_obj_add_style(lv_arc_1, &style_arc_indicator, LV_PART_INDICATOR);
        lv_obj_add_style(lv_arc_1, &style_knob, LV_PART_KNOB);

        lv_obj_t * lv_arc_2 = lv_arc_create(lv_obj_0);
        lv_arc_set_bg_start_angle(lv_arc_2, 140);
        lv_arc_set_bg_end_angle(lv_arc_2, 40);
        lv_obj_set_align(lv_arc_2, LV_ALIGN_CENTER);
        lv_obj_set_width(lv_arc_2, 390);
        lv_obj_set_height(lv_arc_2, 390);
        lv_arc_set_min_value(lv_arc_2, 0);
        lv_arc_set_max_value(lv_arc_2, 250);
        lv_arc_bind_value(lv_arc_2, &turbo_boost);
        lv_obj_set_flag(lv_arc_2, LV_OBJ_FLAG_CLICKABLE, false);
        lv_obj_add_style(lv_arc_2, &style_arc, 0);
        lv_obj_add_style(lv_arc_2, &style_arc_indicator, LV_PART_INDICATOR);
        lv_obj_add_style(lv_arc_2, &style_knob, LV_PART_KNOB);

        lv_obj_t * lv_label_0 = lv_label_create(lv_obj_0);
        lv_label_bind_text(lv_label_0, &engine_rpm, NULL);
        lv_obj_set_align(lv_label_0, LV_ALIGN_TOP_MID);
        lv_obj_set_y(lv_label_0, 280);

        lv_obj_t * lv_label_1 = lv_label_create(lv_obj_0);
        lv_label_bind_text(lv_label_1, &speed, "%3dkm/h");
        lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);
        lv_obj_set_y(lv_label_1, 140);

        lv_obj_t * lv_label_2 = lv_label_create(lv_obj_0);
        lv_label_bind_text(lv_label_2, &turbo_boost_str, NULL);
        lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);
        lv_obj_set_y(lv_label_2, -150);

        lv_obj_t * lv_label_3 = lv_label_create(lv_obj_0);
        lv_label_bind_text(lv_label_3, &coolant_temp, "%2d°C");
        lv_obj_set_align(lv_label_3, LV_ALIGN_BOTTOM_MID);
        lv_obj_set_y(lv_label_3, -120);
        lv_obj_set_x(lv_label_3, 50);
        lv_obj_set_style_text_font(lv_label_3, roboto_bold_150, 0);

        lv_obj_t * lv_label_4 = lv_label_create(lv_obj_0);
        lv_label_bind_text(lv_label_4, &fuel_capacity, "%d L");
        lv_obj_set_align(lv_label_4, LV_ALIGN_BOTTOM_MID);
        lv_obj_set_y(lv_label_4, -10);

        lv_obj_t * lv_image_0 = lv_image_create(lv_obj_0);
        lv_image_set_src(lv_image_0, warning);
        lv_obj_set_align(lv_image_0, LV_ALIGN_LEFT_MID);
        lv_obj_set_x(lv_image_0, 50);
        lv_obj_bind_flag_if_eq(lv_image_0, &can_error, LV_OBJ_FLAG_HIDDEN, 0);

        lv_obj_t * lv_image_1 = lv_image_create(lv_obj_0);
        lv_image_set_src(lv_image_1, disconnect);
        lv_obj_set_align(lv_image_1, LV_ALIGN_RIGHT_MID);
        lv_obj_set_x(lv_image_1, -38);
        lv_obj_set_y(lv_image_1, -88);
        lv_obj_bind_flag_if_eq(lv_image_1, &con_error, LV_OBJ_FLAG_HIDDEN, 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

