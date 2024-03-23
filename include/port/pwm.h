#ifndef __PICOWJS_PWM_H
#define __PICOWJS_PWM_H

#include <stdint.h>
#define PICOWJS_PWM_DEFAULT_FREQUENCY 490
#define PICOWJS_PWM_DEFAULT_DUTY 1.0
#define PICOWJS_PWM_DUTY_MIN 0
#define PICOWJS_PWM_DUTY_MAX 1.0

/**
 * Initialize all PWM when system started
 */
void picowjs_pwm_init();

/**
 * Cleanup all PWM when system cleanup
 */
void picowjs_pwm_cleanup();

/**
 * Setup a PWM inversion pin
 *
 * @param {uint8_t} pin
 * @param {uint8_t} inv_pin
 * @return result status code
 */
int picowjs_pwm_set_inversion(uint8_t pin, uint8_t inv_pin);

/**
 * Setup a PWM channel
 *
 * @param {uint8_t} pin
 * @param {double} frequency
 * @param {double} duty
 * @return result status code
 */
int picowjs_pwm_setup(uint8_t pin, double frequency, double duty);

/**
 * Check inv_pin can make inversion of the pin
 *
 * @param {uint8_t} pin
 * @param {int8_t} inv_pin, inversion pin, -1 when inv_pin is not used.
 * @return -1 if inv_pin can't generate inversion of the pin
 */
int picowjs_check_pwm_inv_port(uint8_t pin, int8_t inv_pin);

/**
 * Start the PWM channel
 *
 * @param {uint8_t} pin
 */
int picowjs_pwm_start(uint8_t pin);

/**
 * Stop the PWM channel
 *
 * @param {uint8_t} pin
 */
int picowjs_pwm_stop(uint8_t pin);

/**
 * Return the current frequency
 *
 * @param {uint8_t} pin
 * @return {double}
 */
double picowjs_pwm_get_frequency(uint8_t pin);

/**
 * Set the current frequency
 *
 * @param {uint8_t} pin
 * @param {double} frequency
 */
int picowjs_pwm_set_frequency(uint8_t pin, double frequency);

/**
 * Return the current duty cycle
 *
 * @param {uint8_t} pin
 * @return {double}
 */
double picowjs_pwm_get_duty(uint8_t pin);

/**
 * Set the current duty cycle
 *
 * @param {uint8_t} pin
 * @param {double} duty
 */
int picowjs_pwm_set_duty(uint8_t pin, double duty);

/**
 * Close the PWM channel
 */
int picowjs_pwm_close(uint8_t pin);

#endif /* __PICOWJS_PWM_H */
