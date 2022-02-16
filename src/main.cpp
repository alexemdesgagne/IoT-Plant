#include <Arduino.h>
#include <SoftwareSerial.h>

#include "AppTask.h"

void setup() {
    Serial.begin(9600);

    Serial.println("");
    Serial.println("******************************");
    Serial.println("        Program start         ");
    Serial.println("******************************");

    AppTask task = AppTask();
    task.createTasks();
}
 
void loop() {
    // NOTHING
}