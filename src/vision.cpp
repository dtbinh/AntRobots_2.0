/**************************************************************************
 * @file	vision.cpp: read pixy camera and follow desired object
 * @about	Implementation of AntRobots 2.0
 * @author	Mustafa Mukadam
 * @date	06/26/2015	file created
 *
 * (c) Copyright - Georgia Institute of Technology
 *************************************************************************/

/*************************************************************************/
/*********************************includes********************************/
/*************************************************************************/
#include <iostream>

extern "C" {
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
}

#include "defs.h"
#include "vision.h"
#include "motor_control.h"

using namespace std;

/*************************************************************************/
/*********************************variables*******************************/
/*************************************************************************/
// extern variables
struct blockData blocksData[BLOCKS_DATA_NOS+1];

// variables only in this file
struct Block blocks[BLOCK_BUFFER_SIZE];
int tf_error;
int tf_prev_error;
double tf_int;

/*************************************************************************/
/********************************read_camera******************************/
/*************************************************************************/
void read_camera() {
	blocksData[PHEREMONE].update = 0;
	blocksData[PHEREMONE].counter++;
	blocksData[PH_END].update = 0;
	blocksData[PH_END].counter++;

	if(pixy_blocks_are_new()) {
		int blocks_copied = pixy_get_blocks(BLOCK_BUFFER_SIZE,&blocks[0]);
		if(blocks_copied < 0) {
			// Error: pixy_get_blocks
			printf("pixy_get_blocks(): ");
			pixy_error(blocks_copied);
		}

		// get biggest block from each signature
		for(int i = 0; i != blocks_copied; ++i) {
			switch (blocks[i].signature) {
				case PHEREMONE: // pheremone trail
					blocksData[PHEREMONE].signature = PHEREMONE;
					blocksData[PHEREMONE].x = blocks[i].x;
					blocksData[PHEREMONE].y = blocks[i].y;
					blocksData[PHEREMONE].width = blocks[i].width;
					blocksData[PHEREMONE].height = blocks[i].height;
					blocksData[PHEREMONE].update = 1;
					blocksData[PHEREMONE].counter = 0;
					break;
				case PH_END: // pheremone trail ends
					blocksData[PH_END].signature = PH_END;
					blocksData[PH_END].x = blocks[i].x;
					blocksData[PH_END].y = blocks[i].y;
					blocksData[PH_END].width = blocks[i].width;
					blocksData[PH_END].height = blocks[i].height;
					blocksData[PH_END].update = 1;
					blocksData[PH_END].counter = 0;
					break;
				default:
					break;
			}
		}
	}
}

/*************************************************************************/
/********************************test_camera******************************/
/*************************************************************************/
void camera_test() {
	int count = 0;
	std::cout<<"***************TESTING CAMERA***************"<<std::endl;
	while(count <= 10) {
		read_camera();
		blocksData[PHEREMONE].print_blockData();
		count++;
		usleep(500000);
	}
	std::cout<<"***************CAMERA TESTED****************"<<std::endl;
}

/*************************************************************************/
/****************************follow_trail_init****************************/
/*************************************************************************/
void follow_trail_init() {
	// initialize variables for trail follow PID
	tf_error = 0;
	tf_prev_error = 0;
	tf_int = 0.0;
}

/*************************************************************************/
/*******************************follow_trail******************************/
/*************************************************************************/
void follow_trail() {
	tf_error = TRAIL_CENTER - blocksData[PHEREMONE].x;

	tf_int += tf_error*TF_RATE;
	turn = tf_error*KP_TF + ((tf_error - tf_prev_error)/TF_RATE)*KD_TF + tf_int*KI_TF;
	tf_prev_error = tf_error;

	// bound turn
	if (turn > MAX_TURN) {
		turn = MAX_TURN;
		tf_int = 0.99*tf_int;
	}
	else if (turn < -MAX_TURN) {
		turn = -MAX_TURN;
		tf_int = 0.99*tf_int;
	}
	motor_control(vref,-turn);
}
