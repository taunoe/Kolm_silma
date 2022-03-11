/**
 * Project: Kolm silma
 * File:    main.cpp
 * Hardware:Selfmade Arduino Uno
 *          W2812 RGB NeoPixels  x 3pc.
 *          RCWL-0516 Microwave Proximity sensor
 * Author:  Tauno Erik https://taunoerik.art/
 * Started: 08.03.2022
 * Edited:  11.03.2022
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
uint8_t proximity_val = 0;


Adafruit_NeoPixel Pixels(PX_NUM, PX_PIN, NEO_GRB + NEO_KHZ800);

/* Global variables 
   Timing
*/
unsigned long pixelPrevious = 0;          // Previous Pixel Millis
unsigned long patternPrevious = 0;        // Previous Pattern Millis
int           patternCurrent = 0;         // Current Pattern Number
int           patternInterval = 5000;     // Pattern Interval (ms)
int           px_interval = 50;         // Pixel Interval (ms)
int           px_queue = 0;             // Pattern Pixel Queue
int           px_cycle = 0;             // Pattern Pixel Cycle
uint16_t      px_current = 0;           // Pattern Current Pixel Number
uint16_t      px_number = PX_NUM;  // Total Number of Pixels


/*******************************************/
// Functions


/* Input a value 0 to 255 to get a color value.
   The colours are a transition r - g - b - back to r.
*/
uint32_t wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return Pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return Pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return Pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


/* 
Fill strip pixels one after another with a color. Strip is NOT cleared
first; anything there will be covered pixel by pixel. Pass in color
(as a single 'packed' 32-bit value, which you can get by calling
strip.Color(red, green, blue) as shown in the loop() function above),
and a delay time (in milliseconds) between pixels.
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
Theatre-style crawling lights with rainbow effect
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

  //
  boot_blink();
}

/*******************************************/
// 

void setup() {
  Serial.begin(115200);
  Serial.println("\"Kolm Silma\" starts!");

  // Initialize pins
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(PROXIMITY_PIN, INPUT);

  Pixels.begin();
  Pixels.show();            // off
  Pixels.setBrightness(50); // 0-255
}

void loop() {
  //Pixels.clear(); // Set all pixel colors to 'off'

  unsigned long currentMillis = millis();                     //  Update current time
  if((currentMillis - patternPrevious) >= patternInterval) {  //  Check for expired time
    patternPrevious = currentMillis;
    patternCurrent++;                                         //  Advance to next pattern
    if(patternCurrent >= 7)
      patternCurrent = 0;
  }


  if(currentMillis - pixelPrevious >= px_interval) {  //  Check for expired time
    pixelPrevious = currentMillis;                    //  Run current frame
    switch (patternCurrent) {
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

  proximity_val = digitalRead(PROXIMITY_PIN);
/*
  if (proximity_val == HIGH) {
    Serial.println("High");
  } else {
    Serial.println("Low");
  }
*/

/********************************
  for(int i=0; i<NEOPIXEL_NUM; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 150, 0));
    pixels.show();
    delay(500);
  }
*/
/*
  color_wipe(pixels.Color(255,   0,   0), 100); // Red
  color_wipe(pixels.Color(  0, 255,   0), 100); // Green
  color_wipe(pixels.Color(  0,   0, 255), 100); // Blue
*/

/*
  theater_chase(pixels.Color(127, 127, 127), 50); // White, half brightness
  theater_chase(pixels.Color(127,   0,   0), 50); // Red, half brightness
  theater_chase(pixels.Color(  0,   0, 127), 50); // Blue, half brightness
*/

  //rainbow(10);

  //theaterChaseRainbow(50);
}