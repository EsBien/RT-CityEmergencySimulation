
#include "EmergencyDispatch.h"
#include "Logging.h"
#include <stdlib.h>
#include <string.h>
 // responsible for transmitting the transmitters


// generates a random event code (1-3) and sends it to the dispatcher queue
void vEventTimerCallback(TimerHandle_t xTimer) {

    EmergencyEvent_t newEvent = { .eventCode = -1,.message = {0} };
    LogEntry_t logEntry;

    newEvent.eventCode = (rand() % 4) + 1; // 1,2,3,4
    newEvent.priority = getEventPriority(newEvent.eventCode);
    switch (newEvent.eventCode) {
    case POLICE_CODE:
        snprintf(newEvent.message, sizeof(newEvent.message), "Police incident reported");
        break;
    case AMBULANCE_CODE:
        snprintf(newEvent.message, sizeof(newEvent.message), "AMBULANCE Medical emergency reported");
        break;
    case FIRE_CODE:
        snprintf(newEvent.message, sizeof(newEvent.message), "Fire emergency reported");
        break;
    default:
        snprintf(newEvent.message, sizeof(newEvent.message), "Unknown event received");
        break;
    }

    initLog(&logEntry, "Event Generated", newEvent.eventCode, "Timer Callback", 0, newEvent.message, xTaskGetTickCount());
    logEvent(logEntry);

    BaseType_t xHigherPriorityTaskWoken = pdFALSE; //do i need this
    if (xQueueSendFromISR(xDispatcherQueue, &newEvent, &xHigherPriorityTaskWoken) == pdFAIL) {// Interrupt Service Routine
        initLog(&logEntry, "Queue Send Failed", newEvent.eventCode, "Dispatcher Queue", 0, "Send Failed", xTaskGetTickCount());
        logEvent(logEntry);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken); //ensures timely response by immediately switching 

    // generates a random unterval 1-5
    xTimerStop(xTimer, 0);
    int newInterval = (rand() % 5 + 1) * 1000;
    xTimerChangePeriod(xTimer, pdMS_TO_TICKS(newInterval), 0);
    xTimerStart(xTimer, 0);

    // Log the timer restart with the new interval
    initLog(&logEntry, "Timer Restarted", -1, "Event Timer", newInterval, "New Interval Set", xTaskGetTickCount());
    logEvent(logEntry);

}

//The dispatcher task will read events from the queue and forward them to the relevant department
// ? is printing a message to console good?
void vDispatcherTask(void* param) {

    EmergencyEvent_t receivedEvent;
    EmergencyEvent_t ptiotityQueueEvents[TOTAL_SIZE] = { 0 };
    LogEntry_t logEntry;
    int priorityEventCount = 0;

    for (;;) {
        if (xQueueReceive(xDispatcherQueue, &receivedEvent, portMAX_DELAY) == pdPASS) {

            initLog(&logEntry, "Event Received", receivedEvent.eventCode, "Dispatcher", 0, "Processing", xTaskGetTickCount());
            logEvent(logEntry); // Log the receive event
            // Add to the priority event list and sort by priority
            ptiotityQueueEvents[priorityEventCount++] = receivedEvent;
            sortEventsByPriority(ptiotityQueueEvents, priorityEventCount);

            // Handle the highest-priority event in the sorted array
            EmergencyEvent_t eventToHandle = ptiotityQueueEvents[0];

            const char*  departmentName = NULL;
            switch (eventToHandle.eventCode) {
            case POLICE_CODE:
                departmentName = "Police Department";
                xSemaphoreGive(xPoliceSemaphore);
                break;
            case AMBULANCE_CODE:
                departmentName = "Ambulance Department";
                xSemaphoreGive(xAmbulanceSemaphore);
                // Trigger ambulance response
                break;
            case FIRE_CODE:
                departmentName = "Fire Department";
                xSemaphoreGive(xFireSemaphore);
                // Trigger fire department response
                break;
            case CORONA_CODE:
                departmentName = "Covid Department";
                xSemaphoreGive(xCovidSemaphore);
                // Trigger fire department response
                break;
            default:
                printf("Dispatcher: Unknown event code, no department assigned.\n");
                initLog(&logEntry, "Event Handling Failed", eventToHandle.eventCode, "Unknown Department", 0, "Failed", xTaskGetTickCount());
                logEvent(logEntry);
                break;
            }

            // Log forwarding the event to department
            initLog(&logEntry, "Event Forwarded", eventToHandle.eventCode, departmentName, 0, "Forwarding", xTaskGetTickCount());
            logEvent(logEntry);

            // remove the processed event from the array by shifting the remaining events
            for (int i = 0; i < priorityEventCount - 1; i++) {
                ptiotityQueueEvents[i] = ptiotityQueueEvents[i + 1];
            }
            priorityEventCount--;  // Reduce the count of events in the queue

            // Log the completion of processing for the event
            initLog(&logEntry, "Event Processed", eventToHandle.eventCode, departmentName, 0, "Completed", xTaskGetTickCount());
            logEvent(logEntry);
        }
        else {
            //printf("Failed to receive event from DispatcherQueue\n");
            initLog(&logEntry, "Failed to Receive Event", 0, "Dispatcher", 0, "Failed", xTaskGetTickCount());
            logEvent(logEntry);
        }
   
    }  
}

int getEventPriority(int eventCode) {
    switch (eventCode) {
    case POLICE_CODE: return PRIORITY_LOW;
    case AMBULANCE_CODE: return PRIORITY_HIGH;
    case FIRE_CODE: return PRIORITY_HIGH;
    case CORONA_CODE: return PRIORITY_LOW;
    default: return PRIORITY_LOW;
    }
}

// Sort function (Insertion Sort)
void sortEventsByPriority(EmergencyEvent_t* events, int count) {
    for (int i = 1; i < count; i++) {
        EmergencyEvent_t key = events[i];
        int j = i - 1;
        while (j >= 0 && events[j].priority < key.priority) {
            events[j + 1] = events[j];
            j--;
        }
        events[j + 1] = key;
    }
}

