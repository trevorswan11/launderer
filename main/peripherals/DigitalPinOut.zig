const std = @import("std");
const idf = @import("esp_idf");

const layout = @import("pin_layout.zig");

const Self = @This();
const FeatherV2Pin = layout.FeatherV2Pin;

const check = idf.err.espCheckError;

gpio_num: idf.sys.gpio_num_t,

pub fn init(pin: FeatherV2Pin) !Self {
    const gpio_num = pin.gpio();
    try check(idf.sys.gpio_reset_pin(gpio_num));
    try check(idf.sys.gpio_set_direction(gpio_num, idf.sys.GPIO_MODE_INPUT));
    return .{ .gpio_num = gpio_num };
}

pub fn setHigh(self: Self) !void {
    try check(idf.gpio_set_level(self.gpio_num, 1));
}

pub fn setLow(self: Self) !void {
    try check(idf.gpio_set_level(self.gpio_num, 0));
}
