/*
 * ESP32 Edge AI SIS - Pipeline Vandalism Detection
 * Featuring: Non-Blocking IIoT, Remote Threat Injection, and Color-Coded Telemetry
 */

#define BLYNK_TEMPLATE_ID "TMPL2DjRQVc30"
#define BLYNK_TEMPLATE_NAME "Pipeline SIS Dashboard"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN_HERE" // <-- INSERT YOUR TOKEN

#include <Pipeline-Vandalism-Detection_inferencing.h> // Ensure this matches your ZIP name
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "YOUR_WIFI_SSID_HERE";       // <-- INSERT YOUR WIFI NAME
char pass[] = "YOUR_WIFI_PASSWORD_HERE";   // <-- INSERT YOUR WIFI PASSWORD

bool is_system_latched = false;
int system_mode = 0; // 0 = Standby (Default), 1 = Idle, 2 = Hacksaw, 3 = Hammer

// Hardware Pins
const int GREEN_LED = 2; 
const int RED_LED = 4;   
const int RELAY_PIN = 5; 

// --- DYNAMIC DATA BUFFERS ---
// -> PASTE YOUR FULL 375-FLOAT ARRAYS HERE <-
float features_idle[375]    = { 0.2000, 0.0000, 10.2000 /* ... paste rest of idle data ... */ }; 
float features_hacksaw[375] = { -0.7000, 2.9000, 9.9000 /* ... paste rest of hacksaw data ... */ };
float features_hammer[375]  = { 20.4000, -0.3000, 3.6000 /* ... paste rest of hammer data ... */ };

// This is the active buffer the AI reads from
float current_features[375]; 

// BLYNK V3: Remote Simulation Injector (Menu Widget)
BLYNK_WRITE(V3) {
  system_mode = param.asInt(); // Update the global mode variable
  
  if (system_mode == 0) {
    Serial.println(">>> System: STANDBY MODE (AI Paused & Latch Cleared)");
  } else {
    // Restore the normal label when AI is active
    Blynk.setProperty(V1, "label", "AI Confidence Score"); 
    
    if (system_mode == 1) {
      memcpy(current_features, features_idle, sizeof(current_features));
      Serial.println(">>> Simulation: IDLE MODE (AI Active)");
    } else if (system_mode == 2) {
      memcpy(current_features, features_hacksaw, sizeof(current_features));
      Serial.println(">>> Simulation: HACKSAW MODE (AI Active)");
    } else if (system_mode == 3) {
      memcpy(current_features, features_hammer, sizeof(current_features));
      Serial.println(">>> Simulation: HAMMER MODE (AI Active)");
    }
  }
}

// BLYNK V2: Remote System Reset (Push Button)
BLYNK_WRITE(V2) {
  int resetReq = param.asInt();
  if (resetReq == 1 && is_system_latched) {
    Serial.println("Re-arming system from SCADA...");
    delay(1000);
    ESP.restart(); // Clear buffers and reboot
  }
}

// Memory pointer for Edge Impulse Inference
int raw_feature_get_data(size_t offset, size_t length, float *out_ptr) {
    memcpy(out_ptr, current_features + offset, length * sizeof(float));
    return 0;
}

void setup() {
    Serial.begin(115200);
    delay(1000); 
    Serial.println("System Initializing...");

    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);

    // Initial State: Safe Operations
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RELAY_PIN, LOW); // Relay disengaged initially
    digitalWrite(RED_LED, HIGH);

    // Initialize with Idle data to prevent immediate trip
    memcpy(current_features, features_idle, sizeof(current_features));

    // --- NON-BLOCKING WIFI CONNECTION ---
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, pass);
    
    int wifi_timeout = 0;
    while (WiFi.status() != WL_CONNECTED && wifi_timeout < 20) {
        delay(500);
        Serial.print(".");
        wifi_timeout++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi Connected!");
        Blynk.config(auth);   // Setup Blynk without blocking
        Blynk.connect(3000);  // 3-second timeout
        if(Blynk.connected()){
           Serial.println("Blynk SCADA Connected! System Ready.");
           // Forces the ESP32 to download the current menu selection on boot
           Blynk.syncVirtual(V3); 
        }
    } else {
        Serial.println("\n⚠️ WiFi Failed. Running in OFFLINE EDGE MODE.");
    }
}

