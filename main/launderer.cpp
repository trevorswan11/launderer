#include <chrono>
#include <cmath>
#include <cstdint>
#include <optional>
#include <print>
#include <thread>
#include <utility>

#include "gpio_cxx.hpp"
#include "lcd.hpp"
#include "pin_layout.hpp"
#include "pins.hpp"

using namespace std::chrono_literals;

enum class Dirtiness {
    NORMAL,
    DIRTY,
    NASTY,
};

using i32   = std::int32_t;
using usize = std::size_t;

constexpr auto HISTORY_SIZE = 20uz;

dev::Lcd lcd{0x27};

dev::DigitalPinIn white_tare{15, idf::GPIOPullMode::PULLDOWN()};
dev::DigitalPinIn red{33, idf::GPIOPullMode::PULLDOWN()};
dev::DigitalPinIn blue{14, idf::GPIOPullMode::PULLDOWN()};
dev::DigitalPinIn yellow{32, idf::GPIOPullMode::PULLDOWN()};
dev::DigitalPinIn white_reset{27, idf::GPIOPullMode::PULLDOWN()};

dev::PwmOut   pump{dev::pin_layout::A0};
dev::AnalogIn pressure_sensor{dev::pin_layout::A1};

// From desmos fit
constexpr float FIT_A = -478286.117341f;
constexpr float FIT_B = 830.395033677f;
constexpr float FIT_C = 580.401879334f;

[[nodiscard]] auto stones_from_adc(i32 zeroed) noexcept -> float {
    if (zeroed <= 0) { return 0.0f; }
    return FIT_A / (zeroed - FIT_C) - FIT_B;
}

// The amount of time to reach the tip of the tube
constexpr auto PRIME_TIME    = 0.15s;
constexpr auto MEDIUM_NORMAL = 0.5;
constexpr auto MEDIUM_DIRTY  = 1.0;
constexpr auto MEDIUM_NASTY  = 1.5;

constexpr auto SMALL_SCALAR = 1 / 3.0f;
constexpr auto LARGE_SCALAR = 1.667f;

// The number of go stones to be considered a medium load
constexpr auto MEDIUM_CUTOFF      = 130.0f;
constexpr auto CUTOFF_BUFFER      = 15.0f;
constexpr auto MEDIUM_LOWER_BOUND = MEDIUM_CUTOFF - CUTOFF_BUFFER;
constexpr auto MEDIUM_UPPER_BOUND = MEDIUM_CUTOFF + CUTOFF_BUFFER;

// Any number of go stones below this should not pump
constexpr auto MINIMUM_CUTOFF = 10.0f;

// 272 stones is a medium load, any more is large, any less is small
[[nodiscard]] auto dispense_time(float go_stones, Dirtiness dirt) noexcept
    -> std::chrono::duration<long double> {
    if (go_stones < MINIMUM_CUTOFF) { return 0s; }
    const auto scalar = go_stones > MEDIUM_UPPER_BOUND
                            ? LARGE_SCALAR
                            : (go_stones < MEDIUM_LOWER_BOUND ? SMALL_SCALAR : 1.0f);
    const auto normal = MEDIUM_NORMAL * scalar;
    const auto dirty  = MEDIUM_DIRTY * scalar;
    const auto nasty  = MEDIUM_NASTY * scalar;

    switch (dirt) {
    case Dirtiness::NORMAL: return PRIME_TIME + std::chrono::duration<double>(normal);
    case Dirtiness::DIRTY:  return PRIME_TIME + std::chrono::duration<double>(dirty);
    case Dirtiness::NASTY:  return PRIME_TIME + std::chrono::duration<double>(nasty);
    }
    std::unreachable();
}

extern "C" auto app_main() -> void {
    lcd.init();

    std::array<i32, HISTORY_SIZE> history{};
    usize                         history_index = 0;
    bool                          history_full  = false;

    i32  tare_offset = 0;
    bool tared       = false;

    const auto current_average = [&]() -> i32 {
        usize count = history_full ? HISTORY_SIZE : history_index;
        if (count == 0) { return 0; }
        i32 sum = 0;
        for (usize i = 0; i < count; ++i) { sum += history[i]; }
        return sum / static_cast<i32>(count);
    };

    while (true) {
        try {
            // Every iteration should track a new reading
            auto raw               = pressure_sensor.read();
            history[history_index] = raw;
            history_index          = (history_index + 1) % HISTORY_SIZE;
            if (history_index == 0) { history_full = true; }

            i32 zeroed = current_average() - tare_offset;

            lcd.clear();
            pump.write(4'095);

            if (white_reset) {
                tared         = false;
                tare_offset   = 0;
                history_index = 0;
                history_full  = false;
                history.fill(0);
            }

            if (tared) {
                lcd.print("Tared Scale :)");
                lcd.set_cursor(0, 1);
                lcd.print("Select Dirt");
            } else {
                lcd.print("Tare scale!");
            }
            std::println("raw={} ADC, avg={} ADC, zeroed={} ADC", raw, current_average(), zeroed);
            const auto stones = stones_from_adc(zeroed);
            std::println("stones={} weight={}g", stones, stones * 2.5f);

            std::optional<Dirtiness> dirt;
            if (white_tare) {
                tare_offset = current_average();
                tared       = true;
            } else if (tared) {
                if (red) {
                    dirt.emplace(Dirtiness::NORMAL);
                } else if (blue) {
                    dirt.emplace(Dirtiness::DIRTY);
                } else if (yellow) {
                    dirt.emplace(Dirtiness::NASTY);
                }

                if (dirt) {
                    lcd.clear();
                    tared = false;

                    switch (*dirt) {
                    case Dirtiness::NORMAL: lcd.print("Normal load!"); break;
                    case Dirtiness::DIRTY:  lcd.print("Dirty load!"); break;
                    case Dirtiness::NASTY:  lcd.print("Nasty load!"); break;
                    }

                    lcd.set_cursor(0, 1);
                    lcd.print("Dispensing!");
                    const auto stones = stones_from_adc(zeroed);
                    const auto time   = dispense_time(stones, *dirt);
                    std::println("stones={} weight={}g time={}", stones, stones * 2.5f, time);

                    pump.write(0);
                    std::this_thread::sleep_for(time);
                    pump.write(4'095);
                }
            }

            std::this_thread::sleep_for(25ms);
        } catch (...) { continue; }
    }
}
