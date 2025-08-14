#include <M5Unified.h>
#include <Adafruit_SHT31.h>
#include "M5UnitENV.h"
#include <Wire.h>

Adafruit_SHT31 sht31;
QMP6988 qmp;
int attempt_count = 0;

void prepareDevice() {
    auto cfg = M5.config();
    M5.begin(cfg);
    Serial.begin(115200);
}

void prepareScreen() {
    M5.Display.clear();
    M5.Display.setCursor(0, 0);
    M5.Display.setRotation(3);
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
}

void setup() {
    prepareDevice();
    delay(1000);
    prepareScreen();
    if (!sht31.begin(0x44)) {
        M5.Display.println("Error SHT31!");
    } else if (!qmp.begin(&Wire, QMP6988_SLAVE_ADDRESS_L, 32, 33, 400000U)) {
        M5.Display.println("Error QMP6988!");
        while (1) delay(1);
    } else {
        M5.Display.println("All sensors ready!");
    }
}

void loop() {
    attempt_count++;
    bool qmp_ok = qmp.update();
    prepareScreen();
    float t = sht31.readTemperature();
    float h = sht31.readHumidity();
    if (isnan(t) || isnan(h)) {
        M5.Display.println("Error SHT31!");
        Serial.println("Error reading SHT31.");
    } else if (!qmp_ok) {
        M5.Display.println("Error QMP6988!");
        Serial.println("Error reading QMP6988.");
    } else {
        M5.Display.printf("Temp: %.2f C\n", t);
        M5.Display.printf("Hum: %.2f %%\n", h);
        Serial.printf("SHT31: Temp: %.2f C | Hum: %.2f %%\n", t, h);

        float pressure_hpa = qmp.pressure / 100.0f;
        float pressure_mmhg = pressure_hpa * 0.750062f;
        M5.Display.printf("Press: %.2f mmHg\n", pressure_mmhg);
        M5.Display.printf("Alt: %.2f m\n", qmp.altitude);
        Serial.printf("QMP6988: Temp: %.2f C | Press: %.2f Pa | Alt: %.2f m\n", qmp.cTemp, qmp.pressure, qmp.altitude);
    }
    M5.Display.printf("Attempt #%d\n", attempt_count);
    delay(5000);
}