/*
 Name:		PainelSenhas.ino
 Created:	4/24/2017 1:32:51 AM
 Author:	Erik Yeger Scudeler
*/

#include <LedControl.h>
#include <SPI.h>
#include <RH_ASK.h>
#include <IRremote.h>
#include <TimerFreeTone.h>

const int RECV_PIN = 9;
const int TONE_PIN = 2; // Close to GND pin
const int RF_BPS = 5000;

const long IR_KEY_1 = 0xC101E57B;
const long IR_KEY_2 = 0x97483BFB;
const long IR_KEY_3 = 0xF0C41643;
const long IR_KEY_4 = 0x9716BE3F;
const long IR_KEY_5 = 0x3D9AE3F7;
const long IR_KEY_6 = 0x6182021B;
const long IR_KEY_7 = 0x8C22657B;
const long IR_KEY_8 = 0x488F3CBB;
const long IR_KEY_9 = 0x449E79F;
const long IR_KEY_0 = 0x1BC0157B;
const long IR_KEY_STAR = 0x32C6FDF7;
const long IR_KEY_HASH = 0x3EC3FC1B;
const long IR_KEY_UP = 0x511DBB;
const long IR_KEY_DOWN = 0xA3C8EDDB;
const long IR_KEY_LEFT = 0x52A3D41F;
const long IR_KEY_RIGHT = 0x20FE4DBB;
const long IR_KEY_OK = 0xD7E84B1B;

const int NO_BEEP = 0;
const int BEEP_KEY = 1;
const int BEEP_ALERT = 2;
const int BEEP_LALA = 3;
const int BEEP_ACCEPT = 3;
const int SEND_REPEAT = 3;

/* Create a new LedControl variable.
* Pin 12 is connected to the DATA IN-pin of the first MAX7221
* Pin 11 is connected to the CLK-pin of the first MAX7221
* Pin 10 is connected to the LOAD(/CS)-pin of the first MAX7221
*/
LedControl lc = LedControl(12, 11, 10, 1);

/*
Create a new IR remote control results.
*/
IRrecv irrecv(RECV_PIN);
decode_results results;
long irvalue;

/*
RF Variables
*/
RH_ASK driver(RF_BPS, 5, 6);
/*
Application variables
*/
int oldValue = 0;
int currentValue = 0;
int inputValueA = 0;
int inputValueB = 0;
int sendIndex = 0;

/*
Functions
*/
void beep(int beepType) {
    switch (beepType)
    {
        case BEEP_KEY:
          TimerFreeTone(TONE_PIN, 10000, 50);
          break;
        case BEEP_ALERT:
          TimerFreeTone(TONE_PIN, 5000, 200);
          break;
        case BEEP_ACCEPT:
          TimerFreeTone(TONE_PIN, 8000, 30);
          delay(20);
          TimerFreeTone(TONE_PIN, 8000, 30);
          break;
    }
}

void displayInput()
{
    lc.setDigit(0, 0, inputValueA, false);
    lc.setDigit(0, 1, inputValueB, false);
}

void sendPacket(int digitA, int digitB, int beep)
{
    sendIndex++;
    sendIndex = sendIndex % 10;

    char buffer[8];
    sprintf(buffer, "D%d%dB%dS%d\0", digitA, digitB, beep, sendIndex);
    
    // Enviar várias vezes pra ter certeza que chegou no display
    for (int i = 0; i < SEND_REPEAT; i++)
    {
      driver.send((uint8_t *)buffer, strlen(buffer));
      driver.waitPacketSent();
      delay(200);
    }
  
}

void displayCurrent(int beep)
{
    int digitA = (int)(currentValue / 10);
    int digitB = (int)(currentValue % 10);
    lc.setDigit(0, 0, digitA, false);
    lc.setDigit(0, 1, digitB, false);

    sendPacket(digitA, digitB, beep);
}    

void setDigit(int digit)
{
    inputValueA = inputValueB;
    inputValueB = digit;
    displayInput();
}

