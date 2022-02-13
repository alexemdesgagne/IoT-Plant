#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>
#include <Seeed_Arduino_FreeRTOS.h>
#include <SoftwareSerial.h>

#include "AppTask.h"
#include "app.h"

#define LED_PIN 3

static enum appMode mMode;
static bool mLedOn;

static void ledThread(void * pvParameters);
static void sensorThread(void * pvParameters);
static void commThread(void * pvParameters);

AppTask::AppTask()
{
    mLedOn = false;
    mMode = normal;
}

AppTask::~AppTask()
{

}

void AppTask::createTasks(void)
{
    vNopDelayMS(1000); // prevents usb driver crash on startup, do not omit this

    // Create the threads that will be managed by the rtos
    // Sets the stack size and priority of each task
    // Also initializes a handler pointer to each task, which are important to communicate with and retrieve info from tasks
    xTaskCreate(ledThread, "LED Task", 256, NULL, 2, NULL);
    xTaskCreate(sensorThread, "Sensor Task", 256, NULL, 1, NULL);
    xTaskCreate(commThread, "Comm Task", 256, NULL, 1, NULL);

    // Start the RTOS, this function will never return and will schedule the tasks.
    vTaskStartScheduler();
}

static void ledThread(void * pvParameters) 
{
    Serial.println("Led Thread: Started");
    pinMode(LED_PIN, OUTPUT);
 
    while (1) {
        if(mLedOn){ 
            digitalWrite(LED_PIN, 1);
        }
        else{ 
            digitalWrite(LED_PIN, 0); 
            analogWrite(LED_PIN, 0);
        }
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

static void commThread(void * pvParameters)
{
    SoftwareSerial ble(7, 6);
    char data_from_ble = 0;

    Serial.println("Comm Thread: Started");
    ble.begin(9600);
    delay(1000);

    while(true){
        // Receive something from USB serial.
        if (Serial.available()){
            ble.write(Serial.read()); 
        }
        // Receive something from BLE module.
        if (ble.available()){
            data_from_ble = ble.read();
            Serial.write(data_from_ble);
            // Process receiving command.
            if (data_from_ble == '1'){
                data_from_ble = 0;
                Serial.println("LED ON");
                mLedOn = true; 
            }
            else if (data_from_ble == '0'){
                data_from_ble = 0;
                Serial.println("LED OFF");
                mLedOn = false;
            }
            else if (data_from_ble == 'n'){
                data_from_ble = 0;
                Serial.println("Normal mode");
                mLedOn = false;
                mMode = normal;
            }
            else if (data_from_ble == 's'){
                data_from_ble = 0;
                Serial.println("Setting mode");
                mMode = setting;
            }
        }
    }
}