#include <Arduino.h>

#include "app_task.h"

void setup() {
    Serial.begin(115200);

    Serial.println("");
    Serial.println("******************************");
    Serial.println("        Program start         ");
    Serial.println("******************************");

    createTask();
}
 
void loop() {
    // NOTHING
}