/**************************************************************************
 * @file	state_machine.h:  magnetometer, gyro, state, etc.
 * @about	Implementation of AntRobots 2.0 
 * @author	Mustafa Mukadam
 * @date	07/17/2015	file created
 *
 * (c) Copyright - Georgia Institute of Technology
 *************************************************************************/

#ifndef _STATE_MACHINE_H
#define _STATE_MACHINE_H

#define GOING_IN 1
#define DIGGING 2
#define TURNING 3
#define GOING_OUT 4
#define DEPOSITING 5

#define GOING_IN_HEADING 280
#define GOING_OUT_HEADING 5
#define COUNTER_THRES 50
#define HEADING_THRES 40

extern int curr_state;
extern int prev_state;
extern double heading;
extern double heading_start;
extern double heading_end;
extern int reaching_end;

void heading_read();
void heading_test();
void calc_heading_end();
void update_state();
void execute_state();

#endif
