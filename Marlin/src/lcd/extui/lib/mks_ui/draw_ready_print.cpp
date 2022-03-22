/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#include "../../../../inc/MarlinConfigPre.h"

#if HAS_TFT_LVGL_UI

#include "draw_ready_print.h"
#include "draw_tool.h"
#include <lv_conf.h>
#include "tft_lvgl_configuration.h"
#include "mks_hardware_test.h"
#include "draw_ui.h"

#include <lvgl.h>

#include "../../../../module/temperature.h"
#include "../../../../inc/MarlinConfig.h"

#if ENABLED(TOUCH_SCREEN_CALIBRATION)
  #include "../../../tft_io/touch_calibration.h"
  #include "draw_touch_calibration.h"
#endif

#include <stdio.h>
#include "../../../../gcode/queue.h"


#define ICON_POS_Y          260
#define TARGET_LABEL_MOD_Y -36
#define LABEL_MOD_Y         30

extern lv_group_t*  g;
static lv_obj_t *scr;
static lv_obj_t *buttonExt1, *labelExt1, *buttonFanstate, *labelFan;
static lv_obj_t *buttonPresetState1, *labelPresetState1;
static lv_obj_t *buttonPresetState2, *labelPresetState2;

static lv_obj_t *buttonCooldown, *labelCooldown;
// static lv_obj_t *label_emStop;

#if HAS_MULTI_EXTRUDER && DISABLED(SINGLENOZZLE)
  static lv_obj_t *labelExt2;
  static lv_obj_t *buttonExt2;
#endif

#if HAS_HEATED_BED
  static lv_obj_t* labelBed;
  static lv_obj_t* buttonBedstate;
#endif

#if ENABLED(MKS_TEST)
  uint8_t curent_disp_ui = 0;
#endif

enum {
  ID_TOOL = 1,
  ID_SET,
  ID_PRINT,
  ID_INFO_EXT,
  ID_INFO_FAN,
  ID_INFO_BED,
  ID_P_ABS,
  ID_P_PLA,
  ID_COOLDOWN,
  ID_EMSTOP,
};
static void event_handler(lv_obj_t *obj, lv_event_t event) {
  if (event != LV_EVENT_RELEASED) return;

  // if(obj->mks_obj_id != ID_P_ABS && obj->mks_obj_id != ID_P_PLA ){
  //   lv_clear_ready_print();
  // }
  switch (obj->mks_obj_id){
    case ID_TOOL ... ID_INFO_BED:
      lv_clear_ready_print();
    break;
  }

  switch (obj->mks_obj_id) {
    case ID_TOOL:   lv_draw_tool(); break;
    case ID_SET:    lv_draw_set(); break;
    case ID_PRINT: TERN(MULTI_VOLUME, lv_draw_media_select(), lv_draw_print_file()); break;
    case ID_INFO_EXT:  uiCfg.curTempType = 0; lv_draw_preHeat(); break;
    case ID_INFO_FAN:  lv_draw_fan(); break;
    case ID_INFO_BED:  uiCfg.curTempType = 1; lv_draw_preHeat(); break;
    case ID_P_ABS:
      thermalManager.setTargetHotend(PREHEAT_2_TEMP_HOTEND, 0);
      thermalManager.setTargetBed(PREHEAT_2_TEMP_BED);
    break;

    case ID_P_PLA:

      thermalManager.setTargetHotend(PREHEAT_1_TEMP_HOTEND, 0);
      thermalManager.setTargetBed(PREHEAT_1_TEMP_BED);
    break;

    case ID_COOLDOWN:
    queue.inject_P(PSTR("M106 S0\nM104 S0\nM140 S0\n"));
    break;
    case ID_EMSTOP:
    queue.inject_P(PSTR("M112\n"));
    break;
  }
}

lv_obj_t *limit_info, *det_info;
lv_obj_t *tmc_state_info;
lv_style_t limit_style, det_style, tmc_state_style;
void disp_Limit_ok() {
  limit_style.text.color.full = 0xFFFF;
  lv_obj_set_style(limit_info, &limit_style);
  lv_label_set_text(limit_info, "Limit:ok");
}
void disp_Limit_error() {
  limit_style.text.color = lv_color_make(0xFB, 0x33, 0x14);
  lv_obj_set_style(limit_info, &limit_style);
  lv_label_set_text(limit_info, "Limit:error");
}

void disp_det_ok() {
  det_style.text.color.full = 0xFFFF;
  lv_obj_set_style(det_info, &det_style);
  lv_label_set_text(det_info, "det:ok");
}
void disp_det_error() {
  det_style.text.color = lv_color_make(0xFB, 0x33, 0x14);
  lv_obj_set_style(det_info, &det_style);
  lv_label_set_text(det_info, "det:error");
}

