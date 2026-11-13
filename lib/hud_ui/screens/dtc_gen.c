/**
 * @file dtc_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "dtc_gen.h"
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

lv_obj_t * dtc_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_btn_back;
    static lv_style_t style_engine_badge;
    static lv_style_t style_list_cont;
    static lv_style_t style_dtc_card;
    static lv_style_t style_btn_row;
    static lv_style_t style_btn_read;
    static lv_style_t style_btn_clear;
    static lv_style_t style_pressed;

    static bool style_inited = false;

    if (!style_inited) {
        /*Init all styles*/
        lv_style_init(&style_btn_back);
        lv_style_init(&style_engine_badge);
        lv_style_init(&style_list_cont);
        lv_style_init(&style_dtc_card);
        lv_style_init(&style_btn_row);
        lv_style_init(&style_btn_read);
        lv_style_init(&style_btn_clear);
        lv_style_init(&style_pressed);

        lv_style_set_width(&style_btn_back, 74);
        lv_style_set_height(&style_btn_back, 34);
        lv_style_set_radius(&style_btn_back, 17);
        lv_style_set_bg_color(&style_btn_back, lv_color_hex(0x272c38));
        lv_style_set_border_width(&style_btn_back, 1);
        lv_style_set_border_color(&style_btn_back, lv_color_hex(0x404859));
        lv_style_set_width(&style_engine_badge, 310);
        lv_style_set_height(&style_engine_badge, 26);
        lv_style_set_radius(&style_engine_badge, 13);
        lv_style_set_pad_all(&style_engine_badge, 0);
        lv_style_set_bg_color(&style_engine_badge, lv_color_hex(0x102b1c));
        lv_style_set_border_width(&style_engine_badge, 1);
        lv_style_set_border_color(&style_engine_badge, lv_color_hex(0x27ae60));
        lv_style_set_width(&style_list_cont, 340);
        lv_style_set_height(&style_list_cont, 195);
        lv_style_set_radius(&style_list_cont, 12);
        lv_style_set_bg_color(&style_list_cont, lv_color_hex(0x13171e));
        lv_style_set_border_width(&style_list_cont, 1);
        lv_style_set_border_color(&style_list_cont, lv_color_hex(0x272d3b));
        lv_style_set_pad_all(&style_list_cont, 8);
        lv_style_set_layout(&style_list_cont, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_list_cont, LV_FLEX_FLOW_COLUMN);
        lv_style_set_width(&style_dtc_card, 322);
        lv_style_set_height(&style_dtc_card, 62);
        lv_style_set_radius(&style_dtc_card, 8);
        lv_style_set_bg_color(&style_dtc_card, lv_color_hex(0x1c212c));
        lv_style_set_border_width(&style_dtc_card, 1);
        lv_style_set_border_color(&style_dtc_card, lv_color_hex(0x384155));
        lv_style_set_pad_all(&style_dtc_card, 6);
        lv_style_set_width(&style_btn_row, 340);
        lv_style_set_height(&style_btn_row, 48);
        lv_style_set_bg_opa(&style_btn_row, 0);
        lv_style_set_border_width(&style_btn_row, 0);
        lv_style_set_pad_all(&style_btn_row, 0);
        lv_style_set_layout(&style_btn_row, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&style_btn_row, LV_FLEX_FLOW_ROW);
        lv_style_set_pad_column(&style_btn_row, 14);
        lv_style_set_width(&style_btn_read, 100);
        lv_style_set_height(&style_btn_read, 40);
        lv_style_set_radius(&style_btn_read, 20);
        lv_style_set_bg_color(&style_btn_read, lv_color_hex(0x24426b));
        lv_style_set_border_width(&style_btn_read, 1);
        lv_style_set_border_color(&style_btn_read, lv_color_hex(0x35629e));
        lv_style_set_width(&style_btn_clear, 100);
        lv_style_set_height(&style_btn_clear, 40);
        lv_style_set_radius(&style_btn_clear, 20);
        lv_style_set_bg_color(&style_btn_clear, lv_color_hex(0x8a2318));
        lv_style_set_border_width(&style_btn_clear, 1);
        lv_style_set_border_color(&style_btn_clear, lv_color_hex(0xb8382a));
        lv_style_set_bg_color(&style_pressed, lv_color_hex(0xffffff));
        lv_style_set_bg_opa(&style_pressed, 100);
        lv_style_set_radius(&style_pressed, 0);

        style_inited = true;
    }


    lv_obj_t * the_root = NULL;

    #if HUD_UI_CHECK_COMPILE_TARGET(HUD_UI_TARGET_ALL)
    if (hud_ui_check_target(HUD_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "dtc_#");

        lv_obj_add_style(lv_obj_0, &style_dark, 0);
        lv_obj_t * dtc_back = lv_button_create(lv_obj_0);
        lv_obj_set_name(dtc_back, "dtc_back");
        lv_obj_set_x(dtc_back, 165);
        lv_obj_set_y(dtc_back, 5);
        lv_obj_add_style(dtc_back, &style_btn_back, 0);
        lv_obj_t * lv_label_0 = lv_label_create(dtc_back);
        lv_label_set_text(lv_label_0, "Back");
        lv_obj_set_align(lv_label_0, LV_ALIGN_CENTER);

        lv_obj_t * dtc_engine_badge = lv_obj_create(lv_obj_0);
        lv_obj_set_name(dtc_engine_badge, "dtc_engine_badge");
        lv_obj_set_align(dtc_engine_badge, LV_ALIGN_TOP_MID);
        lv_obj_set_y(dtc_engine_badge, 58);
        lv_obj_set_scrollbar_mode(dtc_engine_badge, LV_SCROLLBAR_MODE_OFF);
        lv_obj_add_style(dtc_engine_badge, &style_engine_badge, 0);
        lv_obj_t * dtc_engine_label = lv_label_create(dtc_engine_badge);
        lv_obj_set_name(dtc_engine_label, "dtc_engine_label");
        lv_label_set_text(dtc_engine_label, "Engine OFF (Clear Allowed)");
        lv_obj_set_align(dtc_engine_label, LV_ALIGN_CENTER);
        lv_obj_set_style_text_color(dtc_engine_label, lv_color_hex(0x2ecc71), 0);

        lv_obj_t * dtc_list_cont = lv_obj_create(lv_obj_0);
        lv_obj_set_name(dtc_list_cont, "dtc_list_cont");
        lv_obj_set_align(dtc_list_cont, LV_ALIGN_TOP_MID);
        lv_obj_set_y(dtc_list_cont, 88);
        lv_obj_set_scrollbar_mode(dtc_list_cont, LV_SCROLLBAR_MODE_AUTO);
        lv_obj_add_style(dtc_list_cont, &style_list_cont, 0);
        lv_obj_t * dtc_card_sample1 = lv_obj_create(dtc_list_cont);
        lv_obj_set_name(dtc_card_sample1, "dtc_card_sample1");
        lv_obj_set_scrollbar_mode(dtc_card_sample1, LV_SCROLLBAR_MODE_OFF);
        lv_obj_add_style(dtc_card_sample1, &style_dtc_card, 0);
        lv_obj_t * dtc_code_sample1 = lv_label_create(dtc_card_sample1);
        lv_obj_set_name(dtc_code_sample1, "dtc_code_sample1");
        lv_label_set_text(dtc_code_sample1, "P0101");
        lv_obj_set_x(dtc_code_sample1, 4);
        lv_obj_set_y(dtc_code_sample1, 0);
        lv_obj_set_style_text_font(dtc_code_sample1, roboto_regular_24, 0);
        lv_obj_set_style_text_color(dtc_code_sample1, lv_color_hex(0xf39c12), 0);

        lv_obj_t * dtc_desc_sample1 = lv_label_create(dtc_card_sample1);
        lv_obj_set_name(dtc_desc_sample1, "dtc_desc_sample1");
        lv_label_set_text(dtc_desc_sample1, "MAF Sensor Range / Performance");
        lv_obj_set_x(dtc_desc_sample1, 4);
        lv_obj_set_y(dtc_desc_sample1, 30);
        lv_obj_set_style_text_color(dtc_desc_sample1, lv_color_hex(0xdfe4ea), 0);

        lv_obj_t * dtc_card_sample2 = lv_obj_create(dtc_list_cont);
        lv_obj_set_name(dtc_card_sample2, "dtc_card_sample2");
        lv_obj_set_scrollbar_mode(dtc_card_sample2, LV_SCROLLBAR_MODE_OFF);
        lv_obj_add_style(dtc_card_sample2, &style_dtc_card, 0);
        lv_obj_t * dtc_code_sample2 = lv_label_create(dtc_card_sample2);
        lv_obj_set_name(dtc_code_sample2, "dtc_code_sample2");
        lv_label_set_text(dtc_code_sample2, "P0234");
        lv_obj_set_x(dtc_code_sample2, 4);
        lv_obj_set_y(dtc_code_sample2, 0);
        lv_obj_set_style_text_font(dtc_code_sample2, roboto_regular_24, 0);
        lv_obj_set_style_text_color(dtc_code_sample2, lv_color_hex(0xf39c12), 0);

        lv_obj_t * dtc_desc_sample2 = lv_label_create(dtc_card_sample2);
        lv_obj_set_name(dtc_desc_sample2, "dtc_desc_sample2");
        lv_label_set_text(dtc_desc_sample2, "Turbocharger Engine Overboost");
        lv_obj_set_x(dtc_desc_sample2, 4);
        lv_obj_set_y(dtc_desc_sample2, 30);
        lv_obj_set_style_text_color(dtc_desc_sample2, lv_color_hex(0xdfe4ea), 0);

        lv_obj_t * dtc_status = lv_label_create(lv_obj_0);
        lv_obj_set_name(dtc_status, "dtc_status");
        lv_label_set_text(dtc_status, "Tap 'Read' to query OBD");
        lv_obj_set_align(dtc_status, LV_ALIGN_TOP_MID);
        lv_obj_set_y(dtc_status, 287);
        lv_obj_set_width(dtc_status, 360);
        lv_obj_set_style_text_align(dtc_status, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(dtc_status, lv_color_hex(0x8f9bb3), 0);

        lv_obj_t * dtc_btn_row = lv_obj_create(lv_obj_0);
        lv_obj_set_name(dtc_btn_row, "dtc_btn_row");
        lv_obj_set_align(dtc_btn_row, LV_ALIGN_TOP_MID);
        lv_obj_set_y(dtc_btn_row, 330);
        lv_obj_set_x(dtc_btn_row, 2);
        lv_obj_set_scrollbar_mode(dtc_btn_row, LV_SCROLLBAR_MODE_OFF);
        lv_obj_set_width(dtc_btn_row, 216);
        lv_obj_set_height(dtc_btn_row, 50);
        lv_obj_add_style(dtc_btn_row, &style_btn_row, 0);
        lv_obj_t * dtc_btn_read = lv_button_create(dtc_btn_row);
        lv_obj_set_name(dtc_btn_read, "dtc_btn_read");
        lv_obj_add_style(dtc_btn_read, &style_btn_read, 0);
        lv_obj_t * lv_label_1 = lv_label_create(dtc_btn_read);
        lv_label_set_text(lv_label_1, "Read");
        lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);
        lv_obj_set_style_text_color(lv_label_1, lv_color_hex(0xffffff), 0);

        lv_obj_t * dtc_btn_clear = lv_button_create(dtc_btn_row);
        lv_obj_set_name(dtc_btn_clear, "dtc_btn_clear");
        lv_obj_add_style(dtc_btn_clear, &style_btn_clear, 0);
        lv_obj_t * dtc_clear_label = lv_label_create(dtc_btn_clear);
        lv_obj_set_name(dtc_clear_label, "dtc_clear_label");
        lv_label_set_text(dtc_clear_label, "Clear");
        lv_obj_set_align(dtc_clear_label, LV_ALIGN_CENTER);
        lv_obj_set_style_text_color(dtc_clear_label, lv_color_hex(0xffffff), 0);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

