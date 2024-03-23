#ifndef __PICOWJS_ADC_H
#define __PICOWJS_ADC_H

#include <stdint.h>

/**
 * Initialize all ADC channels when system started
 */
void picowjs_adc_init();

/**
 * Cleanup all ADC channels when system cleanup
 */
void picowjs_adc_cleanup();

/**
 * Setup a ADC channel
 *
 * @param pin Pin number.
 * @return Returns channel number on success or -1 on failure.
 */
int picowjs_adc_setup(uint8_t pin);

/**
 * Read value from a ADC channel
 *
 * @param ADC index (output of picowjs_adc_setup).
 * @return Return a value read between 0 and 1.
 */
double picowjs_adc_read(uint8_t adcIndex);

/**
 * Close the ADC channel
 *
 * @param pin Pin number.
 * @return Returns 0 on success or -1 on failure.
 */
int picowjs_adc_close(uint8_t pin);

#endif /* __PICOWJS_ADC_H */
