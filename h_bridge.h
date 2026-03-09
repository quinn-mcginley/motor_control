// function prototypes for the INA219 current sensor

#ifndef H_BRIDGE_H
#define H_BRIDGE_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

void init_h_bridge();
void set_motor(float level);

#endif