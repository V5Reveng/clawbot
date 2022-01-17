#include <pros/misc.hpp>
#include <pros/motors.hpp>
#include <pros/rtos.hpp>
#include "main.hpp"

void initialize() {}
void disabled() {}
void competition_initialize() {}
void autonomous() {}

static pros::Controller controller (pros::E_CONTROLLER_controller);
static constexpr double dampening = 1.0 / 3.0;

int32_t motor_power(pros::controller_analog_e_t channel) {
	int32_t raw_input = controller.get_analog(channel)
	double input = raw_input / 100.0;
	return (int32_t)(input * input * input * 127.0 * dampening);
}

void opcontrol() {
	bool claw_open = false; //First button press will always open claw

	//V4 motors for the robot base
	pros::ADIMotor left_front (1);
	pros::ADIMotor left_back (2);
	pros::ADIMotor right_front (3);
	pros::ADIMotor right_back (4);

	//V5 motors for the robot arm
	pros:Motor arm_lift (1);
	pros:Motor claw_lift (2);
	pros:Motor claw_clamp (3);

	while (true) {
		//Move robot base
		auto const forward_axis = motor_power(pros::E_CONTROLLER_ANALOG_LEFT_Y);
		auto const strafe_axis = motor_power(pros::E_CONTROLLER_ANALOG_LEFT_X);
		auto const rotate_axis = motor_power(pros::E_CONTROLLER_ANALOG_RIGHT_X);
		left_front.set_value(forward_axis + strafe_axis + rotate_axis);
		right_front.set_value(-1*(forward_axis - strafe_axis - rotate_axis)); //Inversed
		left_back.set_value(forward_axis - strafe_axis + rotate_axis);
		right_back.set_value(-1*(forward_axis + strafe_axis - rotate_axis)); //Inversed

		//Move robot arm
		if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1)) {
		  arm_lift.move_velocity(66); //Midrange speed
		}
		else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_R2)) {
		  arm_lift.move_velocity(-66);
		}
		else {
		  arm_lift.move_velocity(0);
		}

		//Move robot claw
		if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_L1)) {
		  claw_lift.move_velocity(66);
		}
		else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_L2)) {
		  claw_lift.move_velocity(-66);
		}
		else {
		  claw_lift.move_velocity(0);
		}

		//Close/open claw
		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_X)) {
			if (claw_open) {
				claw_clamp.move_velocity(66);
			}
			else {
				claw_clamp.move_velocity(-66);
			}
		}

		//Stop moving claw once opened/closed
		if (claw_clamp.get_efficiency() < 5) {
			claw_clamp.move_velocity(0);
			claw_open = !claw_open;
		}

		pros::Task::delay(2);
	}	
}