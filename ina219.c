// init and read functions for the INA219 current sensor

#include "ina219.h"

#define INA219_ADDR 0b1000000 // I2C address
#define INA219_REG_CONFIG 0x00 // config register address
#define INA219_REG_CURRENT 0x04 // current register
#define INA219_REG_CALIBRATION 0x05 // calibration register

#define SDA_PIN 14
#define SCL_PIN 15
#define I2C_INST i2c1

// create private function prototypes
void writeINA219(int reg, int value);
signed short readINA219(unsigned char reg);

void init_ina219(){

    // init I2C on GP14 (SDA) and GP15 (SCL) on I2C1
    i2c_init(I2C_INST, 400 * 1000); // baud of 400kHz
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    // set the INA219 sensitivity - 10 bit, plus/minus160mV, 148us per sample
    unsigned short ina219_calValue = 1024;
    unsigned short ina219_config = 0b0011000010001111;
    writeINA219(INA219_REG_CALIBRATION, ina219_calValue);
    writeINA219(INA219_REG_CONFIG, ina219_config);
}

float read_ina219(){
    float current = 0;
    signed short value = readINA219(INA219_REG_CURRENT);
    current = value / 3.0;   // conversion factor
    
    return current;
}

// write 2 bytes
void writeINA219(int reg, int value){
    uint8_t buf[3];
    buf[0] = reg;           // first byte is the register address
    buf[1] = value>>8;      // second byte is the high byte of the value (done by shifting one byte to the right)
    buf[2] = value&0xff;    // third byte is the low byte of the value (done by masking the high byte with 0xff hex value)
    i2c_write_blocking(I2C_INST, INA219_ADDR, buf, 3, false);   // write using I2C_INST standard, to the INA219_ADDR, the 3 byte buffer, with no repeated start
}

// read 2 bytes
signed short readINA219(unsigned char reg){
    printf("hello");
    i2c_write_blocking(I2C_INST, INA219_ADDR, &reg, 1, true);   // write the register address to the INA219, with a repeated start so it's ready to read
    uint8_t buffer[2];      // create a buffer to hold the 2 bytes read from the INA219
    i2c_read_blocking(I2C_INST, INA219_ADDR, buffer, 2, false); // read 2 bytes into the buffer, with no repeated start
    signed short value = (buffer[0]<<8)|buffer[1];  // combine the two bytes into a single signed short value (done by shifting the first byte to the left and bitwise OR with the second byte)
    return value;
}