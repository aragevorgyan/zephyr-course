/*
 * Public API for the on-board LED sensor.
 *
 * sensor_sample_fetch() turns the LED on.
 * sensor_channel_get() turns the LED off.
 */

#ifndef OUR_DRIVERS_OUR_DRIVER_H_
#define OUR_DRIVERS_OUR_DRIVER_H_

#include <zephyr/device.h>
#include <stdint.h>

/* Store a caller-chosen level in the driver's runtime data. */
int our_driver_set_level(const struct device *dev, int32_t level);

#endif /* OUR_DRIVERS_OUR_DRIVER_H_ */
