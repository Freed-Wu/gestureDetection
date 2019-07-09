#include "inc\main.hpp"
void setup() {
	Serial.begin(115200);
	Serial.print("Serial reports:\n");
	Serial.print("--I am OK!\n");

fdc.init();
lcd.init();
keypadInit();

Serial.print("Serial reports:\n");
Serial.print("--The basement is ");
Serial.print(BASEMENT);
Serial.print(".\n");

}

/*API:
fdc.getReading(CH_SEL);
fdcReport(result);

keypad.getKey();
keyReport(key);

lcd.clear();
lcd.print(String str);
lcdReport();
*/
void loop() {
	//{@Plot.MyPlotWindow.Result.Green getResult()}
	lcd.clear();
	delay(1000);
	lcd.print("A:\xB2\xC2\xC8\xAD\xB2\xE2\xCA\xD4");//A:猜拳测试
	lcd.print("B:\xBB\xAE\xC8\xAD\xB2\xE2\xCA\xD4",1);//B:划拳测试
	lcd.print("1-5,7-9:\xD1\xB5\xC1\xB7", 2);//1-5,7-9:训练

	Serial.print("MCU reports:\n");\
	Serial.print("--I wait for a legal key!\n");
	key = NULL;
	while (legalKeys.indexOf(key) == -1 || key == NULL) {
		if (key != NULL) {
			keypadReport(key);
			Serial.print("MCU reports:\n");
			Serial.print("--I got a illegal key!\n");
		}
		key = keypad.getKey();
	}
	keypadReport(key);
	Serial.print("MCU reports:\n");
	Serial.print("--I got a legal key!\n");

	lcd.clear();
	switch (key)
	{
	case MORRA_TEST:
		indexNumber = 3;
		indexBegin = 7;
		delay(1000);
		lcd.print("\xB2\xC2\xC8\xAD\xB2\xE2\xCA\xD4");//猜拳测试
		break;

	case NUMBER_TEST:
		indexNumber = 5;
		indexBegin = 1;
		delay(1000);
		lcd.print("\xBB\xAE\xC8\xAD\xB2\xE2\xCA\xD4");//划拳测试
		break;

	default:
		charsTemp[9] = { key };
		index = atoi(charsTemp);
		delay(1000);
		lcd.print(strs[index]+"\xD1\xB5\xC1\xB7" );//训练
	}

	Serial.print("MCU reports:\n");
	Serial.print("--I wait for a stable result!\n");
	result = BASEMENT;
	cycle = 0;
	while (cycle < 2) {//检验2次
		while (result >= BASEMENT) {
	     	result = getResult(CH_SEL);
			itoa(result, charsTemp, 10);
			lcd.print(charsTemp, 1);
		}
		fdcReport(result);
		if (cycle == 0) {
			cycle++;//第一次检验完成
			cycleTemp = result;
			delay(CYCLE_DELAY);
		}
		else {
			Serial.print("MCU reports:\n");
			if (DIFFER(result, cycleTemp) < LIMIT) {//检测2次的结果是否相差太多决定是否稳定
				time = millis();
				cycle++;//第二次检验完成
				Serial.print("--I got a stable result!\n");
			}
			else {
				cycle--;//得到的结果与第一次相差太多，舍弃
				Serial.print("--I got a unstable result!\n");
			}
		}
	}
	for(int i = 0; i < SAMPLE_TIMES; i++)//采样20次
		result = DIFFER(result, getResult(CH_SEL));
	itoa(millis() - time, charsTemp, 10);
	Serial.print("--The Time is " + (String)charsTemp + ".\n");

	itoa((int)result, charsTemp, 10);
	lcd.print("\xBD\xE1\xB9\xFB:" + (String)charsTemp, 1);//结果:
	if (key == MORRA_TEST || key == NUMBER_TEST)
		for (int i = indexBegin; i < indexBegin + indexNumber; i++)
			index = DIFFER(record[i], result) < DIFFER(record[index], result) ? i : index;
	itoa(record[index], charsTemp, 10);
	lcd.print("\xBC\xC7\xC2\xBC:" + (String)charsTemp, 2);//记录:
	lcd.print("\xCA\xD6\xCA\xC6: " + strs[index], 3);//手势:
	if (key != MORRA_TEST && key != NUMBER_TEST)
		record[index] = result;
	delay(3000);
}
