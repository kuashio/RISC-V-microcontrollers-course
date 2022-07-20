/******************************************************************************
    Red-V Thing Plus I2C scanner, not published
    by Eduardo Corpeño

    Using a Qwiic 16x2 character LCD screen, print a hello-world message
    and the low-byte of the Real-Time Clock's free-running counter.

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
#define I2C_BAUDRATE 9600
#define I2C_MASTER 1

// Macro Functions
#define  Red_V_RTC_start()      *((uint32_t *) 0x10000040) |= 1<<12
#define  Red_V_RTC_rtccountlo  (*((uint32_t *) 0x10000048))

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

int main(void){
  struct metal_i2c *i2c;
  uint32_t now;
  unsigned char buff[16];

  Red_V_RTC_start();

  i2c = metal_i2c_get_device(0);
  metal_i2c_init(i2c, I2C_BAUDRATE, I2C_MASTER);

  LCD_clear(i2c);
  LCD_print_str(i2c,"Hey there!");

  now = Red_V_RTC_rtccountlo & 0xff;

  sprintf(buff,"Time: %d\0",now);

  LCD_gotoxy(i2c,3,1);
  LCD_print_str(i2c,buff);

  while (1); // Reset to print again
}
