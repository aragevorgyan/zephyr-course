#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

static const struct device *sensor_dev(void)
{
	return DEVICE_DT_GET_ANY(our_driver);
}

static int cmd_sensor_fetch(const struct shell *sh, size_t argc, char **argv)
{
	const struct device *dev = sensor_dev();
	int ret;

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	if (!device_is_ready(dev)) {
		shell_error(sh, "sensor not ready");
		return -ENODEV;
	}

	ret = sensor_sample_fetch(dev);
	if (ret < 0) {
		shell_error(sh, "fetch failed: %d", ret);
		return ret;
	}

	shell_print(sh, "sample fetched");
	return 0;
}

static int cmd_sensor_read(const struct shell *sh, size_t argc, char **argv)
{
	const struct device *dev = sensor_dev();
	struct sensor_value val;
	int ret;

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	if (!device_is_ready(dev)) {
		shell_error(sh, "sensor not ready");
		return -ENODEV;
	}

	ret = sensor_channel_get(dev, SENSOR_CHAN_LIGHT, &val);
	if (ret < 0) {
		shell_error(sh, "read failed: %d", ret);
		return ret;
	}

	shell_print(sh, "value: %d", val.val1);
	return 0;
}

static int cmd_sensor_info(const struct shell *sh, size_t argc, char **argv)
{
	const struct device *dev = sensor_dev();

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	shell_print(sh, "name: %s", dev->name);
	shell_print(sh, "ready: %s", device_is_ready(dev) ? "yes" : "no");
	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_sensor,
	SHELL_CMD(fetch, NULL, "Call sensor_sample_fetch()", cmd_sensor_fetch),
	SHELL_CMD(read, NULL, "Call sensor_channel_get() and print the result", cmd_sensor_read),
	SHELL_CMD(info, NULL, "Print the device name and ready state", cmd_sensor_info),
	SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(sensor, &sub_sensor, "LED sensor commands", NULL);

int main(void)
{
	const struct device *dev = sensor_dev();

	if (!device_is_ready(dev)) {
		LOG_ERR("LED sensor not ready");
		return 0;
	}

	LOG_INF("shell ready: sensor fetch | read | info");
	k_sleep(K_FOREVER);
	return 0;
}
