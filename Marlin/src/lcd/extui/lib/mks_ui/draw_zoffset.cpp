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

#if HAS_TFT_LVGL_UI && ANY(MESH_BED_LEVELING, AUTO_BED_LEVELING_BILINEAR)

#include "../../../../MarlinCore.h"
#include "draw_ui.h"

#include "../../../../module/temperature.h"
#include "../../../../gcode/queue.h"
#include "../../../../gcode/gcode.h"
#include "../../../../module/probe.h"

#include "../../../../feature/bedlevel/bedlevel.h"

#if ENABLED(EEPROM_SETTINGS)
  #include "../../../../module/settings.h"
#endif

#if ENABLED(AUTO_BED_LEVELING_BILINEAR)
  extern bed_mesh_t z_values;
#endif

extern lv_group_t *g;
static lv_obj_t *scr, *labelV, *buttonV, *zOffsetText;
// #ifdef MESH_BED_LEVELING
// #endif
  static lv_obj_t *labelExt1, *labelBed;
  static lv_obj_t *buttonExt1, *buttonBed;

static lv_obj_t *buttonNext, *buttonSave, *buttonBack;

static float step_dist = 0.01;
static float zoffset_diff = 0;

static bool saved = false;

static uint8_t manual_probe_index=0;
constexpr uint8_t total_probe_points = TERN(AUTO_BED_LEVELING_3POINT, 3, GRID_MAX_POINTS);

enum {
  ID_ZOFFSET_INIT = 1,
  ID_ZOFFSET_ZOFFSETPOS,
  ID_ZOFFSET_ZOFFSETNEG,
  ID_ZOFFSET_SAVE,
  ID_ZOFFSET_NEXT,
  ID_ZOFFSET_STEPS,
  ID_ZOFFSET_RETURN
 };

static void event_handler(lv_obj_t * obj, lv_event_t event) {
  if (event != LV_EVENT_RELEASED) return;
  char baby_buf[30] = { 0 };
  char str_1[40];
  switch (obj->mks_obj_id) {
    case ID_ZOFFSET_INIT:
      lv_obj_set_hidden( buttonNext, false );
      lv_obj_set_hidden( buttonSave, true );
      // lv_obj_set_hidden( buttonBack, true );
      zoffset_do_init(true);
      break;
    case ID_ZOFFSET_ZOFFSETPOS:
      #if ENABLED(AUTO_BED_LEVELING_BILINEAR)
        sprintf_P(baby_buf, PSTR("M290 Z%s"), dtostrf(step_dist, 1, 3, str_1));
      #else
        sprintf_P(baby_buf, PSTR("G1 Z%s"), dtostrf(current_position.z + step_dist, 1, 3, str_1));
      #endif
      gcode.process_subcommands_now_P(PSTR(baby_buf));

      zoffset_diff += step_dist;
      break;
    case ID_ZOFFSET_ZOFFSETNEG:
      #if ENABLED(AUTO_BED_LEVELING_BILINEAR)
        sprintf_P(baby_buf, PSTR("M290 Z%s"), dtostrf(-step_dist, 1, 3, str_1));
      #else
        sprintf_P(baby_buf, PSTR("G1 Z%s"), dtostrf(current_position.z - step_dist, 1, 3, str_1));
      #endif
      gcode.process_subcommands_now_P(PSTR(baby_buf));
      zoffset_diff -= step_dist;
      break;
    case ID_ZOFFSET_SAVE:
      if (!queue.ring_buffer.full(2)) {
        #if ENABLED(AUTO_BED_LEVELING_BILINEAR) && DISABLED(Z_MIN_PROBE_USES_Z_MIN_ENDSTOP_PIN)
          for (uint8_t x = 0; x < GRID_MAX_POINTS_X; x++)
            for (uint8_t y = 0; y < GRID_MAX_POINTS_Y; y++)
              z_values[x][y] = z_values[x][y] + zoffset_diff;
        #endif
        // #ifdef AUTO_BED_LEVELING_BILINEAR
        //   sprintf_P(baby_buf, PSTR("M851Z%s\nM420S1"), dtostrf(current_position.z, 1, 3, str_1));
        //   queue.enqueue_now_P(PSTR(baby_buf));
        // #endif
        TERN_(EEPROM_SETTINGS, (void)settings.save());
        queue.enqueue_now_P(PSTR("G28XY"));
        saved= true;
        manual_probe_index=0;
        zoffset_diff = 0;
      }
      break;
    case ID_ZOFFSET_NEXT:
      // sprintf_P(str_1, PSTR("G28\nG1 Z10 F2400\nG1 X%d Y%d\nG0 Z0.3"), X_MAX_POS / 2, Y_MAX_POS / 2);
      queue.enqueue_one_now(PSTR("G29 S2"));
      if(++manual_probe_index  >= total_probe_points){
        zoffset_diff = 0;
        lv_obj_set_hidden( buttonNext, true );
        lv_obj_set_hidden( buttonSave, false );
        // lv_obj_set_hidden( buttonBack, false );
        queue.clear();
      }
      //   if (!queue.ring_buffer.length) {
      //  // mesh_bed_leveling::set_zigzag_z(manual_probe_index, current_position.z + zoffset_diff);

      //   }
      break;
    case ID_ZOFFSET_STEPS:
      if (abs((int)(100 * step_dist)) == 1)
        step_dist = 0.05;
      else if (abs((int)(100 * step_dist)) == 5)
        step_dist = 0.1;
      else
        step_dist = 0.01;
      disp_step_dist();
      break;
    case ID_ZOFFSET_RETURN:
      TERN_(HAS_SOFTWARE_ENDSTOPS, soft_endstop._enabled = true);
      // lv_clear_zoffset_settings();
      // #ifdef MESH_BED_LEVELING
      //   lv_draw_manualLevel();
      // #else
      //   lv_draw_return_ui();
      // #endif
      if(!saved){
        queue.enqueue_now_P(PSTR("G28XY"));  // fix-wang
      }
      // lv_clear_cur_ui();
      // lv_draw_return_ui();
      lv_clear_zoffset_settings();
      lv_draw_manualLevel();
      break;
  }
}