void loop() {
    // Only process cloud data if connected (Prevents Freezing)
    if (Blynk.connected()) {
        Blynk.run();
    }
    
    // THE PAUSE BUTTON: If in Standby (0), skip the AI and hardware updates!
    if (system_mode == 0) {
        // STANDBY DEFAULT STATE (Both LEDs Steady ON)
        digitalWrite(GREEN_LED, HIGH);
        digitalWrite(RED_LED, HIGH);
        digitalWrite(RELAY_PIN, LOW); 
        
        Blynk.setProperty(V0, "color", "#FFFFFF"); // Set LED to White for Standby
        Blynk.virtualWrite(V0, 255);              
        
        // Change the title label, and zero out the math
        Blynk.setProperty(V1, "label", "SYSTEM STANDBY (AI PAUSED)"); 
        Blynk.virtualWrite(V1, 0); 
        return; // This kicks the ESP32 back to the top of the loop
    }

    ei_impulse_result_t result = { 0 };
    signal_t features_signal;
    features_signal.total_length = sizeof(current_features) / sizeof(current_features[0]);
    features_signal.get_data = &raw_feature_get_data;

    // 1. Run AI Inference
    EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false);
    if (res != EI_IMPULSE_OK) return;

    // 2. Track highest confidence label
    float high_score = 0;
    char* detected_label = "";

    Serial.println("--- AI Analysis ---");
    for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        Serial.print(result.classification[i].label);
        Serial.print(": ");
        Serial.println(result.classification[i].value);

        if (result.classification[i].value > high_score) {
            high_score = result.classification[i].value;
            detected_label = (char*)result.classification[i].label;
        }
    }

    // 3. LOGIC SOLVER: Decide to Shut Down or Keep Running
    // Triggers if it's NOT idle AND confidence > 60%
    if (strcmp(detected_label, "idle_normal") != 0 && high_score > 0.60) {
        is_system_latched = true;
        
        // A. TRIGGER PHYSICAL ESD (Safe State)
        digitalWrite(GREEN_LED, LOW);
        digitalWrite(RED_LED, HIGH);
        digitalWrite(RELAY_PIN, LOW); 

        // B. UPDATE BLYNK DASHBOARD (Color Coded Red)
        if (Blynk.connected()) {
            Blynk.setProperty(V0, "color", "#D3435C"); // Set LED to Red
            Blynk.virtualWrite(V0, 255);              
            Blynk.virtualWrite(V1, high_score); 
            Blynk.logEvent("vandalism_detected", String("Alert: ") + detected_label);
        }

        // C. ENTER SAFETY LATCH
        while(is_system_latched) {
            if (Blynk.connected()) { Blynk.run(); } // Listen for remote reset
            
            // Blink the physical Red LED and Virtual LED
            digitalWrite(RED_LED, LOW); delay(200);
            digitalWrite(RED_LED, HIGH); delay(200);
        }
    } else {
        Serial.println("✅ Status: Normal Operations.");
        digitalWrite(GREEN_LED, HIGH);
        digitalWrite(RELAY_PIN, HIGH); // ENERGIZE RELAY (PUMP ON)
        digitalWrite(RED_LED, LOW);
        
        // Normal Operations (Color Coded Green)
        if (Blynk.connected()) {
            Blynk.setProperty(V0, "color", "#23C48E"); // Set LED to Green
            Blynk.virtualWrite(V0, 255);               
            Blynk.virtualWrite(V1, high_score);
        }
    }
    delay(1000);
}