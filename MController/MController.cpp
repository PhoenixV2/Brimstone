#include "MController.h"

MController::MController(int dirPin, int stepPin){
	_step_pin = stepPin;
	_dir_pin = dirPin;
	_direction = HIGH; // Default to clockwise
	pinMode(_step_pin, OUTPUT);
	pinMode(_dir_pin, OUTPUT);

	_min_pulse_width = 10; // (6 for A4988) Microseconds
	_step_interval = 0;
	_prev_step_time = 0;
	_v_time = 0;
	_step_count = 0;
	_eStop = false;

	// Default Values
	_max_velocity = ARDUINO_MAX_V;
	_velocity = 0;
	_acceleration = 100; //20
	_a = 0;
	_jerk = 25; //2

	set_motor_zero();
}

// IMPLEMENT THIS PROPERLY
void MController::run(){
	// TODO: 
	if (!_eStop){
		long steps_to_go = steps_remaining();
		float new_v = calc_new_velocity(steps_to_go);
		// TODO: 
	
	}
}

bool MController::run_velocity(){
	// if "_step_interval == 0" then no motion should occur
	if(_step_interval){
		_curr_time = micros();
		// Check if the _step_interval has elapsed since the previous
		// step
		if(_curr_time - _prev_step_time >= _step_interval){
			if(_direction){
				_step_count++;
			}else{
				_step_count--;
			}
			_prev_step_time = _curr_time;
			step();
		}
		return true;
	}
	return false;
}

long MController::steps_remaining(){
	return _step_count - _new_position;
}

float MController::calc_new_velocity(float V){

	// Get time step
	_T_TEMP = micros();
	float dt = (_T_TEMP - _v_time)/1000000.0; // Scale it back to seconds
	_v_time = _T_TEMP;
	// Serial.println(dt);
	if(V > _max_velocity){
		V = _max_velocity;
	}
	if(abs(V - _velocity) > _v_error){
		float nV = 0.5*_a*_a/_jerk;
		
		// Get the sign of the difference between current and target V
		int c = ((V - _velocity) > 0) - ((V - _velocity) < 0);

		if(abs(V - _velocity) <= nV){
			if(abs(_a) > 0){
				_a += -c*_jerk*dt; 
			}
		}else if(abs(_a) < _acceleration){
			_a += c*_jerk*dt;
		}
	}else{
		// We have reached the target velocity
		_a = 0;
		_velocity = V;
	}
	_velocity += _a*dt; 
	// Set direction, if V is positive -> clockwise
	// if V is negative -> anticlockwise
	_direction = (_velocity >= 0)?HIGH:LOW; 
	return _velocity;
}

void MController::set_speed(float targetV){
	// Calculate the pulse width for the specified steps/s
	if(targetV <= ARDUINO_MAX_V){
		_velocity = targetV;
	}else{
		_velocity = ARDUINO_MAX_V;
	}
	_step_interval = 1000000.0/abs(_velocity); // µs per step
}

void MController::calc_interval(){
	if(_velocity >= _max_velocity){
		_velocity = _max_velocity;
	}
	if(abs(_velocity) > 0){
		_step_interval = 1000000.0/abs(_velocity); // µs per step
	}else{
		_step_interval = 0;
	}
}

void MController::set_motor_zero(){
	_step_count = 0;
}

void MController::reset_eStop(){
	_eStop = false;
}

void MController::set_eStop(int eStopPin){
	_eStop_pin = eStopPin;
	pinMode(eStopPin, INPUT);
	_eStop = false;
}

void MController::set_max_velocity(float maxV){
	if(maxV < ARDUINO_MAX_V){
		_max_velocity = maxV;
	}else{
		_max_velocity = ARDUINO_MAX_V;
	}
}

void MController::set_acceleration(float maxA){
	_acceleration = abs(maxA);
}

void MController::set_on_pulse(int microseconds){
	_min_pulse_width = abs(microseconds);
}

void MController::step(){
	// TODO: Change to set pins HIGH with
	// eg. PORTB |= _BV(PB3);
	// and set them Low with eg PORTB &= ~_BV(PB3);
	// need to configure the specific pins somewhere
	digitalWrite(_dir_pin, _direction);
	digitalWrite(_step_pin, HIGH);
	// Wait minimum pulse width <- This is blocking
	// TODO: Find a non-blocking method for this step procedure for controllers with high clock speeds
	delayMicroseconds(_min_pulse_width);
	digitalWrite(_step_pin, LOW);
}


/**
 * USER UTILITY FUNCTIONS 
 * The following functions do not aid in the running of the motor controller
 * but can be used for debugging
 */

void MController::print_speed(float targetV){
	
	Serial.print(_velocity);
	Serial.print("\t");
	Serial.print(_a);
	Serial.print("\t");
	Serial.print(_step_count);
	Serial.print("\t");
	Serial.println(targetV);
}

void MController::print_headers(){
	Serial.print("Velocity");
	Serial.print("\tAcceleration");
	Serial.print("\tPosition");
	Serial.println("\tTarget velocity");
}

float MController::get_velocity(){
	return _velocity;
}