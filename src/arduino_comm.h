/**************************************************************************
 * @file	arduino_comm.cpp: serial comm with Arduino
 * @about	Implementation of AntRobots 2.0
 * @author	Mustafa Mukadam
 * @date	05/27/2015	file created
 *
 * (c) Copyright - Georgia Institute of Technology
 *************************************************************************/

#ifndef _ARDUINO_COMM_H
#define _ARDUINO_COMM_H

#include <string>
#include <cstring>
#include <sstream>

using std::string;

template <typename T>
std::string to_string(T value) {
	std::ostringstream os;
	os<<value;
	return os.str();
}

extern int fd;

int arduino_comm_init();
void set_blocking(int should_block);
void send_arduino(string sdata);
int read_arduino(string * rdata);
void arduino_comm_test();

#endif
