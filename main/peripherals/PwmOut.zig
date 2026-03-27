const std = @import("std");
const idf = @import("esp_idf");

const layout = @import("pin_layout.zig");

const Self = @This();
const FeatherV2Pin = layout.FeatherV2Pin;

const check = idf.err.espCheckError;

channel: idf.sys.ledc_channel_t,

var next_channel: u8 = 0;
var timer_init = false;

pub fn init(pin: FeatherV2Pin) !Self {
    if (!timer_init) {
        const t_cfg = idf.sys.ledc_timer_config_t{
            .speed_mode = idf.sys.LEDC_LOW_SPEED_MODE,
            .duty_resolution = idf.sys.LEDC_TIMER_12_BIT,
            .timer_num = idf.sys.LEDC_TIMER_0,
            .freq_hz = 5000,
            .clk_cfg = idf.sys.LEDC_AUTO_CLK,
        };
        try check(idf.sys.ledc_timer_config(&t_cfg));
        timer_init = true;
    }

    const ch: idf.sys.ledc_channel_t = @intCast(next_channel);
    next_channel += 1;

    const c_cfg = idf.sys.ledc_channel_config_t{
        .gpio_num = pin.gpio(),
        .speed_mode = idf.sys.LEDC_LOW_SPEED_MODE,
        .channel = ch,
        .intr_type = idf.sys.LEDC_INTR_DISABLE,
        .timer_sel = idf.sys.LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
    };
    try check(idf.sys.ledc_channel_config(&c_cfg));

    return .{ .channel = ch };
}

pub fn write(self: Self, duty: u32) !void {
    try check(idf.sys.ledc_set_duty(idf.sys.LEDC_LOW_SPEED_MODE, self.channel, duty));
    try check(idf.sys.ledc_update_duty(idf.sys.LEDC_LOW_SPEED_MODE, self.channel));
}
