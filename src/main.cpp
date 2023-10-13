
#include "lvgl.cpp"

/*
	arm-none-eabi-addr2line -faps -e ./bin/hot.package.elf
	arm-none-eabi-addr2line -faps -e ./bin/cold.package.elf
	arm-none-eabi-addr2line -faps -e ./bin/cold.package.elf
*/

/*
	0x38332ec
		0x383333c
		0x78035f0
		0x3854e10
		0x38532fc
		0x384d78c
*/

/* ========================================================================== */
/*                                 Initialize                                 */
/* ========================================================================== */
void initialize()
{

	// load lvgl loading screen
	ace::lvgl::init_lvgl();

	pros::delay(250); // Stop the user from doing anything while legacy ports configure.

	// Configure your chassis controls
	chassis.toggle_modify_curve_with_controller(false); // Enables modifying the controller curve with buttons on the joysticks
	chassis.set_active_brake(0);                        // Sets the active brake kP. We recommend 0.1.
	chassis.set_curve_default(10, 0);                   // Defaults for curve. If using tank, only the first parameter is used. (Comment this line out if you have an SD card!)
	default_constants();                                // Set the drive to your own constants from autons.cpp!
	exit_condition_defaults();                          // Set the exit conditions to your own constants from autons.cpp!

	// init flap
	lv_label_set_text(ace::lvgl::label_load_flap, "Init Flap       -  OK");

	// clear screen on master controller
	//ace::__task_update_leds_task.set_priority(TASK_PRIORITY_DEFAULT - 2);
	//ace::led.set_all(ace::led_color_red_bright);

	ace::__task_update_cntr_task.set_priority(TASK_PRIORITY_DEFAULT - 1);

	lv_label_set_text(ace::lvgl::label_load_shenan, "Init Shenan     -  OK");

	// init chassis
	chassis.initialize();
	ace::intakeMotorLeft.init();
	ace::intakeMotorRight.init();
	ace::launcherMotorLeft.init();
	pros::lcd::shutdown();

	ace::endgame_timer.currTime = ace::endgame_timer.maxTime + 100;

	// get ambient light sample
	ace::ambient_light = ace::lightSensor.get_value();

	// Go to main screen
	lv_label_set_text(ace::lvgl::label_load_imu, "IMU Calibrate -  OK");
	ace::lvgl::start_preloader_anim();

	ace::update_cntr_haptic(".");
}

/* -------------------------------- Disabled -------------------------------- */
void disabled() {}

/* ----------------------------       ---------------------------- */
void competition_initialize() {}

/* ========================================================================== */
/*                                 Autonomous                                 */
/* ========================================================================== */
void autonomous()
{

	chassis.reset_pid_targets();               // Resets PID targets to 0
	chassis.reset_gyro();                      // Reset gyro position to 0
	chassis.reset_drive_sensor();              // Reset drive sensors to 0
	chassis.set_drive_brake(MOTOR_BRAKE_HOLD); // Set motors to hold.  This helps autonomous consistency.

	std::string curr_auton = ace::auton::auton_selection[ace::auton::auton_selection_index];

	ace::reset_motors();

	if (curr_auton == "1")
	{
		ace::auton::score();
	}
	else if (curr_auton == "2")
	{
		ace::auton::contact();
	}
	else if (curr_auton == "3")
	{
		ace::auton::skills();
	}
}

