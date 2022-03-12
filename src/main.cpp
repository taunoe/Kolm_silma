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
uint16_t      px_interval = 50;     // Pixel Interval (ms)
int           px_queue = 0;         // Pattern Pixel Queue
int           px_cycle = 0;         // Pattern Pixel Cycle
uint16_t      px_current = 0;       // Pattern Current Pixel Number
uint16_t      px_number = PX_NUM;   // Total Number of Pixels
uint8_t r = 0;
uint8_t g = 0;
uint8_t b = 0;
bool change_color = false;

/*******************************************/
// Functions

/*
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
    px_interval = wait;  // Global var
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
void theater_chase_rainbow(uint16_t wait) {
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

  // Random number
  randomSeed(analogRead(0));

  boot_blink();
}

void loop() {
  unsigned long time_now = millis();

  is_movement = digitalRead(PROXIMITY_PIN);

  if (is_movement) {
    //Serial.print("H");
    change_color = true;
  } else {
    //Serial.print("L");
    change_color = false;
  }


  if(time_now - time_px_prev >= px_interval) {
    time_px_prev = time_now;

    long random_RGB = random(3); // r, g or b

    if (change_color) {

      // Select pixsel
      long random_px = random(PX_NUM);
      // Get colors
      uint32_t px_colors = Pixels.getPixelColor(random_px);
      // Split to r,g,b
      r = (uint8_t)(px_colors>>16 & 0xFF);
      g = (uint8_t)(px_colors>>8 & 0xFF);
      b = (uint8_t)(px_colors>>0 & 0xFF);

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
    /*for(uint16_t i=0; i < px_number; i++) {
      Pixels.setPixelColor(i, Pixels.Color(r, g, b));
    }*/

    Pixels.show();
    change_color = false;
    }


  }

}