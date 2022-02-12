#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>
#include <Seeed_Arduino_FreeRTOS.h>

#include "app_task.h"

#define LED_PIN 3

// static enum appMode mode = normal;

void createTask(void)
{
    vNopDelayMS(1000); // prevents usb driver crash on startup, do not omit this

    // Create the threads that will be managed by the rtos
    // Sets the stack size and priority of each task
    // Also initializes a handler pointer to each task, which are important to communicate with and retrieve info from tasks
    xTaskCreate(ledThread, "LED Task", 256, NULL, 2, NULL);
    xTaskCreate(sensorThread, "Sensor Task", 256, NULL, 1, NULL);

    // Start the RTOS, this function will never return and will schedule the tasks.
    vTaskStartScheduler();
}

static void ledThread(void* pvParameters) {
    Serial.println("Led Thread: Started");
    pinMode(LED_PIN, OUTPUT);
 
    while (1) {
        digitalWrite(LED_PIN, HIGH);
        delay(500);
        digitalWrite(LED_PIN, LOW);
        delay(500);
    }
}
 
static void sensorThread(void* pvParameters) {
    Serial.println("Sensor Thread: Started");
 
    while(true){
       Serial.println("LOG: Test");
       delay(1000);
    }
}