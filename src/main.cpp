/**************************************************************************
 * @file	main.cpp: main program
 * @about	Implementation of AntRobots 2.0 
 * @author	Mustafa Mukadam
 * @date	05/26/2015	file created
 *
 * (c) Copyright - Georgia Institute of Technology
 *************************************************************************/

/*************************************************************************/
/*********************************includes********************************/
/*************************************************************************/
#include <iostream>

extern "C" {
#include <bcm2835.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
}

#include "defs.h"
#include "encoders.h"
#include "arduino_comm.h"
#include "motor_control.h"
#include "vision.h"
#include "state_machine.h"

#define TEST 0
#define DEBUG 1

using namespace std;

/*************************************************************************/
/*********************************variables*******************************/
/*************************************************************************/
int time_count = 0;
int loop10ms = 0;
int loop200ms = 0;
int loop1s = 0;
int run_flag = 1;

/*************************************************************************/
/*******************************timer_interrupt***************************/
/*************************************************************************/
void timer_handler(int signum) {
	time_count++;
	loop10ms = 1;
	if ((time_count%20) == 0)
		loop200ms = 1;	
	if ((time_count%100) == 0)
		loop1s = 1;
}

/*************************************************************************/
/*****************************timer_interrupt_init************************/
/*************************************************************************/
void timer_interrupt_init() {
	struct sigaction sa;
	struct itimerval timer;

	/* Install timer_handler as the signal handler for SIGVTALRM. */
	memset(&sa, 0, sizeof (sa));
	sa.sa_handler = &timer_handler;
	sigaction(SIGVTALRM,&sa,NULL);

	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = 10000; // timer expires after 10 ms

	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 10000; // timer expires every 10 ms

	setitimer(ITIMER_VIRTUAL,&timer,NULL);
}

/*************************************************************************/
/*****************************handle Ctrl+C signal************************/
/*************************************************************************/
void my_handler(int s) {
	run_flag = 0;
}

/*************************************************************************/
/************************************main*********************************/
/*************************************************************************/
int main(int argc, char **argv) {
	//*****load raspi gpio
	if (!bcm2835_init())
		return 1;
	
	//*****start spi for all connected devices
	bcm2835_spi_begin();
	
	//*****establish serial with arduino
	if (!arduino_comm_init())
		cout<<"Connection with Arduino not established"<<endl;
	#if TEST
		arduino_comm_test(); // test comm with arduino
	#endif

	//*****setup for handling Ctrl+C signal
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = my_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT,&sigIntHandler,NULL);

	//*****initialize encoders (uses SPI)
	encoders_init();
	#if TEST
		encoders_test(); // test encoders
	#endif
	#if TEST
		motor_test(); // test motors
	#endif
	
	//*****load Pixy
	int pixy_init_status = pixy_init();
	if(!pixy_init_status == 0) { // handle error initializing pixy
		printf("pixy_init(): ");
		pixy_error(pixy_init_status);
		return pixy_init_status;
	}
	#if TEST
		camera_test(); // test camera
	#endif

	//*****heading
	#if TEST
		heading_test(); // test camera
	#endif

	//*****initialize motor controller variables
	motor_control_init();
	//*****initialize trail following variables
	follow_trail_init();
	
	//*****initialize timer interrupt
	timer_interrupt_init();

	//*****state machine loop
	vref = 30.0;
	turn = 0.0;
	curr_state = GOING_IN;
	prev_state = GOING_IN;
	while (run_flag) {
		if (loop10ms) {
			encoders_read();
			encoders_cal_vel();
			read_camera();
			update_state();
			execute_state();
			loop10ms = 0;
		}
		if (loop200ms) {
			heading_read();
			loop200ms = 0;
		}
		if (loop1s) {
			//blocksData[PH_END].print_blockData();
			std::cout<<"State: "<<curr_state<<",\t"<<"Left Vel:"<<velL<<",\t"<<"Right Vel:"<<velR<<",\t"<<"Turn:"<<turn<<",\t"<<"Heading:"<<heading<<std::endl;
			loop1s = 0;
		}
	}	

	//*****stop and close everything after Ctrl+C signal
	stop_motors(); // stop motors
	pixy_close(); // close pixy camera
	bcm2835_spi_end(); // end spi
	bcm2835_close(); // close raspi gpio
	return 0;
}
