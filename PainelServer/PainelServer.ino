/*      
 *    - 8 -  
 * 5 |     | 6
 *    - 4 -
 * 9 |     | 7
 *    - 3 -
 *  GND Verde
 *  VCC AZUL  
 *  RX ROXO
 *  TX CINZA
 */

#include <RH_ASK.h>
#include <SPI.h>
#include "pitches.h"
#include <MsTimer2.h>
// #include <TimerFreeTone.h>
// #include <EEPROM.h>

const int EEPROM_ADDR_VOLUME = 0;

const int PIN_SEG_A = 8;
const int PIN_SEG_B = 6;
const int PIN_SEG_C = 5;
const int PIN_SEG_D = 4;
const int PIN_SEG_E = 9;
const int PIN_SEG_F = 7;
const int PIN_SEG_G = 3;
const int PIN_SEG_H = 2;
const int PIN_DIG_0 = 12;
const int PIN_DIG_1 = 13;
const int PIN_RF_RX = 11;
const int RF_BPS = 5000;
const int PIN_SPEAKER = 10;

// PORTD - 0 a 7
// PORTB - 8 a 13

const int PORTDMASK = B00000011;
const int PORTBMASK = B11111100;
const int NUMD_0 = B11101000; // Digito 0, Segmentos 8, 6, 5, 9, 7, 3
const int NUMB_0 = B00000011;
const int NUMD_1 = B00100000; // Digito 1, Segmentos 5, 9
const int NUMB_1 = B00000010;
const int NUMD_2 = B01011000; // Digito 2, Segmentos 8, 6, 4, 9, 3
const int NUMB_2 = B00000011;
const int NUMD_3 = B11011000; // Digito 3, Segmentos 8, 6, 4, 7, 3
const int NUMB_3 = B00000001;
const int NUMD_4 = B11110000; // Digito 4, Segmentos 6, 5, 4, 7
const int NUMB_4 = B00000000;
const int NUMD_5 = B10111000; // Digito 5, Segmentos 8, 5, 4, 7, 3
const int NUMB_5 = B00000001;
const int NUMD_6 = B10111000; // Digito 6, Segmentos 8, 5, 4, 9, 7, 3
const int NUMB_6 = B00000011;
const int NUMD_7 = B11100000; // Digito 7, Segmentos 6, 8, 5, 7
const int NUMB_7 = B00000001;
const int NUMD_8 = B11111000; // Digito 8, Segmentos 8, 6, 5, 4, 9, 7, 3
const int NUMB_8 = B00000011;
const int NUMD_9 = B11110000; // Digito 9, Segmentos 8, 6, 5, 4, 7
const int NUMB_9 = B00000001;

int mapD[] = {NUMD_0, NUMD_1, NUMD_2, NUMD_3, NUMD_4, NUMD_5, NUMD_6, NUMD_7, NUMD_8, NUMD_9};
int mapB[] = {NUMB_0, NUMB_1, NUMB_2, NUMB_3, NUMB_4, NUMB_5, NUMB_6, NUMB_7, NUMB_8, NUMB_9};

int currentDigit = 0;
int sendIndex = 0;
int currentDigitA = 0;
int currentDigitB = 0;
int currentBeep = 0;
byte volume = 8;

// Test Song
int testMelody[] = { NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4 };
int testDurations[] = { 4, 8, 8, 4, 4, 4, 4, 4 };

// 2000 bits per second, RX on PIN 11
RH_ASK driver(RF_BPS, PIN_RF_RX);

uint8_t buf[8];
uint8_t buflen = sizeof(buf);

void selfTest()
{

}

