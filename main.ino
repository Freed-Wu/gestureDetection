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
	lcd.print("A:\xB2\xC2\xC8\xAD\xB2\xE2\xCA\xD4");//A:��ȭ����
	lcd.print("B:\xBB\xAE\xC8\xAD\xB2\xE2\xCA\xD4",1);//B:��ȭ����
	lcd.print("1-5,7-9:\xD1\xB5\xC1\xB7", 2);//1-5,7-9:ѵ��

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
		lcd.print("\xB2\xC2\xC8\xAD\xB2\xE2\xCA\xD4");//��ȭ����
		break;

	case NUMBER_TEST:
		indexNumber = 5;
		indexBegin = 1;
		delay(1000);
		lcd.print("\xBB\xAE\xC8\xAD\xB2\xE2\xCA\xD4");//��ȭ����
		break;

	default:
		charsTemp[9] = { key };
		index = atoi(charsTemp);
		delay(1000);
		lcd.print(strs[index]+"\xD1\xB5\xC1\xB7" );//ѵ��
	}

	Serial.print("MCU reports:\n");
	Serial.print("--I wait for a stable result!\n");
	result = BASEMENT;
	cycle = 0;
	while (cycle < 2) {//����2��
		while (result >= BASEMENT) {
	     	result = getResult(CH_SEL);
			itoa(result, charsTemp, 10);
			lcd.print(charsTemp, 1);
		}
		fdcReport(result);
		if (cycle == 0) {
			cycle++;//��һ�μ������
			cycleTemp = result;
			delay(CYCLE_DELAY);
		}
		else {
			Serial.print("MCU reports:\n");
			if (DIFFER(result, cycleTemp) < LIMIT) {//���2�εĽ���Ƿ����̫������Ƿ��ȶ�
				time = millis();
				cycle++;//�ڶ��μ������
				Serial.print("--I got a stable result!\n");
			}
			else {
				cycle--;//�õ��Ľ�����һ�����̫�࣬����
				Serial.print("--I got a unstable result!\n");
			}
		}
	}
	for(int i = 0; i < SAMPLE_TIMES; i++)//����20��
		result = DIFFER(result, getResult(CH_SEL));
	itoa(millis() - time, charsTemp, 10);
	Serial.print("--The Time is " + (String)charsTemp + ".\n");

	itoa((int)result, charsTemp, 10);
	lcd.print("\xBD\xE1\xB9\xFB:" + (String)charsTemp, 1);//���:
	if (key == MORRA_TEST || key == NUMBER_TEST)
		for (int i = indexBegin; i < indexBegin + indexNumber; i++)
			index = DIFFER(record[i], result) < DIFFER(record[index], result) ? i : index;
	itoa(record[index], charsTemp, 10);
	lcd.print("\xBC\xC7\xC2\xBC:" + (String)charsTemp, 2);//��¼:
	lcd.print("\xCA\xD6\xCA\xC6: " + strs[index], 3);//����:
	if (key != MORRA_TEST && key != NUMBER_TEST)
		record[index] = result;
	delay(3000);
}
