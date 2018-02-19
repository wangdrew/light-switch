// This #include statement was automatically added by the Particle IDE.
#include "rest_client.h"

RestClient client = RestClient("192.168.69.3", 4200);

int button0 = D3;
int button1 = D4;
int button2 = D5;
int button3 = D6;

void setup() {
    Serial.begin(9600);
    Serial.println("Starting...");
    pinMode(button0, INPUT_PULLUP);
    pinMode(button1, INPUT_PULLUP);
    pinMode(button2, INPUT_PULLUP);
    pinMode(button3, INPUT_PULLUP);

}

void loop() {
    int statusCode;
    String response = "";
    String data = "";

    // Everything off
    if (digitalRead(button0) == LOW) {
        char* body = "{\"desired_state\": {\"powered\": false}}";
        statusCode = client.put("/wink/binary_switches/370507", body, &response);
        data = String::format("Status Code: %d, Response: %s", statusCode, response.c_str());
        Particle.publish("TestEvent", data, 60, PRIVATE);
        
        statusCode = client.put("/wink/light_bulbs/2376917", body, &response);
        data = String::format("Status Code: %d, Response: %s", statusCode, response.c_str());
        Particle.publish("TestEvent", data, 60, PRIVATE);
    }
    
    // Main light on
    else if (digitalRead(button1) == LOW) {
        char* body = "{\"desired_state\": {\"powered\": true}}";
        statusCode = client.put("/wink/binary_switches/370507", body, &response);
        data = String::format("Status Code: %d, Response: %s", statusCode, response.c_str());
        Particle.publish("TestEvent", data, 60, PRIVATE);
    }
    
    // Desk lamp on
    else if (digitalRead(button2) == LOW) {
        char* body = "{\"desired_state\": {\"powered\": true, \"brightness\": 1.00, \"color_model\": \"color_temperature\", \"color_temperature\": 3000}}";
        statusCode = client.put("/wink/light_bulbs/2376917", body, &response);
        data = String::format("Status Code: %d, Response: %s", statusCode, response.c_str());
        Particle.publish("TestEvent", data, 60, PRIVATE);
    }
    
    // Desk lamp nightlight
    else if (digitalRead(button3) == LOW) {
        char* body = "{\"desired_state\": {\"powered\": true, \"brightness\": 0.10, \"color_model\": \"color_temperature\", \"color_temperature\": 2000}}";
        statusCode = client.put("/wink/light_bulbs/2376917", body, &response);
        data = String::format("Status Code: %d, Response: %s", statusCode, response.c_str());
        Particle.publish("TestEvent", data, 60, PRIVATE);
    }
}