void lv_draw_zoffset_settings(void) {
  #ifdef MESH_BED_LEVELING
    scr = lv_screen_create(ZOFFSET_UI, machine_menu.MeshBLConfTitle);
  #else
    scr = lv_screen_create(ZOFFSET_UI, machine_menu.ZoffsetConfTitle);
  #endif
  // Create image buttons
  lv_big_button_create(scr, "F:/bmp_Add.bin", machine_menu.BLTouchOffsetpos, INTERVAL_V, titleHeight, event_handler, ID_ZOFFSET_ZOFFSETPOS);


  lv_obj_t *buttonInitstate = lv_imgbtn_create(scr, NULL);
  lv_obj_set_event_cb_mks(buttonInitstate, event_handler, ID_ZOFFSET_INIT, NULL, 0);
  lv_imgbtn_set_src_both(buttonInitstate, "F:/bmp_init_state.bin");
  lv_obj_set_pos(buttonInitstate, 145, 50);

  lv_obj_t *labelInit = lv_label_create(scr, 125, 115, nullptr);
  lv_label_set_text(labelInit, machine_menu.BLTouchInit);
  lv_obj_align(labelInit, buttonInitstate, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

  buttonExt1 = lv_img_create(scr, nullptr);
  lv_img_set_src(buttonExt1, "F:/bmp_ext1_state.bin");
  lv_obj_set_pos(buttonExt1, 216, 50);

  buttonBed = lv_img_create(scr, nullptr);
  lv_img_set_src(buttonBed, "F:/bmp_bed_state.bin");
  lv_obj_set_pos(buttonBed, 287, 50);

  labelExt1 = lv_label_create(scr, 196, 115, nullptr);
  labelBed  = lv_label_create(scr, 267, 115, nullptr);

  #ifdef MESH_BED_LEVELING

    zOffsetText = lv_label_create(scr, 120, 140, nullptr);
  #else
    zOffsetText = lv_label_create(scr, 170, 140, nullptr);
  #endif



  lv_big_button_create(scr, "F:/bmp_Dec.bin", machine_menu.BLTouchOffsetneg, BTN_X_PIXEL * 3 + INTERVAL_V * 4, titleHeight, event_handler, ID_ZOFFSET_ZOFFSETNEG);

  buttonV = lv_imgbtn_create(scr, nullptr, INTERVAL_V, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_ZOFFSET_STEPS);
  labelV  = lv_label_create_empty(buttonV);

  buttonSave = lv_big_button_create(scr, "F:/bmp_save.bin", machine_menu.BLTouchSave, BTN_X_PIXEL * 2 + INTERVAL_V * 3, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_ZOFFSET_SAVE);
  #ifdef MESH_BED_LEVELING
    buttonNext = lv_big_button_create(scr, "F:/bmp_mbl_next.bin", machine_menu.MeshblNext, BTN_X_PIXEL * 2 + INTERVAL_V * 3, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_ZOFFSET_NEXT);
    lv_obj_set_hidden( buttonSave, true );
  #endif

  buttonBack = lv_big_button_create(scr, "F:/bmp_return.bin", common_menu.text_back, BTN_X_PIXEL * 3 + INTERVAL_V * 4, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_ZOFFSET_RETURN);
  // lv_obj_set_hidden( buttonBack, true );

  disp_step_dist();
  disp_zoffset_value();

  zoffset_diff = 0;
}

void disp_step_dist() {
  if ((int)(100 * step_dist) == 1)
    lv_imgbtn_set_src_both(buttonV, "F:/bmp_baby_move0_01.bin");
  else if ((int)(100 * step_dist) == 5)
    lv_imgbtn_set_src_both(buttonV, "F:/bmp_baby_move0_05.bin");
  else if ((int)(100 * step_dist) == 10)
    lv_imgbtn_set_src_both(buttonV, "F:/bmp_baby_move0_1.bin");

  if (gCfgItems.multiple_language) {
    if ((int)(100 * step_dist) == 1) {
      lv_label_set_text(labelV, move_menu.step_001mm);
      lv_obj_align(labelV, buttonV, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);
    }
    else if ((int)(100 * step_dist) == 5) {
      lv_label_set_text(labelV, move_menu.step_005mm);
      lv_obj_align(labelV, buttonV, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);
    }
    else if ((int)(100 * step_dist) == 10) {
      lv_label_set_text(labelV, move_menu.step_01mm);
      lv_obj_align(labelV, buttonV, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);
    }
  }
}

void disp_zoffset_value() {
  char buf[20];
  char str_1[16];
  if(saved){
    sprintf_P(buf, PSTR(machine_menu.MeshblSaved), "none");
  }
  else{
    #ifdef MESH_BED_LEVELING
      sprintf_P(buf, PSTR("%s: %d/%d - %s: %smm"),move_menu.currPoint,(manual_probe_index +1), total_probe_points, move_menu.zoffset, dtostrf(current_position.z, 1, 2, str_1) );
    #else
      sprintf_P(buf, PSTR("%s: %smm"), move_menu.zoffset, dtostrf(probe.offset.z, 1, 2, str_1) );
  #endif

  }

  lv_label_set_text(zOffsetText, buf);

    #if HAS_HOTEND
      sprintf(public_buf_l, printing_menu.temp1, (int)thermalManager.temp_hotend[0].celsius, (int)thermalManager.temp_hotend[0].target);
      lv_label_set_text(labelExt1, public_buf_l);
      lv_obj_align(labelExt1, buttonExt1, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    #endif


    #if HAS_HEATED_BED
      sprintf(public_buf_l, printing_menu.bed_temp, (int)thermalManager.temp_bed.celsius, (int)thermalManager.temp_bed.target);
      lv_label_set_text(labelBed, public_buf_l);
      lv_obj_align(labelBed, buttonBed, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    #endif
  // #ifdef MESH_BED_LEVELING
  // #endif
}

void zoffset_do_init(bool resetZoffset) {

  TERN_(HAS_BED_PROBE, probe.offset.z = 0);
  TERN_(HAS_SOFTWARE_ENDSTOPS, soft_endstop._enabled = false);
  // TERN_(HAS_LEVELING, reset_bed_level());
  // TERN_(EEPROM_SETTINGS, (void)settings.save());
  queue.clear();
  manual_probe_index= 0;
  saved = false;
  // str_1= "G28 S0";
  #ifdef MESH_BED_LEVELING

   queue.enqueue_now_P(PSTR("G28\nG29 S1"));
  #else
    char str_1[50];

    if (resetZoffset)
    {
      sprintf_P(str_1, PSTR("M851 Z0\nG28\nG1 Z10 F2400\nG1 X%d Y%d\nG0 Z0"), X_MAX_POS / 2, Y_MAX_POS / 2);
    }
    else
    {
      sprintf_P(str_1, PSTR("G28\nG1 Z10 F2400\nG1 X%d Y%d\nG0 Z0"), X_MAX_POS / 2, Y_MAX_POS / 2);
    }
    queue.enqueue_now_P(str_1);
  #endif
  // else
  // {
    // sprintf_P(str_1, PSTR("G28\nG1 Z10 F2400\nG1 X%d Y%d\nG0 Z0.3"), X_MAX_POS / 2, Y_MAX_POS / 2);
  // }
  // queue.enqueue_now_P(PSTR(str_1));  // fix-wang
}

void lv_clear_zoffset_settings() {
  #if HAS_ROTARY_ENCODER
    if (gCfgItems.encoder_enable) lv_group_remove_all_objs(g);
  #endif
  lv_obj_del(scr);
}

#endif // HAS_TFT_LVGL_UI
