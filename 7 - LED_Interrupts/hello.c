/******************************************************************************
    Red-V Thing Plus LED_Interrupts demo,
    by Eduardo Corpeño

    Using the built-in LED and two external buttons to control its brightness by polling,
    while the on-board LED blinks at 1Hz independently by interrupts.

    Date:  June 20, 2022
    Developed using Freedom Studio v4.18.0.2021-04-1 on Windows 10
    LICENSE: This code is released under the MIT License
    (http://opensource.org/licenses/MIT)
  ******************************************************************************/

#include <metal/gpio.h>
#include <metal/pwm.h>
#include <metal/csr.h>
#include <stdint.h>
#include <time.h>

// Macro Functions

#define Red_V_enable_output(x) *((uint32_t *) 0x10012008) |=  (1<<(x))
#define Red_V_set_pin(x)       *((uint32_t *) 0x1001200C) |=  (1<<(x))
#define Red_V_clear_pin(x)     *((uint32_t *) 0x1001200C) &= ~(1<<(x))
#define Red_V_read_pin(x)     (*((uint32_t *) 0x10012000) &   (1<<(x)))
#define Red_V_enable_pullup(x) *((uint32_t *) 0x10012010) |=  (1<<(x))
#define Red_V_enable_DS(x)     *((uint32_t *) 0x10012014) |=  (1<<(x))
#define Red_V_enable_XOR(x)    *((uint32_t *) 0x10012040) |=  (1<<(x))

#define Red_V_PWM2_1_set_value(x) *((uint32_t *) 0x10035024) =   (x)
#define Red_V_PWM2_sticky()       *((uint32_t *) 0x10035000) |=  (1<<8)
#define Red_V_PWM2_clear_flag(x)  *((uint32_t *) 0x10035000) &= ~(1<<((x)+28))

#define Red_V_PLIC_PWM2_set_priority(ch,p) *((uint32_t *) (0x0C0000C0+4*(ch))) = (p)
#define Red_V_PLIC_clear_ie()              *((uint32_t *)  0x0C002000) = 0;\
                                           *((uint32_t *)  0x0C002004) = 0
#define Red_V_PLIC_set_ie1(x)              *((uint32_t *)  0x0C002000) |= (1<<(x))
#define Red_V_PLIC_set_ie2(x)              *((uint32_t *)  0x0C002004) |= (1<<(x))

#define Red_V_PLIC_claim                   *((uint32_t *)  0x0C200004)


void pwm2_isr(void) __attribute__((interrupt, aligned(64)));
void pwm2_isr(){
	static uint32_t count = 0;
	uint32_t plic_id;

	count++;

    // Blinky code
    if(count == 500)
  	  Red_V_clear_pin(5);    // Turn on-board LED OFF

    if(count == 1000){
    	Red_V_set_pin(5);  // Turn on-board LED ON
    	count = 0;
    }

    Red_V_PWM2_clear_flag(0);     // Clear PWM2_0 flag
    plic_id = Red_V_PLIC_claim;   // Level 2: Claim GPIO interrupt
    Red_V_PLIC_claim = plic_id;
}

// Custom delay function
void delay(int number_of_microseconds){
  clock_t start_time = clock();  // Storing start time
  while (clock() < start_time + number_of_microseconds);
}

