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
char* serverHostname = "192.168.69.3";
int serverPort = 4200;

char photonName[32] = "";

RestClient client = RestClient(serverHostname, serverPort);

Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

char* photon2Config[4][3] = {
    {"POST","/wink/groups/6870297/activate",
        "{\"desired_state\": {\"powered\": false}}"},
    {"PUT","/wink/binary_switches/370507/desired_state",
        "{\"desired_state\":{\"powered\":true}}"},
    {"PUT", "/wink/light_bulbs/2376917/desired_state",
        "{\"desired_state\": {\"powered\": true, \"brightness\": 1.00, \"color_model\": \"color_temperature\", \"color_temperature\": 3000}}"},
    {"PUT", "/wink/light_bulbs/2376917/desired_state",
        "{\"desired_state\": {\"powered\": true, \"brightness\": 0.40, \"color_model\": \"color_temperature\", \"color_temperature\": 2500}}"}
};

char* photon3Config[4][3] = {
    {"POST","/wink/groups/11886987/activate",
        "{\"desired_state\":{\"powered\":false}}"},
    {"PUT","/wink/light_bulbs/3676075/desired_state",
        "{\"desired_state\": {\"powered\": false}}"},
    {"PUT", "/wink/light_bulbs/3676075/desired_state",
        "{\"desired_state\":{\"powered\":true, \"brightness\":1.00}}"},
    {"PUT", "/wink/light_bulbs/3676075/desired_state",
        "{\"desired_state\":{\"powered\": true, \"brightness\":0.05}}"}
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

int makeRequest(char* method, char* url, char* body) {
    String response = "";
    int statusCode;
    if (strcmp(method, "POST") == 0) {
        statusCode = client.post(url, body, &response);
    } else {
        statusCode = client.put(url, body, &response);
    }
    if (DEBUG_LOG) {
        Particle.publish("LightSwitch", 
            String::format("Status Code: %d, Response: %s", statusCode, response.c_str()), 60, PRIVATE);
    }
    return statusCode;
}

int request(int button) {
    int statusCode;
    if (strcmp(photonName, "photon-3") == 0) {
        statusCode = makeRequest(photon3Config[button][0], photon3Config[button][1], photon3Config[button][2]);
    } else {
        statusCode = makeRequest(photon2Config[button][0], photon2Config[button][1], photon2Config[button][2]);
    }
    return statusCode;
}

void loop() {
    if (digitalRead(BUTTON0) == LOW) {
        lightUpRequestMade(2);

        int statusCode = request(BUTTON0);
        lightUpRequestComplete(2, isRequestSuccess(statusCode));
    }

    else if (digitalRead(BUTTON1) == LOW) {
        lightUpRequestMade(1);
        int statusCode = request(BUTTON1);
        lightUpRequestComplete(1, isRequestSuccess(statusCode));
    }
    
    else if (digitalRead(BUTTON2) == LOW) {
        lightUpRequestMade(3);
        int statusCode = request(BUTTON2);
        lightUpRequestComplete(3, isRequestSuccess(statusCode));
    }
    
    else if (digitalRead(BUTTON3) == LOW) {
        lightUpRequestMade(0);
        int statusCode = request(BUTTON3);
        lightUpRequestComplete(0, isRequestSuccess(statusCode));
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

