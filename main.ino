#define BLYNK_TEMPLATE_ID "TMPL6HicHX7zC"
#define BLYNK_TEMPLATE_NAME "amri"
#define BLYNK_AUTH_TOKEN "T2hnJiV6e4NXOKyhFGm0tPciPVNypHQY"

#include <Arduino.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// WiFi credentials
const char* ssid = "wkwk";
const char* password = "aaaaaaaa";

// Blynk virtual pins
#define VPIN_FLOW_RATE V1
#define VPIN_TOTAL_LITRES V2

// Variables for flow calculation
#define SENSOR 27
#define BUZZER 32
#define LED 33

long currentMillis = 0;
long previousMillis = 0;
const int interval = 1000;
const float calibrationFactor = 4.5;
volatile byte pulseCount = 0;
byte pulse1Sec = 0;
float flowRate = 0.0;   
unsigned int flowMilliLitres = 0;
unsigned long totalMilliLitres = 0;
unsigned long totalLitres = 0;

BlynkTimer timer;

// Define the FlowData struct before any function declarations or implementations
struct FlowData {
    float flowRate;
    float totalLitres;
};

void IRAM_ATTR pulseCounter() {
    pulseCount++;
}

void initFlowCalculation() {
    pinMode(SENSOR, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(SENSOR), pulseCounter, FALLING);
}

FlowData calculateFlowRate() {
    currentMillis = millis();
    if (currentMillis - previousMillis > interval) {
        pulse1Sec = pulseCount;
        pulseCount = 0;

        // Calculate flow rate in litres/minute
        flowRate = ((1000.0 / (currentMillis - previousMillis)) * pulse1Sec) / calibrationFactor;
        previousMillis = currentMillis;

        // Convert flow rate to millilitres
        flowMilliLitres = (flowRate / 60) * 1000;
        totalMilliLitres += flowMilliLitres;
        totalLitres = totalMilliLitres / 1000;

        // Print flow rate and total volume
        Serial.print("Flow rate: ");
        Serial.print(int(flowRate));
        Serial.print(" L/min\t");

        Serial.print("Volume Quantity: ");
        Serial.print(int(totalLitres));
        Serial.println(" L");
    }

    FlowData data = { flowRate, totalLitres };
    return data;
}

void sendData(int flowRate, int volumeLitres) {
    // Send flow rate to Blynk
    Blynk.virtualWrite(VPIN_FLOW_RATE, flowRate);

    // Send total litres to Blynk
    Blynk.virtualWrite(VPIN_TOTAL_LITRES, volumeLitres);
}


void setup() {
    Serial.begin(115200);

    //LED BUZZER
    pinMode(LED,OUTPUT); //LED
    pinMode(BUZZER,OUTPUT); //BUZZER

    // Connect to WiFi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi..");

    // Wait until connected to WiFi
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }

    Serial.println("Connected to WiFi");

    // Initialize Blynk
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

    // Initialize flow calculation
    initFlowCalculation();
}

void loop() {
    Blynk.run();

    FlowData flowData = calculateFlowRate();

    // Use flowData.flowRate and flowData.totalLitres as needed
    sendData(flowData.flowRate, flowData.totalLitres);
    if(int(flowData.totalLitres) == 2){
      tone(BUZZER,1000,5000);
      digitalWrite(LED, HIGH);
      delay(5000);
  } 
    digitalWrite(LED, LOW);
    // timer.run();
    

}
