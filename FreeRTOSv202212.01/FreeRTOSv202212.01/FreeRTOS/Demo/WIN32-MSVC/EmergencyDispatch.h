#pragma once

#ifndef CITY_EMERGENCY_DISPATCH
#define CITY_EMERGENCY_DISPATCH

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

#include "DepartmentManagment.h"


#define QUEUE_DISPATCHER_LENGTH 20


#define QUEUE_DISPATCHER_ITEM_SIZE sizeof( EmergencyEvent_t )




// Queue for dispatching events
extern QueueHandle_t xDispatcherQueue;

// Event timer handle
extern TimerHandle_t xEventTimer;

// Task function
void vDispatcherTask(void* param); // responsible for transmitting the transmitters

// Eevent timer
void vEventTimerCallback(TimerHandle_t xTimer);

// Function to set priority based on event code
int getEventPriority(int eventCode);

// Sort function (Insertion Sort)
void sortEventsByPriority(EmergencyEvent_t* events, int count);

#endif CITY_EMERGENCY_DISPATCH //CITY_EMERGENCY_DISPATCH