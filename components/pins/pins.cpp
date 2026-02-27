#include <atomic>
#include <cmath>

#include <driver/ledc.h>
#include <esp_adc/adc_oneshot.h>
#include <esp_exception.hpp>

#include "pins.hpp"

namespace dev {

auto ADCUnitHandles::one() -> ADCUnitHandle {
    static ADCUnitHandles adc_handles{};
    return handle_one;
}

auto ADCUnitHandles::two() -> ADCUnitHandle {
    static ADCUnitHandles adc_handles{};
    return handle_two;
}

ADCUnitHandles::ADCUnitHandles() {
    std::call_once(init_flag_, []() {
        const auto init_config_1 = adc_oneshot_unit_init_cfg_t{
            .unit_id  = ADC_UNIT_1,
            .clk_src  = ADC_RTC_CLK_SRC_DEFAULT,
            .ulp_mode = {},
        };
        CHECK_THROW(adc_oneshot_new_unit(&init_config_1, &handle_one));

        const auto init_config_2 = adc_oneshot_unit_init_cfg_t{
            .unit_id  = ADC_UNIT_2,
            .clk_src  = ADC_RTC_CLK_SRC_DEFAULT,
            .ulp_mode = {},
        };
        CHECK_THROW(adc_oneshot_new_unit(&init_config_2, &handle_two));
    });
}

AnalogIn::AnalogIn(Pin pin, adc_bitwidth_t bitwidth, adc_atten_t atten)
    : bitwidth_{bitwidth}, atten_{atten} {
    const auto adc = pin_layout::pin_to_adc(pin);
    if (!adc) { throw idf::GPIOException{ESP_ERR_NOT_FOUND}; }
    const auto& [unit, channel] = *adc;

    const adc_oneshot_chan_cfg_t config = {
        .atten    = atten,
        .bitwidth = bitwidth,
    };

    if (unit == ADC_UNIT_1) {
        CHECK_THROW(adc_oneshot_config_channel(ADCUnitHandles::one(), channel, &config));
    } else {
        CHECK_THROW(adc_oneshot_config_channel(ADCUnitHandles::two(), channel, &config));
    }

    unit_    = unit;
    channel_ = channel;
}

auto AnalogIn::get_range() const noexcept -> int { return (2 << (bitwidth_ - 1)) - 1; }

auto AnalogIn::read() -> int {
    int value;
    if (unit_ == ADC_UNIT_1) {
        CHECK_THROW(adc_oneshot_read(ADCUnitHandles::one(), channel_, &value));
    } else {
        CHECK_THROW(adc_oneshot_read(ADCUnitHandles::two(), channel_, &value));
    }
    return value;
}

PwmOut::PwmOut(Pin pin, ledc_timer_bit_t resolution) : pin_{pin}, resolution_{resolution} {
    ensure_init(resolution);

    channel_                     = claim_next_channel();
    ledc_channel_config_t config = {
        .gpio_num   = static_cast<int>(pin),
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = channel_,
        .intr_type  = LEDC_INTR_DISABLE,
        .timer_sel  = LEDC_TIMER_0,
        .duty       = 0,
        .hpoint     = 0,
        .sleep_mode = {},
        .flags      = {},
    };
    ledc_channel_config(&config);
}

auto PwmOut::ensure_init(ledc_timer_bit_t resolution) -> void {
    std::call_once(init_flag_, [resolution]() {
        const auto timer_cfg = ledc_timer_config_t{
            .speed_mode      = LEDC_LOW_SPEED_MODE,
            .duty_resolution = resolution,
            .timer_num       = LEDC_TIMER_0,
            .freq_hz         = 5'000,
            .clk_cfg         = LEDC_AUTO_CLK,
            .deconfigure     = {},
        };
        ledc_timer_config(&timer_cfg);
    });
}

auto PwmOut::write(uint32_t duty) -> void {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel_, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel_);
}

auto PwmOut::claim_next_channel() -> ledc_channel_t {
    static std::atomic<int> next_ch{0};
    auto                    ch = next_ch.fetch_add(1);
    if (ch >= LEDC_CHANNEL_MAX) { throw idf::GPIOException{ESP_ERR_INVALID_STATE}; }
    return static_cast<ledc_channel_t>(ch);
}

}; // namespace dev
