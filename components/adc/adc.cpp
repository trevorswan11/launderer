#include <atomic>
#include <cmath>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <utility>

#include <driver/ledc.h>
#include <esp_adc/adc_oneshot.h>
#include <esp_err.h>
#include <hal/adc_types.h>

extern "C" {
#include "adc.h"
}

class ADCUnitHandles {
    using ADCUnitHandle = struct adc_oneshot_unit_ctx_t*;

  public:
    [[nodiscard]] static auto one() -> ADCUnitHandle {
        static ADCUnitHandles adc_handles{};
        return handle_one;
    }

    [[nodiscard]] static auto two() -> ADCUnitHandle {
        static ADCUnitHandles adc_handles{};
        return handle_two;
    }

  private:
    ADCUnitHandles() {
        std::call_once(init_flag_, []() {
            const auto init_config_1 = adc_oneshot_unit_init_cfg_t{
                .unit_id  = ADC_UNIT_1,
                .clk_src  = ADC_RTC_CLK_SRC_DEFAULT,
                .ulp_mode = {},
            };
            if (adc_oneshot_new_unit(&init_config_1, &handle_one) != ESP_OK) {
                throw std::runtime_error{"Failed to create handle 1"};
            }

            const auto init_config_2 = adc_oneshot_unit_init_cfg_t{
                .unit_id  = ADC_UNIT_2,
                .clk_src  = ADC_RTC_CLK_SRC_DEFAULT,
                .ulp_mode = {},
            };
            if (adc_oneshot_new_unit(&init_config_2, &handle_two) != ESP_OK) {
                throw std::runtime_error{"Failed to create handle 2"};
            }
        });
    }

  private:
    static inline std::once_flag init_flag_;
    static inline ADCUnitHandle  handle_one;
    static inline ADCUnitHandle  handle_two;
};

constexpr auto pin_to_adc(uint32_t pin) -> std::optional<std::pair<adc_unit_t, adc_channel_t>> {
    switch (pin) {
    case 26: return std::pair{ADC_UNIT_2, ADC_CHANNEL_9};
    case 25: return std::pair{ADC_UNIT_2, ADC_CHANNEL_8};
    case 34: return std::pair{ADC_UNIT_1, ADC_CHANNEL_6};
    case 39: return std::pair{ADC_UNIT_1, ADC_CHANNEL_3};
    case 36: return std::pair{ADC_UNIT_1, ADC_CHANNEL_0};
    case 4:  return std::pair{ADC_UNIT_2, ADC_CHANNEL_0};
    case 37: return std::pair{ADC_UNIT_1, ADC_CHANNEL_1};
    case 35: return std::pair{ADC_UNIT_1, ADC_CHANNEL_7};
    case 13: return std::pair{ADC_UNIT_2, ADC_CHANNEL_4};
    case 12: return std::pair{ADC_UNIT_2, ADC_CHANNEL_5};
    case 27: return std::pair{ADC_UNIT_2, ADC_CHANNEL_7};
    case 33: return std::pair{ADC_UNIT_1, ADC_CHANNEL_5};
    case 15: return std::pair{ADC_UNIT_2, ADC_CHANNEL_3};
    case 32: return std::pair{ADC_UNIT_1, ADC_CHANNEL_4};
    case 14: return std::pair{ADC_UNIT_2, ADC_CHANNEL_6};
    default: return std::nullopt;
    }
}

#define TRY(expr)                                \
    try {                                        \
        const auto result = (expr);              \
        if (result != ESP_OK) { return result; } \
    } catch (...) { return ESP_FAIL; }

int analog_in_init(analog_in* analog, uint8_t pin) {
    const auto adc = pin_to_adc(pin);
    if (!adc) { return ESP_ERR_NOT_FOUND; }
    const auto [unit, channel] = *adc;

    const adc_oneshot_chan_cfg_t config = {
        .atten    = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };

    if (unit == ADC_UNIT_1) {
        TRY(adc_oneshot_config_channel(ADCUnitHandles::one(), channel, &config));
    } else {
        TRY(adc_oneshot_config_channel(ADCUnitHandles::two(), channel, &config));
    }

    *analog = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten    = ADC_ATTEN_DB_12,
        .channel  = channel,
        .unit     = unit,
    };
    return ESP_OK;
}

int analog_in_read(analog_in* analog, int* value) {
    if (analog->unit == ADC_UNIT_1) {
        TRY(adc_oneshot_read(ADCUnitHandles::one(), static_cast<adc_channel_t>(analog->channel), value));
    } else {
        TRY(adc_oneshot_read(ADCUnitHandles::two(), static_cast<adc_channel_t>(analog->channel), value));
    }
    return ESP_OK;
}
