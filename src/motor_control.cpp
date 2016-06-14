/**************************************************************************
 * @file	motor_control.cpp: coupled PI controller for motors
 * @about	Implementation of AntRobots 2.0 
 * @author	Mustafa Mukadam
 * @date	06/09/2015	file created
 *
 * (c) Copyright - Georgia Institute of Technology
 *************************************************************************/

/*************************************************************************/
/*********************************includes********************************/
/*************************************************************************/
#include <iostream>
#include <cmath>

extern "C" {
#include <unistd.h>
}

#include "defs.h"
#include "motor_control.h"
#include "arduino_comm.h"
#include "encoders.h"

using namespace std;

/*************************************************************************/
/*********************************variables*******************************/
/*************************************************************************/
// extern variables
double vref;
double turn;

// variables only in this file
double e_steer, eL, oldeL, eR, oldeR, intL, intR, uL, uR;

/*************************************************************************/
/****************************motor_control_init***************************/
/*************************************************************************/
void motor_control_init() {
	// initialize variables for coupled PI
	e_steer = 0.0;
	eL = 0.0;
	eR = 0.0;
	oldeL = 0.0;
	oldeR = 0.0;
	intL = 0.0;
	intR = 0.0;
	uL = 0.0;
	uR = 0.0;
	turn = 0.0;
}

/*************************************************************************/
/*****************************motor_send_pwm******************************/
/*************************************************************************/
void motor_send_pwm(char m, int pwm) {
	string sdata;
	if (m == 'l')
		sdata += "lm,";
	else
		sdata += "rm,";
	if (pwm >= 0)
		sdata += "0,";
	else
		sdata += "1,";
	sdata += to_string(abs(pwm));
	send_arduino(sdata);
}

/*************************************************************************/
/********************************stop_motors******************************/
/*************************************************************************/
void stop_motors() {
	motor_send_pwm('l',0);
	motor_send_pwm('r',0);
}

/*************************************************************************/
/***************************coupled PI controller*************************/
/*************************************************************************/
void motor_control(double vel, double trn) {
	e_steer = trn + velR - velL;
	eL = (vel - velL) + (e_steer*KP_TURN_MC);
	eR = (vel - velR) - (e_steer*KP_TURN_MC);
	intL += ((eL + oldeL)*(MC_RATE/2));
	intR += ((eR + oldeR)*(MC_RATE/2));
	uL = (KP_MC*eL) + (KI_MC*intL);
	uR = (KP_MC*eR) + (KI_MC*intR);

	oldeL = eL;
	oldeR = eR;

	// bound inputs and take care of integral wind up
	if (uL > 255) {
		uL = 255;
		intL = 0.99*intL;
	}
	else if (uL < -255) {
		uL = -255;
		intL = 0.99*intL;
	}
	if (uR > 255) {
		uR = 255;
		intR = 0.99*intR;
	}
	else if (uR < -255) {
		uR = -255;
		intR = 0.99*intR;
	}

	// Write inputs to PWM
	motor_send_pwm('l',(int)uL);
	motor_send_pwm('r',(int)uR);
}

/*************************************************************************/
/******************************motor_test*********************************/
/*************************************************************************/
void motor_test() {
	cout<<"***************TESTING MOTORS***************"<<endl;
	motor_send_pwm('l',100);
	cout<<"LEFT MOTOR FORWARD"<<endl;
	sleep(2);
	motor_send_pwm('l',0);
	cout<<"LEFT MOTOR STOP"<<endl;
	sleep(2);
	motor_send_pwm('l',-200);
	cout<<"LEFT MOTOR BACKWARD"<<endl;
	sleep(2);
	motor_send_pwm('l',0);
	cout<<"LEFT MOTOR STOP"<<endl;
	motor_send_pwm('r',100);
	cout<<"RIGHT MOTOR FORWARD"<<endl;
	sleep(2);
	motor_send_pwm('r',0);
	cout<<"RIGHT MOTOR STOP"<<endl;
	sleep(2);
	motor_send_pwm('r',-200);
	cout<<"RIGHT MOTOR BACKWARD"<<endl;
	sleep(2);
	motor_send_pwm('r',0);
	cout<<"RIGHT MOTOR STOP"<<endl;
	cout<<"****************MOTORS TESTED***************"<<endl;
}