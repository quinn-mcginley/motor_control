// init and control functions for h_bridge

#include "h_bridge.h"

#define IN1_PIN 12 //GPIO
#define IN2_PIN 13 //GPIO

#define FREQ 20000.0f // 20 kHz PWM frequency
#define FIDELITY 1.0f // fidelity of PWM steps, from 0.0 to 1.0
#define TOP (uint16_t)(FIDELITY * 65535) // top value for PWM, as defined by fidelity

void init_h_bridge(){
    // initialize the h-bridge IN1 and IN2 as PWM signals (both on the same slice)
    gpio_set_function(IN1_PIN, GPIO_FUNC_PWM);      // set the IN1 pin to be a PWM output
    gpio_set_function(IN2_PIN, GPIO_FUNC_PWM);      // set the IN2 pin to be a PWM output
    uint slice_num = pwm_gpio_to_slice_num(IN1_PIN);// find out which slice the pins are connected to

    float f_sys = clock_get_hz(clk_sys);            // find the system clock frequency
    float div = f_sys / (FREQ*(TOP+1));             // calculate clock divider 

    if (div < 1.0f) {   // make sure the divider is within the valid range of 1.0 to 256.0
        div = 1.0f;
    } else if (div >= 256.0f) {
        div = 256.0f;
    }

    pwm_set_clkdiv(slice_num, div); // set the clock divider for the PWM slice to achieve the desired frequency
    pwm_set_wrap(slice_num, TOP);   // set the wrap value to be the constant TOP, which is user-defined by AMP_FIDELITY

    // set initial duty cycles to 100% for slow decay (braking) behavior
    pwm_set_gpio_level(IN1_PIN, TOP);
    pwm_set_gpio_level(IN2_PIN, TOP);

    pwm_set_enabled(slice_num, true); // enable PWM output
}


void set_motor(float percent_power) {
    if (percent_power > 100.0f) {   // address control saturation
        percent_power = 100.0f;
    } else if (percent_power < -100.0f) {
        percent_power = -100.0f;
    }

    // printf("Percent power is %f\r\n", percent_power);

    if (percent_power >= 0) {       // IN1 is always 100%, IN2 is (100% - duty cycle) → slow decay forward
        pwm_set_gpio_level(IN1_PIN, TOP);
        pwm_set_gpio_level(IN2_PIN, (uint16_t)((100.0f - percent_power) / 100.0f * TOP));
    } else {                        // IN2 is always 100%, IN1 is (100% + duty cycle) → slow decay reverse
        pwm_set_gpio_level(IN2_PIN, TOP);
        pwm_set_gpio_level(IN1_PIN, (uint16_t)((100.0f + percent_power) / 100.0f * TOP));
    }

    // printf("Level is %f\r\n", level);
}