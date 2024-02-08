
#include "ace.h"

namespace ace::auton {

/* ========================================================================== */
/*                        Global Variables Definitions                        */
/* ========================================================================== */

int auton_selection_index = 0;
/* ========================================================================== */
/*                               Score Auton                                   */
/* ========================================================================== */
void score() {




}

/* ========================================================================== */
/*                               Contact Auton                                  */
/* ========================================================================== */
void contact() {




  
}

/* ========================================================================== */
/*                              Three Side Auton                              */
/* ========================================================================== */
void three_side() {
  drive_chassis(48, DRIVE_SPEED);
  turn_chassis(180, TURN_SPEED);
  drive_chassis(12, DRIVE_SPEED);
  intake_toggle(true);
  // Dead
}

// ross is good

/* ========================================================================== */
/*                               Two Side Auton                               */
/* ========================================================================== */
void two_side() {
  /*drive_chassis(43, DRIVE_SPEED);
  turn_chassis(-90, TURN_SPEED);
  ace::intake_reverse(true);
  pros::delay(1000);
  ace::intake_reverse(false);
  drive_chassis(-8, DRIVE_SPEED);
  turn_chassis(90, TURN_SPEED);
  drive_chassis(-20, DRIVE_SPEED);
  drive_chassis(3,DRIVE_SPEED);
  turn_chassis(180, TURN_SPEED);
  drive_chassis(46, DRIVE_SPEED);
  turn_chassis(90, TURN_SPEED);
  drive_chassis(34, DRIVE_SPEED);
  */
  // Dead
}

/* ========================================================================== */
/*                                   Skills                                   */
/* ========================================================================== */
void skills() {
}
/* actual skilss

launch_auton(50000, LAUNCH_SPEED);
drive_chassis(24,DRIVE_SPEED);
turn_chassis(-90, TURN_SPEED);
drive_chassis(24, DRIVE_SPEED);
turn_speed(90, TURN_SPEED);
drive_speed(96, DRIVE_SPEED); */

/* ========================================================================== */
/*                         Auton Function Definitions                         */
/* ========================================================================== */

/* ------------------------------ Auton Page Up ----------------------------- */
void auton_page_up() {
  auton_selection_index += 1;
  if (auton_selection_index > auton_selection.size() - 1) {
    auton_selection_index = 0;
  }
}

/* ----------------------------- Auton Page Down ---------------------------- */
void auton_page_down() {
  auton_selection_index -= 1;
  if (auton_selection_index < 0) {
  }
  auton_selection_index = auton_selection.size() - 1;
}

/* ------------------------------ Roller Auton ------------------------------ */

void roller_auton(float rollerDegrees) {
  float actual_degrees = rollerDegrees * 3.0;
  // Untested, previously only for 1 motor
  intakeMotorLeft.move_relative(actual_degrees, 100);
  intakeMotorRight.move_relative(actual_degrees, 100);
  while (intakeMotorLeft.is_stopped() == 0) {
    pros::delay(ez::util::DELAY_TIME);
  }
}

/* --------------------------- Drive Chassis Auton -------------------------- */
void drive_chassis(float distance, float speed, bool wait) {
  bool slew_enabled = distance >= 14;
  chassis.set_drive_pid(distance, speed, slew_enabled);
  if (wait) {
    chassis.wait_drive();
  }
}

/* --------------------------- Turn Chassis Auton --------------------------- */
void turn_chassis(float angle, float speed, bool wait) {
  chassis.set_turn_pid(angle, speed);
  if (wait) {
    chassis.wait_drive();
  }
}

/* ------------------------------ Launch Auton ------------------------------ */
/*void launch_auton(float time, float speed, bool isLong, bool early_exit) {

  ace::util::timer launch_timer(time);

  int launchedCounter = 0;
  while (1)
  {
    //launch(speed, isLong);
      launch(speed);
    // detect if disk launched
    if (light_sensor_detect())
    {
      launchedCounter++;
      long_launch_timer.reset();
    }

    // if 3, exit
    if (launchedCounter >= 3 && early_exit)
    {
      //break;
    }

    // Timer
    launch_timer.update(ez::util::DELAY_TIME);
    if (launch_timer.done())
    {
      break;
    }

    pros::delay(ez::util::DELAY_TIME);
  }
  launcherMotor.spin_percent(0);
}
*/

void launch_auton(float time, float speed) {
  ace::util::timer launch_timer(time);
  launch(ace::launch_speed);

  while (!launch_timer.done()) {
    launch_timer.update(ez::util::DELAY_TIME);
  }
}

/* ------------------------------ Endgame Auton ----------------------------- */
void endgame_auton() {
  endgame_toggle(true);
  while (!endgame_timer.done()) {
    pros::delay(ez::util::DELAY_TIME);
    endgame_toggle(false);
  }
  endgame_toggle(false);
}
}  // namespace ace::auton