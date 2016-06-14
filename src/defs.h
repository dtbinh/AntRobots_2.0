/**************************************************************************
 * @file	defs.h: definitions and constants
 * @about	Implementation of AntRobots 2.0 
 * @author	Mustafa Mukadam
 * @date	05/26/2015	file created
 *
 * (c) Copyright - Georgia Institute of Technology
 *************************************************************************/

#ifndef _DEFS_H
#define _DEFS_H

// testing
#define TESTTIME 5000 // 5 sec

// encoders
#define CS_ENCODER_L RPI_GPIO_P1_11
#define CS_ENCODER_R RPI_GPIO_P1_12
#define VEL_CNV_CMPS 186.4210 // conversion from enc count to cm/sec
#define ENC_SAMPLE_RATE 0.010 // sample rate 10 ms 	

// pixy
#define BLOCK_BUFFER_SIZE 10
#define BLOCKS_DATA_NOS 5 // unique signatures trained
#define PHEREMONE 1
#define PH_END 2

#endif