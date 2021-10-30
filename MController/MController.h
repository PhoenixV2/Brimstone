#ifndef MController_h
#define MController_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

#define ARDUINO_MAX_V 3000 // Steps per second

class MController{
	public:
		MController(){}
		MController(int dirPin, int stepPin);
		
		// Runs the motor linearly from the current position to the new position at max velocity
		// Stops at new position with V = 0
		void run();
		// Triggers a step with velocity spacing set by "_step_interval"
		bool run_velocity();
		// Sets the motor "_velocity" and "_step_interval" so that the motor can run
		// at a velocity indefinitely by calling run_v()
		void set_speed(float targetV);
		// Sets the maximum velocity in steps per second
		void set_max_velocity(float maxV);
		// Sets the maximum acceleration in steps per second^2
		void set_acceleration(float maxA);
		// Sets the minimum on time in µs for the motor controller to register a step command
		void set_on_pulse(int microseconds);
		// Sets the current position of the motor to be zero - resets the step counter (_step_count)
		void set_motor_zero();
		
		// Sets the input pin to trigger the emergency stop
		// Ideally place on an interrupt pin
		// TODO: Manage eStop interrupts
		void set_eStop(int eStopPin);
		void reset_eStop();

		// Prints the current speed of the motor and the last time it stepped
		void print_speed(float targetV);
		void print_headers();

		void calc_interval();
		float calc_new_velocity(float V);
		float get_velocity();

	private:
		// Triggers a step pulse to the motor controller
		void step();
		void step_nonblocking();
		// Calculates the new velocity for the next step to reach the target velocity
		// incorporating jerk
		// float calc_new_velocity(float V);
		// Returns the number of steps from the current position to the _new_position
		long steps_remaining();

	private:
		// Direction pin of the motor controller
		int _dir_pin;
		// Step pin of the motor controller
		int _step_pin;
		// Sets the direction of the motors operation: HIGH or LOW
		bool _direction;
		// Flag for step pulse width timing
		bool _stepping;

		// Current speed of the motor
		float _velocity;
		// Maximum velocity
		float _max_velocity;
		// Current acceleration
		float _a;
		// Maximum acceleration
		float _acceleration;
		// Max Jerk
		int _jerk;
		// v calc time
		unsigned long _v_time;
		float _v_error = 0.001; //0.0001
		float _p_error = 0.001;
		
		// Pulse width is defined by the minimum pulse width to trigger the step signal (µs)
		long _min_pulse_width; 
		// Time between steps required to achieve the set velocity (µs)
		float _step_interval;
		// Time in µs that the previous step was taken
		unsigned long _prev_step_time;
		// Time in µs that the step pin went high
		unsigned long _step_high_time;
		// Current time - assigning here to save memory
		unsigned long _curr_time;

		// For storing temporay time values
		unsigned long _T_TEMP;

		// Total number of steps taken by the motor from its zeroed position
		long int _step_count;
		// Step count of the new position to be moved to
		long _new_position;

		// Flag to emergency stop the motors motion in its current position
		volatile bool _eStop;
		int _eStop_pin;
};


#endif