/* irrigator_esp32.c
 * Irrigation Control Module for Smartfarm Application using ESP32
 *
 */

#include "irrigator_esp32.h"



int irrigation_init(irrigation_config_t *config)
{
    //initialize GPIOs
    //zero-initialize the GPIO config structure.
    gpio_config_t io_conf = {};

    // GPIO config for solenoid output 
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << (config->solenoid_gpio));
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    //GPIO config for manual button
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << (config->manual_button_gpio));
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    config->state = IRRIGATION_STATE_IDLE;

    return 0;
}

int irrigation_bind_button_handler(irrigation_config_t *config, void (*button_handler)())
{
    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(config->manual_button_gpio, 
		         button_handler, 
			 (void*)config->manual_button_gpio);
    return 0;
}

int irrigation_turn_on(irrigation_config_t *config)
{
    // set GPIO to turn on solenoid
    gpio_set_level(config->solenoid_gpio, 1);
    return 0;
}

int irrigation_turn_off(irrigation_config_t *config)
{
    // set GPIO to turn off solenoid
    gpio_set_level(config->solenoid_gpio, 0);
    return 0;
}

int irrigation_get_solenoid_status(irrigation_config_t *config)
{
    int solenoid_state;
    // read current GPIO value of solenoid CTRL pin 
    solenoid_state = gpio_get_level(config->solenoid_gpio);
    return solenoid_state;
}

int irrigation_toggle(irrigation_config_t *config)
{
    int solenoid_state; 
    // read state of solenoid
    solenoid_state = irrigation_get_solenoid_status(config);
    printf("state = %d\n", solenoid_state);
    if(solenoid_state == 0) irrigation_turn_on(config);
    else                    irrigation_turn_off(config);
    return 0;
}