/* ========================================================================== */
/*                                User Control                                */
/* ========================================================================== */
void opcontrol()
{
	
	chassis.set_drive_brake(MOTOR_BRAKE_COAST);

	while (true)
	{

		/* ========================================================================== */
		/*                                  Get Input                                 */
		/* ========================================================================== */
		
		// Intake Toggle
		if (ace::btn_intake_toggle.get_press())
		{
			ace::intake_enabled = true;
			ace::intake_reverse_enabled = false;
			ace::update_cntr_haptic("-", false);

		} else 
		{
			ace::intake_enabled = false;
		}


		//ace::intake_enabled = ace::btn_intake_toggle.get_press();

		// Intake Reverse
		if (ace::btn_intake_reverse.get_press())
		{
			ace::intake_enabled = false;
			ace::intake_reverse_enabled = true;
		}
		else
		{
			ace::intake_reverse_enabled = false;
		}

		// Launcher
		if (ace::btn_launch.get_press())
		{
      		ace::launch_enabled = true;
		} 
		else {	
      		ace::launch_enabled = false;
    	}

		// Endgame Enabled
		ace::endgame_enabled = ace::btn_endgame.get_press();

		// Flapjack Enabled
		if (ace::btn_flap.get_press_new())
		{
			ace::flap_enabled = !ace::flap_enabled;
		}

		// Standby Enabled
		if (ace::btn_standby.get_press_new())
		{
			ace::launcher_standby_enabled = !ace::launcher_standby_enabled;
		}

		// Auton Page Up
		if (ace::btn_auton.get_press_new())
		{
			ace::auton::auton_page_up();
		}

		// Alliance Toggle
		if (ace::btn_alliance.get_press_new())
		{
			ace::is_red_alliance = !ace::is_red_alliance;
		}

		// Launcher Speed Increase
		if (ace::btn_launch_speed_increase.get_press_new()) 
		{
			ace::launch_speed += 2.5;
		}

		// Launcher Speed Decrease
		if (ace::btn_launch_speed_decrease.get_press_new()) 
		{
			ace::launch_speed -= 2.5;
		}

		// Light Sensor
		if (ace::light_sensor_detect())
		{
			ace::update_cntr_haptic(".");
		}

		/* --------------------------- Chassis Tank Drive --------------------------- */
   		chassis.tank();


		/* ========================================================================== */
		/*                        DO STUFF WITH ACQUIRED INPUT                        */
		/* ========================================================================== */
		for (int i = 0; i < 1; i++)
		{
			 
			ace::endgame_toggle(ace::endgame_enabled);

			// Launch
			if (ace::launch_enabled)
			{
				ace::launch(ace::launch_speed);

      		} else {

				ace::launch(0);
			}

			// Launch Reverse
			//if (ace::launch_reverse_enabled)
		//	{
			//	ace::launch_reverse(ace::launch_speed);

      		//} else {

			//	ace::launch_reverse(0);
		//	}



			// Intake Reverse
			if (ace::intake_reverse_enabled)
			{
				ace::intake_reverse(true);
				break;
			}
			else
			{
				ace::intake_reverse(false);
			}

			// Intake Toggle
			if (ace::intake_enabled)
			{
				ace::intake_toggle(true);
				break;
			}
			else
			{
				ace::intake_toggle(false);
			}

			// flapjack
			ace::flap_toggle(ace::flap_enabled);

		}

		/* ------------------------- Controller Screen Draw ------------------------- */
		// Line 1 - Master
		ace::update_cntr_text(ace::cntr_master, 0,
			(std::string)"Master" +
			"  " + std::to_string((int)ace::launcherMotorLeft.get_temp()) + "F" + " " + std::to_string((int)pros::battery::get_capacity()) + "%");
 
		// Line 1 - Partner
		ace::update_cntr_text(ace::cntr_partner, 0,
			(std::string)"Partner" +
			"  " + std::to_string((int)ace::intakeMotorLeft.get_temp()) + "F" +
			"  " + std::to_string((int)pros::battery::get_capacity()) + "%");

		// Line 2
		std::string ally_str = "";
		if (ace::is_red_alliance)
		{
			ally_str = "red";
		}
		else
		{
			ally_str = "blue";
		}

		std::string auton_string = "";
		if (ace::auton::auton_selection[ace::auton::auton_selection_index] == "contact")
		{
			auton_string = "c";
		}
		else if (ace::auton::auton_selection[ace::auton::auton_selection_index] == "score")
		{
			auton_string = "sc";
		}
		else
		{
			auton_string = "sk";
		}
		
		ace::update_cntr_text(ace::cntr_both, 1,
			"auto? " + ace::util::bool_to_str(ace::auto_targeting_enabled) +
			" l? " + std::to_string((int)ace::launch_speed) +
			" " + auton_string +
			" " + ally_str);
		
		// Line 3
		/*
		ace::update_cntr_text(ace::cntr_both, 2,
			(std::string)"idle? " + ace::util::bool_to_str(ace::launcher_standby_enabled) +
			"  flap? " + ace::util::bool_to_str(ace::flap_enabled));
		*/
		/* ---------------------------------- Delay --------------------------------- */

		pros::delay(ez::util::DELAY_TIME);
	}
}

