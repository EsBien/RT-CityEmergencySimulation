
#include "DepartmentManagment.h"
#include "Logging.h"
// Semaphore declarations
SemaphoreHandle_t xPoliceSemaphore;
SemaphoreHandle_t xAmbulanceSemaphore;
SemaphoreHandle_t xFireSemaphore;
SemaphoreHandle_t xCovidSemaphore;
//SemaphoreHandle_t xBorrowedSemaphore;



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
    }
}
void vAmbulanceTask(void* param) {
    SemaphoreHandle_t xBorrowedSemaphore = NULL;
    for (;;) {

        if (xSemaphoreTake(xAmbulanceSemaphore, 0) == pdPASS) {
            printf("Ambulance: Ready to respond to events.\n");
        }
        else {
            xBorrowedSemaphore = borrowResource(PRIORITY_HIGH);
            if (xBorrowedSemaphore) { // is not null
                printf("Ambulance: Borrowed resource from another department.\n");
            }
            else {
                printf("--Ambulance: No resources available, waiting...\n");
                xSemaphoreTake(xAmbulanceSemaphore, portMAX_DELAY);  // Wait for primary resource
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS((rand() % 4) * 1000));// Randon Task duration

        // Release resource back to the appropriate semaphore
        if (xBorrowedSemaphore) {
            xSemaphoreGive(xBorrowedSemaphore);  // Return borrowed resource
            xBorrowedSemaphore = NULL;           // Reset tracking variable
            printf("Ambulance: Released borrowed resource.\n");
        }
        else {
            xSemaphoreGive(xAmbulanceSemaphore);  // Return primary resource
            printf("Ambulance: Released primary resource.\n");
        }
    }
}
void vFireTask(void* param) {
    SemaphoreHandle_t xBorrowedSemaphore = NULL;
    for (;;) {
        if (xSemaphoreTake(xFireSemaphore, 0) == pdPASS) {
            printf("Fire Department: Ready to respond to events.\n");
        }
        else {
            xBorrowedSemaphore = borrowResource(PRIORITY_HIGH);
            if (xBorrowedSemaphore) { // is not null
                printf("Fire: Borrowed resource from another department.\n");
            }
            else {
                printf("Fire: No resources available, waiting...\n");
                xSemaphoreTake(xFireSemaphore, portMAX_DELAY);  // Wait for primary resource
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS((rand() % 4) * 1000));// Randon Task duration

        // Release resource back to the appropriate semaphore
        if (xBorrowedSemaphore) {
            xSemaphoreGive(xBorrowedSemaphore);  // Return borrowed resource
            xBorrowedSemaphore = NULL;           // Reset tracking variable
            printf("Fire: Released borrowed resource.\n");
        }
        else {
            xSemaphoreGive(xFireSemaphore);  // Return primary resource
            printf("Fire: Released primary resource.\n");
        }
    }
}
void vCoranaTask(void* param) {
    SemaphoreHandle_t xBorrowedSemaphore = NULL;
    for (;;) {

        if (xSemaphoreTake(xCovidSemaphore, 0) == pdPASS) {
            printf("Covid Department: Ready to respond to events.\n");
        }
        else {
            xBorrowedSemaphore = borrowResource(PRIORITY_LOW);
            if (xBorrowedSemaphore) { // is not null
                printf("Covid: Borrowed resource from another department.\n");
            }
            else {
                printf("Covid: No resources available, waiting...\n");
                xSemaphoreTake(xCovidSemaphore, portMAX_DELAY);  // Wait for primary resource
            }
        }

        vTaskDelay(pdMS_TO_TICKS((rand() % 4) * 1000));// Randon Task duration
        // Release resource back to the appropriate semaphore
        if (xBorrowedSemaphore) {
            xSemaphoreGive(xBorrowedSemaphore);  // Return borrowed resource
            xBorrowedSemaphore = NULL;           // Reset tracking variable
            printf("Corana: Released borrowed resource.\n");
        }
        else {
            xSemaphoreGive(xCovidSemaphore);  // Return primary resource
            printf("Corana: Released primary resource.\n");
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
        if (isResourceAvailable(xAmbulanceSemaphore) && xSemaphoreTake(xAmbulanceSemaphore, 0) == pdPASS) {
            printf("Resource borrowed from Ambulance.\n");
            return xAmbulanceSemaphore;
        }
        else if (isResourceAvailable(xFireSemaphore) && xSemaphoreTake(xFireSemaphore, 0) == pdPASS) {
            printf("Resource borrowed from Fire.\n");
            return xFireSemaphore;
        }
    }
    else if (isResourceAvailable(xPoliceSemaphore) && xSemaphoreTake(xPoliceSemaphore, 0) == pdPASS) {
        printf("Resource borrowed from Police.\n");
        return xPoliceSemaphore;
    }
    
    else if (isResourceAvailable(xCovidSemaphore) && xSemaphoreTake(xCovidSemaphore, 0) == pdPASS) {
        printf("Resource borrowed from covid.\n");
        return xCovidSemaphore;
    }
    return NULL;  // No resources available to borrow
}
//void logEvent(const char* action, EmergencyEvent_t event) {
//    printf("[%s] Event ID: %d | Code: %d | Priority: %d | Message: %s\n",
//        action, event.eventID, event.eventCode, event.priority, event.message);
//}