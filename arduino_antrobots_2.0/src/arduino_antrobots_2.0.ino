/**************************************************************************
 * @file	arduino_antrobots_2.0.ino: main program for arduino
 * @about	Implementation of AntRobots 2.0 
 * @author	Mustafa Mukadam
 * @date	05/26/2015	file created
 *
 * (c) Copyright - Georgia Institute of Technology
 *************************************************************************/

/*************************************************************************/
/*********************************includes********************************/
/*************************************************************************/
#include <string.h>
#include <Wire.h>
#include "SFE_HMC6343.h"

/*************************************************************************/
/*********************************defines*********************************/
/*************************************************************************/
#define DEBUG 0

// pin definitions
#define LEFT_MOTOR_PWM 2
#define LEFT_MOTOR_ENABLE 30
#define RIGHT_MOTOR_PWM 3
#define RIGHT_MOTOR_ENABLE 31
#define LED_PIN 13

/*************************************************************************/
/*********************************variables*******************************/
/*************************************************************************/
char rbuf[100];
String rdata;
String data[10];
int pos;
boolean recv_done;
int iter;

unsigned long time_count;
int loop10ms;
int loop200ms;
int loop1000ms;
int tcnt1;

SFE_HMC6343 compass;
int currHeading;

/*************************************************************************/
/******************************check_command******************************/
/*************************************************************************/
void check_command(String rdata) {
	int old_index = -1;
	int index;
	int i = 0;
	int j = 0;
	while ((index = rdata.indexOf(',',old_index+1)) != -1) {
		data[i] = rdata.substring(old_index+1,index);
		i++;
		old_index = index;
	}
	data[i] = rdata.substring(old_index+1);

	// comm test
	if (data[0].equals("test")) {
		Serial.print("Hello Pi ");
		Serial.print(iter);
		Serial.print("$");
		iter++;
	}
	// left mototr
	else if (data[0].equals("lm")) {
		if (data[1].equals("0")) {
			digitalWrite(LEFT_MOTOR_ENABLE,LOW);
			analogWrite(LEFT_MOTOR_PWM,data[2].toInt());
		}
		else {
			digitalWrite(LEFT_MOTOR_ENABLE,HIGH);
			analogWrite(LEFT_MOTOR_PWM,255-data[2].toInt());
		}
	}
	// right motor
	else if (data[0].equals("rm")) {
		if (data[1].equals("0")) {
			digitalWrite(RIGHT_MOTOR_ENABLE,LOW);
			analogWrite(RIGHT_MOTOR_PWM,data[2].toInt());
		}
		else {
			digitalWrite(RIGHT_MOTOR_ENABLE,HIGH);
			analogWrite(RIGHT_MOTOR_PWM,255-data[2].toInt());
		}
	}
	// magnetometer
	else if (data[0].equals("heading")) {
		Serial.print(currHeading);
		Serial.print("$");
	}
}

/*************************************************************************/
/***********************************setup*********************************/
/*************************************************************************/
void setup() {
	// serial comm with pi
	Serial.begin(9600);
	Serial.flush();
	Serial1.begin(115200);
	Serial1.flush();

	pos = 0;
	recv_done = false;
	iter = 1;

	// motors
	pinMode(LEFT_MOTOR_PWM,OUTPUT);
	pinMode(LEFT_MOTOR_ENABLE,OUTPUT);
	digitalWrite(LEFT_MOTOR_ENABLE,LOW);
	analogWrite(LEFT_MOTOR_PWM,0);

	pinMode(RIGHT_MOTOR_PWM,OUTPUT);
	pinMode(RIGHT_MOTOR_ENABLE,OUTPUT);
	digitalWrite(RIGHT_MOTOR_ENABLE,LOW);
	analogWrite(RIGHT_MOTOR_PWM,0);

	pinMode(LED_PIN,OUTPUT);
	time_count = 0;
	loop10ms = 0;
	loop200ms = 0;
	loop1000ms = 0;
	// initialize timer1 
	noInterrupts();           // disable all interrupts
	TCCR1A = 0;
	TCCR1B = 0;

	tcnt1 = 49536;
	TCNT1 = tcnt1;            // preload timer 65536-16MHz
	TCCR1B |= (1 << CS10);    // 16MHz/1 prescaler 
	TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
	interrupts();             // enable all interrupts

	// Initialize the HMC6343 and verify its physical presence
	if (!compass.init()) {
		Serial1.println("HMC6343 Initialization Failed\n\r"); // Report failure, is the sensor wiring correct?
	}

	// ready for raspi
	Serial.print("Ready$");
	#if DEBUG
		Serial1.println("Ready");
	#endif
}

/*************************************************************************/
/***********************************loop**********************************/
/*************************************************************************/
void loop() {
	if (Serial.available()) {
		char c = Serial.read();
		rbuf[pos++] = c;
		if (c == '\0') {
			pos = 0;
			recv_done = true;
		}
	}
	
	if (recv_done) {
		rdata = String(rbuf);
		#if DEBUG
			Serial1.println(rdata);
		#endif
		check_command(rdata);
		recv_done = false;
	}
	
	if (loop10ms) {
		loop10ms = 0;
	}

	if (loop200ms) {
		compass.readHeading();
		currHeading = compass.heading;
		loop200ms = 0;
	}

	if (loop1000ms) {
		loop1000ms = 0;
		digitalWrite(LED_PIN,digitalRead(LED_PIN)^1);
	}
}

/*************************************************************************/
/******************************timer_interrupt****************************/
/*************************************************************************/
ISR(TIMER1_OVF_vect) {
	TCNT1 = tcnt1; // preload timer
	time_count++;
	if ((time_count%10) == 0) {
		loop10ms = 0;
	}
	if ((time_count%200) == 0) {
		loop200ms = 1;
	}
	if ((time_count%1000) == 0) {
		loop1000ms = 0;
	}
}