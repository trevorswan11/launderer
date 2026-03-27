const idf = @import("esp_idf");

pub const FeatherV2Pin = enum(u8) {
    TX = 8,
    RX = 7,

    SDA = 22,
    SCL = 20,

    SS = 33,
    MOSI = 19,
    MISO = 21,
    SCK = 5,

    A0 = 26,
    A1 = 25,
    A2 = 34,
    A3 = 39,
    A4 = 36,
    A5 = 4,
    A6 = 14,
    A7 = 32,
    A8 = 15,
    A9 = 33,
    A10 = 27,
    A11 = 12,
    A12 = 13,
    A13 = 35,

    D37 = 37,
    D35 = 35,
    D13 = 13,
    D12 = 12,
    D27 = 27,
    D33 = 33,
    D15 = 15,
    D32 = 32,
    D14 = 14,

    BATT_MONITOR = 35,
    BUTTON = 38,
    LED_BUILTIN = 13,
    PIN_NEOPIXEL = 0,
    NEOPIXEL_I2C_POWER = 2,

    T0 = 4,
    T1 = 0,
    T2 = 2,
    T3 = 15,
    T4 = 13,
    T5 = 12,
    T6 = 14,
    T7 = 27,
    T8 = 33,
    T9 = 32,

    DAC1 = 25,
    DAC2 = 26,

    pub const AdcMapping = struct {
        unit: idf.sys.adc_unit_t,
        channel: idf.sys.adc_channel_t,
    };

    // Attempts to map a GPIO pin number to its corresponding ADC channel and unit.
    pub fn adc(self: FeatherV2Pin) ?AdcMapping {
        return switch (self) {
            .A0 => .{ idf.sys.ADC_UNIT_2, idf.sys.ADC_CHANNEL_9 },
            .A1 => .{ idf.sys.ADC_UNIT_2, idf.sys.ADC_CHANNEL_8 },
            .A2 => .{ idf.sys.ADC_UNIT_1, idf.sys.ADC_CHANNEL_6 },
            .A3 => .{ idf.sys.ADC_UNIT_1, idf.sys.ADC_CHANNEL_3 },
            .A4 => .{ idf.sys.ADC_UNIT_1, idf.sys.ADC_CHANNEL_0 },
            .A5 => .{ idf.sys.ADC_UNIT_2, idf.sys.ADC_CHANNEL_0 },
            .D37 => .{ idf.sys.ADC_UNIT_1, idf.sys.ADC_CHANNEL_1 },
            .D35 => .{ idf.sys.ADC_UNIT_1, idf.sys.ADC_CHANNEL_7 },
            .D13 => .{ idf.sys.ADC_UNIT_2, idf.sys.ADC_CHANNEL_4 },
            .D12 => .{ idf.sys.ADC_UNIT_2, idf.sys.ADC_CHANNEL_5 },
            .D27 => .{ idf.sys.ADC_UNIT_2, idf.sys.ADC_CHANNEL_7 },
            .D33 => .{ idf.sys.ADC_UNIT_1, idf.sys.ADC_CHANNEL_5 },
            .D15 => .{ idf.sys.ADC_UNIT_2, idf.sys.ADC_CHANNEL_3 },
            .D32 => .{ idf.sys.ADC_UNIT_1, idf.sys.ADC_CHANNEL_4 },
            .D14 => .{ idf.sys.ADC_UNIT_2, idf.sys.ADC_CHANNEL_6 },
            else => null,
        };
    }

    // Helper for type conversion
    pub fn gpio(self: FeatherV2Pin) idf.sys.gpio_num_t {
        return @intFromEnum(self);
    }
};
