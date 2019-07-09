#ifndef _FDC2214_H_
#define _FDC2214_H_

#include <Wire.h>

#if ARDUINO >= 100
#include "Arduino.h"
#define WIRE_WRITE Wire.write
#else
#include "WProgram.h"
#define WIRE_WRITE Wire.send
#endif

#if defined(__SAM3X8E__)
typedef volatile RwReg PortReg;
 typedef uint32_t PortMask;
#define HAVE_PORTREG
#elif defined(ARDUINO_ARCH_SAMD)
// not supported
#elif defined(ESP8266) || defined(ESP32) || defined(ARDUINO_STM32_FEATHER) || defined(__arc__)
typedef volatile uint32_t PortReg;
  typedef uint32_t PortMask;
#elif defined(__AVR__)
typedef volatile uint8_t PortReg;
  typedef uint8_t PortMask;
#define HAVE_PORTREG
#else
// chances are its 32 bit so assume that
typedef volatile uint32_t PortReg;
typedef uint32_t PortMask;
#endif

#define FDC2214_I2C_ADDR_0   0x2A
#define FDC2214_I2C_ADDR_1   0x2B
// Address is 0x2A (default) or 0x2B (if ADDR is high)

//bitmasks
#define FDC2214_CH0_UNREADCONV 0x0008         //denotes unread CH0 reading in STATUS register
#define FDC2214_CH1_UNREADCONV 0x0004         //denotes unread CH1 reading in STATUS register
#define FDC2214_CH2_UNREADCONV 0x0002         //denotes unread CH2 reading in STATUS register
#define FDC2214_CH3_UNREADCONV 0x0001         //denotes unread CH3 reading in STATUS register


//registers
#define FDC2214_DATA_CH0_MSB	            0x00
#define FDC2214_DATA_CH0_LSB    		    0x01
#define FDC2214_DATA_CH1_MSB	            0x02
#define FDC2214_DATA_CH1_LSB    		    0x03
#define FDC2214_DATA_CH2_MSB	            0x04
#define FDC2214_DATA_CH2_LSB    		    0x05
#define FDC2214_DATA_CH3_MSB	            0x06
#define FDC2214_DATA_CH3_LSB    		    0x07

#define FDC2214_RCOUNT_CH0          		0x08
#define FDC2214_RCOUNT_CH1          		0x09
#define FDC2214_RCOUNT_CH2          		0x0A
#define FDC2214_RCOUNT_CH3          		0x0B

#define FDC2214_OFFSET_CH0		          	0x0C
#define FDC2214_OFFSET_CH1          		0x0D
#define FDC2214_OFFSET_CH2    			    0x0E
#define FDC2214_OFFSET_CH3         			0x0F

#define FDC2214_SETTLECOUNT_CH0     		0x10
#define FDC2214_SETTLECOUNT_CH1     		0x11
#define FDC2214_SETTLECOUNT_CH2     		0x12
#define FDC2214_SETTLECOUNT_CH3     		0x13

#define FDC2214_CLOCK_DIVIDERS_CH0  		0x14
#define FDC2214_CLOCK_DIVIDERS_CH1  		0x15
#define FDC2214_CLOCK_DIVIDERS_CH2  		0x16
#define FDC2214_CLOCK_DIVIDERS_CH3  		0x17

#define FDC2214_STATUS              		0x18
#define FDC2214_CONFIG              		0x1A
#define FDC2214_MUX_CONFIG          		0x1B

#define FDC2214_DRIVE_CH0           		0x1E
#define FDC2214_DRIVE_CH1           		0x1F
#define FDC2214_DRIVE_CH2           		0x20
#define FDC2214_DRIVE_CH3           		0x21

#define FDC2214_DEVICE_ID           		0x7F

// mask for 28bit data to filter out flag bits
#define FDC2214_DATA_CHx_MASK_DATA         	0x0FFF  
#define FDC2214_DATA_CHx_MASK_ERRAW        	0x1000  
#define FDC2214_DATA_CHx_MASK_ERRWD        	0x2000  

#define CHAN_COUNT 4
#define FDC2214_I2C_ADDR FDC2214_I2C_ADDR_0

#define CH_SEL 3

unsigned long enviroment[4] = {};

void fdcReport(unsigned int result) {
	Serial.print("FDC2214 reports:\n");
	Serial.print("--The result is ");
	Serial.print(result);
	Serial.print(".\n");
}

