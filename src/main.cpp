/**
 * Project: Kolm silma
 * File:    main.cpp
 * Hardware:Selfmade Arduino Uno
 *          W2812 RGB NeoPixels  x 3pc.
 *          RCWL-0516 Microwave Proximity sensor
 * Author:  Tauno Erik https://taunoerik.art/
 * Started: 08.03.2022
 * Edited:  13.03.2022
 **/

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// Built in LED
const uint8_t GREEN_LED_PIN = 13;
// NeoPixel
const uint8_t PX_PIN = 2;
const uint8_t PX_NUM = 3;
// Sensor
const uint8_t PROXIMITY_PIN = 3;

const uint16_t OFF_TIME = 60000;
uint32_t no_movment_time = 0;

/* Global variables:
*/
uint32_t time_px_prev = 0;  // Millis
uint16_t px_interval = 50;  // Millis
uint8_t r = 0;
uint8_t g = 0;
uint8_t b = 0;
bool is_movement = false;
bool change_color = false;

Adafruit_NeoPixel Pixels(PX_NUM, PX_PIN, NEO_GRB + NEO_KHZ800);

/*******************************************/
// Functions

/*
  Returns new colour value.
*/
uint8_t new_value(uint8_t value) {
  long change_dir = random(2); // ++ or --
  long change_step = random(1, 10); // ++ or --

  if (change_dir >= 1) {
    value = value + change_step;
  } else {
    value = value - change_step;
  }

  return value;
}


void boot_blink() {
  uint16_t del_time = 500;

  // Green LED on back
  for (uint8_t i = 0; i < 4; i++)
  {
    digitalWrite(GREEN_LED_PIN, HIGH);
    delay(del_time);
    digitalWrite(GREEN_LED_PIN, LOW);
    delay(del_time);
  }
  // Front pixels
  Pixels.clear();
}

/*******************************************/

void setup() {
  Serial.begin(115200);
  // Boot message
  Serial.print("Project: ");
  Serial.println("\"Kolm Silma\"");
  Serial.print("Compiled: ");
  Serial.print(__TIME__);
  Serial.print(" ");
  Serial.println(__DATE__);
  Serial.println("Made by Tauno Erik.");

  // Initialize pins
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(PROXIMITY_PIN, INPUT);

  // Initialize neopixsels
  Pixels.begin();
  Pixels.show();
  Pixels.setBrightness(50); // 0-255

  // Random number
  randomSeed(analogRead(0));

  boot_blink();
}

void loop() {
  uint32_t time_now = millis();

  is_movement = digitalRead(PROXIMITY_PIN);

  if (is_movement) {
    //Serial.print("H");
    change_color = true;
    no_movment_time = time_now;
  } else {
    //Serial.print("L");
    change_color = false;
  }


  if (time_now - no_movment_time >= OFF_TIME) {
    // all LEDs off
    for (uint8_t i = 0; i < PX_NUM; i++) {
      Pixels.setPixelColor(i, Pixels.Color(0, 0, 0));
    }
    Pixels.show();
  }


  // Colour change routine
  if (time_now - time_px_prev >= px_interval) {
    time_px_prev = time_now;

    if (change_color) {
      // Select random color chanel
      long random_RGB = random(3); // r, g or b

      // Select random pixel
      long random_px = random(PX_NUM);

      Serial.print(" Pixel:");
      Serial.print(random_px+1);

      // Get this pixel old colors
      uint32_t px_colors = Pixels.getPixelColor(random_px);
      // Split to r,g,b
      r = (uint8_t)(px_colors>>16 & 0xFF);
      g = (uint8_t)(px_colors>>8 & 0xFF);
      b = (uint8_t)(px_colors>>0 & 0xFF);

      // Update pixel color
      switch (random_RGB) {
        case 0:
          Serial.print(" r");
          r = new_value(r);
          Serial.print(r);
          break;
        case 1:
          Serial.print(" g");
          g = new_value(g);
          Serial.print(g);
          break;
        case 2:
          Serial.print(" b");
          b = new_value(b);
          Serial.print(b);
          break;
        default:
          break;
      }

      Pixels.setPixelColor(random_px, Pixels.Color(r, g, b));

      Pixels.show();
      change_color = false;
    }
  }


}