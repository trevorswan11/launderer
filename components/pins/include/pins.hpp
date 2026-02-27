#pragma once

#include <mutex>

#include <gpio_cxx.hpp>
#include <hal/adc_types.h>
#include <hal/ledc_types.h>

#include "pin_layout.hpp"

namespace dev {

class DigitalPinOut {
  public:
    explicit DigitalPinOut(Pin pin) : pin_{idf::GPIONum{pin}} {}

    auto set_high() -> void { pin_.set_high(); }
    auto set_low() -> void { pin_.set_low(); }

  private:
    idf::GPIO_Output pin_;
};

class ADCUnitHandles {
    using ADCUnitHandle = struct adc_oneshot_unit_ctx_t*;

  public:
    [[nodiscard]] static auto one() -> ADCUnitHandle;
    [[nodiscard]] static auto two() -> ADCUnitHandle;

  private:
    ADCUnitHandles();

  private:
    static inline std::once_flag init_flag_;
    static inline ADCUnitHandle  handle_one;
    static inline ADCUnitHandle  handle_two;
};

class AnalogIn {
  public:
    explicit AnalogIn(Pin            pin,
                      adc_bitwidth_t bitwidth = ADC_BITWIDTH_12,
                      adc_atten_t    atten    = ADC_ATTEN_DB_12);

    // Returns the maximum value a read from this pin can return.
    auto               get_range() const noexcept -> int;
    [[nodiscard]] auto read() -> int;

  private:
    adc_bitwidth_t bitwidth_;
    adc_atten_t    atten_;
    adc_channel_t  channel_;
    adc_unit_t     unit_;
};

class PwmOut {
  public:
    explicit PwmOut(Pin pin, ledc_timer_bit_t resolution = LEDC_TIMER_12_BIT);

    auto write(uint32_t duty) -> void;

  private:
    static auto ensure_init(ledc_timer_bit_t resolution = LEDC_TIMER_12_BIT) -> void;
    static auto claim_next_channel() -> ledc_channel_t;

  private:
    static inline std::once_flag init_flag_;

    Pin              pin_;
    ledc_channel_t   channel_;
    ledc_timer_bit_t resolution_;
};

} // namespace dev
