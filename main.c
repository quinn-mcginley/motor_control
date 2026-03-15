#include <stdio.h>
#include "pico/stdlib.h"
#include "ina219.h"
#include "h_bridge.h"
#include "encoder.h"

#define BUF_SIZE 200

enum mode_t {IDLE, PWM, ITEST, TRACK};    // enumerates state variables
volatile enum mode_t mode = IDLE;        // initialize mode to idle

float percent_power = 0.0f;     // global variable to hold the current percent power for the motor

float Kp_current = 0.1f;        // proportional PID gain for current control
float Ki_current = 0.1f;        // integral PID gain for current control
float eint_current = 0;
float saved_desired_ITEST[400];
float saved_actual_ITEST[400];
int init_ITEST();

float Kp_position = 0.1f;       // proportional PID gain for position control
float Ki_position = 0.1f;       // integral PID gain for position control
float Kd_position = 0.1f;       // derivative PID gain for position control
float eint_position = 0;
float saved_desired_TRACK[4000];
float saved_actual_TRACK[4000];
float desired_current;
float last_error_position = 0.0f;

bool current_control(struct repeating_timer * cur_timer) {
    switch(mode){
        case IDLE:
        {
            set_motor(0.0f);
            return true;
        }
        case PWM:
        {
            set_motor(percent_power);
            return true;
        }
        case ITEST:
        {
            static int index_ITEST = 0;

            float desired = saved_desired_ITEST[index_ITEST];
            float actual = read_ina219();
            saved_actual_ITEST[index_ITEST] = actual;
            
            // simple PI control algorithm
            float error_current = desired - actual;
            eint_current += error_current;
            
            if (Ki_current != 0) {     // anti-windup, limits integral term to full power for given Ki
                if (eint_current > 100/Ki_current) {
                    eint_current = 100/Ki_current;
                } else if (eint_current < -100/Ki_current) {
                    eint_current = -100/Ki_current;
                }
            }

            percent_power = Kp_current * error_current + Ki_current * eint_current;   // update the global percent power variable based on the error and the PID gains

            set_motor(percent_power);   // set the motor power based on the updated percent power variable
            
            index_ITEST++;
            if (index_ITEST >= 400) {
                index_ITEST = 0;
                mode = IDLE;
            }

            return true;
        }
        case TRACK:
        {
            // as with ITEST, do current control against desired_current just don't save anything
            float actual = read_ina219();
            
            // simple PI control algorithm
            float error_current = desired_current - actual;
            eint_current += error_current;
            
            if (Ki_current != 0) {     // anti-windup, limits integral term to full power for given Ki
                if (eint_current > 100/Ki_current) {
                    eint_current = 100/Ki_current;
                } else if (eint_current < -100/Ki_current) {
                    eint_current = -100/Ki_current;
                }
            }

            percent_power = Kp_current * error_current + Ki_current * eint_current;   // update the global percent power variable based on the error and the PID gains

            set_motor(percent_power);   // set the motor power based on the updated percent power variable
            
            return true;
        }
    }
}

bool position_control(struct repeating_timer * pos_timer) {
    switch(mode){
        case TRACK:
        {
            static int index_TRACK = 0;

            float desired = saved_desired_TRACK[index_TRACK];
            float actual = getEncoder()/(4*334.0f) * 360.0f;   // convert encoder counts to degrees
            saved_actual_TRACK[index_TRACK] = actual;

            // simple PID control algorithm
            float error_position = desired - actual;
            eint_position += error_position;

            if (Ki_position!=0) {       // anti-windup, limits integral term to full power for given Ki
                if (eint_position > 100/Ki_position) {
                    eint_position = 100/Ki_position;
                } else if (eint_position < -100/Ki_position) {
                    eint_position = -100/Ki_position;
                }
            }

            desired_current = Kp_position * error_position + Ki_position * eint_position + Kd_position * (error_position - last_error_position);

            last_error_position = error_position;
            index_TRACK++;
            if (index_TRACK >= 4000) {
                index_TRACK = 0;
                mode = IDLE;
            }

            return true;
        }
        default:
        {
            // uh oh
            return true;
        }
    }
}

