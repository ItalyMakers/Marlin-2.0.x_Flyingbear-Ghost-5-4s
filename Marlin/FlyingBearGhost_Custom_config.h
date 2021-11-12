#pragma once

//default motherboard
#define FBGHOST_MOTHERBOARD     BOARD_MKS_ROBIN_NANO
#define FBGHOST_MACHINE_NAME    "Flying Bear Ghost 5"



// #define ALL_DRV_2208
// //#define FB_4S_STOCK
// //#define FB_5_STOCK

// #ifdef ALL_DRV_2208
// #define USR_E0_DIR true
// #define USR_X_DIR false
// #define USR_Y_DIR false
// #define USR_Z_DIR true
// #endif

// #ifdef FB_4S_STOCK
// #define USR_E0_DIR false
// #define USR_X_DIR true
// #define USR_Y_DIR true
// #define USR_Z_DIR false
// #endif

// #ifdef FB_5_STOCK
// #if MOTHERBOARD == BOARD_MKS_ROBIN_NANO_V1_3_F4
// #define USR_E0_DIR false
// #define USR_X_DIR true
// #define USR_Y_DIR true
// #define USR_Z_DIR false
// #else
// #define USR_E0_DIR false
// #define USR_X_DIR false
// #define USR_Y_DIR false
// #define USR_Z_DIR false
// #endif
// #endif

/**
 *  * Use TMC2208/TMC2208_STANDALONE for TMC2225 drivers and TMC2209/TMC2209_STANDALONE for TMC2226 drivers.
 *
 * Options: A4988, A5984, DRV8825, LV8729, L6470, L6474, POWERSTEP01,
 *          TB6560, TB6600, TMC2100,
 *          TMC2130, TMC2130_STANDALONE, TMC2160, TMC2160_STANDALONE,
 *          TMC2208, TMC2208_STANDALONE, TMC2209, TMC2209_STANDALONE,
 *          TMC26X,  TMC26X_STANDALONE,  TMC2660, TMC2660_STANDALONE,
 *          TMC5130, TMC5130_STANDALONE, TMC5160, TMC5160_STANDALONE
 * :['A4988', 'A5984', 'DRV8825', 'LV8729', 'L6470', 'L6474', 'POWERSTEP01', 'TB6560', 'TB6600', 'TMC2100', 'TMC2130', 'TMC2130_STANDALONE', 'TMC2160', 'TMC2160_STANDALONE', 'TMC2208', 'TMC2208_STANDALONE', 'TMC2209', 'TMC2209_STANDALONE', 'TMC26X', 'TMC26X_STANDALONE', 'TMC2660', 'TMC2660_STANDALONE', 'TMC5130', 'TMC5130_STANDALONE', 'TMC5160', 'TMC5160_STANDALONE']
 */
#define FBGHOST_X_DRIVER_TYPE   A4988
#define FBGHOST_Y_DRIVER_TYPE   A4988
#define FBGHOST_Z_DRIVER_TYPE   A4988
#define FBGHOST_E0_DRIVER_TYPE  A4988

// Invert the stepper direction. Change (or reverse the motor connector) if an axis goes the wrong way.
// true with TMC2208, TMC2209, TMC2225 TMC2226
#define FBGHOST_INVERT_X_DIR    true
#define FBGHOST_INVERT_Y_DIR    true
#define FBGHOST_INVERT_Z_DIR    false
#define FBGHOST_INVERT_E0_DIR   false



/**
 * Default Axis Steps Per Unit (steps/mm)
 * Override with M92
*                                         X, Y, Z [, I [, J [, K]]], E0 [, E1[, E2...]]
 */
#define FBGHOST_DEFAULT_AXIS_STEPS_PER_UNIT   { 80, 80, 400, 400}





/**
 * BLTOUCH
 *
 * decommentare per abilitare il BLTOUCH
 */
// #define FBGHOST_BLTOUCH

#if ENABLED(FBGHOST_BLTOUCH)
  #define FBGHOST_AUTO_BED_LEVELING_BILINEAR
#else
  #define FBGHOST_MESH_BED_LEVELING
  #define FBGHOST_MIN_SOFTWARE_ENDSTOP_Z
  #define FBGHOST_MESH_INSET          10          // Set Mesh bounds as an inset region of the bed
  #define FBGHOST_GRID_MAX_POINTS_X   3           // Don't use more than 7 points per axis, implementation limited.
  #define FBGHOST_GRID_MAX_POINTS_Y   GRID_MAX_POINTS_X
#endif



