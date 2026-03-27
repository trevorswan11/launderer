const std = @import("std");
const idf = @import("esp_idf");

const layout = @import("pin_layout.zig");

const Self = @This();
const FeatherV2Pin = layout.FeatherV2Pin;

const check = idf.err.espCheckError;

gpio_num: idf.sys.gpio_num_t,

pub fn init(pin: FeatherV2Pin, mode: idf.gpio.PullMode) !Self {
    const gpio_num = pin.gpio();
    try check(idf.sys.gpio_reset_pin(gpio_num));
    try check(idf.sys.gpio_set_direction(gpio_num, idf.sys.GPIO_MODE_INPUT));
    try check(idf.sys.gpio_set_pull_mode(gpio_num, @intFromEnum(mode)));
    return .{ .gpio_num = gpio_num };
}

pub fn read(self: Self) bool {
    return idf.sys.gpio_get_level(self.gpio_num) != 0;
}
