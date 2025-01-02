#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

#define REPORTING_PERIOD_MS     200

LiquidCrystal_I2C lcd(0x27, 16, 2);

PulseOximeter pox;
uint32_t tsLastReport = 0;
bool measuring = true;

void debugLightState(int pin, const char* state) {
    Serial.print("Pin ");
    Serial.print(pin);
    Serial.print(" is ");
    Serial.println(state);
}

void onBeatDetected() {
    Serial.println("Beat Detected!");
}

void setup() {
    Serial.begin(115200);
    lcd.init();
    lcd.backlight();

    lcd.setCursor(0, 0);
    lcd.print("Initializing...");
    delay(1000);

    if (!pox.begin()) {
        Serial.println("Sensor Initialization Failed!");
        lcd.setCursor(0, 1);
        lcd.print("FAILED");
        for (;;);
    }
    pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
    pox.setOnBeatDetectedCallback(onBeatDetected);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Place Finger...");
}

void loop() {
    pox.update();

    if (measuring) {
        float bpm = pox.getHeartRate();
        float spO2 = pox.getSpO2();

        if (bpm > 30 && bpm < 220 && spO2 > 0) { // Ensure valid readings
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("BPM : ");
            lcd.print(bpm);
            lcd.setCursor(0, 1);
            lcd.print("SpO2: ");
            lcd.print(spO2);
            lcd.print("%");
            delay(2000);

            // Finalize measurement and indicate status
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Final BPM:");
            lcd.setCursor(0, 1);
            lcd.print(bpm);
            measuring = false; // Stop measuring after getting a valid reading
        }
    } else {
        // Keep displaying the final values indefinitely
        float bpm = pox.getHeartRate(); // Use the last valid BPM
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Final BPM:");
        lcd.setCursor(0, 1);
        lcd.print(bpm);
        delay(1000);
    }

    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Measuring...");
        lcd.setCursor(0, 1);
        lcd.print("Hold steady");
        tsLastReport = millis();
    }
}
