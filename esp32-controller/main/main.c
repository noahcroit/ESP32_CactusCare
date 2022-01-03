#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

/*
 * For ADC
 *
 */
#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          //Multisampling
#if CONFIG_IDF_TARGET_ESP32
static const adc_channel_t channel = ADC_CHANNEL_6;     //GPIO34 if ADC1, GPIO14 if ADC2
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
#elif CONFIG_IDF_TARGET_ESP32S2
static const adc_channel_t channel = ADC_CHANNEL_6;     // GPIO7 if ADC1, GPIO17 if ADC2
static const adc_bits_width_t width = ADC_WIDTH_BIT_13;
#endif
static const adc_atten_t atten = ADC_ATTEN_DB_11;
static const adc_unit_t unit = ADC_UNIT_1;



/*
 * For RTOS
 *
 */
#define STACKSIZE_TASK1 4096
#define STACKSIZE_TASK2 4096
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

// This task is used for ADC experiment
void vTask2( void * pvParameters )
{
  //Configure ADC
  if (unit == ADC_UNIT_1) {
      adc1_config_width(width);
      adc1_config_channel_atten(channel, atten);
  } else {
      adc2_config_channel_atten((adc2_channel_t)channel, atten);
  }

  for( ;; )
  {
      // Task code goes here.
      printf("task2 activated!\n");
      
      uint32_t adc_reading = 0;
      //Multisampling
      for (int i = 0; i < NO_OF_SAMPLES; i++) {
          if (unit == ADC_UNIT_1) {
              adc_reading += adc1_get_raw((adc1_channel_t)channel);
          } else {
              int raw;
              adc2_get_raw((adc2_channel_t)channel, width, &raw);
              adc_reading += raw;
          }
      }
      adc_reading /= NO_OF_SAMPLES;
      //Convert adc_reading to voltage in mV
      uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
      printf("Raw: %d\tVoltage: %dmV\n", adc_reading, voltage);
      
      vTaskDelay( 2000 / portTICK_PERIOD_MS );
  }
}



// Main program
void app_main(void)
{
    // Create 2 tasks	
    xTaskCreate( vTask1, "task1", STACKSIZE_TASK1, NULL, 1 | portPRIVILEGE_BIT, &taskHandler_task1 );
    xTaskCreate( vTask2, "task2", STACKSIZE_TASK2, NULL, 2 | portPRIVILEGE_BIT, &taskHandler_task2 );
}



