#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>
#include <Seeed_Arduino_FreeRTOS.h>
#include <SoftwareSerial.h>

#include "AppTask.h"
#include "app.h"

#define MAIN_LED_PIN                  3
#define DEBUG_LED_PIN                 13
#define HUMIDITY_SENSOR_PIN           4

static enum appMode mMode;
static bool mLedOn;

static QueueHandle_t commQueue;

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

    // Create the queues.
    commQueue = xQueueCreate(5, sizeof(uint8_t));
    if(commQueue != NULL){
        Serial.println("LOG: Comm Queue created.");
    }

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
    pinMode(MAIN_LED_PIN, OUTPUT);
    pinMode(DEBUG_LED_PIN, OUTPUT);
 
    while (1) {
        if (mMode == normal){
            if(mLedOn){ 
                digitalWrite(MAIN_LED_PIN, 1);
            }
            else{ 
                digitalWrite(MAIN_LED_PIN, 0); 
                analogWrite(MAIN_LED_PIN, 0);
            }
        }
        else if (mMode == setting){
            digitalWrite(DEBUG_LED_PIN, 0);
            delay(500);
            digitalWrite(DEBUG_LED_PIN, 1);
        }
        delay(50);
    }
}
 
static void sensorThread(void * pvParameters) 
{
    Serial.println("Sensor Thread: Started");
    uint16_t humidSensorVal = 0;
    const uint16_t dryVal = 895;
    const uint16_t wetVal = 444;

    while(true){
        if(mMode == normal){
            //Serial.println("LOG: Sensor reading.");
            humidSensorVal = analogRead(HUMIDITY_SENSOR_PIN);
            // uint8_t val = map(humidSensorVal, wetVal, dryVal, 255, 0); // Humidity sensor
            uint8_t val = map(humidSensorVal, 1023, 0, 255, 0); // Test potentiometer.
            Serial.println(val);
            //ble.write(byte(val)); 
            xQueueSend(commQueue, &val, ( TickType_t ) 0); // Send the value to the comm task. NO WAIT.
            if (mLedOn){
                analogWrite(MAIN_LED_PIN, val);
            }
        }
        else if(mMode == setting){
            // Do nothing.
        }
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
        // Check if something must be send to the BLE module.
        uint8_t sensor_val = 0;
        if(xQueueReceive(commQueue, &sensor_val, ( TickType_t ) 0)){
            ble.write(byte(sensor_val));
        }

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
                Serial.println("LOG: LED ON");
                mLedOn = true; 
            }
            else if (data_from_ble == '0'){
                data_from_ble = 0;
                Serial.println("LOG: LED OFF");
                mLedOn = false;
            }
            else if (data_from_ble == 'n'){
                data_from_ble = 0;
                Serial.println("LOG: Normal mode");
                mLedOn = false;
                mMode = normal;
            }
            else if (data_from_ble == 's'){
                data_from_ble = 0;
                Serial.println("LOG: Setting mode");
                mMode = setting;
            }
        }
        delay(50);
    }
}