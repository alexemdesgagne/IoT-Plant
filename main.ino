#include <Seeed_Arduino_FreeRTOS.h>

#define LED_PIN 13

// TaskHandle_t handle_led_task;
// TaskHandle_t handle_log_task;
 
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
 
static void logThread(void* pvParameters) {
    Serial.println("Log Thread: Started");
 
    while(true){
       Serial.println("LOG: Test");
       delay(1000);
    }
}
 
void setup() {
 
    Serial.begin(115200);
 
    vNopDelayMS(1000); // prevents usb driver crash on startup, do not omit this
    //while(!Serial);  // Wait for Serial terminal to open port before starting program
 
    Serial.println("");
    Serial.println("******************************");
    Serial.println("        Program start         ");
    Serial.println("******************************");
 
    // Create the threads that will be managed by the rtos
    // Sets the stack size and priority of each task
    // Also initializes a handler pointer to each task, which are important to communicate with and retrieve info from tasks
    xTaskCreate(ledThread, "LED Task", 256, NULL, 2, NULL);
    xTaskCreate(logThread, "LOG Task", 256, NULL, 1, NULL);
 
    // Start the RTOS, this function will never return and will schedule the tasks.
    vTaskStartScheduler();
}
 
void loop() {
    // NOTHING
}
