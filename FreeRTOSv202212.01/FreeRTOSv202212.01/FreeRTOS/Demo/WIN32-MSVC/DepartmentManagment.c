
#include "DepartmentManagment.h"
#include "Logging.h"
// Semaphore declarations
SemaphoreHandle_t xPoliceSemaphore;
SemaphoreHandle_t xAmbulanceSemaphore;
SemaphoreHandle_t xFireSemaphore;
SemaphoreHandle_t xCovidSemaphore;
//SemaphoreHandle_t xBorrowedSemaphore;

// Mutex declaration
SemaphoreHandle_t xPoliceMutex;
SemaphoreHandle_t xAmbulanceMutex;
SemaphoreHandle_t xFireMutex;
SemaphoreHandle_t xCovidMutex;



//? does the vtaskDispatcher need to call them
void vPoliceTask(void* param) {
    SemaphoreHandle_t xBorrowedSemaphore = NULL;
    LogEntry_t logEntry;
    TickType_t taskStartTime;
    TickType_t taskDuration;
    for (;;) {
        taskStartTime = xTaskGetTickCount(); // Record start time
        // Block until a notification is received from the dispatcher
        //ulTaskNotifyTake(pdTRUE, portMAX_DELAY);  // Wait indefinitely for a notification
        if (xSemaphoreTake(xPoliceMutex, portMAX_DELAY) == pdPASS) {

            if (xSemaphoreTake(xPoliceSemaphore, 0) == pdPASS) { // Blocks until a resource is available
                initLog(&logEntry, "Resource Acquired", POLICE_CODE, "Police", 0, "Primary Resource Acquired", taskStartTime);
                logEvent(logEntry);
            }
            else {
                xBorrowedSemaphore = borrowResource(PRIORITY_MEDIUM);
                if (xBorrowedSemaphore) { // is not null
                    initLog(&logEntry, "Resource Borrowed", POLICE_CODE, "Police", 0, "Borrowed Resource Acquired succefully", taskStartTime);
                    logEvent(logEntry);
                }
                else {
                    initLog(&logEntry, "Resource Wait", POLICE_CODE, "Police", 0, "Waiting for Primary Resource", taskStartTime);
                    logEvent(logEntry);
                    xSemaphoreTake(xPoliceSemaphore, portMAX_DELAY);  // Wait for primary resource
                    initLog(&logEntry, "Resource Acquired After Wait", POLICE_CODE, "Police", 0, "Primary Resource Acquired", xTaskGetTickCount());
                    logEvent(logEntry);
                }
            }

            // Task simulation with random delay
            int taskDurationMs = (rand() % 4) * 1000;
            taskDuration = pdMS_TO_TICKS(taskDurationMs);
            vTaskDelay(taskDuration);

            initLog(&logEntry, "Task Completed", POLICE_CODE, "Police", taskDurationMs, "Task Completed Successfully", xTaskGetTickCount());
            logEvent(logEntry);

            // Release resource back to the appropriate semaphore
            if (xBorrowedSemaphore) {
                xSemaphoreGive(xBorrowedSemaphore);  // Return borrowed resource
                xBorrowedSemaphore = NULL;           // Reset tracking variable
                initLog(&logEntry, "Resource Released", POLICE_CODE, "Police", 0, "Borrowed Resource Released", xTaskGetTickCount());
                logEvent(logEntry);
            }
            else {
                xSemaphoreGive(xPoliceSemaphore);  // Return primary resource
                initLog(&logEntry, "Resource Released", POLICE_CODE, "Police", 0, "Primary Resource Released", xTaskGetTickCount());
                logEvent(logEntry);
            }
            xSemaphoreGive(xPoliceMutex);  // Release mutex after use
        }
    }
}

