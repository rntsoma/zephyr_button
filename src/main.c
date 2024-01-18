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

#define DEBOUNCE_DELAY 50

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led_proto = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);
static struct gpio_callback button_callback;
static int last_read = 0;

static void button_handler(const struct device *port,
        struct gpio_callback *cb,
        gpio_port_pins_t pins) {

    printk("Button Handler\n");
    if (gpio_pin_get_dt(&button)) {
        printk("Pressed\n");
        if (k_uptime_get() - last_read >= DEBOUNCE_DELAY) {
            gpio_pin_toggle_dt(&led);
            gpio_pin_toggle_dt(&led_proto);
        }
        last_read = k_uptime_get();
    }
}

int main(void) {
    int err;
    printk("Hello\n");

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

	err = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
	if (err < 0) {
		return 0;
	}

	gpio_init_callback(&button_callback, button_handler, BIT(button.pin));

	err = gpio_add_callback_dt(&button, &button_callback);
	if (err < 0) {
		return 0;
	}

	err = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
	if (err < 0) {
		return 0;
	}

	err = gpio_pin_configure_dt(&led_proto, GPIO_OUTPUT_INACTIVE);
	if (err < 0) {
		return 0;
	}

    return 0;
}
