#ifndef LCD12864RSPI_H
#define LCD12864RSPI_H
#include <inttypes.h>
#include <Arduino.h>

#define DELAYTIME 80

extern byte lcd12864Pins[3];
const int clockPin = lcd12864Pins[0];
const int dataPin = lcd12864Pins[1];
const int latchPin = lcd12864Pins[2];

void lcdReport(String str)
{
	Serial.print("LCD12864 reports:\n");
	Serial.print("--The display is \"");
	Serial.print(str);
	Serial.print("\"!\n");
}

class LCD12864RSPI {
public:
	void init();

	void WriteByte(int dat);
	void WriteCommand(int CMD);
	void WriteData(int CMD);

	void clear();
	void print(String ptr,int X, int Y,bool disp);
	void DrawFullScreen(String p);

};

extern "C" {
#include <inttypes.h>
#include <stdio.h>  //not needed yet
#include <string.h> //needed for strlen()
}

void LCD12864RSPI::WriteByte(int dat)
{
	digitalWrite(latchPin, HIGH);
	delayMicroseconds(DELAYTIME);
	shiftOut(dataPin, clockPin, MSBFIRST, dat);
	digitalWrite(latchPin, LOW);
}

void LCD12864RSPI::WriteCommand(int CMD)
{
	int H_data, L_data;
	H_data = CMD;
	H_data &= 0xf0;
	L_data = CMD;
	L_data &= 0x0f;
	L_data <<= 4;
	WriteByte(0xf8);
	WriteByte(H_data);
	WriteByte(L_data);
}


void LCD12864RSPI::WriteData(int CMD)
{
	int H_data, L_data;
	H_data = CMD;
	H_data &= 0xf0;
	L_data = CMD;
	L_data &= 0x0f;
	L_data <<= 4;
	WriteByte(0xfa);
	WriteByte(H_data);
	WriteByte(L_data);
}

void LCD12864RSPI::clear(void)
{
	WriteCommand(0x30);
	WriteCommand(0x01);
}

void LCD12864RSPI::print(String ptr, int X = 0, int Y = 0, bool disp = false) {
	switch (X) {
	case 0:
		Y |= 0x80;
		break;
	case 1:
		Y |= 0x90;
		break;
	case 2:
		Y |= 0x88;
		break;
	case 3:
		Y |= 0x98;
	}
	WriteCommand(Y);

	for (int i = 0; i<ptr.length(); i++)	{
		WriteData(ptr[i]);
	}
	if (disp) {
		delay(100);
		lcdReport(ptr);
	}
}

void LCD12864RSPI::DrawFullScreen(String p)
{
	int ygroup, x, y, i;
	int temp;
	int tmp;

	for (ygroup = 0; ygroup<64; ygroup++){
		if (ygroup<32)
		{
			x = 0x80;
			y = ygroup + 0x80;
		}
		else
		{
			x = 0x88;
			y = ygroup - 32 + 0x80;
		}
		WriteCommand(0x34);
		WriteCommand(y);
		WriteCommand(x);
		WriteCommand(0x30);
		tmp = ygroup * 16;
		for (i = 0; i<16; i++)
		{
			temp = p[tmp++];
			WriteData(temp);
		}
	}
	WriteCommand(0x34);
	WriteCommand(0x36);
}

void LCD12864RSPI::init()
{
	pinMode(latchPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(dataPin, OUTPUT);
	digitalWrite(latchPin, LOW);
	delayMicroseconds(DELAYTIME);

	WriteCommand(0x30);
	WriteCommand(0x0c);
	WriteCommand(0x01);
	WriteCommand(0x06);
	print("    \xBB\xB6\xD3\xAD\xCA\xB9\xD3\xC3");



	
	delay(1000);
	clear();

	delay(1000);
	for (int i = 0; i < 4; i++) {
		enviroment[i] = fdc.getReading(i);
	}
	Serial.print("LCD12864 reports:\n");
	Serial.print("--I am OK!\n");
}




LCD12864RSPI lcd;

#endif
