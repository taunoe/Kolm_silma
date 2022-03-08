/**
 * Project: Kolm silma
 * File:    main.cpp
 * Hardware:Selfmade Arduino Uno
 *          W2812 RGB NeoPixels  x 3pc.
 *          RCWL-0516 Microwave Proximity sensor
 * Author:  Tauno Erik https://taunoerik.art/
 * Started: 08.03.2022
 **/

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// Built in LED
const uint8_t GREEN_LED_PIN = 13;
// NeoPixel
const uint8_t NEOPIXEL_PIN = 2;
const uint8_t NEOPIXEL_NUM = 3;
// Sensor
const uint8_t PROXIMITY_PIN = 3;
uint8_t proximity_val = 0;


Adafruit_NeoPixel pixels(NEOPIXEL_NUM, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  Serial.println("\"Kolm Silma\" starts!");

  // Initialize pins
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(PROXIMITY_PIN, INPUT);

  pixels.begin();
}

void loop() {
  pixels.clear(); // Set all pixel colors to 'off'

  proximity_val = digitalRead(PROXIMITY_PIN);

  if (proximity_val == HIGH) {
    Serial.println("High");
  } else {
    Serial.println("Low");
  }

  for(int i=0; i<NEOPIXEL_NUM; i++) { // For each pixel...

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    pixels.setPixelColor(i, pixels.Color(0, 150, 0));

    pixels.show();   // Send the updated pixel colors to the hardware.

    delay(500); // Pause before next pass through loop
  }

  /*
  digitalWrite(GREEN_LED_PIN, HIGH);
  delay(1000);
  digitalWrite(GREEN_LED_PIN, LOW);
  delay(1000);
  */
}