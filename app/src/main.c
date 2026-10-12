#include <our_drivers/our_driver.h>

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
	const struct device *dev = DEVICE_DT_GET_ANY(our_driver);
	struct sensor_value val;

	if (!device_is_ready(dev)) {
		LOG_ERR("LED sensor not ready");
		return 0;
	}

	if (our_driver_set_level(dev, 42) < 0) {
		LOG_ERR("set level failed");
		return 0;
	}

	while (1) {
		if (sensor_sample_fetch(dev) < 0) {
			LOG_ERR("sample fetch failed");
			return 0;
		}

		k_msleep(500);

		if (sensor_channel_get(dev, SENSOR_CHAN_LIGHT, &val) < 0) {
			LOG_ERR("channel get failed");
			return 0;
		}

		LOG_INF("channel value %d, level %d", val.val1, val.val2);
		k_msleep(500);
	}

	return 0;
}
