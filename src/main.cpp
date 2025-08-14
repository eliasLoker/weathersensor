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

void animateText(const char* text) {
    M5.Display.print(text);
    delay(200);
}

void setup() {
    prepareDevice();
    delay(1000);
    prepareScreen();
    if (!sht31.begin(0x44)) {
        animateText("Error SHT31!");
    } else if (!qmp.begin(&Wire, QMP6988_SLAVE_ADDRESS_L, 32, 33, 400000U)) {
        animateText("Error QMP6988!");
        while (1) delay(1);
    } else {
        animateText("All sensors ready!");
    }
    delay(2000);
}

void loop() {
    attempt_count++;
    bool qmp_ok = qmp.update();
    
    float t = sht31.readTemperature();
    float h = sht31.readHumidity();
    
    prepareScreen();

    if (isnan(t) || isnan(h)) {
        animateText("Error SHT31!");
        Serial.println("Error reading SHT31.");
    } else if (!qmp_ok) {
        animateText("Error QMP6988!");
        Serial.println("Error reading QMP6988.");
    } else {
        char buffer[100];
        
        sprintf(buffer, "Temp: %.2f C\n", t);
        animateText(buffer);

        sprintf(buffer, "Hum: %.2f %%\n", h);
        animateText(buffer);
        
        Serial.printf("SHT31: Temp: %.2f C | Hum: %.2f %%\n", t, h);

        float pressure_hpa = qmp.pressure / 100.0f;
        float pressure_mmhg = pressure_hpa * 0.750062f;
        
        sprintf(buffer, "Press: %.2f mmHg\n", pressure_mmhg);
        animateText(buffer);

        sprintf(buffer, "Alt: %.2f m\n", qmp.altitude);
        animateText(buffer);
        
        Serial.printf("QMP6988: Temp: %.2f C | Press: %.2f Pa | Alt: %.2f m\n", qmp.cTemp, qmp.pressure, qmp.altitude);
    }
    
    char count_buffer[20];
    sprintf(count_buffer, "Attempt #%d", attempt_count);
    M5.Display.setCursor(0, 100);
    animateText(count_buffer);
    
    delay(5000);
}