void disp_tmc_ok() {
  tmc_state_style.text.color.full = 0xFFFF;
  lv_obj_set_style(tmc_state_info, &tmc_state_style);
  lv_label_set_text(tmc_state_info, "TMC CONNECTION OK");
}
void disp_tmc_error() {
  tmc_state_style.text.color = lv_color_make(0xFB, 0x33, 0x14);
  lv_obj_set_style(tmc_state_info, &tmc_state_style);
  lv_label_set_text(tmc_state_info, "TMC CONNECTION ERROR");
}

lv_obj_t *e1, *e2, *e3, *bed;
void mks_disp_test() {
  #if HAS_HOTEND
    char buf[30] = {0};
    sprintf_P(buf, PSTR("e1:%d"), (int)thermalManager.temp_hotend[0].celsius);
    lv_label_set_text(e1, buf);
  #endif
  #if HAS_MULTI_HOTEND
    sprintf_P(buf, PSTR("e2:%d"), (int)thermalManager.temp_hotend[1].celsius);
    lv_label_set_text(e2, buf);
  #endif
  #if HAS_HEATED_BED
    sprintf_P(buf, PSTR("bed:%d"), (int)thermalManager.temp_bed.celsius);
    lv_label_set_text(bed, buf);
  #endif
}

void lv_draw_ready_print(void) {
  #if HAS_HOTEND
    char buf[30] = {0};
  #endif
  lv_obj_t *buttonTool;

  disp_state_stack._disp_index = 0;

  ZERO(disp_state_stack._disp_state);

  if (mks_test_flag == 0x1E) {
    scr = lv_screen_create(MAIN_UI, "");

    // Create image buttons
    buttonTool = lv_imgbtn_create(scr, "F:/bmp_tool.bin", event_handler, ID_TOOL);

    lv_obj_set_pos(buttonTool, 360, 180);

    lv_obj_t *label_tool = lv_label_create_empty(buttonTool);
    if (gCfgItems.multiple_language) {
      lv_label_set_text(label_tool, main_menu.tool);
      lv_obj_align(label_tool, buttonTool, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);
    }

    #if 1
      e1 = lv_label_create_empty(scr);
      lv_obj_set_pos(e1, 20, 20);
      #if HAS_HOTEND
        sprintf_P(buf, PSTR("e1:  %d"), (int)thermalManager.temp_hotend[0].celsius);
        lv_label_set_text(e1, buf);
      #endif
      #if HAS_MULTI_HOTEND
        e2 = lv_label_create_empty(scr);
        lv_obj_set_pos(e2, 20, 45);
        sprintf_P(buf, PSTR("e2:  %d"), (int)thermalManager.temp_hotend[1].celsius);
        lv_label_set_text(e2, buf);
      #endif

      #if HAS_HEATED_BED
        bed = lv_label_create_empty(scr);
        lv_obj_set_pos(bed, 20, 95);
        sprintf_P(buf, PSTR("bed:  %d"), (int)thermalManager.temp_bed.celsius);
        lv_label_set_text(bed, buf);
      #endif

      limit_info = lv_label_create_empty(scr);

      lv_style_copy(&limit_style, &lv_style_scr);
      limit_style.body.main_color.full = 0X0000;
      limit_style.body.grad_color.full = 0X0000;
      limit_style.text.color.full      = 0Xffff;
      lv_obj_set_style(limit_info, &limit_style);

      lv_obj_set_pos(limit_info, 20, 120);
      lv_label_set_text(limit_info, " ");

      det_info = lv_label_create_empty(scr);

      lv_style_copy(&det_style, &lv_style_scr);
      det_style.body.main_color.full = 0X0000;
      det_style.body.grad_color.full = 0X0000;
      det_style.text.color.full      = 0Xffff;
      lv_obj_set_style(det_info, &det_style);

      lv_obj_set_pos(det_info, 20, 145);
      lv_label_set_text(det_info, " ");

      tmc_state_info = lv_label_create_empty(scr);

      lv_style_copy(&tmc_state_style, &lv_style_scr);
      tmc_state_style.body.main_color.full = 0X0000;
      tmc_state_style.body.grad_color.full = 0X0000;
      tmc_state_style.text.color.full      = 0Xffff;
      lv_obj_set_style(tmc_state_info, &tmc_state_style);

      lv_obj_set_pos(tmc_state_info, 20, 170);
      lv_label_set_text(tmc_state_info, " ");
    #endif // if 1

  }
  else {
  scr = lv_screen_create(PRINT_READY_UI);
    lv_big_button_create(scr, "F:/bmp_tool.bin", main_menu.tool, 20, 60, event_handler, ID_TOOL);
    lv_big_button_create(scr, "F:/bmp_set.bin", main_menu.set, 180, 60, event_handler, ID_SET);
    lv_big_button_create(scr, "F:/bmp_printing.bin", main_menu.print, 340, 60, event_handler, ID_PRINT);

    // Monitoring
    #if HAS_HOTEND
      buttonExt1 = lv_big_button_create(scr, "F:/bmp_ext1_state.bin", " ",20, 210, event_handler, ID_INFO_EXT);
    #endif
    // #if HAS_MULTI_HOTEND
    //   buttonExt2 = lv_big_button_create(scr, "F:/bmp_ext2_state.bin", " ", 180, ICON_POS_Y, event_handler, ID_INFO_EXT);
    // #endif
    #if HAS_HEATED_BED
      buttonBedstate = lv_big_button_create(scr, "F:/bmp_bed_state.bin", " ", 20, 260, event_handler, ID_INFO_BED);
    #endif
    #if HAS_FAN
      buttonFanstate = lv_big_button_create(scr, "F:/bmp_fan_state.bin", " ", 320, 260, event_handler, ID_INFO_FAN);
    #endif

      buttonPresetState1= lv_big_button_create(scr, "F:/bmp_temp_mini.bin"," ", 180, 210, event_handler, ID_P_PLA);
      buttonPresetState2= lv_big_button_create(scr, "F:/bmp_temp_mini.bin"," ", 180, 260, event_handler, ID_P_ABS);

      buttonCooldown= lv_big_button_create(scr, "F:/bmp_mini_temp0.bin"," ", 320, 210, event_handler, ID_COOLDOWN);

    // TERN_(HAS_HOTEND, labelExt1 = lv_label_create_empty(scr));
    // TERN_(HAS_MULTI_HOTEND, labelExt2 = lv_label_create_empty(scr));
    // TERN_(HAS_HEATED_BED, labelBed = lv_label_create_empty(scr));
    // TERN_(HAS_FAN, labelFan = lv_label_create_empty(scr));
    labelExt1     = lv_label_create_empty(scr);
    labelBed      = lv_label_create_empty(scr);
    labelFan      = lv_label_create_empty(scr);
    // labelCooldown = lv_label_create_empty(scr);
    // labelPresetState1 = lv_label_create_empty(scr);
    // labelPresetState2 = lv_label_create_empty(scr);

    disp_ext_heart_ready_print();
    lv_temp_refr();
  }

  #if ENABLED(TOUCH_SCREEN_CALIBRATION)
    // If calibration is required, let's trigger it now, handles the case when there is default value in configuration files
    if (!touch_calibration.calibration_loaded()) {
      lv_clear_ready_print();
      lv_draw_touch_calibration_screen();
    }
  #endif
}



