#include "WireHack.h"
#include "driver/i2c.h"
#include <cstdio>

namespace
{
	static constexpr int _port = 0;
	static uint8_t _tx_address = 0;
	static uint8_t _tx_idx = 0;
	static uint8_t _tx_buffer[4] = {0};
	static uint8_t _rx_idx = 0;
	static uint8_t _rx_length = 0;
	static uint8_t _rx_buffer[60] = {0};
	static constexpr TickType_t _timeout = pdMS_TO_TICKS(100);
}

WireHack::WireHack()
{
};

void WireHack::begin()
{
	// there's no way without arduino to know what pins to use...
	// so the user should really call begin() manually before calling Trill::begin()...
}

void WireHack::begin(int sda_pin, int scl_pin, uint32_t frequency)
{
	if (frequency > 4000000) frequency = 4000000;
	i2c_config_t config = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = sda_pin,
		.scl_io_num = scl_pin,
		.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.scl_pullup_en = GPIO_PULLUP_ENABLE,
		.master = { .clk_speed = frequency, },
		.clk_flags = 0,
	};

	esp_err_t err = i2c_param_config(_port, &config);
	if (err != ESP_OK)
	{
		printf("Wire error: i2c_param_config invalid argument\n");
		return;
	}

	err = i2c_driver_install(_port, config.mode, 0, 0, 0);
	if (err == ESP_OK) return;
	switch (err)
	{
		case ESP_ERR_INVALID_ARG:
			printf("Wire error: i2c_driver_install invalid argument\n");
			return;
		case ESP_FAIL:
			printf("Wire error: i2c_driver_install failure\n");
			return;
	}
}

void WireHack::beginTransmission(uint8_t address)
{
	_tx_address = address;
}

void WireHack::write(uint8_t b)
{
	_tx_buffer[_tx_idx++] = b;
}

void WireHack::endTransmission()
{
	esp_err_t err = i2c_master_write_to_device(_port, _tx_address, _tx_buffer, _tx_idx, _timeout);
	_tx_idx = 0;
	if (err == ESP_OK) return;
	switch (err)
	{
		case ESP_ERR_INVALID_ARG:
			printf("WireHack::endTransmission: invalid argument\n");
			return;
		case ESP_FAIL:
			printf("WireHack::endTransmission: failure; no subnode NACK\n");
			return;
		case ESP_ERR_INVALID_STATE:
			printf("WireHack::endTransmission: invalid state; was WireHack::begin() called successfully?\n");
			return;
		case ESP_ERR_TIMEOUT:
			printf("WireHack::endTransmission: i2c bus timeout\n");
			return;
	}
}

uint8_t WireHack::requestFrom(uint8_t address, uint8_t length)
{
	esp_err_t err = i2c_master_read_from_device(_port, address, _rx_buffer, length, _timeout);
	_rx_idx = 0;
	switch(err)
	{
		case ESP_OK:
			_rx_length = length;
			return length;
		case ESP_ERR_INVALID_ARG:
			printf("WireHack::requestFrom: invalid argument\n");
			break;
		case ESP_FAIL:
			printf("WireHack::requestFrom: failure; no subnode NACK\n");
			break;
		case ESP_ERR_INVALID_STATE:
			printf("WireHack::requestFrom: invalid state; was WireHack::begin() called successfully?\n");
			break;
		case ESP_ERR_TIMEOUT:
			printf("WireHack::requestFrom: i2c bus timeout\n");
			break;
	}
	return 0;
}

uint8_t WireHack::available()
{
	return _rx_length - _rx_idx;
}

uint8_t WireHack::read()
{
	if (_rx_idx < _rx_length)
		return _rx_buffer[_rx_idx++];
	else return 0;
}

void delay(uint8_t millis)
{
	vTaskDelay(pdMS_TO_TICKS(millis));
}

WireHack Wire{};
