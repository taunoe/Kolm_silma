/**
 * Project: Kolm silma
 * File:    main.cpp
 * Hardware:Selfmade Arduino Uno
 *          W2812 RGB NeoPixels  x 3pc.
 *          RCWL-0516 Microwave Proximity sensor
 * Author:  Tauno Erik https://taunoerik.art/
 * Started: 08.03.2022
 * Edited:  12.03.2022
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
bool is_movement = false;


Adafruit_NeoPixel Pixels(PX_NUM, PX_PIN, NEO_GRB + NEO_KHZ800);

/* Global variables:
   Timing
*/
unsigned long time_px_prev = 0;     // Millis
unsigned long time_pat_prev = 0;    // Millis
int           pattern_nr = 0;       // Current Pattern Number
int           pat_interval = 5000;  // Pattern Interval (ms)
int           px_interval = 50;     // Pixel Interval (ms)
int           px_queue = 0;         // Pattern Pixel Queue
int           px_cycle = 0;         // Pattern Pixel Cycle
uint16_t      px_current = 0;       // Pattern Current Pixel Number
uint16_t      px_number = PX_NUM;   // Total Number of Pixels


/*******************************************/
// Functions


/* Input a value 0 to 255 to get a color value.
   The colours are a transition r - g - b - back to r.
*/
uint32_t wheel(byte wheel_pos) {
  wheel_pos = 255 - wheel_pos;

  if(wheel_pos < 85) {
    return Pixels.Color(255 - wheel_pos * 3, 0, wheel_pos * 3);
  }

  if(wheel_pos < 170) {
    wheel_pos -= 85;
    return Pixels.Color(0, wheel_pos * 3, 255 - wheel_pos * 3);
  }

  wheel_pos -= 170;
  return Pixels.Color(wheel_pos * 3, 255 - wheel_pos * 3, 0);
}


/* 
  Fill strip pixels one after another with a color. Strip is NOT cleared
  first; anything there will be covered pixel by pixel. Pass in color
  (as a single 'packed' 32-bit value, which you can get by calling
  strip.Color(red, green, blue) as shown in the loop() function above),
  and a delay time (in milliseconds) between pixels.

  color_wipe(Pixels.Color(0, 0, 255), 50); // Blue
*/
void color_wipe(uint32_t color, int wait) {
  if(px_interval != wait) {
    px_interval = wait;
  }

  Pixels.setPixelColor(px_current, color);
  Pixels.show();
  px_current++;

  if(px_current >= PX_NUM) {
    px_current = 0;
  }
}


/*
Theater-marquee-style chasing lights. Pass in a color (32-bit value,
a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
between frames.

theater_chase(Pixels.Color(127, 0, 0), 50); // Red
*/
void theater_chase(uint32_t color, int wait) {
  if(px_interval != wait) {
    px_interval = wait;
  }

  for(uint16_t i = 0; i < px_number; i++) {
    Pixels.setPixelColor(i + px_queue, color);
  }

  Pixels.show();

  for(uint16_t i=0; i < px_number; i++) {
    Pixels.setPixelColor(i + px_queue, Pixels.Color(0, 0, 0));
  }

  px_queue++;

  if(px_queue >= 3) {
    px_queue = 0;
  }
}


/*
  Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
  rainbow(10);
*/
void rainbow(uint8_t wait) {
  if(px_interval != wait) {
    px_interval = wait;
  }
  for(uint16_t i=0; i < px_number; i++) {
    Pixels.setPixelColor(i, wheel((i + px_cycle) & 255));
  }
  Pixels.show();
  px_cycle++;
  if(px_cycle >= 256) {
    px_cycle = 0;
  }
}


/* 
  Theatre-style crawling lights with rainbow effect.

  theater_chase_rainbow(50);
*/
void theater_chase_rainbow(uint8_t wait) {
  if(px_interval != wait)
    px_interval = wait;
  for(int i=0; i < px_number; i+3) {
    Pixels.setPixelColor(i + px_queue, wheel((i + px_cycle) % 255));
  }
  Pixels.show();
  for(int i=0; i < px_number; i+3) {
    Pixels.setPixelColor(i + px_queue, Pixels.Color(0, 0, 0));
  }      
  px_queue++;
  px_cycle++;
  if(px_queue >= 3) {
    px_queue = 0;
  }
  if(px_cycle >= 256) {
    px_cycle = 0;
  }
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
// 

void setup() {
  Serial.begin(115200);
  // Boot message
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
  Pixels.show();            // off
  Pixels.setBrightness(50); // 0-255

  boot_blink();
}

void loop() {
  //Pixels.clear(); // Set all pixel colors to 'off'

  is_movement = digitalRead(PROXIMITY_PIN);

  // Change pattern by movement
  if (is_movement) {
    //Serial.println("High");
    // Mingi aeg?
    pattern_nr++;
    if(pattern_nr >= 7) {
      pattern_nr = 0;
    }
  } else {
    //Serial.println("Low");
  }

  unsigned long time_now = millis();

  // Change pattern by time
  /*
  if((time_now - time_pat_prev) >= pat_interval) {
    time_pat_prev = time_now;
    pattern_nr++;
    if(pattern_nr >= 7) {
      pattern_nr = 0;
    }
  }
  */

  if(time_now - time_px_prev >= px_interval) {
    time_px_prev = time_now;

    switch (pattern_nr) {
      case 7:
        Serial.println("7");
        theater_chase_rainbow(50);
        break;
      case 6:
        Serial.println("6");
        rainbow(10);
        break;
      case 5:
        Serial.println("5");
        theater_chase(Pixels.Color(0, 0, 127), 50);
        break;
      case 4:
        Serial.println("4");
        theater_chase(Pixels.Color(127, 0, 0), 50); // Red
        break;
      case 3:
        Serial.println("3");
        theater_chase(Pixels.Color(127, 127, 127), 50); // White
        break;
      case 2:
        Serial.println("2");
        color_wipe(Pixels.Color(0, 0, 255), 50); // Blue
        break;
      case 1:
        Serial.println("1");
        color_wipe(Pixels.Color(0, 255, 0), 50); // Green
        break;        
      default:
        Serial.println("default");
        color_wipe(Pixels.Color(255, 0, 0), 50); // Red
        break;
    }
  }

}