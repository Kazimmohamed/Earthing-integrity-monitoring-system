#include <SoftwareSerial.h>

// Constants for the ADC pin and smoothing capacitor
const int sensorPin = A0;  // Input pin for the voltage across the burden resistor
const int numReadings = 100;  // Number of readings to average
const int MOSFET_PIN = 7;     // Pin to control MOSFET gate
const int ANALOG_PIN = A1;    // Pin to read voltage across Rref
const float Rref = 1000.0;    // Reference resistor value in ohms
const float THRESHOLD = 50.0; // Threshold resistance for continuity in ohms
const int BUZZER_PIN = 8;     // Pin to control buzzer
const int RESET_PIN = 9;      // GSM module reset pin

// Variables for storing readings and the averaged value
int readings[numReadings];  // Array to store multiple readings
int readIndex = 0;  // Current reading index
long total = 0;  // Running total of readings
int averageReading = 0;  // The averaged reading

// GSM Module pins
SoftwareSerial GSM(10, 11); // RX, TX

void setup() {
  Serial.begin(9600);            // Start serial communication
  GSM.begin(9600);               // Initialize GSM module

  pinMode(MOSFET_PIN, OUTPUT);   // Set MOSFET gate pin as output
  digitalWrite(MOSFET_PIN, LOW); // Ensure MOSFET is OFF initially

  pinMode(BUZZER_PIN, OUTPUT);   // Set buzzer pin as output
  digitalWrite(BUZZER_PIN, LOW); // Ensure buzzer is OFF initially

  pinMode(RESET_PIN, OUTPUT);    // Set GSM module reset pin as output
  digitalWrite(RESET_PIN, HIGH); // Ensure GSM is not in reset state

  // Initialize all readings to 0
  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;
  }
}

void sendSMS(const String &message) {
  GSM.println("AT+CMGF=1");        // Set SMS mode to text
  delay(100);
  GSM.println("AT+CMGS=\"+919600026609\""); // Replace with recipient's phone number
  delay(100);
  GSM.print(message);
  delay(100);
  GSM.write(26);                 // Send Ctrl+Z to send the message
  delay(5000);                   // Wait for message to be sent
}

void resetGSM() {
  digitalWrite(RESET_PIN, LOW);   // Pull reset pin low
  delay(100);                    // Wait 100 ms
  digitalWrite(RESET_PIN, HIGH); // Pull reset pin high
  delay(2000);                   // Wait for GSM module to restart
}

void loop() {
  // Leakage Current Measurement
  total -= readings[readIndex];
  readings[readIndex] = analogRead(sensorPin);
  total += readings[readIndex];
  readIndex = (readIndex + 1) % numReadings;
  averageReading = total / numReadings;

  float voltage = (averageReading / 1023.0) * 5.0;  // Convert ADC value to voltage
  float current = voltage / 1000.0;  // Current through burden resistor (1kΩ)

  // Earth Resistance Measurement
  digitalWrite(MOSFET_PIN, HIGH);
  delay(1000); // Allow system to stabilize

  int adcValue = analogRead(ANALOG_PIN);
  float voltageRref = (adcValue / 1023.0) * 5.0; // Voltage across Rref
  float testCurrent = voltageRref / Rref;        // Test current through Rref
  float earthResistance = (7.6 - voltageRref) / testCurrent; // Calculate earth resistance

  digitalWrite(MOSFET_PIN, LOW);

  // Output results to Serial Monitor
  Serial.print("Leakage Current: ");
  Serial.print(current, 6);
  Serial.println(" A");

  Serial.print("Earth Resistance: ");
  Serial.print(earthResistance);
  Serial.println(" ohms");

  // Check for faults and activate buzzer if necessary
  bool fault = false;
  if (earthResistance >= THRESHOLD) {
    Serial.println("Status: EARTHING CONTINUITY FAILED");
    fault = true;
  } else {
    Serial.println("Status: EARTHING CONTINUITY OK");
  }

  if (current > 0.005) { // Example threshold for leakage current
    Serial.println("Status: LEAKAGE CURRENT DETECTED");
    fault = true;
  }

  if (fault) {
    digitalWrite(BUZZER_PIN, HIGH); // Activate buzzer
  } else {
    digitalWrite(BUZZER_PIN, LOW);  // Deactivate buzzer
  }

  // Create message and send SMS
  String message = "Leakage Current: ";
  message += String(current, 6) + " A\n";
  message += "Earth Resistance: ";
  message += String(earthResistance) + " ohms\n";

  if (earthResistance < THRESHOLD) {
    message += "Status: EARTHING CONTINUITY OK\n";
  } else {
    message += "Status: EARTHING CONTINUITY FAILED\n";
  }

  if (current > 0.005) {
    message += "Status: LEAKAGE CURRENT DETECTED";
  }

  sendSMS(message);

  delay(5000); // Wait before the next check
}