void lv_temp_refr(){
  #if HAS_HOTEND
    sprintf_P(public_buf_l, printing_menu.temp1, thermalManager.wholeDegHotend(0), thermalManager.degTargetHotend(0));
    lv_label_set_text(labelExt1, public_buf_l);
    lv_obj_align(labelExt1, buttonExt1, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
  #endif
  #if HAS_MULTI_HOTEND
    sprintf_P(public_buf_l, printing_menu.temp1, thermalManager.wholeDegHotend(1), thermalManager.degTargetHotend(1));
    lv_label_set_text(labelExt2, public_buf_l);
    lv_obj_align(labelExt2, buttonExt2, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
  #endif
  #if HAS_HEATED_BED
    sprintf_P(public_buf_l, printing_menu.bed_temp, thermalManager.wholeDegBed(), thermalManager.degTargetBed());
    lv_label_set_text(labelBed, public_buf_l);
    lv_obj_align(labelBed, buttonBedstate, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
  #endif
  #if HAS_FAN
    sprintf_P(public_buf_l, PSTR("%d%%"), (int)thermalManager.fanSpeedPercent(0));
    lv_label_set_text(labelFan, public_buf_l);
    lv_obj_align(labelFan, buttonFanstate, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
  #endif
}

void lv_clear_ready_print() {
  #if HAS_ROTARY_ENCODER
    if (gCfgItems.encoder_enable) lv_group_remove_all_objs(g);
  #endif
  lv_obj_del(scr);
}

void disp_ext_heart_ready_print() {



    labelPresetState1  = lv_label_create(scr, 230, 220, PREHEAT_1_LABEL);
    labelPresetState2  = lv_label_create(scr, 230, 270, PREHEAT_2_LABEL);
    labelCooldown = lv_label_create(scr, 370, 220, "Cool Down");

    // lv_label_set_text(labelPresetState1, PREHEAT_1_LABEL);
    // lv_obj_align(labelPresetState1, buttonPresetState1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
    // lv_label_set_text(labelPresetState2, PREHEAT_2_LABEL);
    // lv_obj_align(labelPresetState1, buttonPresetState2, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

    // lv_label_set_text(labelCooldown, "Cool Down");
    // lv_obj_align(labelCooldown, buttonCooldown, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

}

#endif // HAS_TFT_LVGL_UI
