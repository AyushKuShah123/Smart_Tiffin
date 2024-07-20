#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire for the metal sheet sensor is plugged into pin D2 (GPIO4) on the ESP8266
#define ONE_WIRE_BUS_METAL D3  //D3
// Data wire for the battery sensor is plugged into pin D3 (GPIO0) on the ESP8266
#define ONE_WIRE_BUS_BATTERY D4 //D4

// Relay and pull-up switch pin
#define RELAY_PIN D0    //D0
#define SWITCH_PIN D5     //D5

// #define LED D0

// LED indicators pins
#define RED_LED_PIN D6
#define GREEN_LED_PIN D7
#define BLUE_LED_PIN D8


// Setup oneWire instances to communicate with each DS18B20 sensor
OneWire oneWireMetal(ONE_WIRE_BUS_METAL);
OneWire oneWireBattery(ONE_WIRE_BUS_BATTERY);

// Pass oneWire references to Dallas Temperature.
DallasTemperature sensorsMetal(&oneWireMetal);
DallasTemperature sensorsBattery(&oneWireBattery);



// Temperature thresholds
#define METAL_SHEET_TEMP_THRESHOLD 60.0
#define BATTERY_TEMP_CUTOFF 45.0
#define BATTERY_TEMP_RESUME 40.0

bool relayState = false;

void setup() {
  // Start the serial communication
  Serial.begin(115200);

  // Start up the DallasTemperature library instances
  sensorsMetal.begin();
  sensorsBattery.begin();

  // Relay, switch, and LED setup
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
   pinMode(GREEN_LED_PIN, OUTPUT);
   //pinMode(LED, OUTPUT);

    // Initialize relay state and LEDs
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(BLUE_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
  //digitalWrite(LED, LOW);
}

void loop() {
  sensorsMetal.requestTemperatures();
  sensorsBattery.requestTemperatures();
  float metalSheetTemp = sensorsMetal.getTempCByIndex(0);
  float batteryTemp = sensorsBattery.getTempCByIndex(0);
  Serial.print("Metal Sheet Temperature: ");
  Serial.println(metalSheetTemp);
  Serial.print("Battery Temperature: ");
  Serial.println(batteryTemp);
  
  // Control LED indicators based on temperature thresholds
  if (metalSheetTemp >= METAL_SHEET_TEMP_THRESHOLD) {
    digitalWrite(RED_LED_PIN, HIGH); // Turn on red LED
  } else {
    digitalWrite(RED_LED_PIN, LOW); // Turn off red LED
  }

  if (batteryTemp >= BATTERY_TEMP_CUTOFF) {
    digitalWrite(BLUE_LED_PIN, HIGH); // Turn on blue LED
    digitalWrite(RED_LED_PIN, HIGH); // Turn on red LED
  } else {
    digitalWrite(BLUE_LED_PIN, LOW); // Turn off blue LED
    digitalWrite(RED_LED_PIN, LOW); // Turn off red LED
  }

  // Temperature control logic for relay
  if (batteryTemp >= BATTERY_TEMP_CUTOFF) {
    digitalWrite(RELAY_PIN, HIGH);
    relayState = false;
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("Battery temperature too high! Relay turned off.");
  } else if (batteryTemp < BATTERY_TEMP_RESUME && relayState == false) {
    relayState = true;
    Serial.println("Battery temperature back to safe level.");
  }

    // Check relay control based on metal sheet temperature and switch state
  if (relayState) {
    if (digitalRead(SWITCH_PIN) == LOW && metalSheetTemp < METAL_SHEET_TEMP_THRESHOLD) {
      digitalWrite(RELAY_PIN, HIGH);
      digitalWrite(GREEN_LED_PIN, HIGH);  //Turn on green LED
      
      Serial.println("Switch pressed and metal sheet temperature below threshold. Relay turned on.");
    } else if (metalSheetTemp >= METAL_SHEET_TEMP_THRESHOLD) {
      digitalWrite(RELAY_PIN, LOW);
      digitalWrite(GREEN_LED_PIN, LOW);   //Turn off green LED
      
      Serial.println("Metal sheet temperature above threshold. Relay turned off.");
    }
  }

  // Wait a bit before taking the next readings
  // digitalWrite(LED, LOW);
  delay(450);
  // digitalWrite(LED, HIGH);
  // delay(100);

}
