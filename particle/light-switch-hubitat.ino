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

// Something in the code is blocking and causing the photon to 
// breathe green because it no longer connects to Particle cloud.
// Do particle cloud stuff in a separate thread and try to debug 
// where the code freezes with the web console 
SYSTEM_THREAD(ENABLED);
bool DEBUG_LOG = true;
char* serverHostname = ""; // FIXME
int serverPort = 80;
char* accessToken = ""; // FIXME
char photonName[32] = "";

RestClient client = RestClient(serverHostname, serverPort);

Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

char* urlSuffix = "?access_token=";
char* urlPrefix = "/apps/api/37/devices";

const int numRequestsPerButton = 2;
char* photonConfigUrls[4][numRequestsPerButton] = {
    {"/68/off","/7/off"},
    {"/7/on", ""},
    {"/68/setLevel/100", "/68/setColorTemperature/3000"},
    {"/68/setLevel/40", "/68/setColorTemperature/2500"},
};

void saveName(const char *topic, const char *data) {
    strncpy(photonName, data, sizeof(photonName)-1);
    Serial.println("received " + String(topic) + ": " + String(data));
}

void setup() {
    Serial.begin(9600);
    Serial.println("Starting...");
    
    pinMode(BUTTON0, INPUT_PULLUP);
    pinMode(BUTTON1, INPUT_PULLUP);
    pinMode(BUTTON2, INPUT_PULLUP);
    pinMode(BUTTON3, INPUT_PULLUP);
    
    strip.begin();
    strip.setBrightness(BRIGHTNESS);

    waitUntil(Particle.connected);
    
    // do something cool when everything's set up
    rainbowCycle(3);
    lightsOff();
    Particle.subscribe("particle/device/name", saveName);
    Particle.publish("particle/device/name");  // ask the cloud for the name to be sent to you
}

int makeRequest(char* url) {
    String response = "";
    int statusCode = client.get(url, &response);
    if (DEBUG_LOG) {
        Particle.publish("LightSwitch", 
            String::format("Status Code: %d, Response: %s", statusCode, response.c_str()), 60, PRIVATE);
    }
    return statusCode;
}

bool request(int button) {
    for (int i=0; i<numRequestsPerButton; i++) {
        if (photonConfigUrls[button][i] != '\0') {
            int statusCode;
            char url[128];
            strcpy(url, urlPrefix);
            strcat(url, photonConfigUrls[button][i]);
            strcat(url, urlSuffix);
            strcat(url, accessToken);
            Particle.publish("LightSwitch", String::format(url), 60, PRIVATE);
            statusCode = makeRequest(url);
            if (!isRequestSuccess(statusCode)) {
                return false;
            }
        }
    }
    return true;
}

void loop() {
    if (digitalRead(BUTTON0) == LOW) {
        lightUpRequestMade(2);
        lightUpRequestComplete(2, request(BUTTON0));
    }

    else if (digitalRead(BUTTON1) == LOW) {
        lightUpRequestMade(1);
        lightUpRequestComplete(1, request(BUTTON1));
    }
    
    else if (digitalRead(BUTTON2) == LOW) {
        lightUpRequestMade(3);
        lightUpRequestComplete(3, request(BUTTON2));
    }
    
    else if (digitalRead(BUTTON3) == LOW) {
        lightUpRequestMade(0);
        lightUpRequestComplete(0, request(BUTTON3));
    }
}

bool isRequestSuccess(int statusCode) {
    return statusCode >= 200 && statusCode < 299;
}

void lightUpRequestMade(int pixel) {
    strip.setPixelColor(pixel, strip.Color(64, 128, 0)); // orange
    strip.show();
}

void lightUpRequestComplete(int pixel, bool success) {
    int numFlashes = 3;
    int delayMs = 50;
    uint32_t color = strip.Color(128, 0, 0); // green
    if (!success) {
        delayMs = 400;
        color = strip.Color(0, 255, 0); // red
    }
    
    for (int i=0; i<numFlashes; i++) {
        strip.setPixelColor(pixel, color);
        strip.show();
        delay(delayMs);
        strip.setPixelColor(pixel, strip.Color(0, 0, 0)); // off
        strip.show();
        if (i < numFlashes-1) {
            delay(delayMs);
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