void selfTest()
{
    // Internal Display
    for (int i = 0; i < 100; i++)
    {
        inputValueA = (int)(i / 10);
        inputValueB = (int)(i % 10);
        displayInput();
        delay(10);
    }

    // Internal Beeps
    beep(BEEP_KEY);
    delay(200);
    beep(BEEP_ALERT);
    delay(200);
    beep(BEEP_ACCEPT);
    delay(200);

    lc.clearDisplay(0);
    
    // External Displays
    for (int i = 0; i < 10; i++)
    {
        currentValue = (i * 10) + i;
        displayCurrent(NO_BEEP);
    }

    // Reset
    inputValueA = 0;
    inputValueB = 0;
    currentValue = 0;
    displayCurrent(BEEP_ALERT);
}

/*
Setup
*/
void setup() {
  	Serial.begin(9600);

    if (!driver.init()) {
        Serial.println("init failed");   
    }
  
  	// Initialize Display
  	lc.shutdown(0, false);
  	lc.setIntensity(0, 10);
  	lc.clearDisplay(0);

    // Self-test
    selfTest();
    
  	// Initialize IR remote control
  	irrecv.enableIRIn();
}

/*
Loop
*/
void loop() {
  
	if (irrecv.decode(&results))
	{
		irvalue = (results.value);

		switch (irvalue)
		{
			case IR_KEY_1: 
        beep(BEEP_KEY);
        setDigit(1);
				break;
			case IR_KEY_2:
        beep(BEEP_KEY);
        setDigit(2);
				break;
      case IR_KEY_3:
        beep(BEEP_KEY);
        setDigit(3);
        break;
      case IR_KEY_4:
        beep(BEEP_KEY);
        setDigit(4);
        break;
      case IR_KEY_5:
        beep(BEEP_KEY);
        setDigit(5);
        break;
      case IR_KEY_6:
        beep(BEEP_KEY);
        setDigit(6);
        break;
      case IR_KEY_7:
        beep(BEEP_KEY);
        setDigit(7);
        break;
      case IR_KEY_8:
        beep(BEEP_KEY);
        setDigit(8);
        break;
      case IR_KEY_9:
        beep(BEEP_KEY);
        setDigit(9);
        break;
      case IR_KEY_0:
        beep(BEEP_KEY);
        setDigit(0);
        break;

      case IR_KEY_STAR:
        beep(BEEP_KEY);
        inputValueA = -1;
        inputValueB = -1;
        displayCurrent(NO_BEEP);
        break;
			
      case IR_KEY_HASH:
        beep(BEEP_KEY);
        displayCurrent(BEEP_KEY);
        break;

      case IR_KEY_OK:
        beep(BEEP_KEY);

        if (inputValueA == -1 || inputValueB == -1) {
          displayCurrent(BEEP_KEY);
          inputValueA = -1;
          inputValueB = -1;          
        } else {
          oldValue = currentValue;
          currentValue = (inputValueA * 10) + inputValueB;
          displayCurrent(BEEP_KEY);
          inputValueA = -1;
          inputValueB = -1;
        }
        break;
			
      case IR_KEY_UP:
        beep(BEEP_KEY);
        oldValue = currentValue;
        currentValue++;
        if (currentValue == 100) currentValue = 0;
        displayCurrent(BEEP_KEY);
        break;
			
      case IR_KEY_DOWN:
        beep(BEEP_KEY);
        oldValue = currentValue;
        currentValue--;
        if (currentValue == -1) currentValue = 99;
        displayCurrent(BEEP_KEY);
        break;

      case IR_KEY_LEFT:
        beep(BEEP_KEY);
        inputValueA = (int)(oldValue / 10);
        inputValueB = (int)(oldValue % 10);
        displayInput();
        break;

      /*
      case IR_KEY_RIGHT:
        char buffer[8];
        sprintf(buffer, "VOL%d\0", inputValueB);
        
        // Enviar várias vezes pra ter certeza que chegou no display
        for (int i = 0; i < SEND_REPEAT; i++)
        {
          driver.send((uint8_t *)buffer, strlen(buffer));
          driver.waitPacketSent();
          delay(200);
        }
        break;
        */

			default:
				// Serial.println(irvalue, HEX);
				break;
		}

		irrecv.resume();
	}


}