class FDC2214 {
public:
    void init(uint8_t chanMask, uint8_t autoscanSeq, uint8_t deglitchValue);
    unsigned long getReading(uint8_t channel);

private:
    void write16FDC(uint16_t address, uint16_t data);
    uint16_t read16FDC(uint16_t address);
};


void FDC2214::write16FDC(uint16_t address, uint16_t data) {
	Wire.beginTransmission(FDC2214_I2C_ADDR);
	Wire.write(address & 0xFF);
	Wire.write(data >> 8);
	Wire.write(data);
	Wire.endTransmission();
}

uint16_t FDC2214::read16FDC(uint16_t address) {
	uint16_t data;

	Wire.beginTransmission(FDC2214_I2C_ADDR);
	//    Wire.write(address >> 8);
	Wire.write(address);
	Wire.endTransmission(false); //restart

	Wire.requestFrom((uint8_t)FDC2214_I2C_ADDR, (uint8_t)2);
	while (!Wire.available());
	data = Wire.read();
	data <<= 8;
	while (!Wire.available());
	data |= Wire.read();
	Wire.endTransmission(true); //end
	return data;
}

unsigned long FDC2214::getReading(uint8_t channel) {
	int timeout = 100;
	unsigned long reading = 0;
	long long fsensor = 0;
	int status = read16FDC(FDC2214_STATUS);
	uint8_t addressMSB;
	uint8_t addressLSB;
	uint8_t bitUnreadConv;
	switch (channel) {
	case (0):
		addressMSB = FDC2214_DATA_CH0_MSB;
		addressLSB = FDC2214_DATA_CH0_LSB;
		bitUnreadConv = FDC2214_CH0_UNREADCONV;
		break;
	case (1):
		addressMSB = FDC2214_DATA_CH1_MSB;
		addressLSB = FDC2214_DATA_CH1_LSB;
		bitUnreadConv = FDC2214_CH1_UNREADCONV;
		break;
	case (2):
		addressMSB = FDC2214_DATA_CH2_MSB;
		addressLSB = FDC2214_DATA_CH2_LSB;
		bitUnreadConv = FDC2214_CH2_UNREADCONV;
		break;
	case (3):
		addressMSB = FDC2214_DATA_CH3_MSB;
		addressLSB = FDC2214_DATA_CH3_LSB;
		bitUnreadConv = FDC2214_CH3_UNREADCONV;
		break;
	default: return 0;
	}

	while (timeout && !(status & bitUnreadConv)) {
		status = read16FDC(FDC2214_STATUS);
		timeout--;
	}
	if (timeout == 100) {
		// #####################################################################################################
		// There was this weird double read, as "first readout could be stale" in Nelsons file. 
		// I have not confirmed the existence of this silicon bug.
		// I suspect that it might be due to crappy breadboard or rats nest wiring or lack of signal integrity for other reason
		// 
		// On the other hand, I have done far too little testing to be sure, so I am leaving that bit in for now.
		//	
		// #####################################################################################################
		//could be stale grab another //could it really it? ?????
		//read the 28 bit result
		reading = (uint32_t)(read16FDC(addressMSB) & FDC2214_DATA_CHx_MASK_DATA) << 16;
		reading |= read16FDC(addressLSB);
		while (timeout && !(status & FDC2214_CH0_UNREADCONV)) {
			status = read16FDC(FDC2214_STATUS);
			timeout--;
		}
	}
	if (timeout) {
		//read the 28 bit result
		reading = (uint32_t)(read16FDC(addressMSB) & FDC2214_DATA_CHx_MASK_DATA) << 16;
		reading |= read16FDC(addressLSB);
		return reading;
	}
	else {
		// Could not get data, chip readynes flag timeout
		reading = (uint32_t)(read16FDC(addressMSB) & FDC2214_DATA_CHx_MASK_DATA) << 16;
		reading |= read16FDC(addressLSB);
		return reading;
	}
}

