/*
 * irrigator_esp32.h
 *
 */
#ifndef IRRIGATOR_ESP32_H
#define IRRIGATOR_ESP32_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "driver/gpio.h"

#define ESP_INTR_FLAG_DEFAULT 0
typedef struct{
    uint8_t start_hour;
    uint8_t start_min;
    uint8_t duration;
}irrigation_schedule_t;

#define IRRIGATION_TIME_PER_DAY   2
#define IRRIGATION_STATE_IDLE     0 // do not have any work
#define IRRIGATION_STATE_WAITING  1 // waiting for irrigation time
#define IRRIGATION_STATE_RUNNING  2 // irrigating until end duration time

typedef struct{
    gpio_num_t solenoid_gpio;
    gpio_num_t manual_button_gpio;
    irrigation_schedule_t schedule[IRRIGATION_TIME_PER_DAY];
    int state;
}irrigation_config_t;

int irrigation_init();
int irrigation_bind_button_handler(irrigation_config_t *config, void (*button_handler)());
int irrigation_turn_on();
int irrigation_turn_off();
int irrigation_get_solenoid_status(irrigation_config_t *config);
int irrigation_toggle(irrigation_config_t *config);

#endif
