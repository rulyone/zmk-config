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
/* Devicetree node and device name for &rgb_ug behavior */
#define RGB_UG_NODE DT_NODELABEL(rgb_ug)
#define RGB_UG_DEV_NAME DEVICE_DT_NAME(RGB_UG_NODE)

/* Invoke the &rgb_ug behavior; split-aware on current ZMK. */
static int rgb_ug_invoke_cmd(uint32_t cmd, uint32_t param)
{
    const struct device *dev = DEVICE_DT_GET(RGB_UG_NODE);
    if (!device_is_ready(dev)) {
        LOG_WRN("rgb_ug device not ready");
        return -ENODEV;
    }

    const struct zmk_behavior_binding binding = {
        .behavior_dev = RGB_UG_DEV_NAME, /* const char * device name */
        .param1 = cmd,
        .param2 = param,
    };

    const struct zmk_behavior_binding_event ev = {
        .position = 0,                 /* unused by rgb_ug */
        .timestamp = k_uptime_get(),
    };

    /* true => “pressed” edge; rgb_ug reacts on press. */
    int rc = zmk_behavior_invoke_binding(&binding, ev, true);
    if (rc) {
        LOG_WRN("rgb_ug cmd=0x%lx param=0x%lx rc=%d",
                (unsigned long)cmd, (unsigned long)param, rc);
    }
    return rc;
}

/* Pack H/S/B exactly like RGB_COLOR_HSB(H,S,B) would. */
static inline uint32_t pack_hsb(uint16_t h, uint8_t s, uint8_t b)
{
    return ((uint32_t)h << 16) | ((uint32_t)s << 8) | (uint32_t)b;
}

/* Map each top layer to an underglow color and push via behavior. */
static void set_color_for_layer(uint8_t layer)
{
    uint16_t h; uint8_t s; uint8_t b;

    switch (layer) {
    case 3:  h = 0;   s = 100; b = 100; break;   /* Mouse */
    case 4:  h = 240; s = 100; b = 100; break;   /* Num   */
    case 1:  h = 120; s = 100; b = 100; break;   /* Lower */
    case 2:  h = 45;  s = 100; b = 100; break;   /* Raise */
    case 0:
    default: h = 0;   s = 0;   b = 15;  break;   /* Base  */
    }

    /* Turn on + set color through the behavior (split-synced). */
    (void)rgb_ug_invoke_cmd(RGB_ON_CMD, 0);
    (void)rgb_ug_invoke_cmd(RGB_COLOR_HSB_CMD, pack_hsb(h, s, b));
}

/* Remember last top layer so we don't spam the behavior. */
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

    set_color_for_layer(top);
    LOG_INF("Top layer -> %u", top);

    return ZMK_EV_EVENT_BUBBLE;

}

ZMK_LISTENER(layer_ug, layer_ug_listener);
ZMK_SUBSCRIPTION(layer_ug, zmk_layer_state_changed);