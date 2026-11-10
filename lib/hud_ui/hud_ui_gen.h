/**
 * @file hud_ui_gen.h
 */

#ifndef LVGL_PRO_HUD_UI_GEN_H
#define LVGL_PRO_HUD_UI_GEN_H

#ifndef UI_SUBJECT_STRING_LENGTH
#define UI_SUBJECT_STRING_LENGTH 256
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif



/* Prototypes for target functions, needed by responsive const definitions */

void hud_ui_set_target(uint32_t target);
uint32_t hud_ui_get_target(void);
bool hud_ui_check_target(uint32_t target);

/*********************
 *      DEFINES
 *********************/

#define HUD_UI_TARGET_UNDEFINED       (0 << 1)
#define HUD_UI_TARGET_WAVESHARE_1_46  (1 << 1)
#define HUD_UI_TARGET_TARGET1         (1 << 2)
#define HUD_UI_TARGET_ALL             0x0FFFFFFF

/* By default compile for all targets, allowing to switch to any targets at runtime */
#ifndef HUD_UI_COMPILE_TARGET
#define HUD_UI_COMPILE_TARGET HUD_UI_TARGET_ALL
#endif

#define HUD_UI_CHECK_COMPILE_TARGET(target) (HUD_UI_COMPILE_TARGET & (target) ? 1 : 0)

#ifndef LV_XML_EVAL_STRING_BUF_SIZE
    #define LV_XML_EVAL_STRING_BUF_SIZE 256
#endif

/**
 * Animation path of a style transition with a `bezier(…)` easing. The control
 * points come from the transition descriptor's `user_data`, as a transition
 * descriptor cannot store animation parameters itself.
 */
static inline int32_t hud_ui_style_transition_bezier_path_helper(const lv_anim_t * a)
{
    const lv_anim_bezier3_para_t * para = (const lv_anim_bezier3_para_t *)lv_anim_get_user_data(a);
    if(para == NULL) return lv_anim_path_linear(a);

    lv_anim_t a_tmp = *a;
    lv_anim_set_bezier3_param(&a_tmp, para->x1, para->y1, para->x2, para->y2);
    return lv_anim_path_custom_bezier3(&a_tmp);
}

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL VARIABLES
 **********************/

/*-------------------
 * Permanent screens
 *------------------*/

/*----------------
 * Global styles
 *----------------*/

extern lv_style_t style_dark;

/*----------------
 * Fonts
 *----------------*/

/* Targets: any */
extern lv_font_t * roboto_regular_24;
extern lv_font_t * roboto_bold_40;
extern lv_font_t * roboto_bold_150;


/*----------------
 * Images
 *----------------*/

/* Targets: any */
extern const void * rotation;
extern const void * brightness;
extern const void * hud;
extern const void * warning;
extern const void * disconnect;
extern const void * back;
extern const void * restart;
extern const void * turbo;

/*----------------
 * Subjects
 *----------------*/

extern lv_subject_t engine_rpm;
extern lv_subject_t engine_rpm_str;
extern lv_subject_t coolant_temp;
extern lv_subject_t coolant_temp_str;
extern lv_subject_t speed;
extern lv_subject_t speed_str;
extern lv_subject_t fuel_consumption;
extern lv_subject_t fuel_consumption_str;
extern lv_subject_t fuel_consumption_val_str;
extern lv_subject_t fuel_consumption_unit_str;
extern lv_subject_t turbo_boost;
extern lv_subject_t turbo_boost_str;
extern lv_subject_t intake_temp;
extern lv_subject_t intake_temp_str;
extern lv_subject_t g_force_str;
extern lv_subject_t can_error;
extern lv_subject_t con_error;
extern lv_subject_t settings_brightness;
extern lv_subject_t settings_rotation;
extern lv_subject_t settings_hud;
extern lv_subject_t settings_restart;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*----------------
 * Event Callbacks
 *----------------*/

/*----------------
 * Easing Callbacks
 *----------------*/

/**
 * Initialize the component library
 */

void hud_ui_init_gen(const char * asset_path);

/**********************
 *      MACROS
 **********************/

/**********************
 *   POST INCLUDES
 **********************/

/*Include all the widgets, components and screens of this library*/
#include "components/settings_item/settings_item_gen.h"
#include "screens/dashboard_gen.h"
#include "screens/settings_gen.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_HUD_UI_GEN_H*/