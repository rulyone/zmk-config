#include <zephyr/sys/util.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <stdbool.h>
#include <stdint.h>

LOG_MODULE_REGISTER(layer_ug, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/keymap.h>
#include <zmk/rgb_underglow.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zmk/behavior.h>
#include <dt-bindings/zmk/rgb.h>  // for RGB_ON, RGB_COLOR_HSB(), etc.

// Node label provided by ZMK’s behaviors; same thing you write as &rgb_ug in keymaps.
#define RGB_UG_NODE DT_NODELABEL(rgb_ug)

static int rgb_ug_invoke_param(uint32_t p1) {
    const struct device *dev = DEVICE_DT_GET(RGB_UG_NODE);
    if (!device_is_ready(dev)) {
        LOG_WRN("rgb_ug device not ready");
        return -ENODEV;
    }

    const struct zmk_behavior_binding binding = {
        .behavior_dev = dev,
        .param1 = p1,
        .param2 = 0,
    };

    // “Pressed” event; position is not used by this behavior.
    const struct zmk_behavior_binding_event ev = {
        .position = 0,
        .timestamp = k_uptime_get(),
        .state = true,
    };

    return zmk_behavior_invoke_binding(&binding, ev);
}


static int layer_ug_listener(const zmk_event_t *eh);

ZMK_LISTENER(layer_ug, layer_ug_listener);
ZMK_SUBSCRIPTION(layer_ug, zmk_layer_state_changed);

static inline uint32_t pack_hsb(uint16_t h, uint8_t s, uint8_t b) {
    // Matches the packing used by &rgb_ug RGB_COLOR_HSB(H,S,B)
    return ((uint32_t)h << 16) | ((uint32_t)s << 8) | (uint32_t)b;
}

static void set_color_for_layer(uint8_t layer) {
    uint16_t h; uint8_t s; uint8_t b;

    switch (layer) {
    case 3: h = 0;   s = 100; b = 100; break;    // Mouse
    case 4: h = 240; s = 100; b = 100; break;    // Num
    case 1: h = 120; s = 100; b = 100; break;    // Lower
    case 2: h = 45;  s = 100; b = 100; break;    // Raise
    case 0:
    default: h = 0;  s = 0;   b = 15;  break;    // Base
    }

    // Ensure LEDs are on (do this via behavior so both halves turn on)
    (void)rgb_ug_invoke_param(RGB_ON);

    // Set HSB via behavior => split-synced
    (void)rgb_ug_invoke_param(pack_hsb(h, s, b));
}

// // Map each layer to an underglow color.
// static void set_color_for_layer(uint8_t layer) {
//     struct zmk_led_hsb color;

//     switch (layer) {
//     case 3: // Mouse layer
//         color = (struct zmk_led_hsb){ .h = 0, .s = 100, .b = 100 };
//         break;
//     case 4: // Num layer
//         color = (struct zmk_led_hsb){ .h = 240, .s = 100, .b = 100 };
//         break;
//     case 1: // Lower layer
//         color = (struct zmk_led_hsb){ .h = 120, .s = 100, .b = 100 };
//         break;
//     case 2: // Raise layer
//         color = (struct zmk_led_hsb){ .h = 45, .s = 100, .b = 100 };
//         break;
//     case 0: // Default/base layer
//     default:
//         color = (struct zmk_led_hsb){ .h = 0, .s = 0, .b = 15 };
//         break;
//     }

//     (void)zmk_rgb_underglow_set_hsb(color);
// }

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
