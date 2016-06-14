/**************************************************************************
 * @file	arduino_comm.cpp: serial comm with Arduino
 * @about	Implementation of AntRobots 2.0
 * @author	Mustafa Mukadam
 * @date	05/27/2015	file created
 *
 * (c) Copyright - Georgia Institute of Technology
 *************************************************************************/

/*************************************************************************/
/*********************************includes********************************/
/*************************************************************************/
#include <iostream>

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
}

#include "defs.h"
#include "arduino_comm.h"

using namespace std;

/*************************************************************************/
/*********************************variables*******************************/
/*************************************************************************/
// extern variables
int fd;

// variables only in this file
char sbuf[100];
char rbuf[100];

/*************************************************************************/
/****************************arduino_comm_init****************************/
/*************************************************************************/
int arduino_comm_init() {
	char portname[] = "/dev/ttyACM0";
	int speed = B9600;
	int parity = 0;
	
	fd = open(portname,O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		printf("error %d opening %s: %s",errno,portname,strerror(errno));
		return -1;
	}

	struct termios tty;
	memset (&tty,0,sizeof tty);
	if (tcgetattr(fd,&tty) != 0) {
		std::cout<<"Error "<<errno<<" from tcgetattr: "<<strerror(errno)<<std::endl;
		return -1;
	}

	cfsetospeed(&tty,speed);
	cfsetispeed(&tty,speed);

	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
	// disable IGNBRK for mismatched speed tests; otherwise receive break
	// as \000 chars
	tty.c_iflag &= ~IGNBRK;         // disable break processing
	tty.c_lflag = 0;                // no signaling chars, no echo,
	                        		// no canonical processing
	tty.c_oflag = 0;                // no remapping, no delays
	tty.c_cc[VMIN]  = 0;            // read doesn't block
	tty.c_cc[VTIME] = 0.01;         // 0.5 seconds read timeout

	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

	tty.c_cflag |= (CLOCAL | CREAD);		// ignore modem controls,
	                        				// enable reading
	tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
	tty.c_cflag |= parity;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CRTSCTS;

	if (tcsetattr(fd,TCSANOW,&tty) != 0) {
		std::cout<<"Error "<<errno<<" from tcsetattr"<<std::endl;
		return -1;
	}
	
	set_blocking(0);             // set no blocking
	//sleep(2);
	tcflush(fd,TCIOFLUSH);

	// establish connection with arduino i.e wait for a "Ready'
	string rdata;
	int iter = 0;
	while (iter <= 20) {
		read_arduino(&rdata);
		if (rdata == "Ready")
			return 1;
		iter++;
		usleep(100000);
	}

	return 0;
}

/*************************************************************************/
/******************************set_blocking*******************************/
/*************************************************************************/
void set_blocking(int should_block) {
	struct termios tty;
	memset(&tty,0,sizeof tty);
	if (tcgetattr(fd,&tty) != 0) {
		std::cout<<"Error "<<errno<<" from tcgetattr: "<<strerror(errno)<<std::endl;
		return;
	}

	tty.c_cc[VMIN]  = should_block ? 1 : 0;
	tty.c_cc[VTIME] = 0.01;            // 5 -> 0.5 seconds read timeout

	if (tcsetattr(fd,TCSANOW,&tty) != 0)
		std::cout<<"Error "<<errno<<" from tcsetattr"<<std::endl;
}

/*************************************************************************/
/******************************send_arduino*******************************/
/*************************************************************************/
void send_arduino(string sdata) {
	int size = sdata.size();
	strcpy(sbuf,sdata.c_str());
	sbuf[size] = '\0';
	write(fd,sbuf,size+1);
}



/*************************************************************************/
/******************************read_arduino*******************************/
/*************************************************************************/
int read_arduino(string * rdata) {
	if (read(fd,&rbuf[0],1) > 0) {
		int cnt = 0; 
		do {
			cnt++;
			read(fd,&rbuf[cnt],1);
		} while(rbuf[cnt] != '$');
		rbuf[cnt] = '\0';
		*rdata = string(rbuf);
		return 1;
	}
	else 
		return 0;
}


/*************************************************************************/
/****************************arduino_comm_test****************************/
/*************************************************************************/
void arduino_comm_test() {
	cout<<"***********TESTING ARDUINO COMM*************"<<endl;
	int iter = 1;
	string rdata, sdata;
	sdata = "test,Hello Arduino " + to_string(iter);
	send_arduino(sdata);
	while (iter <= 5) {
		if (read_arduino(&rdata)) {
			cout<<rdata<<endl;
			iter++;
			sdata = "test,Hello Arduino " + to_string(iter);
			send_arduino(sdata);
		}
		sleep(1);
	}
	tcflush(fd,TCIOFLUSH);
	cout<<"************ARDUINO COMM TESTED*************"<<endl;
}
