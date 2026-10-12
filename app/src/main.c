#include <our_drivers/our_driver.h>

#include <stdlib.h>

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>

#define SENSOR_LEVEL_MIN 0
#define SENSOR_LEVEL_MAX 100

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

	shell_print(sh, "value: %d, level: %d", val.val1, val.val2);
	return 0;
}

static int cmd_sensor_set(const struct shell *sh, size_t argc, char **argv)
{
	const struct device *dev = sensor_dev();
	char *end = NULL;
	long level;
	int ret;

	if (argc < 2 || argv[1][0] == '\0') {
		shell_error(sh, "missing value");
		return -EINVAL;
	}

	level = strtol(argv[1], &end, 10);
	if (end == argv[1] || *end != '\0') {
		shell_error(sh, "invalid value");
		return -EINVAL;
	}

	if (level < SENSOR_LEVEL_MIN || level > SENSOR_LEVEL_MAX) {
		shell_error(sh, "value out of range (%d-%d)", SENSOR_LEVEL_MIN,
			    SENSOR_LEVEL_MAX);
		return -EINVAL;
	}

	if (!device_is_ready(dev)) {
		shell_error(sh, "sensor not ready");
		return -ENODEV;
	}

	ret = our_driver_set_level(dev, (int32_t)level);
	if (ret < 0) {
		shell_error(sh, "set failed: %d", ret);
		return ret;
	}

	shell_print(sh, "level set to %ld", level);
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
	SHELL_CMD_ARG(set, NULL, "Set the driver level (0-100)", cmd_sensor_set, 2, 0),
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

	LOG_INF("shell ready: sensor fetch | read | info | set <value>");
	k_sleep(K_FOREVER);
	return 0;
}