void vAmbulanceTask(void* param) {
    SemaphoreHandle_t xBorrowedSemaphore = NULL;
    LogEntry_t logEntry = {0};
    TickType_t taskStartTime;
    TickType_t taskDuration;
    for (;;) {
        
        taskStartTime = xTaskGetTickCount();
        if (xSemaphoreTake(xAmbulanceMutex, portMAX_DELAY) == pdPASS) {
            if (xSemaphoreTake(xAmbulanceSemaphore, 0) == pdPASS) {
                initLog(&logEntry, "Resource Acquired", AMBULANCE_CODE, "Ambulance", 0, "Primary Resource Acquired", taskStartTime);
                logEvent(logEntry);
            }
            else {
                xBorrowedSemaphore = borrowResource(PRIORITY_HIGH);
                if (xBorrowedSemaphore) { // is not null
                    initLog(&logEntry, "Resource Borrowed", AMBULANCE_CODE, "Ambulance", 0, "Borrowed Resource Acquired succefully", taskStartTime);
                    logEvent(logEntry);
                }
                else {
                    initLog(&logEntry, "Resource Wait", AMBULANCE_CODE, "Ambulance", 0, "No resources available, waiting...", taskStartTime);
                    logEvent(logEntry);
                    xSemaphoreTake(xAmbulanceSemaphore, portMAX_DELAY);  // Wait for primary resource
                    initLog(&logEntry, "Resource Acquired After Wait", AMBULANCE_CODE, "Ambulance", 0, "Primary Resource Acquired", xTaskGetTickCount());
                    logEvent(logEntry);
                }
            }

            int taskDurationMs = (rand() % 4) * 1000;
            taskDuration = pdMS_TO_TICKS(taskDurationMs);
            vTaskDelay(taskDuration);

            initLog(&logEntry, "Task Completed", AMBULANCE_CODE, "Ambulance", taskDurationMs, "Task Completed Successfully", xTaskGetTickCount());
            logEvent(logEntry);

            // Release resource back to the appropriate semaphore
            if (xBorrowedSemaphore) {
                xSemaphoreGive(xBorrowedSemaphore);  // Return borrowed resource
                xBorrowedSemaphore = NULL;           // Reset tracking variable
                initLog(&logEntry, "Released resource", AMBULANCE_CODE, "Ambulance", 0, "Released borrowed resource", xTaskGetTickCount());
                logEvent(logEntry);
            }
            else {
                xSemaphoreGive(xAmbulanceSemaphore);  // Return primary resource
                initLog(&logEntry, "Primary Released resource", AMBULANCE_CODE, "Ambulance", 0, "Released Primary resource", xTaskGetTickCount());
                logEvent(logEntry);
            }
            xSemaphoreGive(xAmbulanceMutex);  // Release mutex after use
        }
    }
}

void vFireTask(void* param) {
    SemaphoreHandle_t xBorrowedSemaphore = NULL;
    LogEntry_t logEntry = { 0 };
    TickType_t taskStartTime;
    TickType_t taskDuration;
    for (;;) {
        taskStartTime = xTaskGetTickCount();
        if (xSemaphoreTake(xFireMutex, portMAX_DELAY) == pdPASS) {
            if (xSemaphoreTake(xFireSemaphore, 0) == pdPASS) {
                initLog(&logEntry, "Resource Acquired", FIRE_CODE, "Fire", 0, "Primary Resource Acquired", taskStartTime);
                logEvent(logEntry);
            }
            else {
                xBorrowedSemaphore = borrowResource(PRIORITY_HIGH);
                if (xBorrowedSemaphore) { // is not null
                    initLog(&logEntry, "Resource Borrowed", FIRE_CODE, "Fire", 0, "Borrowed Resource Acquired succefully", taskStartTime);
                    logEvent(logEntry);
                }
                else {
                    initLog(&logEntry, "Resource Wait", FIRE_CODE, "fire", 0, "No resources available, waiting...", taskStartTime);
                    logEvent(logEntry);
                    xSemaphoreTake(xFireSemaphore, portMAX_DELAY);  // Wait for primary resource
                    initLog(&logEntry, "Resource Acquired After Wait", FIRE_CODE, "Fire", 0, "Primary Resource Acquired", xTaskGetTickCount());
                    logEvent(logEntry);
                }
            }

            int taskDurationMs = (rand() % 4) * 1000;
            taskDuration = pdMS_TO_TICKS(taskDurationMs);
            vTaskDelay(taskDuration);

            initLog(&logEntry, "Task Completed", FIRE_CODE, "Fire", taskDurationMs, "Task Completed Successfully", xTaskGetTickCount());
            logEvent(logEntry);

            // Release resource back to the appropriate semaphore
            if (xBorrowedSemaphore) {
                xSemaphoreGive(xBorrowedSemaphore);  // Return borrowed resource
                xBorrowedSemaphore = NULL;           // Reset tracking variable
                initLog(&logEntry, "Released resource", FIRE_CODE, "Fire", 0, "Released borrowed resource", xTaskGetTickCount());
                logEvent(logEntry);
            }
            else {
                xSemaphoreGive(xFireSemaphore);  // Return primary resource
                initLog(&logEntry, "Primary Released resource", FIRE_CODE, "Fire", 0, "Released Primary resource", xTaskGetTickCount());
                logEvent(logEntry);
            }
            xSemaphoreGive(xFireMutex);  // Release mutex after use
        }
    }
}

