// init and control functions for h_bridge

#include "h_bridge.h"

#define IN1_PIN 12
#define IN2_PIN 13

#define FREQ 20000.0f // 20 kHz PWM frequency
#define AMP_FIDELITY 1.0f
#define TOP (uint16_t)(AMP_FIDELITY * 65535)

void init_h_bridge(){
    // initialize the h-bridge IN1 PWM signal
    gpio_set_function(IN1_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(IN1_PIN);

    float f_sys = clock_get_hz(clk_sys);
    float div = f_sys / (FREQ*(TOP+1));

    if (div < 1.0f) {
        div = 1.0f;
    } else if (div >= 256.0f) {
        div = 256.0f;
    }

    pwm_set_clkdiv(slice_num, div);
    pwm_set_wrap(slice_num, TOP);

    pwm_set_enabled(slice_num, true);



    // initialize the h-bridge IN2 pin as a GPIO output and set it low
    gpio_set_function(IN2_PIN, GPIO_FUNC_SIO);
    gpio_set_dir(IN2_PIN, GPIO_OUT);
    gpio_put(IN2_PIN, 0);
}


void set_motor(float percent_power) {
    uint16_t level;

    if (percent_power >= 0) {
        gpio_put(IN2_PIN, 0);
        level = percent_power/100.0f * TOP;
    } else {
        gpio_put(IN2_PIN, 1);
        level = (100-percent_power)/100.0f * TOP;
    }
    
    pwm_set_gpio_level(IN1_PIN, level);
}