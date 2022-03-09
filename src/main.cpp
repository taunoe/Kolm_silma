/**
 * Project: Kolm silma
 * File:    main.cpp
 * Hardware:Selfmade Arduino Uno
 *          W2812 RGB NeoPixels  x 3pc.
 *          RCWL-0516 Microwave Proximity sensor
 * Author:  Tauno Erik https://taunoerik.art/
 * Started: 08.03.2022
 * Edited:  09.03.2022
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


/*******************************************/
// Functions


/* 
Fill strip pixels one after another with a color. Strip is NOT cleared
first; anything there will be covered pixel by pixel. Pass in color
(as a single 'packed' 32-bit value, which you can get by calling
strip.Color(red, green, blue) as shown in the loop() function above),
and a delay time (in milliseconds) between pixels.
*/
void color_wipe(uint32_t color, int wait) {
  for(uint8_t i=0; i<pixels.numPixels(); i++) {
    pixels.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    pixels.show();
    delay(wait);
  }
}


/*
Theater-marquee-style chasing lights. Pass in a color (32-bit value,
a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
between frames.
*/
void theater_chase(uint32_t color, int wait) {
  for(uint8_t a=0; a<10; a++) {
    for(uint8_t b=0; b<3; b++) {
      pixels.clear();                //  Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(uint8_t c=b; c<pixels.numPixels(); c += 3) {
        pixels.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      pixels.show();
      delay(wait);
    }
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
  pixels.clear();
}


/*
Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
*/
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    // strip.rainbow() can take a single argument (first pixel hue) or
    // optionally a few extras: number of rainbow repetitions (default 1),
    // saturation and value (brightness) (both 0-255, similar to the
    // ColorHSV() function, default 255), and a true/false flag for whether
    // to apply gamma correction to provide 'truer' colors (default true).
    pixels.rainbow(firstPixelHue);
    // Above line is equivalent to:
    // strip.rainbow(firstPixelHue, 1, 255, 255, true);
    pixels.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}


/*******************************************/
// 

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
/*
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

  rainbow(10);

  
}