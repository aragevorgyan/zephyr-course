#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <our_drivers/our_driver.h>

#define DT_DRV_COMPAT our_driver

LOG_MODULE_REGISTER(our_driver, CONFIG_SENSOR_LOG_LEVEL);

struct our_driver_config {
	struct gpio_dt_spec gpio;
};

struct our_driver_data {
	bool is_on;
};

static int our_driver_sample_fetch(const struct device *dev,
				    enum sensor_channel chan)
{
	const struct our_driver_config *cfg = dev->config;
	struct our_driver_data *data = dev->data;
	int ret;

	ARG_UNUSED(chan);

	ret = gpio_pin_set_dt(&cfg->gpio, 1);
	if (ret < 0) {
		return ret;
	}

	data->is_on = true;
	LOG_INF("LED on");
	return 0;
}

static int our_driver_channel_get(const struct device *dev,
				   enum sensor_channel chan,
				   struct sensor_value *val)
{
	const struct our_driver_config *cfg = dev->config;
	struct our_driver_data *data = dev->data;
	int ret;

	ARG_UNUSED(chan);

	val->val1 = data->is_on ? 1 : 0;
	val->val2 = 0;

	ret = gpio_pin_set_dt(&cfg->gpio, 0);
	if (ret < 0) {
		return ret;
	}

	data->is_on = false;
	LOG_INF("LED off");
	return 0;
}

static DEVICE_API(sensor, our_driver_api) = {
	.sample_fetch = our_driver_sample_fetch,
	.channel_get = our_driver_channel_get,
};

static int our_driver_init(const struct device *dev)
{
	const struct our_driver_config *cfg = dev->config;
	struct our_driver_data *data = dev->data;
	int ret;

	if (!gpio_is_ready_dt(&cfg->gpio)) {
		return -ENODEV;
	}

	ret = gpio_pin_configure_dt(&cfg->gpio, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		return ret;
	}

	data->is_on = false;
	return 0;
}

#define OUR_DRIVER_DEFINE(inst)                                            \
	static struct our_driver_data data_##inst;                         \
	static const struct our_driver_config cfg_##inst = {               \
		.gpio = GPIO_DT_SPEC_INST_GET(inst, gpios),                \
	};                                                                 \
	DEVICE_DT_INST_DEFINE(inst, our_driver_init, NULL, &data_##inst,   \
			      &cfg_##inst, POST_KERNEL,                    \
			      CONFIG_SENSOR_INIT_PRIORITY, &our_driver_api)

DT_INST_FOREACH_STATUS_OKAY(OUR_DRIVER_DEFINE)
