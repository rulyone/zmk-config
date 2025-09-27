#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <stdbool.h>
#include <stdint.h>

#error "layer_underglow build failure test"

#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/keymap.h>
#include <zmk/rgb_underglow.h>

LOG_MODULE_REGISTER(layer_ug, CONFIG_ZMK_LOG_LEVEL);

ZMK_LISTENER(layer_ug, layer_ug_listener);
ZMK_SUBSCRIPTION(layer_ug, zmk_layer_state_changed);

// Map each layer to an underglow color.
static void set_color_for_layer(uint8_t layer) {
    switch (layer) {
    case 3: // Mouse layer
        zmk_rgb_underglow_set_hsb(0, 100, 100); // red
        break;
    case 4: // Num layer
        zmk_rgb_underglow_set_hsb(240, 100, 100); // blue
        break;
    case 1: // Lower layer
        zmk_rgb_underglow_set_hsb(120, 100, 100); // green
        break;
    case 2: // Raise layer
        zmk_rgb_underglow_set_hsb(45, 100, 100); // yellow
        break;
    case 0: // Default/base layer
    default:
        zmk_rgb_underglow_set_hsb(0, 0, 15); // dim white baseline
        break;
    }
}

static int8_t prev_top = -1;

static int layer_ug_listener(const zmk_event_t *eh) {
    if (!as_zmk_layer_state_changed(eh)) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    int8_t top = zmk_keymap_highest_layer_active();
    if (top < 0) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    if (top == prev_top) {
        return ZMK_EV_EVENT_BUBBLE;
    }
    prev_top = top;

    bool on = false;
    if (zmk_rgb_underglow_get_state(&on) == 0 && !on) {
        (void)zmk_rgb_underglow_on();
    }

    set_color_for_layer((uint8_t)top);
    LOG_INF("Layer changed -> top=%d", top);

    return ZMK_EV_EVENT_BUBBLE;
}