void vCoranaTask(void* param) {
    SemaphoreHandle_t xBorrowedSemaphore = NULL;
    LogEntry_t logEntry = { 0 };
    TickType_t taskStartTime;
    TickType_t taskDuration;
    for (;;) {
        taskStartTime = xTaskGetTickCount();
        if (xSemaphoreTake(xCovidMutex, portMAX_DELAY) == pdPASS) {
            if (xSemaphoreTake(xCovidSemaphore, 0) == pdPASS) {
                initLog(&logEntry, "Resource Acquired", CORONA_CODE, "Corana", 0, "Primary Resource Acquired", taskStartTime);
                logEvent(logEntry);
            }
            else {
                xBorrowedSemaphore = borrowResource(PRIORITY_LOW);
                if (xBorrowedSemaphore) { // is not null
                    printf("Covid: Borrowed resource from another department.\n");
                    initLog(&logEntry, "Resource Borrowed", CORONA_CODE, "corona", 0, "Borrowed Resource Acquired succefully", taskStartTime);
                    logEvent(logEntry);
                }
                else {
                    initLog(&logEntry, "Resource Wait", CORONA_CODE, "Corana", 0, "No resources available, waiting...", taskStartTime);
                    logEvent(logEntry);
                    xSemaphoreTake(xCovidSemaphore, portMAX_DELAY);  // Wait for primary resource
                    initLog(&logEntry, "Resource Acquired After Wait", CORONA_CODE, "Corana", 0, "Primary Resource Acquired", xTaskGetTickCount());
                    logEvent(logEntry);
                }
            }

            int taskDurationMs = (rand() % 4) * 1000;
            taskDuration = pdMS_TO_TICKS(taskDurationMs);
            vTaskDelay(taskDuration);

            initLog(&logEntry, "Task Completed", CORONA_CODE, "Corana", taskDurationMs, "Task Completed Successfully", xTaskGetTickCount());
            logEvent(logEntry);

            // Release resource back to the appropriate semaphore
            if (xBorrowedSemaphore) {
                xSemaphoreGive(xBorrowedSemaphore);  // Return borrowed resource
                xBorrowedSemaphore = NULL;           // Reset tracking variable
                initLog(&logEntry, "Released resource", CORONA_CODE, "Corana", 0, "Released borrowed resource", xTaskGetTickCount());
                logEvent(logEntry);
            }
            else {
                xSemaphoreGive(xCovidSemaphore);  // Return primary resource
                initLog(&logEntry, "Primary Released resource", CORONA_CODE, "Corana", 0, "Released Primary resource", xTaskGetTickCount());
                logEvent(logEntry);
            }
            xSemaphoreGive(xCovidMutex);  // Release mutex after use
        }
    }
}


int isResourceAvailable(const SemaphoreHandle_t semaphore) {
    UBaseType_t count = uxSemaphoreGetCount(semaphore);
    return count > 0;
}

SemaphoreHandle_t borrowResource(int requestingDepartmentPriority) {
    // Check other department resources in priority order
    if (requestingDepartmentPriority == PRIORITY_HIGH) {
        if (xSemaphoreTake(xAmbulanceMutex, 0) == pdPASS) {
            if (xSemaphoreTake(xAmbulanceSemaphore, 0) == pdPASS) {
                printf("Resource borrowed from Ambulance.\n");
                return xAmbulanceSemaphore;
            }
            else {
                xSemaphoreGive(xAmbulanceMutex);
            }
        }     
        else if (xSemaphoreTake(xFireSemaphore, 0) == pdPASS) {
            if ( xSemaphoreTake(xFireSemaphore, 0) == pdPASS) {
                printf("Resource borrowed from Fire.\n");
                return xFireSemaphore;
            }
            else {
                xSemaphoreGive(xFireSemaphore);
            }
        }
    }
    else if (xSemaphoreTake(xPoliceSemaphore, 0) == pdPASS) {
        if (xSemaphoreTake(xPoliceSemaphore, 0) == pdPASS) {
            printf("Resource borrowed from Police.\n");
            return xPoliceSemaphore;
        }
        else {
            xSemaphoreGive(xPoliceSemaphore);
        }
    }
    
    else if (xSemaphoreTake(xCovidSemaphore, 0) == pdPASS){
        if (isResourceAvailable(xCovidSemaphore) && xSemaphoreTake(xCovidSemaphore, 0) == pdPASS) {
            printf("Resource borrowed from covid.\n");
            return xCovidSemaphore;
        }
        else{
            xSemaphoreGive(xCovidSemaphore);
        }
    }
    return NULL;  // No resources available to borrow
}
