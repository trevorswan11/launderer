const std = @import("std");
const idf = @import("esp_idf");

const layout = @import("pin_layout.zig");

const c = @cImport({
    @cInclude("hd44780.h");
    @cInclude("i2cdev.h");
    @cInclude("pcf8574.h");
    @cInclude("soc/gpio_num.h");
});

const Self = @This();
const FeatherV2Pin = layout.FeatherV2Pin;

const check = idf.err.espCheckError;

var pcf_device: c.i2c_dev_t = undefined;
var i2c_initialized = false;

fn lcd_write_cb(_: [*c]const c.hd44780_t, data: u8) callconv(.C) idf.sys.esp_err_t {
    return c.pcf8574_port_write(&pcf_device, data);
}

allocator: std.mem.Allocator,

config: c.hd44780_t,
addr: u8,
sda: FeatherV2Pin,
scl: FeatherV2Pin,

// https://github.com/esp-idf-lib/pcf8574/blob/main/examples/i2c/main/main.c
pub fn init(allocator: std.mem.Allocator, addr: u8, sda: FeatherV2Pin, scl: FeatherV2Pin) Self {
    return .{
        .allocator = allocator,
        .addr = addr,
        .sda = sda,
        .scl = scl,
        .config = .{
            .write_cb = lcd_write_cb,
            .pins = .{
                .rs = 0,
                .e = 2,
                .d4 = 4,
                .d5 = 5,
                .d6 = 6,
                .d7 = 7,
                .bl = 3,
            },
            .font = c.HD44780_FONT_5X8,
            .lines = 2,
            .backlight = true,
        },
    };
}

pub fn begin(self: *Self) !void {
    if (!i2c_initialized) {
        try check(c.i2cdev_init());
        try check(c.pcf8574_init_desc(
            &pcf_device,
            self.addr,
            idf.sys.I2C_NUM_0,
            @intCast(self.sda.gpio()),
            @intCast(self.scl.gpio()),
        ));
        i2c_initialized = true;
    }

    try check(c.hd44780_init(&self.config));
    try check(c.hd44780_switch_backlight(&self.config, true));
}

pub fn setCursor(self: *Self, col: u8, row: u8) !void {
    try check(c.hd44780_gotoxy(&self.config, col, row));
}

pub fn print(self: *Self, text: []const u8) !void {
    const c_str = try self.allocator.dupeZ(u8, text);
    defer self.allocator.free(c_str);

    try check(c.hd44780_puts(&self.config, c_str.ptr));
}

pub fn printAt(self: *Self, text: []const u8, col: u8, row: u8) !void {
    try self.setCursor(col, row);
    try self.print(text);
}

pub fn clear(self: *Self) void {
    check(idf.hd44780_clear(&self.config));
}
