#include "MController.h"

MController::MController(int dirPin, int stepPin){
	_step_pin = stepPin;
	_dir_pin = dirPin;
	_direction = HIGH; // Default to clockwise
	pinMode(_step_pin, OUTPUT);
	pinMode(_dir_pin, OUTPUT);

	_min_pulse_width = 6; // (6 for A4988) Microseconds
	_step_interval = 0;
	_prev_step_time = 0;
	_v_time = 0;
	_step_count = 0;
	_eStop = false;
	_stepping = false;

	// Default Values
	_max_velocity = ARDUINO_MAX_V;
	_velocity = 0;
	_max_acceleration = 400; //20
	_acceleration = 0;
	_jerk = 100; //2

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

bool MController::run_speed(float v){
	if(!_eStop){
		calc_new_velocity(v);
		calc_interval();
		run_velocity();
	}
	return (_velocity == v)?true:false;
}

bool MController::run_velocity(){
	// if "_step_interval == 0" then no motion should occur
	if(_step_interval){
		_curr_time = micros();
		// Check if the _step_interval has elapsed since the previous
		if(_curr_time - _prev_step_time >= _step_interval){
			if(_direction){
				_step_count++;
			}else{
				_step_count--;
			}
			_prev_step_time = _curr_time;
			// step();
			step_nonblocking();
		}
		if(_stepping){
			step_nonblocking();
		}
		return true;
	}
	return false;
}

long MController::steps_remaining(){
	return _step_count - _new_position;
}

float MController::calc_new_velocity(float V){

	V = min(V, _max_velocity);
	// Get time step
	_T_TEMP = micros();
	float dt = (_T_TEMP - _v_time)/1000000.0; // Scale it back to seconds
	_v_time = _T_TEMP;
	
	float dV = V - _velocity;
	if(abs(dV) > _v_error){
		
		// nV is the maximum velocity generated from the current acceleration
		// to a point where acceleration is 0
		// Calulated from V = a*t + 1/2*J*t^2 where t = a/J 
		// and we assume no initial conditions so a*t = 0
		float nV = 0.5*_acceleration*_acceleration/_jerk;
		
		// Get the sign of the difference between current and target V
		int c = ((dV) > 0) - ((dV) < 0);

		// Need to start slowing down to reach target velocity with 0 acceleration
		if(abs(dV) <= nV && abs(_acceleration) > 0){
			_acceleration += -c*_jerk*dt;
		}else if(abs(_acceleration) < _max_acceleration){
		// FULL STEAM AHEAD BOIS!
			_acceleration += c*_jerk*dt;
		}
	}else{
		// We have reached the target velocity
		_acceleration = 0;
		_velocity = V;
	}
	_velocity += _acceleration*dt; 
	// Set direction, if V is positive -> clockwise
	// if V is negative -> anticlockwise
	_direction = (_velocity >= 0)?HIGH:LOW; 
	return _velocity;
}

void MController::calc_interval(){
	_velocity = min(_velocity, _max_velocity);

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
	_max_acceleration = abs(maxA);
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

void MController::step_nonblocking(){
	if(!_stepping){
		_stepping = true;
		// Set direction
		digitalWrite(_dir_pin, _direction);
		digitalWrite(_step_pin, HIGH);
		// Get time stamp of pin going high
		_step_high_time = micros();
	}else{
		if(micros() - _step_high_time >= _min_pulse_width){
			digitalWrite(_step_pin, LOW);
			_stepping = false;
		}
	}
}

/**
 * USER UTILITY FUNCTIONS 
 * The following functions do not aid in the running of the motor controller
 * but can be used for debugging
 */

void MController::print_speed(float targetV){
	
	Serial.print(_step_interval);
	Serial.print("\t");
	Serial.print(_velocity);
	Serial.print("\t");
	Serial.print(_acceleration);
	Serial.print("\t");
	Serial.print(_step_count);
	Serial.print("\t");
	Serial.println(targetV);
}

void MController::print_headers(){
	Serial.print("Interval");
	Serial.print("\tVelocity");
	Serial.print("\tAcceleration");
	Serial.print("\tPosition");
	Serial.println("\tTarget velocity");
}

float MController::get_velocity(){
	return _velocity;
}