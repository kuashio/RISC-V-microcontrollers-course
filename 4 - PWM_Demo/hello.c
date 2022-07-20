/******************************************************************************
    Red-V Thing Plus PWM_Demo,
    by Eduardo Corpeño

    Controlling the brightness of the built-in LED with two external buttons using PWM.
    Alternatively, a hobby servo can be controlled with those buttons.
    The servo must receive a 50Hz signal, with pulse widths between 1ms and 2ms.

    Date:  June 20, 2022
    Developed using Freedom Studio v4.18.0.2021-04-1 on Windows 10
    LICENSE: This code is released under the MIT License
    (http://opensource.org/licenses/MIT)
  ******************************************************************************/

#include <metal/pwm.h>
#include <metal/gpio.h>
#include <stdint.h>

// Uncomment the following definition for the servo application.
// Leave it commented for LED brightness application.
// #define SERVO

// Our own macro functions defined for efficiency or because the
// Freedom Metal Library didn't provide their functionality.

#define  Red_V_enable_output(x) *((uint32_t *) 0x10012008) |= (1<<(x))
#define  Red_V_set_pin(x)       *((uint32_t *) 0x1001200C) |= (1<<(x))
#define  Red_V_clear_pin(x)     *((uint32_t *) 0x1001200C) &= ~(1<<(x))
#define  Red_V_read_pin(x)      (*((uint32_t *) 0x10012000) & (1<<(x)))
#define  Red_V_enable_pullup(x) *((uint32_t *) 0x10012010) |= (1<<(x))
#define  Red_V_enable_DS(x)     *((uint32_t *) 0x10012014) |= (1<<(x))
#define  Red_V_enable_XOR(x)    *((uint32_t *) 0x10012040) |= (1<<(x))

// A quick and dirty delay function to be used experimentally.
void delay(int t){
	volatile uint32_t x, y;
	x = t;
	while(x--){
		y=x;
		while(y--);
	}
}

int main (void) {

#ifdef SERVO
  unsigned int dc = 7;        // About the center of the servo's position
#else
  unsigned int dc = 50;       // Default duty cycle at 50%
#endif

  struct metal_gpio *gpio_0;  // Instance of GPIO
  struct metal_pwm *pwm_1;    // Instance of PWM

  // GPIO Configuration for the On-Board LED and Push Buttons
  gpio_0 = metal_gpio_get_device(0);     // Get a GPIO device instance
  // LED
  metal_gpio_disable_input(gpio_0, 5);   // Disable input 5
  Red_V_enable_output(5);                // Enable output 5
  metal_gpio_disable_pinmux(gpio_0, 5);  // Disable alternate functions for pin 5
  // Push Buttons
  metal_gpio_enable_input(gpio_0, 0);    // Enable input 0
  metal_gpio_enable_input(gpio_0, 1);    // Enable input 1
  metal_gpio_disable_output(gpio_0, 0);  // Disable output 0
  metal_gpio_disable_output(gpio_0, 1);  // Disable output 1
  metal_gpio_disable_pinmux(gpio_0, 0);  // Disable alternate functions for pin 0
  metal_gpio_disable_pinmux(gpio_0, 1);  // Disable alternate functions for pin 1
  Red_V_enable_pullup(0);                // Enable pull-up for pin 0
  Red_V_enable_pullup(1);                // Enable pull-up for pin 1

  // PWM Configuration. We'll use PWM1_1, which uses GPIO0_19.
#ifndef SERVO
  Red_V_enable_DS(19);  // High-Output Strength
#endif
  Red_V_enable_XOR(19); // Invert the output for an active-high PWM

  pwm_1 = metal_pwm_get_device(1);        // Get a PWM device instance
  while (pwm_1 == NULL);                  // Halt. Something went wrong
  metal_pwm_enable(pwm_1);                // Enable PWM1
  metal_gpio_disable_pinmux(gpio_0, 20);  // Disable alternate functions for pin 20
  metal_gpio_disable_pinmux(gpio_0, 21);  // Disable alternate functions for pin 21
  metal_gpio_disable_pinmux(gpio_0, 22);  // Disable alternate functions for pin 22

  // Set PWM1 to 1kHz or 50Hz
#ifdef SERVO
  metal_pwm_set_freq(pwm_1, 1, 50);       // Set frequency of PWM1_1 to 50Hz for Servo
#else
  metal_pwm_set_freq(pwm_1, 1, 1000);     // Set frequency of PWM1_1 to 1kHz for LED
#endif

  metal_pwm_set_duty(pwm_1, 1, dc, METAL_PWM_PHASE_CORRECT_DISABLE); // Set Duty Cycle for PWM1_1
  metal_pwm_trigger(pwm_1, 1, METAL_PWM_CONTINUOUS);                 // Start in continuous mode

  while(1){
	    if(Red_V_read_pin(0) == 0){      // Read input pin 0
	      Red_V_set_pin(5);              // Turn LED ON
#ifdef SERVO
	      dc = (dc>5)? dc - 1 : dc;      // steps of 1% for servo
#else
	      dc = (dc>0)? dc - 10 : dc;     // steps of 10% for LED
#endif
	      metal_pwm_set_duty(pwm_1, 1, dc, METAL_PWM_PHASE_CORRECT_DISABLE);
	      delay(300);                    // Software debounce
	      Red_V_clear_pin(5);            // Turn LED OFF
	      while(Red_V_read_pin(0) == 0); // Wait until button is released
	      delay(300);                    // Software debounce
	    }
	    else if(Red_V_read_pin(1) == 0){ // Read input pin 1
	      Red_V_set_pin(5);              // Turn LED ON
#ifdef SERVO
	      dc = (dc<10)? dc + 1 : dc;     // steps of 1% for servo
#else
	      dc = (dc<100)? dc + 10 : dc;   // steps of 10% for LED
#endif
	      metal_pwm_set_duty(pwm_1, 1, dc, METAL_PWM_PHASE_CORRECT_DISABLE);
	      delay(300);                    // Software debounce
	      Red_V_clear_pin(5);            // Turn LED OFF
	      while(Red_V_read_pin(1) == 0); // Wait until button is released
	      delay(300);                    // Software debounce
	    }
  }
  return 0; // Unreachable code
}
