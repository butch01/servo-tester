#include "Arduino.h"
#include <MegaServo.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif


#define FILTER_SIZE 1 // size of array for poti readings Average value will be used if > 1. If 1 its 1 to 1 reading.


unsigned char potPin= A0;  // analog pin used to connect the potentiometer
unsigned char servoPin = 9; // pin where servo signal is connected

char version[] = "1.0.0";

int val;    // average servo value
int valArray[FILTER_SIZE];
unsigned char lastFilterValue=0;


bool isInvertPotiDirection = true; // invert poti direction if needed


MegaServo myServo;  // create servo object to control a servo
U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

int displayWith=0;

#define SPACE_BORDER 28
#define DISPLAY_REFRESH_LIMIT_MILLIS 250
unsigned long lastDisplayRefresh=0;



//The setup function is called once at startup of the sketch
void setup()
{
	Serial.begin(115200);
	myServo.attach(servoPin);
	display.begin();
	display.setContrast(150);
	displayWith = display.getDisplayWidth();

	display.clearBuffer();					// clear the internal memory
	display.setFont(u8g2_font_crox5tb_tr);	// choose a suitable font
	display.setCursor(30, 30);

	display.print("version");
	display.setCursor(40, 60);
	display.print(version);
	display.sendBuffer();
	delay (2000);

}

// The loop function is called in an endless loop
void loop()
{

	valArray[lastFilterValue] = analogRead(potPin);
	if (lastFilterValue < FILTER_SIZE -1)
	{
		lastFilterValue++;
	}
	else
	{
		lastFilterValue=0;
	}


	val=0;
	for (unsigned char i=0; i< FILTER_SIZE; i++)
	{
		val = val + valArray[i];
	}
	val = val/FILTER_SIZE;

	// invert poti direction if configured
	if (isInvertPotiDirection)
	{
		val=map(val, 0,1023,1023,0);
	}

	updateDisplay();
	//	Serial.println(val);
	myServo.write(map(val,0,1023,0,180));                  // sets the servo position according to the scaled value

}

void updateDisplay()
{
	unsigned long currentTime = millis();
	if (lastDisplayRefresh + DISPLAY_REFRESH_LIMIT_MILLIS < currentTime)
	{
		unsigned char displayRightEnd = display.getDisplayWidth() - SPACE_BORDER;

		unsigned int y=20;
		display.clearBuffer();					// clear the internal memory
		display.setFont(u8g2_font_HelvetiPixel_tr);	// choose a suitable font
		display.setCursor(SPACE_BORDER, y);
		display.print("input:");
		display.setCursor(getXPositionAlignRight(val, displayRightEnd), y);
		display.print(val);

		y=35;
		display.setCursor(SPACE_BORDER, y);
		display.print("8bit: ");
		unsigned int myVal = map(val,0,1023,0,255);
		display.setCursor(getXPositionAlignRight(myVal, displayRightEnd), y);
		display.print(map(val,0,1023,0,255));

		y=50;
		display.setCursor(SPACE_BORDER, y);
		display.print("servo: ");
		myVal = map(val,0,1023,0,180);
		display.setCursor(getXPositionAlignRight(myVal, displayRightEnd), y);
		display.print(map(val,0,1023,0,180));

		display.sendBuffer();					// transfer internal memory to the display
		lastDisplayRefresh = currentTime;
	}
}

/**
 * gets cursorPosition (x axis) when using right alignment
 */
unsigned char getXPositionAlignRight(int intValue, unsigned int xEndPosition)
{
	String inputAsString;
	inputAsString = String(intValue);
	char valueAsChar[5];
	inputAsString.toCharArray(valueAsChar, 5);
	unsigned char width = (display.getUTF8Width(valueAsChar));
	int position = xEndPosition - width;
	if (position < 0)
	{
		position = 0 ;
	}
	return (unsigned char) position;
}
