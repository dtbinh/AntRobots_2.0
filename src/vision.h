/**************************************************************************
 * @file	vision.h: read pixy camera and follow desired object
 * @about	Implementation of AntRobots 2.0
 * @author	Mustafa Mukadam
 * @date	06/26/2015	file created
 *
 * (c) Copyright - Georgia Institute of Technology
 *************************************************************************/

#ifndef _VISION_H
#define _VISION_H
#include <iostream>
#include "pixy.h"

#define KP_TF 0.2
#define KI_TF 0.008
#define KD_TF 0.008
#define TF_RATE 0.010 // 10 ms
#define MAX_TURN 30
#define TRAIL_CENTER 160 // 320/2 in x direction

using namespace std;

struct blockData {
	uint16_t signature;
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
	int update;
	long counter;

	void print_blockData() {
		cout<<"sig: "<<signature<<" x: "<<x<<" y: "<<y<<" width: "<<width<<" height: "<<height<<" update: "<<update<<" counter:"<<counter<<endl;
	}
};

extern struct blockData blocksData[BLOCKS_DATA_NOS+1];

void read_camera();
void camera_test();
void follow_trail_init();
void follow_trail();

#endif
