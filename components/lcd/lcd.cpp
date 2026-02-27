#include <mutex>

#include <esp_exception.hpp>
#include <hd44780.h>
#include <i2cdev.h>
#include <pcf8574.h>
#include <soc/gpio_num.h>

#include "lcd.hpp"

static std::optional<i2c_dev_t> pcf_dev;
static std::once_flag           i2c_init_flag;

namespace dev {

Lcd::Lcd(uint8_t addr, Pin sda, Pin scl) : addr_{addr}, sda_{sda}, scl_{scl} {
    // https://github.com/esp-idf-lib/pcf8574/blob/main/examples/i2c/main/main.c
    const auto lcd = hd44780_t{
        .write_cb = [](const hd44780_t* lcd, uint8_t data) -> esp_err_t {
            if (!pcf_dev) { return ESP_ERR_INVALID_STATE; }
            return pcf8574_port_write(&*pcf_dev, data);
        },
        .pins =
            {
                .rs = 0,
                .e  = 2,
                .d4 = 4,
                .d5 = 5,
                .d6 = 6,
                .d7 = 7,
                .bl = 3,
            },
        .font      = HD44780_FONT_5X8,
        .lines     = 2,
        .backlight = true,
    };
    lcd_config_ = std::make_unique<hd44780_t>(lcd);
}

Lcd::~Lcd() = default;

auto Lcd::init() -> void {
    // The i2c device needs to be initialized before any work can happen
    std::call_once(i2c_init_flag, [this]() {
        i2cdev_init();
        pcf_dev.emplace();
        CHECK_THROW(pcf8574_init_desc(&*pcf_dev,
                                      addr_,
                                      I2C_NUM_0,
                                      static_cast<gpio_num_t>(sda_),
                                      static_cast<gpio_num_t>(scl_)));
    });

    CHECK_THROW(hd44780_init(lcd_config_.get()));
    CHECK_THROW(hd44780_switch_backlight(lcd_config_.get(), true));
}

auto Lcd::set_cursor(uint8_t col = 0, uint8_t row = 0) -> void {
    CHECK_THROW(hd44780_gotoxy(lcd_config_.get(), col, row));
}

auto Lcd::print(const std::string& text) -> void {
    CHECK_THROW(hd44780_puts(lcd_config_.get(), text.c_str()));
}

auto Lcd::print(const std::string& text, uint8_t col, uint8_t row) -> void {
    set_cursor(col, row);
    print(text);
}

auto Lcd::clear() -> void { CHECK_THROW(hd44780_clear(lcd_config_.get())); }

} // namespace dev
