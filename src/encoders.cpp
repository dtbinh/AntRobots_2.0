/**************************************************************************
 * @file	main.cpp: read AS5145H magnetic encoders
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
#include <cmath>
#include <iomanip>

extern "C" {
#include <bcm2835.h>
}

#include "defs.h"
#include "encoders.h"

using namespace std;

/*************************************************************************/
/*********************************variables*******************************/
/*************************************************************************/
// extern variables
unsigned int encL;
unsigned int encR;
double velL;
double velR;

// variables only in this file
long Angle1;
long Angle2;
long oldAngle1;
long oldAngle2;
int digitalAngle1;
int digitalAngle2;
int OlddigitalAngle1;
int OlddigitalAngle2;
int firstAngle1;
int firstAngle2;
int initialAngle1;
int initialAngle2;

/*************************************************************************/
/******************************encoders_init******************************/
/*************************************************************************/
void encoders_init() {
	// chip select pin as output and inactive HIGH
	bcm2835_gpio_fsel(CS_ENCODER_L,BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_write(CS_ENCODER_L,HIGH);
	bcm2835_gpio_fsel(CS_ENCODER_R,BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_write(CS_ENCODER_R,HIGH);

	// initialize variables
	encL = 0;
	encR = 0;
	velL = 0.0;
	velR = 0.0;

	Angle1 = 0;
	Angle2 = 0;
	oldAngle1 = 0;
	oldAngle2 = 0;
	OlddigitalAngle1 = 0;
	OlddigitalAngle2 = 0;
	firstAngle1 = 1;
	firstAngle2 = 1;
	initialAngle1 = 0;
	initialAngle2 = 0;
}

/*************************************************************************/
/******************************encoders_read******************************/
/*************************************************************************/
void encoders_read() {
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST); // MSBFIRST
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE3); // polarity=1, phase=1
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_256); // 1MHz

	char buf[3];
	bcm2835_gpio_write(CS_ENCODER_L,LOW); // start reading
	bcm2835_spi_transfern(&buf[0],3);
	bcm2835_gpio_write(CS_ENCODER_L,HIGH); // end reading
	// get encoder count from first 12 MSB (4096 ticks/rev)
	encL = (((unsigned int)(buf[0] & 0x7F))<<5) + (((unsigned int)(buf[1] & 0xF8))>>3);

	bcm2835_gpio_write(CS_ENCODER_R,LOW); // start reading
	bcm2835_spi_transfern(&buf[0],3);
	bcm2835_gpio_write(CS_ENCODER_R,HIGH); // end reading
	// get encoder count from first 12 MSB (4096 ticks/rev)
	encR = (((unsigned int)(buf[0] & 0x7F))<<5) + (((unsigned int)(buf[1] & 0xF8))>>3);

	digitalAngle1 = encL;
	digitalAngle2 = encR;
}

/*************************************************************************/
/*****************************encoders_cal_vel****************************/
/*************************************************************************/
void encoders_cal_vel() {
	if (firstAngle2 == 1) {
		firstAngle2 = 0;
		Angle2 = digitalAngle2;
		OlddigitalAngle2 = digitalAngle2;
		initialAngle2 = digitalAngle2;
		oldAngle2 = initialAngle2;
	}
	else {
		if (abs(digitalAngle2-OlddigitalAngle2) > 1000) {  // 1000 number figured for 2ms sample rate and max speed
			// we have roll over
			if (digitalAngle2 > OlddigitalAngle2) {
				Angle2 = Angle2 - (OlddigitalAngle2) - (4096-digitalAngle2);
			}
			else {
				Angle2 = Angle2 + (4096-OlddigitalAngle2) + (digitalAngle2);
			}
		}
		else { // no roll over
			Angle2 = Angle2 + (digitalAngle2-OlddigitalAngle2);
		}
		OlddigitalAngle2 = digitalAngle2;
	}

	if (firstAngle1 == 1) {
		firstAngle1 = 0;
		Angle1 = digitalAngle1;
		OlddigitalAngle1 = digitalAngle1;
		initialAngle1 = digitalAngle1;
		oldAngle1 = initialAngle1;
	}
	else { // signs are reversed here as the left encoder is reversed
		if (abs(digitalAngle1-OlddigitalAngle1) > 1000) {  // 1000 number figured for 2ms sample rate and max speed
			// we have roll over
			if (digitalAngle1 > OlddigitalAngle1) { // Going reverse
				Angle1 = Angle1 + (OlddigitalAngle1) + (4096-digitalAngle1);
			}
			else { // Going forward
				Angle1 = Angle1 - (4096-OlddigitalAngle1) - (digitalAngle1);
			}
		}
		else { // no roll over
			Angle1 = Angle1 - (digitalAngle1-OlddigitalAngle1);
		}
		OlddigitalAngle1 = digitalAngle1;
	}

	// velocity calculation
	velL = (Angle1 - oldAngle1)/(VEL_CNV_CMPS*ENC_SAMPLE_RATE);
	velR = (Angle2 - oldAngle2)/(VEL_CNV_CMPS*ENC_SAMPLE_RATE);
	oldAngle1 = Angle1;
	oldAngle2 = Angle2;
}

/*************************************************************************/
/********************************encoders_test****************************/
/*************************************************************************/
void encoders_test() {
	long count = 0;
	std::cout<<"**************TESTING ENCODERS**************"<<std::endl;
	std::cout<<std::setprecision(2)<<std::fixed;
	while(count <= TESTTIME/2) {
		encoders_read();
		encoders_cal_vel();
		if (count%250 == 0) {
			std::cout<<"Left Enc:"<<encL<<",\t"<<"Right Enc:"<<encR<<"\t|| ";
			std::cout<<"Left Vel:"<<velL<<",\t"<<"Right Vel:"<<velR<<std::endl;
		}
		count++;
		bcm2835_delay(2);
	}
	std::cout<<"**************ENCODERS TESTED***************"<<std::endl;
}