#ifndef KEYPAD_HPP
#define KEYPAD_HPP

#include <Arduino.h>
#include <Keypad.h>

#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4

extern byte rowPins[KEYPAD_ROWS];
extern byte colPins[KEYPAD_COLS];

const char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
	{ '1','2','3','A' },
{ '4','5','6','B' },
{ '7','8','9','C' },
{ '*','0','#','D' }
};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);

void keypadReport(char key)
{
	Serial.print("Keypad reports:\n");
	Serial.print("--The key is \"");
	Serial.print(key);
	Serial.print("\".\n");
}

void keypadInit()
{
	Serial.print("Keypad reports:\n");
	Serial.print("--I am OK!\n");
}

char key = keypad.getKey();

#endif
