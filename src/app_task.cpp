#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>
#include <Seeed_Arduino_FreeRTOS.h>
#include <SoftwareSerial.h>

#include "app_task.h"
#include "app.h"

#define LED_PIN 3

static void ledThread(void * pvParameters);
static void sensorThread(void * pvParameters);
static void bleThread(void * pvParameters);

static enum appMode mode = normal;
static bool led_on = true;

void createTask(void)
{
    vNopDelayMS(1000); // prevents usb driver crash on startup, do not omit this

    // Create the threads that will be managed by the rtos
    // Sets the stack size and priority of each task
    // Also initializes a handler pointer to each task, which are important to communicate with and retrieve info from tasks
    xTaskCreate(ledThread, "LED Task", 256, NULL, 2, NULL);
    xTaskCreate(sensorThread, "Sensor Task", 256, NULL, 1, NULL);
    xTaskCreate(bleThread, "BLE Task", 256, NULL, 1, NULL);

    // Start the RTOS, this function will never return and will schedule the tasks.
    vTaskStartScheduler();
}

static void ledThread(void * pvParameters) 
{
    Serial.println("Led Thread: Started");
    pinMode(LED_PIN, OUTPUT);
 
    while (1) {
        digitalWrite(LED_PIN, HIGH);
        delay(500);
        digitalWrite(LED_PIN, LOW);
        delay(500);
    }
}
 
static void sensorThread(void * pvParameters) 
{
    Serial.println("Sensor Thread: Started");
 
    while(true){
       Serial.println("LOG: Test");
       delay(1000);
    }
}

static void bleThread(void * pvParameters)
{
    SoftwareSerial ble(7, 6);
    char data_from_ble = 0;

    Serial.println("BLE Thread: Started");
    ble.begin(9600);
    delay(1000);

    while(true){
        if (ble.available()){
            data_from_ble = ble.read();
            Serial.write(data_from_ble);
            if (data_from_ble == '1'){
                data_from_ble = 0;
                Serial.println("LED ON");
                led_on = true; 
            }
            else if (data_from_ble == '0'){
                data_from_ble = 0;
                Serial.println("LED OFF");
                led_on = false;
            }
            else if (data_from_ble == 'n'){
                data_from_ble = 0;
                Serial.println("Normal mode");
                led_on = false;
                mode = normal;
            }
            else if (data_from_ble == 's'){
                data_from_ble = 0;
                Serial.println("Setting mode");
                mode = setting;
            }
        }
    }
}