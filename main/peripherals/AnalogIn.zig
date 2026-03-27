const std = @import("std");
const idf = @import("esp_idf");

const layout = @import("pin_layout.zig");

const c = @cImport({
    @cInclude("hal/adc_types.h");
    @cInclude("esp_adc/adc_oneshot.h");
});

const Self = @This();
const FeatherV2Pin = layout.FeatherV2Pin;

const check = idf.err.espCheckError;

unit: c.adc_oneshot_unit_handle_t,
channel: idf.sys.adc_channel_t,
bitwidth: idf.sys.adc_bitwidth_t,

// Singleton handles for ADC units
var handle_one: ?c.adc_oneshot_unit_handle_t = null;
var handle_two: ?c.adc_oneshot_unit_handle_t = null;

fn getUnitHandle(unit: idf.sys.adc_unit_t) !c.adc_oneshot_unit_handle_t {
    const target_handle = if (unit == idf.sys.ADC_UNIT_1) &handle_one else &handle_two;

    if (target_handle.* == null) {
        const config = c.adc_oneshot_unit_init_cfg_t{
            .unit_id = unit,
            .clk_src = idf.ADC_RTC_CLK_SRC_DEFAULT,
            .ulp_mode = idf.ADC_ONESHOT_ULP_MODE_DISABLE,
        };
        try check(idf.adc_oneshot_new_unit(&config, target_handle));
    }
    return target_handle.*.?;
}

pub fn init(pin: FeatherV2Pin) !Self {
    const mapping = pin.adc() orelse return error.IllegalValue;
    const unit_handle = try getUnitHandle(mapping.unit);

    const atten = idf.sys.ADC_ATTEN_DB_12;
    const bitwidth = idf.sys.ADC_BITWIDTH_12;

    const config = c.adc_oneshot_chan_cfg_t{
        .atten = atten,
        .bitwidth = bitwidth,
    };
    try check(idf.adc_oneshot_config_channel(unit_handle, mapping.channel, &config));

    return .{
        .unit = unit_handle,
        .channel = mapping.channel,
        .bitwidth = bitwidth,
    };
}

pub fn read(self: Self) !i32 {
    var val: i32 = 0;
    try check(idf.adc_oneshot_read(self.unit, self.channel, &val));
    return val;
}
