/******************************************************************************
    Red-V Thing Plus Interrupt_Demo,
    by Eduardo Corpeño

    Using the built-in LED and two external buttons to control it.
    One button is handled by polling, and the other by interrupts.

    Date:  June 20, 2022
    Developed using Freedom Studio v4.18.0.2021-04-1 on Windows 10
    LICENSE: This code is released under the MIT License
    (http://opensource.org/licenses/MIT)
  ******************************************************************************/

#include <metal/gpio.h> //include GPIO library, https://sifive.github.io/freedom-metal-docs/apiref/gpio.html
#include <stdint.h>
#include <metal/csr.h>

// Macro Functions

#define  Red_V_enable_output(x)   *((uint32_t *) 0x10012008) |= (1<<(x))
#define  Red_V_set_pin(x)         *((uint32_t *) 0x1001200C) |= (1<<(x))
#define  Red_V_clear_pin(x)       *((uint32_t *) 0x1001200C) &= ~(1<<(x))
#define  Red_V_read_pin(x)       (*((uint32_t *) 0x10012000) & (1<<(x)))
#define  Red_V_enable_pullup(x)   *((uint32_t *) 0x10012010) |= (1<<(x))

#define  Red_V_GPIO_set_ie(x)     *((uint32_t *) 0x10012020) |= (1<<(x))
#define  Red_V_GPIO_clear_flag(x) *((uint32_t *) 0x10012024) |= (1<<(x))

#define  Red_V_PLIC_GPIO_set_priority(pin,p) *((uint32_t *) (0x0C000020+4*(pin))) = (p)
#define  Red_V_PLIC_clear_ie()               *((uint32_t *)  0x0C002000) = 0;\
                                             *((uint32_t *)  0x0C002004) = 0
#define  Red_V_PLIC_set_ie1(x)               *((uint32_t *)  0x0C002000) |= (1<<(x))
#define  Red_V_PLIC_set_ie2(x)               *((uint32_t *)  0x0C002004) |= (1<<(x))

#define  Red_V_PLIC_claim                    *((uint32_t *)  0x0C200004)

void gpio_isr(void) __attribute__((interrupt, aligned(64)));
void gpio_isr(){
	uint32_t plic_id;

	Red_V_set_pin(5);             // Turn LED ON

	Red_V_GPIO_clear_flag(1);     // Level 3: Clear GPIO0_1 flag
    plic_id = Red_V_PLIC_claim;   // Level 2: Claim GPIO interrupt
    Red_V_PLIC_claim = plic_id;
}


int main (void) {
  struct metal_gpio *gpio_0; // Make instance of GPIO
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


  // Interrupt Configuration

  // Level 3: GPIO0_1 Falling Edge Interrupt Enable
  Red_V_GPIO_set_ie(1);
  Red_V_GPIO_clear_flag(1);

  // Level 2: PLIC Setting for GPIO0
  Red_V_PLIC_GPIO_set_priority(1,7); // Pin 1, priority 7

  // Level 2: PLIC (IE1, bit 9) for GPIO0_1
  Red_V_PLIC_clear_ie(); // Disable all other interrupts
  Red_V_PLIC_set_ie1(9); // Enable GPIO0_1

  // Level 1: Enable interrupts with MIE in mstatus[3]
  volatile uintptr_t saved_config;
  METAL_CPU_GET_CSR(mstatus,saved_config);
  saved_config |= (0x1U<<3);
  METAL_CPU_SET_CSR(mstatus,saved_config);

  // Level 1: Set base vector mtvec
  METAL_CPU_SET_CSR(mtvec,&gpio_isr);

  // Level 1: Enable interrupts with MEIE in mie[11]
  METAL_CPU_GET_CSR(mie,saved_config);
  saved_config |= (0x1U<<11);
  METAL_CPU_SET_CSR(mie,saved_config);

  while(1){
	    if(Red_V_read_pin(0) == 0)      // Read input pin 0
	      Red_V_clear_pin(5);          // Turn LED OFF
  }
  return 0; // Unreachable code
}
