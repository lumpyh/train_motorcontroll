#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stddef.h>
#include <string.h>

#include "i2c.h"

#define CMD_SPEED 0x00
static uint8_t speed = 0; // encoded lowest bit direction higher bits speed

int8_t i2c_tx(uint8_t idx, uint8_t *val)
{
	switch (idx) {
		case CMD_SPEED:
			*val = speed;
			break;
		default:
			*val = idx;
			return -1;
	}
	return 0;
}

int8_t i2c_rx(uint8_t idx, uint8_t val)
{
	switch (idx) {
		case CMD_SPEED:
			speed = val;
			break;
		default:
			return -1;
	}
	return 0;
}

static void setup_tca(void)
{
	PORTMUX_CTRLC = PORTMUX_TCA00_bm; // move WO0 to pa6/pin7 so we can use WO3

	TCA0_SPLIT_CTRLD = TCA_SPLIT_SPLITM_bm; // enable splitmode

	TCA0_SPLIT_CTRLB = TCA_SPLIT_LCMP0EN_bm | TCA_SPLIT_HCMP0EN_bm; // enable low and high cnt cmp 0

	TCA0_SPLIT_LPER = 0xFE; // set period to full 8bit range
	TCA0_SPLIT_HPER = 0xFE;

	TCA0_SPLIT_LCMP0 = 0x00; // set dutycycle to 0%
	TCA0_SPLIT_HCMP0 = 0x00; // set dutycycle to 0%
	

	TCA0_SPLIT_CTRLA = TCA_SPLIT_CLKSEL_DIV64_gc | TCA_SPLIT_ENABLE_bm;
}

static void setspeed_and_dir(uint8_t dir, uint8_t speed)
{
	TCA0_SPLIT_LCMP0 = dir * speed;
	TCA0_SPLIT_HCMP0 = (!dir) * speed;
}

int main(void)
{
	uint16_t tmp;

	PORTA.DIR = (0x1 << 3) | (0x1 << 7);

	i2c_device_init(0x61);

	setup_tca();

	CPU_SREG |= CPU_I_bm;

	while (1) {
		setspeed_and_dir((speed & 1), (speed & ~1));
		_delay_ms(100);
	}

	return 0;
}
