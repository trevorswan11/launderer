const idf = @import("esp_idf");

pub const FeatherV2Pin = enum(u8) {
    SDA = 22,
    SCL = 20,

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

    _,

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
            D37 => .{ idf.sys.ADC_UNIT_1, idf.sys.ADC_CHANNEL_1 },
            D35 => .{ idf.sys.ADC_UNIT_1, idf.sys.ADC_CHANNEL_7 },
            D13 => .{ idf.sys.ADC_UNIT_2, idf.sys.ADC_CHANNEL_4 },
            D12 => .{ idf.sys.ADC_UNIT_2, idf.sys.ADC_CHANNEL_5 },
            D27 => .{ idf.sys.ADC_UNIT_2, idf.sys.ADC_CHANNEL_7 },
            D33 => .{ idf.sys.ADC_UNIT_1, idf.sys.ADC_CHANNEL_5 },
            D15 => .{ idf.sys.ADC_UNIT_2, idf.sys.ADC_CHANNEL_3 },
            D32 => .{ idf.sys.ADC_UNIT_1, idf.sys.ADC_CHANNEL_4 },
            D14 => .{ idf.sys.ADC_UNIT_2, idf.sys.ADC_CHANNEL_6 },
            else => null,
        };
    }

    // Helper for type conversion
    pub fn gpio(self: FeatherV2Pin) idf.sys.gpio_num_t {
        return @intFromEnum(self);
    }
};

pub const TX: FeatherV2Pin = @enumFromInt(8);
pub const RX: FeatherV2Pin = @enumFromInt(7);

pub const D37: FeatherV2Pin = @enumFromInt(37);
pub const D35: FeatherV2Pin = @enumFromInt(35);
pub const D13: FeatherV2Pin = @enumFromInt(13);
pub const D12: FeatherV2Pin = @enumFromInt(12);
pub const D27: FeatherV2Pin = @enumFromInt(27);
pub const D33: FeatherV2Pin = @enumFromInt(33);
pub const D15: FeatherV2Pin = @enumFromInt(15);
pub const D32: FeatherV2Pin = @enumFromInt(32);
pub const D14: FeatherV2Pin = @enumFromInt(14);

pub const SS: FeatherV2Pin = @enumFromInt(33);
pub const MOSI: FeatherV2Pin = @enumFromInt(19);
pub const MISO: FeatherV2Pin = @enumFromInt(21);
pub const SCK: FeatherV2Pin = @enumFromInt(5);

pub const BATT_MONITOR: FeatherV2Pin = @enumFromInt(35);
pub const BUTTON: FeatherV2Pin = @enumFromInt(38);
pub const LED_BUILTIN: FeatherV2Pin = @enumFromInt(13);
pub const PIN_NEOPIXEL: FeatherV2Pin = @enumFromInt(0);
pub const NEOPIXEL_I2C_POWER: FeatherV2Pin = @enumFromInt(2);

pub const T0: FeatherV2Pin = @enumFromInt(4);
pub const T1: FeatherV2Pin = @enumFromInt(0);
pub const T2: FeatherV2Pin = @enumFromInt(2);
pub const T3: FeatherV2Pin = @enumFromInt(15);
pub const T4: FeatherV2Pin = @enumFromInt(13);
pub const T5: FeatherV2Pin = @enumFromInt(12);
pub const T6: FeatherV2Pin = @enumFromInt(14);
pub const T7: FeatherV2Pin = @enumFromInt(27);
pub const T8: FeatherV2Pin = @enumFromInt(33);
pub const T9: FeatherV2Pin = @enumFromInt(32);

pub const DAC1: FeatherV2Pin = @enumFromInt(25);
pub const DAC2: FeatherV2Pin = @enumFromInt(26);
