#include "CircusUart.h"

CircusUart uart(Serial);

void setup()
{
    Serial.begin(57600);

    uart.on("Temp", ':', [](const char *data) {
        Serial.print("Temperature: ");
        Serial.println(data);
    });

    uart.on("Humi", ':', [](const char *data) {
        Serial.print("Humidity: ");
        Serial.println(data);
    });
}

void loop()
{
    uart.loop();

    static uint32_t timer = 0;
    if (millis() > timer) {
        timer = millis() + 1000;
        uart.send("Motor1", ':', "128");
    }
}