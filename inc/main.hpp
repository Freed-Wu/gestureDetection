#ifndef MAIN_HPP
#define MAIN_HPP

#include <Arduino.h>
#include <Wire.h>
#include "FDC2214.hpp"
#include "keypad.hpp"
#include "LCD12864.hpp"

#define DIFFER(x,y) abs(max(x,y)-min(x,y))
#define getResult(i) fdc.getReading(i) / DIVISION

#define MORRA_TEST 'A'
#define NUMBER_TEST 'B'

#define ONE '1'
#define TWO '2'
#define THREE '3'
#define FOUR '4'
#define FIVE '5'

#define SCISSORS '7'
#define STONE '8'
#define CLOTH '9' 

#define RECORD_NUMBER 8
#define CYCLE_DELAY 150

#define SAMPLE_TIMES 20
#define STABLE_LIMIT 10

#define DIVISION 1000
#define LIMIT 50
#define BASEMENT enviroment[CH_SEL]/DIVISION-LIMIT

byte rowPins[KEYPAD_ROWS] = { 4,5,6,7 };
byte colPins[KEYPAD_COLS] = { 8,9,10,11 };
byte lcd12864Pins[3] = { 12,3,2 };

unsigned int result = BASEMENT;

char legalKeysChar[] = { ONE,TWO,THREE,FOUR,FIVE,SCISSORS,STONE,CLOTH,MORRA_TEST,NUMBER_TEST };
String legalKeys = legalKeysChar;
String strs[] = { "0 ","1 ","2 ","3 ","4 ","5 ","6 ","\xBC\xF4\xB5\xB6" ,"\xCA\xAF\xCD\xB7","\xB2\xBC" };

byte indexBegin = 0;
byte indexNumber = 0;
byte index = 0;

unsigned int record[RECORD_NUMBER + 2] = { 0,13443,13344,13218,13080,12796,0,13222,13495,12915 };

char charsTemp[9] = {};

byte cycle = 0;
unsigned int cycleTemp = 0;

unsigned long time = 0;

bool getStablity() {
	unsigned int resultMin = getResult(CH_SEL);
	unsigned int resultMax = getResult(CH_SEL);
	for (int i = 1; i < SAMPLE_TIMES; i++) {
		resultMin = min(resultMin, getResult(CH_SEL));
		resultMax = max(resultMax, getResult(CH_SEL));
	}
	if (DIFFER(resultMax, resultMin) < STABLE_LIMIT)
		return true;
	return false;
}

#endif
