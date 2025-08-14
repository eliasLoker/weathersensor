#include <M5Unified.h>
#include <Adafruit_SHT31.h>
#include "M5UnitENV.h"
#include <Wire.h>

Adafruit_SHT31 sht31;
QMP6988 qmp;
int attempt_count = 0;

void setup() {
    auto cfg = M5.config();
    M5.begin(cfg);
    Serial.begin(115200);

    delay(1000);
    
    if (!sht31.begin(0x44)) {
        Serial.println("Error: SHT31 not found. Check connection.");
    } else {
        Serial.println("SHT31 found.");
    }

    if (!qmp.begin(&Wire, QMP6988_SLAVE_ADDRESS_L, 32, 33, 400000U)) {
        Serial.println("Couldn't find QMP6988");
        while (1) delay(1);
    } else {
        Serial.println("QMP6988 found.");
    }
}

void loop() {
    attempt_count++;
    float t = sht31.readTemperature();
    float h = sht31.readHumidity();
    
    bool qmp_ok = qmp.update();

    M5.Display.clear();
    M5.Display.setCursor(0, 0);
    M5.Display.setRotation(3);
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);

    if (isnan(t) || isnan(h)) {
        M5.Display.println("Error SHT31!");
        Serial.println("Error reading SHT31.");
    } else {
        M5.Display.printf("Temp: %.2f C\n", t);
        M5.Display.printf("Hum: %.2f %%\n", h);
        Serial.printf("SHT31: Temp: %.2f C | Hum: %.2f %%\n", t, h);
    }

    if (qmp_ok) {
        float pressure_hpa = qmp.pressure / 100.0f;
        float pressure_mmhg = pressure_hpa * 0.750062f;
        M5.Display.printf("Press: %.2f mmHg\n", pressure_mmhg);
        M5.Display.printf("Alt: %.2f m\n", qmp.altitude);
        Serial.printf("QMP6988: Temp: %.2f C | Press: %.2f Pa | Alt: %.2f m\n", qmp.cTemp, qmp.pressure, qmp.altitude);
    } else {
        M5.Display.println("Error QMP6988!");
        Serial.println("Error reading QMP6988.");
    }

    M5.Display.printf("Attempt #%d\n", attempt_count);

    delay(5000);
}