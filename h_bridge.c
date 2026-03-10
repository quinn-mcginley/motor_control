// init and control functions for h_bridge

#include "h_bridge.h"

#define IN1_PIN 12 //GPIO
#define IN2_PIN 13 //GPIO

#define FREQ 20000.0f // 20 kHz PWM frequency
#define FIDELITY 1.0f // fidelity of PWM steps, from 0.0 to 1.0
#define TOP (uint16_t)(FIDELITY * 65535) // top value for PWM, as defined by fidelity

void init_h_bridge(){
    // initialize the h-bridge IN1 PWM signal
    gpio_set_function(IN1_PIN, GPIO_FUNC_PWM);      // set the IN1 pin to be a PWM output
    uint slice_num = pwm_gpio_to_slice_num(IN1_PIN);// find out which slice the IN1 pin is connected to

    float f_sys = clock_get_hz(clk_sys);            // find the system clock frequency
    float div = f_sys / (FREQ*(TOP+1));             // calculate clock divider 

    if (div < 1.0f) {   // make sure the divider is within the valid range of 1.0 to 256.0
        div = 1.0f;
    } else if (div >= 256.0f) {
        div = 256.0f;
    }

    pwm_set_clkdiv(slice_num, div); // set the clock divider for the PWM slice to achieve the desired frequency
    pwm_set_wrap(slice_num, TOP);   // set the wrap value to be the constant TOP, which is user-defined by AMP_FIDELITY

    pwm_set_enabled(slice_num, true); // enable PWM output



    // initialize the h-bridge IN2 pin as a GPIO output and set it low
    gpio_set_function(IN2_PIN, GPIO_FUNC_SIO);
    gpio_set_dir(IN2_PIN, GPIO_OUT);
    gpio_put(IN2_PIN, 0);
}


void set_motor(float percent_power) {
    uint16_t level;

    if (percent_power >= 0) {   // this logic is defined by the chip
        gpio_put(IN2_PIN, 0);
        level = percent_power/100.0f * TOP;
    } else {
        gpio_put(IN2_PIN, 1);
        level = (100+percent_power)/100.0f * TOP;
    }

    pwm_set_gpio_level(IN1_PIN, level);
}