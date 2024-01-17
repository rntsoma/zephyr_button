#include <stdio.h>
#include <stdbool.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/input/input.h>
#include <zephyr/drivers/gpio.h>

#if !DT_NODE_EXISTS(DT_PATH(leds))
#error "No suitable devicetree overlay specified"
#endif
#if !DT_NODE_EXISTS(DT_PATH(buttons))
#error "No suitable devicetree overlay specified"
#endif
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define BUTTON_NODE DT_ALIAS(button0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led_proto = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);

static void button_handler(struct input_event *evt) {
    gpio_pin_toggle_dt(&led);
    gpio_pin_toggle_dt(&led_proto);
}

INPUT_CALLBACK_DEFINE(NULL, button_handler);

int main(void) {
    int err;

	if (!gpio_is_ready_dt(&led)) {
		return 0;
	}

	if (!gpio_is_ready_dt(&button)) {
		return 0;
	}

	err = gpio_pin_configure_dt(&button, GPIO_INPUT);
	if (err < 0) {
		return 0;
	}

	err = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (err < 0) {
		return 0;
	}

	err = gpio_pin_configure_dt(&led_proto, GPIO_OUTPUT_ACTIVE);
	if (err < 0) {
		return 0;
	}

    while (true) {
		/* BEGIN */
		/* END */

        /* gpio_pin_toggle_dt(&led); */
        /* gpio_pin_toggle_dt(&led_proto); */
        /* k_msleep(2000); */
}

    return 0;
}
