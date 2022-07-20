/******************************************************************************
    Red-V Thing Plus LED_Polling demo,
    by Eduardo Corpeño

    Using the built-in LED and two external buttons to control its brightness, while the
    on-board LED blinks at 1Hz independently.

    Date:  June 20, 2022
    Developed using Freedom Studio v4.18.0.2021-04-1 on Windows 10
    LICENSE: This code is released under the MIT License
    (http://opensource.org/licenses/MIT)
  ******************************************************************************/

#include <metal/gpio.h>
#include <metal/pwm.h>
#include <stdint.h>
#include <time.h>

#define  Red_V_enable_output(x) *((uint32_t *) 0x10012008) |= (1<<(x))
#define  Red_V_set_pin(x)       *((uint32_t *) 0x1001200C) |= (1<<(x))
#define  Red_V_clear_pin(x)     *((uint32_t *) 0x1001200C) &= ~(1<<(x))
#define  Red_V_read_pin(x)      (*((uint32_t *) 0x10012000) & (1<<(x)))
#define  Red_V_enable_pullup(x) *((uint32_t *) 0x10012010) |= (1<<(x))
#define  Red_V_enable_DS(x)     *((uint32_t *) 0x10012014) |= (1<<(x))
#define  Red_V_enable_XOR(x)    *((uint32_t *) 0x10012040) |= (1<<(x))

// Custom delay function
void delay(int number_of_microseconds){
  clock_t start_time = clock();  // Storing start time
  while (clock() < start_time + number_of_microseconds);
}

int main (void) {
  unsigned int dc = 50;       // Default duty cycle at 50%
  struct metal_gpio *gpio_0;  // Make instance of GPIO
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

  while(1){
    // Buttons & external LED PWM code
    if(Red_V_read_pin(0) == 0){      // Read input pin 0
      dc = (dc>0)? dc - 25 : dc;     // Steps of 25% for LED
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
    // Blinky code
    Red_V_clear_pin(5); // Turn on-board LED OFF
    delay(500000);     // 500ms in microseconds
    Red_V_set_pin(5); // Turn on-board LED ON
    delay(500000);   // 500ms in microseconds
  }
  return 0; // Unreachable code
}




