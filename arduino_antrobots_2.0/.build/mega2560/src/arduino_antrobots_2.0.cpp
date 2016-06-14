#include <Arduino.h>
#include <string.h>
void check_command(String rdata);
void setup();
void loop();
#line 1 "src/arduino_antrobots_2.0.ino"
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
//#include <string.h>

/*************************************************************************/
/*********************************defines*********************************/
/*************************************************************************/
#define DEBUG 1

// pin definitions
#define LEFT_MOTOR_PWM 2
#define LEFT_MOTOR_ENABLE 30
#define RIGHT_MOTOR_PWM 3
#define RIGHT_MOTOR_ENABLE 31

/*************************************************************************/
/*********************************variables*******************************/
/*************************************************************************/
char rbuf[100];
String rdata;
String data[10];
int pos;
boolean recv_done;
int iter;

/*************************************************************************/
/******************************check_command******************************/
/*************************************************************************/
void check_command(String rdata) {
	int old_index = -1;
	int index;
	int i = 0;
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
		if (data[1].equals("0"))
			digitalWrite(LEFT_MOTOR_ENABLE,LOW);
		else
			digitalWrite(LEFT_MOTOR_ENABLE,HIGH);
		analogWrite(LEFT_MOTOR_PWM,data[2].toInt());
	}
	// right motor
	else if (data[0].equals("rm")) {
		if (data[1].equals("0"))
			digitalWrite(RIGHT_MOTOR_ENABLE,LOW);
		else
			digitalWrite(RIGHT_MOTOR_ENABLE,HIGH);
		analogWrite(RIGHT_MOTOR_PWM,data[2].toInt());
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
	Serial.print("Ready$");
	#if DEBUG
		Serial1.println("Ready");
	#endif
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
}
