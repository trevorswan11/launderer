const std = @import("std");
const builtin = @import("builtin");
const idf = @import("esp_idf");

const AnalogIn = @import("peripherals/AnalogIn.zig");
const DigitalPinIn = @import("peripherals/DigitalPinIn.zig");
const Lcd = @import("peripherals/Lcd.zig");
const PwmOut = @import("peripherals/PwmOut.zig");
const layout = @import("peripherals/pin_layout.zig");

comptime {
    @export(&main, .{ .name = "app_main" });
}

const Dirtiness = enum {
    normal,
    dirty,
    nasty,
};

// Global state
const history_size: usize = 20;
var history = [_]i32{0} ** history_size;
var history_index: usize = 0;
var history_full = false;
var tare_offset: i32 = 0;
var tared = false;

// From desmos fit
const fit_A: f32 = -478286.117341;
const fit_B: f32 = 830.395033677;
const fit_C: f32 = 580.401879334;

fn stonesFromAdc(zeroed: i32) f32 {
    if (zeroed <= 0) return 0.0;
    const f_zeroed: f32 = @floatFromInt(zeroed);
    return fit_A / (f_zeroed - fit_C) - fit_B;
}

// The amount of time to reach the tip of the tube
const prime_time_ms: idf.sys.TickType_t = 150;
const medium_normal: f32 = 0.5;
const medium_dirty: f32 = 1.0;
const medium_nasty: f32 = 1.5;

const small_scalar: f32 = 1.0 / 3.0;
const large_scalar: f32 = 1.667;

// The number of go stones to be considered a medium load
const medium_cutoff: f32 = 130.0;
const cutoff_buffer: f32 = 15.0;
const medium_lower_bound = medium_cutoff - cutoff_buffer;
const medium_upper_bound = medium_cutoff + cutoff_buffer;

// Any number of go stones below this should not pump
const minimum_stone_cutoff: f32 = 10.0;

fn getDispenseTimeMs(go_stones: f32, dirt: Dirtiness) idf.sys.TickType_t {
    if (go_stones < minimum_stone_cutoff) return 0;

    const scalar: f32 = if (go_stones > medium_upper_bound)
        large_scalar
    else if (go_stones < medium_lower_bound)
        small_scalar
    else
        1.0;

    const duration_s = switch (dirt) {
        .normal => medium_normal * scalar,
        .dirty => medium_dirty * scalar,
        .nasty => medium_nasty * scalar,
    };

    return prime_time_ms + @as(idf.sys.TickType_t, @intFromFloat(duration_s * 1000.0));
}

fn main() callconv(.c) void {
    var heap = idf.heap.HeapCapsAllocator.init(.{ .@"8bit" = true });
    var arena = std.heap.ArenaAllocator.init(heap.allocator());
    defer arena.deinit();
    const allocator = arena.allocator();

    // LCD initialization
    var lcd = Lcd.init(allocator, 0x27, .SDA, .SCL) catch @panic("Failed to create LCD");
    lcd.begin() catch @panic("Failed to begin LCD");

    // Buttons
    const white_tare = DigitalPinIn.init(layout.D15, .pulldown_only) catch @panic("Failed to create tare");
    const red = DigitalPinIn.init(layout.D33, .pulldown_only) catch @panic("Failed to create red");
    const blue = DigitalPinIn.init(layout.D14, .pulldown_only) catch @panic("Failed to create blue");
    const yellow = DigitalPinIn.init(layout.D32, .pulldown_only) catch @panic("Failed to create yellow");
    const white_reset = DigitalPinIn.init(layout.D27, .pulldown_only) catch @panic("Failed to create reset");

    // PP
    const pump = PwmOut.init(.A0) catch @panic("Failed to create pump");
    var pressure_sensor = AnalogIn.init(.A1) catch @panic("Failed to create pressure sensor");

    // Main loop
    while (true) {
        // Reading & History
        const raw = pressure_sensor.read() catch continue;
        history[history_index] = raw;
        history_index = (history_index + 1) % history_size;
        if (history_index == 0) history_full = true;

        // Average Logic
        const count: usize = if (history_full) history_size else history_index;
        var sum: i32 = 0;
        for (history[0..count]) |val| {
            sum += val;
        }
        const avg: i32 = if (count > 0) @divTrunc(sum, @as(i32, @intCast(count))) else 0;
        const zeroed = avg - tare_offset;

        // Reset Logic
        if (white_reset.read()) {
            tared = false;
            tare_offset = 0;
            history_index = 0;
            history_full = false;
            @memset(&history, 0);
        }

        // UI Update
        lcd.clear() catch continue;
        pump.write(4095) catch continue;

        if (tared) {
            lcd.printAt("Tared Scale :)", 0, 0) catch continue;
            lcd.printAt("Select Dirt", 0, 1) catch continue;
        } else {
            lcd.printAt("Tare scale!", 0, 0) catch continue;
        }

        // Logging (TODO REMOVE)
        const stones = stonesFromAdc(zeroed);
        std.log.info("raw={d} avg={d} zeroed={d} stones={d:.2}", .{ raw, avg, zeroed, stones });

        // Button Handling
        var selected_dirt: ?Dirtiness = null;

        if (white_tare.read()) {
            tare_offset = avg;
            tared = true;
        } else if (tared) {
            if (red.read()) {
                selected_dirt = .normal;
            } else if (blue.read()) {
                selected_dirt = .dirty;
            } else if (yellow.read()) {
                selected_dirt = .nasty;
            }
        }

        // Dispense Logic, configured in active low
        if (selected_dirt) |dirt| {
            lcd.clear() catch continue;
            tared = false;

            const msg = switch (dirt) {
                .normal => "Normal load!",
                .dirty => "Dirty load!",
                .nasty => "Nasty load!",
            };
            lcd.printAt(msg, 0, 0) catch continue;
            lcd.printAt("Dispensing!", 0, 1) catch continue;

            const time_ms = getDispenseTimeMs(stones, dirt);
            pump.write(0) catch continue;
            idf.sleepMs(time_ms);
            pump.write(4095) catch continue;
        }

        // TODO: Maybe change delay for better average and refresh of LCD
        idf.sleepMs(25);
    }
}

pub const panic = idf.esp_panic.panic;
const log = std.log.scoped(idf.log.default_log_scope);
pub const std_options: std.Options = .{
    .log_level = switch (builtin.mode) {
        .Debug => .debug,
        else => .info,
    },
    .logFn = idf.log.espLogFn,
};

export fn __udivti3() void {
    @panic("__udivti3: what are you doing here?");
}
