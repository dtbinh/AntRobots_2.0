/**************************************************************************
 * @file	state_machine.cpp: magnetometer, gyro, state, etc.
 * @about	Implementation of AntRobots 2.0 
 * @author	Mustafa Mukadam
 * @date	07/17/2015	file created
 *
 * (c) Copyright - Georgia Institute of Technology
 *************************************************************************/

/*************************************************************************/
/*********************************includes********************************/
/*************************************************************************/
#include <iostream>
#include <cmath>

extern "C" {
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
}

#include "defs.h"
#include "arduino_comm.h"
#include "state_machine.h"
#include "motor_control.h"
#include "vision.h"

using namespace std;

/*************************************************************************/
/*********************************variables*******************************/
/*************************************************************************/
// extern variables
double heading;
int curr_state;
int prev_state;
double heading_start;
double heading_end;
int reaching_end = 0;

/*************************************************************************/
/********************************heading_read*****************************/
/*************************************************************************/
void heading_read() {
	string sdata = "heading";
	send_arduino(sdata);
	string rdata;
	read_arduino(&rdata);
	heading = ((double)atoi(rdata.c_str()))/10.0;
}

/*************************************************************************/
/********************************heading_test*****************************/
/*************************************************************************/
void heading_test() {
	int count = 0;
	std::cout<<"**************TESTING HEADING**************"<<std::endl;
	while(count <= 20) {
		heading_read();
		cout<<"Heading = "<<heading<<endl;
		usleep(200000);
		count++;
	}
	std::cout<<"**************HEADING TESTED***************"<<std::endl;
}

/*************************************************************************/
/*******************************update_state******************************/
/*************************************************************************/
void update_state() {
	switch (curr_state) {
		case GOING_IN:
			if (blocksData[PH_END].update == 1) {
				reaching_end = 1;
			}
			if (reaching_end == 1 && blocksData[PH_END].update == 0 && blocksData[PH_END].counter >= COUNTER_THRES && blocksData[PHEREMONE].update == 0 && blocksData[PHEREMONE].counter >= COUNTER_THRES) {
				prev_state = curr_state;
				curr_state = TURNING;
				reaching_end = 0;
				stop_motors();
			}
			break;
		case TURNING:
			if (prev_state == GOING_IN && abs(heading - GOING_OUT_HEADING) < HEADING_THRES) {
					curr_state = GOING_OUT;
					prev_state = TURNING;
					motor_control_init();
					follow_trail_init();
					stop_motors();
			}
			else if (prev_state == GOING_OUT && abs(heading - GOING_IN_HEADING) < HEADING_THRES) {
					curr_state = GOING_IN;
					prev_state = TURNING;
					motor_control_init();
					follow_trail_init();
					stop_motors();
			}
			break;
		case GOING_OUT:
			if (blocksData[PH_END].update == 1) {
				reaching_end = 1;
			}
			if (reaching_end == 1 && blocksData[PH_END].update == 0 && blocksData[PH_END].counter >= COUNTER_THRES && blocksData[PHEREMONE].update == 0 && blocksData[PHEREMONE].counter >= COUNTER_THRES) {
				prev_state = curr_state;
				curr_state = TURNING;
				reaching_end = 0;
				stop_motors();
			}
			break;
		default:
			break;
	}
}

/*************************************************************************/
/*******************************execute_state*****************************/
/*************************************************************************/
void execute_state() {
	switch (curr_state) {
		case GOING_IN:
			follow_trail();
			break;
		case TURNING:
			motor_send_pwm('l',175);
			motor_send_pwm('r',-175);
			break;
		case GOING_OUT:
			follow_trail();
			break;
		default:
			break;
	}
}

/*************************************************************************/
/*****************************get_heading_end*****************************/
/*************************************************************************/
void calc_heading_end() {
	if ((heading_start+180) > 360)
		heading_end = 180 - (360 - heading_start);
	else
		heading_end = heading_start + 180;
}
