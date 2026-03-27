#include <cstdlib>
#include <mutex>
#include <optional>

#include <esp_err.h>
#include <hd44780.h>
#include <i2cdev.h>
#include <pcf8574.h>
#include <soc/gpio_num.h>

extern "C" {
#include "lcd.h"
}

static std::optional<i2c_dev_t> pcf_dev;
static std::once_flag           i2c_init_flag;

// https://github.com/esp-idf-lib/pcf8574/blob/main/examples/i2c/main/main.c
esp_err_t lcd_create(lcd_t* lcd, uint8_t addr, uint8_t sda, uint8_t scl) {
    const auto lcd_hd = hd44780_t{
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

    auto* config = new hd44780_t;
    if (!config) { return ESP_FAIL; }
    *config = lcd_hd;

    *lcd = lcd_t{
        .lcd_config = config,
        .addr       = addr,
        .sda        = sda,
        .scl        = scl,
    };
    return ESP_OK;
}

void lcd_destroy(lcd_t* lcd) { delete lcd->lcd_config; }

esp_err_t lcd_begin(lcd_t* lcd) {
    // The i2c device needs to be initialized before any work can happen
    std::call_once(i2c_init_flag, [lcd]() {
        i2cdev_init();
        pcf_dev.emplace();
        ESP_ERROR_CHECK(pcf8574_init_desc(&*pcf_dev,
                                          lcd->addr,
                                          I2C_NUM_0,
                                          static_cast<gpio_num_t>(lcd->sda),
                                          static_cast<gpio_num_t>(lcd->scl)));
    });

    ESP_ERROR_CHECK(hd44780_init(lcd->lcd_config));
    ESP_ERROR_CHECK(hd44780_switch_backlight(lcd->lcd_config, true));
    return ESP_OK;
}

esp_err_t lcd_set_cursor(lcd_t* lcd, uint8_t col, uint8_t row) {
    return hd44780_gotoxy(lcd->lcd_config, col, row);
}

esp_err_t lcd_print(lcd_t* lcd, const char* text) { return hd44780_puts(lcd->lcd_config, text); }

esp_err_t lcd_print_at(lcd_t* lcd, const char* text, uint8_t col, uint8_t row) {
    const esp_err_t result = lcd_set_cursor(lcd, col, row);
    if (result != ESP_OK) { return result; }
    return lcd_print(lcd, text);
}

esp_err_t lcd_clear(lcd_t* lcd) { return hd44780_clear(lcd->lcd_config); }