int main (void) {
  unsigned int dc = 50;       // Default duty cycle at 50%
  struct metal_gpio *gpio_0;  // Make instance of GPIO
  struct metal_pwm *pwm_2;    // Instance of PWM
  struct metal_pwm *pwm_1;    // Instance of PWM

  gpio_0 = metal_gpio_get_device(0);

  // On-Board LED Configuration
  metal_gpio_disable_input(gpio_0, 5);
  Red_V_enable_output(5);
  metal_gpio_disable_pinmux(gpio_0, 5);
  metal_gpio_set_pin(gpio_0, 5, 1);

  // Buttons Configuration
  metal_gpio_enable_input(gpio_0, 0);    // enable input 0
  metal_gpio_enable_input(gpio_0, 1);    // enable input 1

  metal_gpio_disable_output(gpio_0, 0);  // disable output 0
  metal_gpio_disable_output(gpio_0, 1);  // disable output 1

  metal_gpio_disable_pinmux(gpio_0, 0);  // disable alternate functions for pin 0
  metal_gpio_disable_pinmux(gpio_0, 1);  // disable alternate functions for pin 1

  Red_V_enable_pullup(0);  // enable pull-up for pin 0
  Red_V_enable_pullup(1);  // enable pull-up for pin 1

  // PWM Configuration. We'll use PWM1_1, which uses GPIO0_19.
  Red_V_enable_DS(19);  // High-Output Strength
  Red_V_enable_XOR(19); // Invert the output for an active-high PWM

  pwm_1 = metal_pwm_get_device(1);        // Get a PWM device instance
  while (pwm_1 == NULL);                  // Halt. Something went wrong
  metal_pwm_enable(pwm_1);                // Enable PWM1
  metal_gpio_disable_pinmux(gpio_0, 20);  // Disable alternate functions for pin 20
  metal_gpio_disable_pinmux(gpio_0, 21);  // Disable alternate functions for pin 21
  metal_gpio_disable_pinmux(gpio_0, 22);  // Disable alternate functions for pin 22

  // Set PWM1 to 1kHz
  metal_pwm_set_freq(pwm_1, 1, 1000);     // Set frequency of PWM1_1 to 1kHz for LED

  metal_pwm_set_duty(pwm_1, 1, dc, METAL_PWM_PHASE_CORRECT_DISABLE); // Set Duty Cycle for PWM1_1
  metal_pwm_trigger(pwm_1, 1, METAL_PWM_CONTINUOUS);                 // Start in continuous mode


  // PWM Configuration. We'll use PWM2 as a periodic interrupt.
  pwm_2 = metal_pwm_get_device(2);        // Get a PWM device instance
  while (pwm_2 == NULL);                  // Halt. Something went wrong
  metal_pwm_enable(pwm_2);                // Enable PWM0
  metal_gpio_disable_pinmux(gpio_0, 10);  // Disable alternate functions for pin 20
  metal_gpio_disable_pinmux(gpio_0, 11);  // Disable alternate functions for pin 21
  metal_gpio_disable_pinmux(gpio_0, 12);  // Disable alternate functions for pin 22
  metal_gpio_disable_pinmux(gpio_0, 13);  // Disable alternate functions for pin 22

  // Set PWM2 to 1kHz
  metal_pwm_set_freq(pwm_2, 1, 1000);     // Set frequency of PWM2_1 to 1kHz for LED
  metal_pwm_trigger(pwm_2, 1, METAL_PWM_CONTINUOUS);                 // Start in continuous mode
  Red_V_PWM2_sticky();
  Red_V_PWM2_1_set_value(8000);


  // Interrupt Configuration

  // Level 3: There's no ie register for PWM channels.
  Red_V_PWM2_clear_flag(0);     // Clear PWM2_0 flag

  // Level 2: PLIC Setting for PWM2
  Red_V_PLIC_PWM2_set_priority(0,7); // Channel 0, priority 7

  // Level 2: PLIC (IE2, bit 16) for PWM2_0
  Red_V_PLIC_clear_ie();  // Disable all other interrupts
  Red_V_PLIC_set_ie2(16); // Enable PWM2_0

  // Level 1: Enable interrupts with MIE in mstatus[3]
  volatile uintptr_t saved_config;
  METAL_CPU_GET_CSR(mstatus,saved_config);
  saved_config |= (0x1U<<3);
  METAL_CPU_SET_CSR(mstatus,saved_config);

  // Level 1: Set base vector mtvec
  METAL_CPU_SET_CSR(mtvec,&pwm2_isr);

  // Level 1: Enable interrupts with MEIE in mie[11]
  METAL_CPU_GET_CSR(mie,saved_config);
  saved_config |= (0x1U<<11);
  METAL_CPU_SET_CSR(mie,saved_config);

  while(1){
    // Buttons & external LED PWM code
    if(Red_V_read_pin(0) == 0){      // Read input pin 0
      dc = (dc>0)? dc - 25 : dc;     // steps of 25% for LED
      metal_pwm_set_duty(pwm_1, 1, dc, METAL_PWM_PHASE_CORRECT_DISABLE);
      delay(50000);                  // Software debounce
      while(Red_V_read_pin(0) == 0); // Wait until button is released
      delay(50000);                  // Software debounce
    }
    else if(Red_V_read_pin(1) == 0){ // Read input pin 1
      dc = (dc<100)? dc + 25 : dc;   // Steps of 25% for LED
      metal_pwm_set_duty(pwm_1, 1, dc, METAL_PWM_PHASE_CORRECT_DISABLE);
      delay(50000);                  // Software debounce
      while(Red_V_read_pin(1) == 0); // Wait until button is released
      delay(50000);                  // Software debounce
    }
  }
  return 0; // Unreachable code
}





