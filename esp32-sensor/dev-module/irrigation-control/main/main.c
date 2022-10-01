#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "irrigator_esp32.h"





// struct for irrigator's configuration
irrigation_config_t irrigator_config;










// ISR handler for GPIO
static xQueueHandle gpio_evt_queue = NULL; // RTOS queue for GPIO interrupt
static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}




static void task_background(void* arg)
{
    int cnt = 0;
    while(1) 
    {
        printf("task1 running... cnt: %d\n", cnt++);
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

/*
 * task_irrigator_button()
 * Button task for manual irrigation. Pressing the button will change the state of solenoid.
 *
 */
static void task_irrigator_button(void* arg)
{
    uint32_t io_num;
    uint8_t  press_cnt=0;
    while(1) 
    {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
	{
            printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));
	    press_cnt++;
	    if(press_cnt > 1){
		printf("Irrigator Toggle!\n");
                irrigation_toggle(&irrigator_config);
		press_cnt = 0;
	    }
        }
        vTaskDelay(10 / portTICK_RATE_MS);
    }
}


/*
 * main task
 *
 */
void app_main(void)
{
    //create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));

    irrigator_config.solenoid_gpio = GPIO_NUM_2;
    irrigator_config.manual_button_gpio = GPIO_NUM_15;
    irrigator_config.schedule[0].start_hour = 0;
    irrigator_config.schedule[0].start_min = 0;
    irrigator_config.schedule[0].duration = 0;
    irrigation_init(&irrigator_config);
    
    irrigation_bind_button_handler(&irrigator_config, &gpio_isr_handler);
    
    printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());
    
    //start  tasks
    xTaskCreate(task_background,
	        "print something do nothing", 
		2048, 
		NULL, 
		10, 
		NULL);

    xTaskCreate(task_irrigator_button,
	        "checking queue of button", 
		2048, 
		NULL, 
		10, 
		NULL);

}


