
#include "EmergencyDispatch.h"
#include "DepartmentManagment.h"

TimerHandle_t xEventTimer;
QueueHandle_t xDispatcherQueue;

int main_final(void) {

    srand(time(NULL));
    // create a timer
    xEventTimer = xTimerCreate("Event Timer", pdMS_TO_TICKS(1000), pdTRUE, (void*)0, vEventTimerCallback);
    // start the timer
    xTimerStart(xEventTimer, 0);

    // create queue
    xDispatcherQueue = xQueueCreate(QUEUE_DISPATCHER_LENGTH, sizeof(EmergencyEvent_t));

    // create semaphore depatments
    xAmbulanceSemaphore = xSemaphoreCreateCounting(AMBULANCE_SIZE, 0);
    xPoliceSemaphore = xSemaphoreCreateCounting(POLICE_SIZE , 0);
    xFireSemaphore = xSemaphoreCreateCounting(FIRE_SIZE, 0);
    xCovidSemaphore = xSemaphoreCreateCounting(CORONA_SIZE, 0);

    // create task
    xTaskCreate(vDispatcherTask, "Dispatcher Task", configMINIMAL_STACK_SIZE, NULL, PRIORITY_HIGH, NULL);
    xTaskCreate(vPoliceTask, "Police Task", configMINIMAL_STACK_SIZE, NULL, PRIORITY_LOW, NULL);
    xTaskCreate(vAmbulanceTask, "Ambulance Task", configMINIMAL_STACK_SIZE, NULL, PRIORITY_MEDIUM, NULL);
    xTaskCreate(vFireTask, "Fire Task", configMINIMAL_STACK_SIZE, NULL, PRIORITY_MEDIUM, NULL);
    xTaskCreate(vCoranaTask, "Corona Task", configMINIMAL_STACK_SIZE, NULL, PRIORITY_LOW, NULL);



    vTaskStartScheduler();

    for (;;);
}