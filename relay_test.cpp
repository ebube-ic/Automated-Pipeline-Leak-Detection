// Define the pin where the relay is connected
const int relayPin = 2; 

void setup() {
  // Tell the ESP32 that pin 2 is an OUTPUT
  pinMode(relayPin, OUTPUT);
}

void loop() {
  digitalWrite(relayPin, HIGH);   // Turn the relay ON
  delay(2000);                    // Wait for 2 seconds (2000 milliseconds)
  
  digitalWrite(relayPin, LOW);    // Turn the relay OFF
  delay(2000);                    // Wait for 2 seconds
}
