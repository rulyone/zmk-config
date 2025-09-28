#include <zephyr/sys/util.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <stdbool.h>
#include <stdint.h>

LOG_MODULE_REGISTER(layer_ug, CONFIG_ZMK_LOG_LEVEL);

/*
 * Only the central half has layer/keymap events linked in.
 * Build the listener there. On peripheral/no-split builds, this file compiles
 * to an empty translation unit (no references -> no link errors).
 */
#if !IS_ENABLED(CONFIG_ZMK_SPLIT) || IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)


#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/keymap.h>
#include <zmk/rgb_underglow.h>

static int layer_ug_listener(const zmk_event_t *eh);

ZMK_LISTENER(layer_ug, layer_ug_listener);
ZMK_SUBSCRIPTION(layer_ug, zmk_layer_state_changed);

// Map each layer to an underglow color.
static void set_color_for_layer(uint8_t layer) {
    struct zmk_led_hsb color;

    switch (layer) {
    case 3: // Mouse layer
        color = (struct zmk_led_hsb){ .h = 0, .s = 100, .b = 100 };
        break;
    case 4: // Num layer
        color = (struct zmk_led_hsb){ .h = 240, .s = 100, .b = 100 };
        break;
    case 1: // Lower layer
        color = (struct zmk_led_hsb){ .h = 120, .s = 100, .b = 100 };
        break;
    case 2: // Raise layer
        color = (struct zmk_led_hsb){ .h = 45, .s = 100, .b = 100 };
        break;
    case 0: // Default/base layer
    default:
        color = (struct zmk_led_hsb){ .h = 0, .s = 0, .b = 15 };
        break;
    }

    (void)zmk_rgb_underglow_set_hsb(color);
}

/* Use an unsigned type (matches API) and an impossible sentinel (0xFF). */
static uint8_t prev_top = 0xFF;

static int layer_ug_listener(const zmk_event_t *eh) {
    if (!as_zmk_layer_state_changed(eh)) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    uint8_t top = zmk_keymap_highest_layer_active();

    if (top == prev_top) {
        return ZMK_EV_EVENT_BUBBLE;
    }
    prev_top = top;

    bool on = false;
    if (zmk_rgb_underglow_get_state(&on) == 0 && !on) {
        (void)zmk_rgb_underglow_on();
    }

    set_color_for_layer(top);
    LOG_INF("Layer changed -> top=%u", top);

    return ZMK_EV_EVENT_BUBBLE;
}

#endif /* central-only */