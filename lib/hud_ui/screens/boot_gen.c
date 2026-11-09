/**
 * @file boot_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "boot_gen.h"
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

lv_obj_t * boot_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if HUD_UI_CHECK_COMPILE_TARGET(HUD_UI_TARGET_ALL)
    if (hud_ui_check_target(HUD_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "boot_#");

        lv_obj_add_style(lv_obj_0, &style_dark, 0);
        lv_obj_t * lv_image_0 = lv_image_create(lv_obj_0);
        lv_image_set_src(lv_image_0, subaru_logo);
        lv_obj_set_align(lv_image_0, LV_ALIGN_CENTER);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

