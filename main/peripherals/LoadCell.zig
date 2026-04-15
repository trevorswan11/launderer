const std = @import("std");
const idf = @import("esp_idf");

const layout = @import("pin_layout.zig");

const c = @cImport({
    @cInclude("load_cell.h");
});

const Self = @This();
const FeatherV2Pin = layout.FeatherV2Pin;

pub const Gain = enum(c_uint) {
    chan_a_128 = 0,
    chan_b_32 = 1,
    chan_a_64 = 2,
};

pub const Power = enum(c_uint) {
    on,
    off,
};

const check = idf.err.espCheckError;

load_cell: c.load_cell_t,

// https://github.com/esp-idf-lib/hx711/blob/main/examples/default/main/main.c
pub fn init(mosi: FeatherV2Pin, sck: FeatherV2Pin) !Self {
    var load_cell: c.load_cell_t = undefined;
    try check(c.load_cell_create(&load_cell, @intFromEnum(mosi), @intFromEnum(sck)));
    return .{
        .load_cell = load_cell,
    };
}

pub fn deinit(self: *Self) void {
    c.load_cell_destroy(&self.lcd);
}

pub fn setPower(self: *Self, power: Power) !void {
    try check(c.load_cell_set_power(&self.load_cell, @intFromEnum(power)));
}

pub fn setGain(self: *Self, gain: Gain) !void {
    try check(c.load_cell_set_gain(&self.load_cell, @intFromEnum(gain)));
}

pub fn isReady(self: *Self) !bool {
    var ready: bool = undefined;
    try check(c.load_cell_is_ready(&self.load_cell, &ready));
    return ready;
}

pub fn wait(self: *Self, timeout_ms: usize) !void {
    try check(c.load_cell_wait(&self.load_cell, timeout_ms));
}

pub fn readData(self: *Self) !i32 {
    var data: i32 = undefined;
    try check(c.load_cell_read_data(&self.load_cell, &data));
    return data;
}

pub fn readAverage(self: *Self, times: usize) !i32 {
    var data: i32 = undefined;
    try check(c.load_cell_read_average(&self.load_cell, times, &data));
    return data;
}
