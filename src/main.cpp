#include <Arduino.h>
#include <SoftwareSerial.h>

#include "app_task.h"

void setup() {
    Serial.begin(9600);

    Serial.println("");
    Serial.println("******************************");
    Serial.println("        Program start         ");
    Serial.println("******************************");

    createTask();
}
 
void loop() {
    // NOTHING
}