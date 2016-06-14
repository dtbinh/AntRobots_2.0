/**************************************************************************
 * @file	motor_control.h: coupled PI controller for motors
 * @about	Implementation of AntRobots 2.0 
 * @author	Mustafa Mukadam
 * @date	06/09/2015	file created
 *
 * (c) Copyright - Georgia Institute of Technology
 *************************************************************************/

#ifndef _MOTOR_CONTROL_H
#define _MOTOR_CONTROL_H

#define KP_TURN_MC 0.8
#define KP_MC 4.5
#define KI_MC 7.5
#define MC_RATE 0.010 // 10 ms

extern double vref;
extern double turn;

void motor_control_init();
void motor_send_pwm(char m, int pwm);
void stop_motors();
void motor_control(double vel, double turn);
void motor_test();

#endif
