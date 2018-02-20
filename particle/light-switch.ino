// This #include statement was automatically added by the Particle IDE.
#include "rest_client.h"

char* serverHostname = "andrew.big.wang";
int serverPort = 4200;

int button0 = D3;
int button1 = D4;
int button2 = D5;
int button3 = D6;

RestClient client = RestClient(serverHostname, serverPort);

void setup() {
    Serial.begin(9600);
    Serial.println("Starting...");
    pinMode(button0, INPUT_PULLUP);
    pinMode(button1, INPUT_PULLUP);
    pinMode(button2, INPUT_PULLUP);
    pinMode(button3, INPUT_PULLUP);

}

void makeRequest(char* url, char* body) {
    String response = "";
    int statusCode = client.put(url, body, &response);
    Particle.publish("LightSwitch", 
        String::format("Status Code: %d, Response: %s", statusCode, response.c_str()), 60, PRIVATE);
}

void loop() {
    // Everything off
    if (digitalRead(button0) == LOW) {
        makeRequest("/wink/binary_switches/370507", "{\"desired_state\": {\"powered\": false}}");
        makeRequest("/wink/light_bulbs/2376917", "{\"desired_state\": {\"powered\": false}}");
    }
    
    // Room light on
    else if (digitalRead(button1) == LOW) {
        makeRequest("/wink/binary_switches/370507", "{\"desired_state\": {\"powered\": true}}");
    }
    
    // Desk lamp on
    else if (digitalRead(button2) == LOW) {
        makeRequest("/wink/light_bulbs/2376917", 
            "{\"desired_state\": {\"powered\": true, \"brightness\": 1.00, \"color_model\": \"color_temperature\", \"color_temperature\": 3000}}");
    }
    
    // Desk lamp "nightlight" mode
    else if (digitalRead(button3) == LOW) {
        makeRequest("/wink/light_bulbs/2376917", 
            "{\"desired_state\": {\"powered\": true, \"brightness\": 0.10, \"color_model\": \"color_temperature\", \"color_temperature\": 2000}}");
    }
}
