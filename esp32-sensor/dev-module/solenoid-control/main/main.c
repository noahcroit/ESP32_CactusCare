#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"



/*
 * For RTOS
 *
 */
#define STACKSIZE_TASK1 4096
TaskHandle_t taskHandler_task1 = NULL;
TaskHandle_t taskHandler_task2 = NULL;

// Simple task
void vTask1( void * pvParameters )
{
  for( ;; )
  {
      vTaskDelay( 500 / portTICK_PERIOD_MS );
  }
}



// Main program
void app_main(void)
{
    // Create tasks	
    xTaskCreate( vTask1, "task1", STACKSIZE_TASK1, NULL, 1 | portPRIVILEGE_BIT, &taskHandler_task1 );
}



