/**************************************************************************
 * @file	encoders.h: read AS5145H magnetic encoders
 * @about	Implementation of AntRobots 2.0 
 * @author	Mustafa Mukadam
 * @date	05/26/2015	file created
 *
 * (c) Copyright - Georgia Institute of Technology
 *************************************************************************/

#ifndef _ENCODERS_H
#define _ENCODERS_H

extern unsigned int encL;
extern unsigned int encR;
extern double velL;
extern double velR;

void encoders_init();
void encoders_read();
void encoders_cal_vel();
void encoders_test();

#endif