void setup()
{
    Serial.begin(9600);
    Serial.println("comecou");

    if (!driver.init())
    {
        Serial.println("init failed");
    }

    pinMode(PIN_SEG_A, OUTPUT); 
    pinMode(PIN_SEG_B, OUTPUT); 
    pinMode(PIN_SEG_C, OUTPUT); 
    pinMode(PIN_SEG_D, OUTPUT); 
    pinMode(PIN_SEG_E, OUTPUT); 
    pinMode(PIN_SEG_F, OUTPUT); 
    pinMode(PIN_SEG_G, OUTPUT); 
    pinMode(PIN_SEG_H, OUTPUT); 
    pinMode(PIN_DIG_0, OUTPUT); 
    pinMode(PIN_DIG_1, OUTPUT); 
  
    // Clear all digits
    PORTD = PORTD & PORTDMASK;
    PORTB = PORTB & PORTBMASK;
  
    digitalWrite(PIN_DIG_0, LOW);
    digitalWrite(PIN_DIG_1, LOW);
  
    // Reads volume from EEPROM and do a sanity check
    /*
    volume = EEPROM.read(EEPROM_ADDR_VOLUME);
    if (volume > 10) volume = 10;
    if (volume < 0) volume = 0;
    */

    selfTest();
}

void callback()
{
    if (currentDigit == 0)
    {    
      // Set the current digit
      PORTD = PORTD | mapD[currentDigitB];
      PORTB = PORTB | mapB[currentDigitB];
  
      digitalWrite(PIN_DIG_0, LOW);
      digitalWrite(PIN_DIG_1, HIGH);

      currentDigit = 1;
    } else {
      // Set the current digit
      PORTD = PORTD | mapD[currentDigitA];
      PORTB = PORTB | mapB[currentDigitA];

      digitalWrite(PIN_DIG_0, HIGH);
      digitalWrite(PIN_DIG_1, LOW);

      currentDigit = 0;
    }
}

void loop()
{
    // Clear all digits
    PORTD = PORTD & PORTDMASK;
    PORTB = PORTB & PORTBMASK;

    if (driver.available())
    {
        if (driver.recv(buf, &buflen)) // Non-blocking
        {
            int currentIndex = 0;
        
            String str = (char*)buf;
            
            if (str.substring(0, 1) == "D" && str.substring(3, 4) == "B" && str.substring(5, 6) == "S")
            {
                currentIndex = str.substring(6, 7).toInt();
                if (currentIndex != sendIndex)
                {
                    sendIndex = currentIndex;
                    // 0123456
                    // D..B.S.
                    currentDigitA = str.substring(1, 2).toInt();
                    currentDigitB = str.substring(2, 3).toInt();
                    currentBeep = str.substring(4, 5).toInt();
                    
                    digitalWrite(PIN_DIG_0, HIGH);
                    digitalWrite(PIN_DIG_1, HIGH);
            
                    PORTD = PORTD | B00010000;
                    PORTB = PORTB | B00000000;

                    if (currentBeep == 0)
                    {
                        noTone(PIN_SPEAKER);
                    }

                    if (currentBeep == 1)
                    {
                        Serial.println("beep1!");
                        tone(PIN_SPEAKER, NOTE_E5);
                        delay(100);
                        tone(PIN_SPEAKER, NOTE_C5);
                        delay(200);
                        noTone(PIN_SPEAKER);
                        delay(100);
                        /*
                        TimerFreeTone(PIN_SPEAKER, 8000, 100, volume);
                        delay(20);
                        TimerFreeTone(PIN_SPEAKER, 8000, 100, volume);
                        delay(20);
                        TimerFreeTone(PIN_SPEAKER, 8000, 100, volume);
                        delay(20);
                        */
                    }

                    if (currentBeep == 2)
                    {
                        Serial.println("beep2!");
                        
                        tone(PIN_SPEAKER, NOTE_E5);
                        delay(100);
                        tone(PIN_SPEAKER, NOTE_C5);
                        delay(200);
                        noTone(PIN_SPEAKER);
                        delay(100);
                    }
                }
            } else {
              Serial.println(str);
            }
            // else if (str.substring(0, 3) == "VOL") {
                // byte newvol = (byte)str.substring(3, 4).toInt();
                // EEPROM.write(EEPROM_ADDR_VOLUME, newvol);
                // TimerFreeTone(PIN_SPEAKER, 8000, 30, volume);
                // delay(20);
                // TimerFreeTone(PIN_SPEAKER, 8000, 30, volume);
            // }
        }
    } else {
        callback();
        delay(5);
    }
}
