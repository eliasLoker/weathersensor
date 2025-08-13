#include <M5Unified.h>
#include <Adafruit_SHT31.h>
#include <Wire.h>

Adafruit_SHT31 sht31 = Adafruit_SHT31();

void setup() {
    auto cfg = M5.config();
    M5.begin(cfg);
    Serial.begin(115200);

    M5.Display.setRotation(3);
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);

    delay(1000);

    Serial.println("Starting SHT31 sensor...");

    if (!sht31.begin(0x44)) {
        Serial.println("Error: SHT31 not found. Check connection.");
    } else {
        Serial.println("SHT31 found.");
    }
}

void loop() {
    float t = sht31.readTemperature();
    float h = sht31.readHumidity();
    
    if (isnan(t) || isnan(h)) {
        M5.Display.clear();
        M5.Display.setCursor(0, 0);
        M5.Display.println("Error SHT31!");
        Serial.println("Error reading SHT31.");
        delay(1000);
        return;
    }

    M5.Display.clear();
    M5.Display.setCursor(0, 0);
    M5.Display.printf("Temperature: %.2f C\n", t);
    M5.Display.printf("Humidity: %.2f %%\n", h);

    Serial.printf("Temperature: %.2f C | Humidity: %.2f %%\n", t, h);

    delay(5000);
}