int main()
{
    stdio_init_all();
    char buffer[BUF_SIZE]; // buffer of data rx from the client

    init_ina219(); // initialize the INA219 current sensor

    struct repeating_timer cur_timer;   // allocate space for timer function for current (and other funcs)
    bool cur_timer_ok = add_repeating_timer_ms(1000.0/1000, current_control, NULL, &cur_timer);   // set up timer func
    if (!cur_timer_ok) {
        printf("Failed to create current timer\r\n");
        return 1;
    }

    struct repeating_timer pos_timer;   // allocate space for timer function for position control
    bool pos_timer_ok = add_repeating_timer_ms(1000/200, position_control, NULL, &pos_timer);   // set up timer func

    if (!pos_timer_ok) {
        printf("Failed to create position timer\r\n");
        return 1;
    }

    init_h_bridge();    // initialize the h-bridge
    init_ITEST();       // initialize the ITEST array
    initEncoder();      // initialize the encoder

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
                // sets Kp and Ki for current control
                scanf("%f %f", &Kp_current, &Ki_current);
                break;
            }
            case 'h':
            {
                // reads out Kp and Ki for current control
                printf("%f %f\r\n", Kp_current, Ki_current);
                break;
            }
            case 'k':
            {
                // measures step response from -100mA to 100mA
                eint_current = 0;   // wipe any eint from prior runs
                mode = ITEST;
                
                while (mode == ITEST) {
                    tight_loop_contents();
                }

                printf("%d\r\n", 400);
                for (int i=0; i<400; i++) {
                    printf("%.2f %.2f\r\n", saved_desired_ITEST[i], saved_actual_ITEST[i]);
                }

                break;
            }
            case 't':
            {
                // reads encoder counts
                int counts = getEncoder();
                printf("%d\r\n", counts);
                break;
            }
            case 'y':
            {
                // reads encoder in degrees
                float degrees = getEncoder() / (4*334.0f) * 360.0f;   // encoder is 334 counts/revolution, angle is 360º/revolution
                printf("%f\r\n", degrees);
                break;
            }
            case 'z':
            {
                // zeroes the encoder
                setEncoderToZero();
                break;
            }
            case 'i':
            {
                // sets Kp, Ki, and Kd for position control
                scanf("%f %f %f", &Kp_position, &Ki_position, &Kd_position);
                break;
            }
            case 'j':
            {
                // reads out Kp, Ki, and Kd for position control
                printf("%f %f %f\r\n", Kp_position, Ki_position, Kd_position);
                break;
            }
            case 'm':
            {
                // load a trajectory
                int num_positions;
                scanf("%d", &num_positions);

                for (int i=0; i<num_positions; i++) {
                    float p = 0.0f;
                    scanf("%f", &p);    // always drain input
                    if (i < 4000) {
                        saved_desired_TRACK[i] = p;
                    }
                }

                break;
            }
            case 'n':
            {
                // load a trajectory
                
                int num_positions;
                scanf("%d", &num_positions);

                for (int i=0; i<num_positions; i++) {
                    float p = 0.0f;
                    scanf("%f", &p);    // always drain input
                    if (i < 4000) {
                        saved_desired_TRACK[i] = p;
                    }
                }

                break;
            }
            case 'o':
            {
                // track trajectory
                eint_position = 0;   // wipe any eint from prior runs
                eint_current = 0;
                mode = TRACK;

                while (mode == TRACK) {
                    tight_loop_contents();
                }
                
                printf("%d\r\n", 4000);
                
                for (int i=0; i<4000; i++) {
                    printf("%.3f %.3f\r\n", saved_desired_TRACK[i], saved_actual_TRACK[i]);
                }

                last_error_position = 0;
                
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


int init_ITEST() {
    // load during initialization rather than on each ITEST run for efficiency
    for (int i=0; i<400; i++) {
        if (i < 100) {
            saved_desired_ITEST[i] = -100.0f;
        } else if (i < 200) {
            saved_desired_ITEST[i] = 100.0f;
        } else if (i < 300) {
            saved_desired_ITEST[i] = -100.0f;
        } else {
            saved_desired_ITEST[i] = 100.0f;
        }
    }
    return 0;
}