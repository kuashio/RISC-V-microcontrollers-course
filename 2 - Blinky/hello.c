/******************************************************************************
    Red-V Thing Plus Blinky,
    Minor modifications by Eduardo Corpeño

    This code was originally written by: Ho Yun "Bobby" Chan and "Tron Monroe"
    @ SparkFun Electronics
    DATE:  11/21/2019

    DEVELOPMENT ENVIRONMENT SPECIFICS:
      Firmware developed using Freedom Studio v4.18.0.2021-04-1 on Windows 10

    ========== RESOURCES ==========
    Freedom E SDK

    ========== DESCRIPTION ==========
    Using the built-in LED. To test with different pin,
    simply modify the reference pin and connect a standard LED
    and 100Ohm resistor between the respective pin and GND.

    LICENSE: This code is released under the MIT License
    (http://opensource.org/licenses/MIT)
  ******************************************************************************/

#include <stdio.h>      //include Serial Library
#include <time.h>       //include Time library
#include <metal/gpio.h> //include GPIO library, https://sifive.github.io/freedom-metal-docs/apiref/gpio.html

#define  Red_V_enable_output(x) *((uint32_t *) 0x10012008) |= (1<<(x))
#define  Red_V_set_pin(x)       *((uint32_t *) 0x1001200C) |= (1<<(x))
#define  Red_V_clear_pin(x)     *((uint32_t *) 0x1001200C) &= ~(1<<(x))

// Custom write delay function since we do not have one like an Arduino
void delay(int number_of_microseconds){

  clock_t start_time = clock();  // Storing start time

  // Looping while required time is not reached
  while (clock() < start_time + number_of_microseconds);
}

int main (void) {
  printf("RED-V Example: Blink\n");

  struct metal_gpio *led0; //Make instance of GPIO

  // Note: The sequence of these function calls matters!

  // Get GPIO device handle, i.e. define IC pin here where IC's GPIO = 5, pin silkscreen = SCK/5
  // this is the GPIO device index that is referenced from 0, make sure to check the schematic
  led0 = metal_gpio_get_device(0);

  // Quick check to see if we set the metal_gpio up correctly, this was based on the "sifive-welcome.c" example code
  if (led0 == NULL) {
    printf("LED is null.\n");
    return 1;
  }

  // Pins are set when initialized so we must disable it when we use it as an input/output
  metal_gpio_disable_input(led0, 5);

  // Set as gpio as output
  //metal_gpio_enable_output(led0, 5);
  //*((uint32_t *) 0x10012008) |= 0x20;
  Red_V_enable_output(5);

  // Pins have more than one function, make sure we disconnect anything connected
  metal_gpio_disable_pinmux(led0, 5);

  // Turn ON pin
  metal_gpio_set_pin(led0, 5, 1);


  while(1){
      //metal_gpio_set_pin(led0, 5, 0);  // Turn OFF pin
	  Red_V_clear_pin(5);
	  delay(500000);  // 500ms in microseconds
      //metal_gpio_set_pin(led0, 5, 1);  // Turn ON pin
	  Red_V_set_pin(5);
	  delay(500000);
  }
  return 0; // Unreachable code
}
