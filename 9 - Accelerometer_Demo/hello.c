/******************************************************************************
    Red-V Thing Plus Accelerometer_Demo,
    by Eduardo Corpeño

    Using a Qwiic 16x2 character LCD screen and an LSM6DSO accelerometer,
    print the acceleration at the X and Y axes constantly.

    Date:  June 20, 2022
    Developed using Freedom Studio v4.18.0.2021-04-1 on Windows 10
    LICENSE: This code is released under the MIT License
    (http://opensource.org/licenses/MIT)
  ******************************************************************************/

#include <metal/i2c.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// LCD screen connected to I2C0 bus
#define LCD_ADDR 0x72
#define ACCEL_ADDR 0x6B
#define I2C_BAUDRATE 9600
#define I2C_MASTER 1

// LCD Functions
void LCD_clear(struct metal_i2c *i2c){
  unsigned char buf[3] = "|-";
  metal_i2c_write(i2c, LCD_ADDR, 2, buf, 1);
}

void LCD_gotoxy(struct metal_i2c *i2c, uint32_t x, uint32_t y){
  unsigned char buf[2];
  x &= 0x0f; // Limit x to 0 through 15
  y &= 0x01; // Limit y to 0 through 1

  buf[0] = 254;
  buf[1] = 128 + x + y*64;
  metal_i2c_write(i2c, LCD_ADDR, 2, buf, 1);
}

void LCD_print_str(struct metal_i2c *i2c, unsigned char *str){
  metal_i2c_write(i2c, LCD_ADDR, strlen(str), str, 1);
}

// A quick and dirty delay function to be used experimentally.
void delay(int t){
	volatile uint32_t x, y;
	x=t;
	while(x--){
		y=x;
		while(y--);
	}
}


int main(void){
  struct metal_i2c *i2c;
  unsigned char buff[16];
  unsigned char reg[2], accel[1];

  i2c = metal_i2c_get_device(0);
  metal_i2c_init(i2c, I2C_BAUDRATE, I2C_MASTER);

  reg[0]=0x10;  // Accelerometer control register 1 address
  reg[1]=0x30;  // Enable the accelerometer at 52Hz
  metal_i2c_write(i2c, ACCEL_ADDR, 2, reg, 1);

  LCD_clear(i2c);

  while (1){
    reg[0]=0x29;  // Read X acceleration
    metal_i2c_write(i2c, ACCEL_ADDR, 1, reg, 1);
    metal_i2c_read(i2c, ACCEL_ADDR, 1, accel, 1);

    sprintf(buff,"X: %d    \0", (signed char) (accel[0]));
    LCD_gotoxy(i2c,3,0);
    LCD_print_str(i2c,buff);

    reg[0]=0x2B;  // Read Y acceleration
    metal_i2c_write(i2c, ACCEL_ADDR, 1, reg, 1);
    metal_i2c_read(i2c, ACCEL_ADDR, 1, accel, 1);

    sprintf(buff,"Y: %d    \0", (signed char) (accel[0]));
    LCD_gotoxy(i2c,3,1);
    LCD_print_str(i2c,buff);

    delay(1000);
  }
}