void FDC2214::init(uint8_t chanMask = 0xF, uint8_t autoscanSeq = 0x6, uint8_t deglitchValue = 0x5) {
	Wire.begin();
	Serial.print("I2C reports:\n");
	Serial.print("--I am OK!\n");
	//Configuration register
	//	Active channel Select: b00 = ch0; b01 = ch1; b10 = ch2; b11 = ch3;
	//  |Sleep Mode: 0 - device active; 1 - device in sleep;
	//  ||Reserved, reserved, set to 1
	//  |||Sensor Activation Mode: 0 - drive sensor with full current. 1 - drive sensor with current set by DRIVE_CURRENT_CHn 
	//  ||||Reserved, set to 1
	//  |||||Reference clock: 0 - use internal; 1 - use external clock
	//  ||||||Reserved, set to 0
	//  |||||||Disable interrupt. 0 - interrupt output on INTB pin; 1 - no interrupt output
	//  ||||||||High current sensor mode: 0 - 1.5mA max. 1 - > 1.5mA, not available if Autoscan is enabled
	//  |||||||||     Reserved, set to 000001
	//  |||||||||     |
	// CCS1A1R0IH000000 -> 0001 1110 1000 0001 -> 0x1E81
	write16FDC(FDC2214_CONFIG, 0x1C81);  //set config

										 //If channel 1 selected, init it..
	if (chanMask & 0x01) {

		//settle count maximized, slow application
		write16FDC(FDC2214_SETTLECOUNT_CH0, 0x64);

		//rcount maximized for highest accuracy
		write16FDC(FDC2214_RCOUNT_CH0, 0xFFFF);

		//no offset
		write16FDC(FDC2214_OFFSET_CH0, 0x0000);

		// Set clock dividers
		//  Reserved
		//  | Sensor Frequency Select. b01 = /1 = sensor freq 0.01 to 8.75MHz; b10 = /2 = sensor freq 0.01 to 10 or 5 to 10 MHz
		//  | | Reserved
		//  | | |         Reference divider. Must be > 1. fref = fclk / this register`
		//  | | |         |
		// 00FF00RRRRRRRRRR -> 0010000000000001 -> 0x2001
		write16FDC(FDC2214_CLOCK_DIVIDERS_CH0, 0x2001);
		//set drive register
		write16FDC(FDC2214_DRIVE_CH0, 0xF800);
	}
	// Init chan2, if selected by channel init mask
	if (chanMask & 0x02) {
		write16FDC(FDC2214_SETTLECOUNT_CH1, 0x64);
		write16FDC(FDC2214_RCOUNT_CH1, 0xFFFF);
		write16FDC(FDC2214_OFFSET_CH1, 0x0000);
		write16FDC(FDC2214_CLOCK_DIVIDERS_CH1, 0x2001);
		write16FDC(FDC2214_DRIVE_CH1, 0xF800);
	}
	if (chanMask & 0x04) {
		write16FDC(FDC2214_SETTLECOUNT_CH2, 0x64);
		write16FDC(FDC2214_RCOUNT_CH2, 0xFFFF);
		write16FDC(FDC2214_OFFSET_CH2, 0x0000);
		write16FDC(FDC2214_CLOCK_DIVIDERS_CH2, 0x2001);
		write16FDC(FDC2214_DRIVE_CH2, 0xF800);
	}
	if (chanMask & 0x08) {
		write16FDC(FDC2214_SETTLECOUNT_CH3, 0x64);
		write16FDC(FDC2214_RCOUNT_CH3, 0xFFFF);
		write16FDC(FDC2214_OFFSET_CH3, 0x0000);
		write16FDC(FDC2214_CLOCK_DIVIDERS_CH3, 0x2001);
		write16FDC(FDC2214_DRIVE_CH3, 0xF800);
	}
	// Autoscan: 0 = single channel, selected by CONFIG.ACTIVE_CHAN
	// | Autoscan sequence. b00 for chan 1-2, b01 for chan 1-2-3, b02 for chan 1-2-3-4
	// | |         Reserved - must be b0001000001
	// | |         |  Deglitch frequency. b001 for 1 MHz, b100 for 3.3 MHz, b101 for 10 Mhz, b111 for 33 MHz
	// | |         |  |
	// ARR0001000001DDD -> b0000 0010 0000 1000 -> h0208
	uint16_t muxVal = 0x0208 | ((uint16_t)autoscanSeq << 13) | deglitchValue;
	//
	write16FDC(FDC2214_MUX_CONFIG, muxVal);  //set mux config for channels
	
	Serial.print("FDC2214 reports:\n");
	if (read16FDC(FDC2214_DEVICE_ID) == 0x3055)
		Serial.print("--I am OK!\n");
	else
		Serial.print("--I fail!\n");
}

FDC2214 fdc;

#endif