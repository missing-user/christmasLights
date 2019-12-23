/*
  ESP8266_Test.ino - Simple sketch to listen for E1.31 data on an ESP8266
                     and print some statistics.

  Project: E131 - E.131 (sACN) library for Arduino
  Copyright (c) 2015 Shelby Merrick
  http://www.forkineye.com

   This program is provided free for you to use in any way that you wish,
   subject to the laws and regulations where you are using it.  Due diligence
   is strongly suggested before using this code.  Please give credit where due.

   The Author makes no warranty of any kind, express or implied, with regard
   to this program or the documentation contained in this document.  The
   Author shall not be liable in any event for incidental or consequential
   damages in connection with, or arising out of, the furnishing, performance
   or use of these programs.

*/
#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>
FASTLED_USING_NAMESPACE

#include <ESP8266WiFi.h>
#include <E131.h>

#define green_pin 14
#define red_pin 15
#define blue_pin 12

#define NUM_LEDS1    100
CRGB leds1[NUM_LEDS1];
#define NUM_LEDS2    50
CRGB leds2[NUM_LEDS2];

int targetPwm[] = {0, 0, 0};
int currentPwm[] = {0, 0, 0};
unsigned long lastMillisPwm = 0;

const char ssid[] = "Jurasic_Park";         /* Replace with your SSID */
const char passphrase[] = "nobodyExpectedThis";   /* Replace with your WPA2 passphrase */

E131 e131;

void setup() {
  Serial.begin(115200);
  delay(10);

  FastLED.addLeds<WS2811, 2, RGB >(leds1, NUM_LEDS1).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2812B, 13, BGR>(leds2, NUM_LEDS2).setCorrection(TypicalLEDStrip);
  pinMode(4, OUTPUT);
  pinMode(green_pin, OUTPUT);
  pinMode(red_pin, OUTPUT);
  pinMode(blue_pin, OUTPUT);
  /* Choose one to begin listening for E1.31 data */
  e131.begin(ssid, passphrase);               /* via Unicast on the default port */
  //e131.beginMulticast(ssid, passphrase, 1); /* via Multicast for Universe 1 */
}

void loop() {

  uint16_t num_channels = e131.parsePacket();

  if (num_channels) {
    digitalWrite(4, e131.data[0] > 128);

    targetPwm[0] =  e131.data[1] * 4;
    targetPwm[1] =  e131.data[2] * 4;
    targetPwm[2] =  e131.data[3] * 4;

    for (int i = 0 ; i < NUM_LEDS1; i++)
    {
      leds1[i].r = e131.data[i * 3 + 4];
      leds1[i].g = e131.data[i * 3 + 5];
      leds1[i].b = e131.data[i * 3 + 6];
    }
    for (int i = 0; i < NUM_LEDS2; i++)
    {
      leds2[i].r = e131.data[NUM_LEDS1 * 3 + i * 3 + 4];
      leds2[i].g = e131.data[NUM_LEDS1 * 3 + i * 3 + 5];
      leds2[i].b = e131.data[NUM_LEDS1 * 3 + i * 3 + 6];
    }
    FastLED.show();
  }


  if (millis() - lastMillisPwm > 5) {
    lastMillisPwm = millis();
    for (int i = 0; i < 3; i++) {
      if (targetPwm[i] - currentPwm[i] > 20)
        currentPwm[i] = targetPwm[i];
      else if (targetPwm[i] > currentPwm[i])
        currentPwm[i]++;
      else if (targetPwm[i] < currentPwm[i])
        currentPwm[i]--;
    }
  }

  //analogWrite(red_pin, currentPwm[0]);
  //analogWrite(green_pin, currentPwm[1]);
  //analogWrite(blue_pin, currentPwm[2]);
}
