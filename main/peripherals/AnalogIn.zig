const std = @import("std");
const idf = @import("esp_idf");

const layout = @import("pin_layout.zig");

const c = @cImport({
    @cInclude("adc.h");
});

const Self = @This();
const FeatherV2Pin = layout.FeatherV2Pin;

const check = idf.err.espCheckError;

analog: c.analog_in,

pub fn init(pin: FeatherV2Pin) !Self {
    var analog: c.analog_in = undefined;
    try check(c.analog_in_init(&analog, @intFromEnum(pin)));
    return .{ .analog = analog };
}

pub fn read(self: *Self) !i32 {
    var value: i32 = undefined;
    try check(c.analog_in_read(&self.analog, &value));
    return value;
}
