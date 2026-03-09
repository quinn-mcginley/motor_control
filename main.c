#include <stdio.h>
#include "pico/stdlib.h"
#include "ina219.h"
#include "h_bridge.h"

#define BUF_SIZE 200

enum mode_t {IDLE, PWM, ITEST, HOLD, TRACK};    // enumerates state variables
enum mode_t mode = IDLE;    // initialize mode to idle

float percent_power = 0.0f; // global variable to hold the current percent power for the motor
float Kp_current = 0.0f;    // proportional PID gain for current control
float Ki_current = 0.0f;    // integral PID gain for current control

bool current_control() {
    switch(mode){
        case IDLE:
            set_motor(0.0f);
            return true;
        case PWM:
            set_motor(percent_power);
            return true;
        case ITEST:
            return false;
        case HOLD:
            return false;
        case TRACK:
            return false;
    }
}

int main()
{
    stdio_init_all();
    char buffer[BUF_SIZE]; // buffer of data rx from the client

    init_ina219(); // initialize the INA219 current sensor

    struct repeating_timer timer;   // allocate space for timer function for current
    bool timer_ok = add_repeating_timer_ms(1000/1000, current_control, NULL, &timer);   // set up timer func

    init_h_bridge(); // initialize the h-bridge

    while (true) {
        scanf("%s", buffer); // wait for the client to send a command
        switch(buffer[0]){
            case 'd':
            {
                // adds 1 to an integer
                volatile int n = 0;
                scanf("%d", &n);
                printf("%d\r\n", n+1);
                break;
            }
            case 'e':
            {
                // adds and subtracts two integers
                volatile int a = 0;
                volatile int b = 0;
                scanf("%d %d", &a, &b);
                printf("%d\r\n%d\r\n", a+b, a-b);
                break;
            }
            case 'a':
            {
                // reads current
                float current = read_ina219();
                printf("%f\r\n", current);
                break;
            }
            case 'f':
            {
                // get duty cycle from -100.0 to 100.0
                // assign this value to a global variable and set state to PWM
                scanf("%f", &percent_power);
                mode = PWM;
                break;
            }
            case 'g':
            {
                // sets Kp and Ki for current
                scanf("%f %f", &Kp_current, &Ki_current);
                break;
            }
            case 'h':
            {
                // reads out Kp and Ki for current
                printf("%f %f\r\n", Kp_current, Ki_current);
                break;
            }
            case 'r':
            {
                // reads out system status
                printf("%d\r\n", mode);
                break;
            }
            case 'p':
            {
                // powers off the system (ie. sets state to IDLE)
                mode = IDLE;
                break;
            }
            case 'q':
            {
                // quits system
                mode = IDLE;
                break;
            }
            default:
            {
                // uh oh
            }
        }
    }
}