/**
 * Nozzle-to-Probe offsets { X, Y, Z }
 *
 * X and Y offset
 *   Use a caliper or ruler to measure the distance from the tip of
 *   the Nozzle to the center-point of the Probe in the X and Y axes.
 *
 * Z offset
 * - For the Z offset use your best known value and adjust at runtime.
 * - Common probes trigger below the nozzle and have negative values for Z offset.
 * - Probes triggering above the nozzle height are uncommon but do exist. When using
 *   probes such as this, carefully set Z_CLEARANCE_DEPLOY_PROBE and Z_CLEARANCE_BETWEEN_PROBES
 *   to avoid collisions during probing.
 *
 * Tune and Adjust
 * -  Probe Offsets can be tuned at runtime with 'M851', LCD menus, babystepping, etc.
 * -  PROBE_OFFSET_WIZARD (configuration_adv.h) can be used for setting the Z offset.
 *
 * Assuming the typical work area orientation:
 *  - Probe to RIGHT of the Nozzle has a Positive X offset
 *  - Probe to LEFT  of the Nozzle has a Negative X offset
 *  - Probe in BACK  of the Nozzle has a Positive Y offset
 *  - Probe in FRONT of the Nozzle has a Negative Y offset
 *
 * Some examples:
 *   #define NOZZLE_TO_PROBE_OFFSET { 10, 10, -1 }   // Example "1"
 *   #define NOZZLE_TO_PROBE_OFFSET {-10,  5, -1 }   // Example "2"
 *   #define NOZZLE_TO_PROBE_OFFSET {  5, -5, -1 }   // Example "3"
 *   #define NOZZLE_TO_PROBE_OFFSET {-15,-10, -1 }   // Example "4"
 *
 *     +-- BACK ---+
 *     |    [+]    |
 *   L |        1  | R <-- Example "1" (right+,  back+)
 *   E |  2        | I <-- Example "2" ( left-,  back+)
 *   F |[-]  N  [+]| G <-- Nozzle
 *   T |       3   | H <-- Example "3" (right+, front-)
 *     | 4         | T <-- Example "4" ( left-, front-)
 *     |    [-]    |
 *     O-- FRONT --+
 */
#define  FBGHOST_NOZZLE_TO_PROBE_OFFSET {50,0,0}   // { 10, 10, 0 }





#define FBGHOST_FIL_RUNOUT_STATE     HIGH        // Pin state indicating that filament is NOT present.
// After a runout is detected, continue printing this length of filament
// before executing the runout script. Useful for a sensor at the end of
// a feed tube.
#define FBGHOST_FILAMENT_RUNOUT_DISTANCE_MM 10



/**
 * Auto-leveling needs preheating
 */
#define FBGHOST_PREHEAT_BEFORE_LEVELING

#define FBGHOST_LEVELING_NOZZLE_TEMP  120   // (°C) Only applies to E0 at this time
#define FBGHOST_LEVELING_BED_TEMP      50




// Preheat Constants
#define FBGHOST_PREHEAT_1_LABEL         "PLA"
#define FBGHOST_PREHEAT_1_TEMP_HOTEND   200
#define FBGHOST_PREHEAT_1_TEMP_BED      65
#define FBGHOST_PREHEAT_1_FAN_SPEED     0 // Value from 0 to 255

#define FBGHOST_PREHEAT_2_LABEL         "PETG"
#define FBGHOST_PREHEAT_2_TEMP_HOTEND   235
#define FBGHOST_PREHEAT_2_TEMP_BED      75
#define FBGHOST_PREHEAT_2_FAN_SPEED     0 // Value from 0 to 255



//PID
#define FBGHOST_BANG_MAX        255
#define FBGHOST_PID_MAX         BANG_MAX
#define FBGHOST_PID_K1          0.95

//HOTEND
#define FBGHOST_DEFAULT_Kp 11.14
#define FBGHOST_DEFAULT_Ki 0.72
#define FBGHOST_DEFAULT_Kd 43.09

//BEDTEMP
#define FBGHOST_DEFAULT_bedKp 52.63
#define FBGHOST_DEFAULT_bedKi 9.75
#define FBGHOST_DEFAULT_bedKd 71.01

//Prevent extrusion if the temperature is below EXTRUDE_MINTEMP
#define FBGHOST_EXTRUDE_MINTEMP 170





/**
 * Default Jerk limits (mm/s)
 * Override with M205 X Y Z E
 *
 * "Jerk" specifies the minimum speed change that requires acceleration.
 * When changing speed and direction, if the difference is less than the
 * value set here, it may happen instantaneously.
 */

#define FBGHOST_CLASSIC_JERK

#define FBGHOST_DEFAULT_XJERK 10.0
#define FBGHOST_DEFAULT_YJERK 10.0
#define FBGHOST_DEFAULT_ZJERK  0.3

#define FBGHOST_DEFAULT_EJERK    10.0  // May be used by Linear Advance




/**
 * Prevent a single extrusion longer than EXTRUDE_MAXLENGTH.
 * Note: For Bowden Extruders make this large enough to allow load/unload.
 */
#define FBGHOST_EXTRUDE_MAXLENGTH 1200

/**
 * Default Max Feed Rate (mm/s)
 * Override with M203
 *                                      X, Y, Z [, I [, J [, K]]], E0 [, E1[, E2...]]
 */
#define FBGHOST_DEFAULT_MAX_FEEDRATE          { 2000, 2000, 5, 70 }

/**
 * Default Max Acceleration (change/s) change = mm/s
 * (Maximum start speed for accelerated moves)
 * Override with M201
 *                                      X, Y, Z [, I [, J [, K]]], E0 [, E1[, E2...]]
 */
#define FBGHOST_DEFAULT_MAX_ACCELERATION {1000,1000,200,80000}   //      { 3000, 3000, 100, 10000 }


