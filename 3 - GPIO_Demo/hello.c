/******************************************************************************
    Red-V Thing Plus GPIO_Demo,
    by Eduardo Corpeño
    Using the built-in LED and two external buttons to control it. 

    Date:  June 20, 2022
    Developed using Freedom Studio v4.18.0.2021-04-1 on Windows 10
    LICENSE: This code is released under the MIT License
    (http://opensource.org/licenses/MIT)
  ******************************************************************************/


#include <metal/gpio.h> //include GPIO library, https://sifive.github.io/freedom-metal-docs/apiref/gpio.html
#include <stdint.h>

#define  Red_V_enable_output(x) *((uint32_t *) 0x10012008) |= (1<<(x))
#define  Red_V_set_pin(x)       *((uint32_t *) 0x1001200C) |= (1<<(x))
#define  Red_V_clear_pin(x)     *((uint32_t *) 0x1001200C) &= ~(1<<(x))
#define  Red_V_read_pin(x)      (*((uint32_t *) 0x10012000) & (1<<(x)))
#define  Red_V_enable_pullup(x) *((uint32_t *) 0x10012010) |= (1<<(x))


int main (void) {
  struct metal_gpio *gpio_0; //Make instance of GPIO
  gpio_0 = metal_gpio_get_device(0);

  // Pins are set when initialized so we must disable it when we use it as an input/output
  metal_gpio_disable_input(gpio_0, 5);

  // Set as gpio as output
  //metal_gpio_enable_output(gpio_0, 5);
  Red_V_enable_output(5);
  // Pins have more than one function, make sure we disconnect anything connected
  metal_gpio_disable_pinmux(gpio_0, 5);

  metal_gpio_enable_input(gpio_0, 0);    // enable input 0
  metal_gpio_enable_input(gpio_0, 1);    // enable input 1

  metal_gpio_disable_output(gpio_0, 0);  // disable output 0
  metal_gpio_disable_output(gpio_0, 1);  // disable output 1

  metal_gpio_disable_pinmux(gpio_0, 0);  // disable alternate functions for pin 0
  metal_gpio_disable_pinmux(gpio_0, 1);  // disable alternate functions for pin 1

  Red_V_enable_pullup(0);  // enable pull-up for pin 0
  Red_V_enable_pullup(1);  // enable pull-up for pin 1


  while(1){
	    if(Red_V_read_pin(0) == 0)      // Read input pin 0
	      Red_V_clear_pin(5);          // Turn LED OFF

	    else if(Red_V_read_pin(1) == 0) // Read input pin 1
	        Red_V_set_pin(5);          // Turn LED ON
  }
  return 0; // Unreachable code
}
