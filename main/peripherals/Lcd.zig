const std = @import("std");
const idf = @import("esp_idf");

const layout = @import("pin_layout.zig");

const c = @cImport({
    @cInclude("lcd.h");
});

const Self = @This();
const FeatherV2Pin = layout.FeatherV2Pin;

const check = idf.err.espCheckError;

allocator: std.mem.Allocator,
lcd: c.lcd_t,

// https://github.com/esp-idf-lib/pcf8574/blob/main/examples/i2c/main/main.c
pub fn init(allocator: std.mem.Allocator, addr: u8, sda: FeatherV2Pin, scl: FeatherV2Pin) !Self {
    var lcd: c.lcd_t = undefined;
    try check(c.lcd_create(&lcd, addr, @intFromEnum(sda), @intFromEnum(scl)));
    return .{
        .allocator = allocator,
        .lcd = lcd,
    };
}

pub fn deinit(self: *Self) void {
    c.lcd_destroy(&self.lcd);
}

pub fn begin(self: *Self) !void {
    try check(c.lcd_begin(&self.lcd));
}

pub fn setCursor(self: *Self, col: u8, row: u8) !void {
    try check(c.lcd_set_cursor(&self.lcd, col, row));
}

pub fn print(self: *Self, text: []const u8) !void {
    const c_str = try self.allocator.dupeZ(u8, text);
    defer self.allocator.free(c_str);
    try check(c.lcd_print(&self.lcd, c_str));
}

pub fn printAt(self: *Self, text: []const u8, col: u8, row: u8) !void {
    try self.setCursor(col, row);
    try self.print(text);
}

pub fn clear(self: *Self) !void {
    try check(c.lcd_clear(&self.lcd));
}
