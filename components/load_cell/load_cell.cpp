#include <driver/gpio.h>
#include <esp_err.h>
#include <hx711.h>

extern "C" {
#include "load_cell.h"
}

int load_cell_create(load_cell_t* cell, uint8_t dout_pin, uint8_t sck_pin) {
    auto hx711_dev = hx711_t{
        .dout   = static_cast<gpio_num_t>(dout_pin),
        .pd_sck = static_cast<gpio_num_t>(sck_pin),
        .gain   = HX711_GAIN_A_64,
    };

    ESP_ERROR_CHECK(hx711_init(&hx711_dev));
    auto* dev = new hx711_t;
    if (!dev) { return ESP_FAIL; }
    *dev = hx711_dev;

    *cell = load_cell_t{.dev = reinterpret_cast<void*>(dev)};
    return ESP_OK;
}

void load_cell_destroy(load_cell_t* cell) {
    ESP_ERROR_CHECK(load_cell_set_power(cell, load_cell_power_t::LOAD_CELL_OFF));
    if (!cell->dev) { return; }
    auto* dev = reinterpret_cast<hx711_t*>(cell->dev);
    delete dev;
    cell->dev = nullptr;
}

int load_cell_set_power(load_cell_t* cell, load_cell_power_t power) {
    auto* dev = reinterpret_cast<hx711_t*>(cell->dev);
    return hx711_power_down(dev, power == load_cell_power_t::LOAD_CELL_OFF);
}

int load_cell_set_gain(load_cell_t* cell, load_cell_gain_t gain) {
    auto* dev = reinterpret_cast<hx711_t*>(cell->dev);
    return hx711_set_gain(dev, static_cast<hx711_gain_t>(gain));
}

int load_cell_is_ready(load_cell_t* cell, bool* ready) {
    auto* dev = reinterpret_cast<hx711_t*>(cell->dev);
    return hx711_is_ready(dev, ready);
}

int load_cell_wait(load_cell_t* cell, size_t timeout_ms) {
    auto* dev = reinterpret_cast<hx711_t*>(cell->dev);
    return hx711_wait(dev, timeout_ms);
}

int load_cell_read_data(load_cell_t* cell, int32_t* data) {
    auto* dev = reinterpret_cast<hx711_t*>(cell->dev);
    return hx711_read_data(dev, data);
}

int load_cell_read_average(load_cell_t* cell, size_t times, int32_t* data) {
    auto* dev = reinterpret_cast<hx711_t*>(cell->dev);
    return hx711_read_average(dev, times, data);
}
