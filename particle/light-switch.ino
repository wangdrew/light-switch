// This #include statement was automatically added by the Particle IDE.
#include <neopixel.h>

// This #include statement was automatically added by the Particle IDE.
#include "rest_client.h"

#define BUTTON0 D0
#define BUTTON1 D1
#define BUTTON2 D2
#define BUTTON3 D3

#define PIXEL_PIN D6
#define PIXEL_COUNT 4
#define PIXEL_TYPE WS2812B
#define BRIGHTNESS 32  // 0 - 255

bool DEBUG_LOG = true;
char* serverHostname = "192.168.69.3";
int serverPort = 4200;

RestClient client = RestClient(serverHostname, serverPort);

Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

void setup() {
    Serial.begin(9600);
    Serial.println("Starting...");
    
    pinMode(BUTTON0, INPUT_PULLUP);
    pinMode(BUTTON1, INPUT_PULLUP);
    pinMode(BUTTON2, INPUT_PULLUP);
    pinMode(BUTTON3, INPUT_PULLUP);
    
    strip.begin();
    strip.setBrightness(BRIGHTNESS);

    // do something cool when everything's set up
    rainbowCycle(3);
    lightsOff();
}

void makeRequest(char* url, char* body) {
    String response = "";
    int statusCode = client.put(url, body, &response);
    if (DEBUG_LOG) {
        Particle.publish("LightSwitch", 
            String::format("Status Code: %d, Response: %s", statusCode, response.c_str()), 60, PRIVATE);
    }
}

void loop() {
    // Everything off
    if (digitalRead(BUTTON0) == LOW) {
        lightUpRequestMade(2);
        makeRequest("/wink/binary_switches/370507", "{\"desired_state\": {\"powered\": false}}");
        makeRequest("/wink/light_bulbs/2376917", "{\"desired_state\": {\"powered\": false}}");
        lightUpRequestComplete(2);
    }
    
    // Room light on
    else if (digitalRead(BUTTON1) == LOW) {
        lightUpRequestMade(1);
        makeRequest("/wink/binary_switches/370507", "{\"desired_state\": {\"powered\": true}}");
        lightUpRequestComplete(1);
    }
    
    // Desk lamp on
    else if (digitalRead(BUTTON2) == LOW) {
        lightUpRequestMade(3);
        makeRequest("/wink/light_bulbs/2376917", 
            "{\"desired_state\": {\"powered\": true, \"brightness\": 1.00, \"color_model\": \"color_temperature\", \"color_temperature\": 3000}}");
        lightUpRequestComplete(3);  
    }
    
    // Desk lamp "nightlight" mode
    else if (digitalRead(BUTTON3) == LOW) {
        lightUpRequestMade(0);
        makeRequest("/wink/light_bulbs/2376917", 
            "{\"desired_state\": {\"powered\": true, \"brightness\": 0.10, \"color_model\": \"color_temperature\", \"color_temperature\": 2000}}");
        lightUpRequestComplete(0);
    }
}

void lightUpRequestMade(int pixel) {
    strip.setPixelColor(pixel, strip.Color(64, 255, 0));
    strip.show();
}

void lightUpRequestComplete(int pixel) {
    int numFlashes = 3;
    
    for (int i=0; i<numFlashes; i++) {
        strip.setPixelColor(pixel, strip.Color(255, 0, 0));
        strip.show();
        delay(50);
        strip.setPixelColor(pixel, strip.Color(0, 0, 0));
        strip.show();
        if (i < numFlashes-1) {
            delay(50);
        } 
    }
}

void lightsOff() {
    for (int i=0; i<PIXEL_COUNT; i++) {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    strip.show();
}

/**
* This stuff is copied from neopixel examples
*/

// Slightly different, this makes the rainbow equally distributed throughout, then wait (ms)
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) { // 1 cycle of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

