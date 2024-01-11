#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/gpio.h>

#if !DT_NODE_EXISTS(DT_PATH(zephyr_user))
#error "No suitable devicetree overlay specified"
#endif
#define LED0_NODE DT_ALIAS(led0)

static const struct adc_dt_spec adc_chan0 = ADC_DT_SPEC_GET(DT_PATH(zephyr_user));
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static struct adc_sequence sequence;

int main(void) {
    int err;
    int32_t val_mv = 0;

	if (!gpio_is_ready_dt(&led)) {
		return 0;
	}

	err = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (err < 0) {
		return 0;
	}

	err = gpio_pin_toggle_dt(&led);
	if (err < 0) {
        printk("failed to light led\n");
		return 0;
	}

    if (!adc_is_ready_dt(&adc_chan0)) {
        printk("ADC controller device not ready\n");
        return 0;
    }

    err = adc_channel_setup_dt(&adc_chan0);
    if (err < 0) {
        printk("Could not setup channel, err %d)\n", err);
        return 0;
    }

    err = adc_sequence_init_dt(&adc_chan0, &sequence);
    if (err) {
        printk("Could not retrieve sequence info, err %d)\n", err);
        return 0;
    }
    sequence.buffer = malloc(10*sizeof(char));
    sequence.buffer_size = 10;

    while (true) {
		/* BEGIN */
        err = adc_read_dt(&adc_chan0, &sequence);
        if (err) {
            printk("Could not read, continue...");
            printf("Could not read, continue...");
            if (err == -ENOMEM) {
                gpio_pin_toggle_dt(&led);
            }
            k_msleep(10000);
            continue;
        }
        err = adc_raw_to_millivolts_dt(&adc_chan0, &val_mv);
        if (err) {
            printk("Error during conversion, err: %d", err);
            k_msleep(1000);
            continue;
        } else {
            printk("Reading: %dmv", val_mv);
            if (val_mv == 0) {
                continue;
            }
        }
		/* END */

        gpio_pin_toggle_dt(&led);
        k_msleep(2000);
}

    return 0;
}
