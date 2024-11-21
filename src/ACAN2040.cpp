//

// this library header
#include <ACAN2040.h>

struct can2040 *_cbusp;

static void PIOx_IRQHandler(void) {
	can2040_pio_irq_handler(_cbusp);
}

ACAN2040::ACAN2040(uint32_t pio_num, uint32_t gpio_tx, uint32_t gpio_rx, uint32_t bitrate, uint32_t sys_clock, void (*callback)(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg)) {

	_pio_num = pio_num;
	_gpio_tx = gpio_tx;
	_gpio_rx = gpio_rx;
	_bitrate = bitrate;
	_sys_clock = sys_clock;
	_callback = callback;
	_cbusp = &_cbus;
}

/// implementation of ACAN2040 class

void ACAN2040::begin() {
	// Initialize _cbusp before setting the IRQ handler
	_cbusp = &_cbus;

	// setup canbus
	can2040_setup(&_cbus, _pio_num);
	can2040_callback_config(&_cbus, _callback);

	// Enable IRQs
	irq_set_exclusive_handler(PIO0_IRQ_0, PIOx_IRQHandler);
	irq_set_priority(PIO0_IRQ_0, 1);
	irq_set_enabled(PIO0_IRQ_0, true);

	// start canbus
	can2040_start(&_cbus, _sys_clock, _bitrate, _gpio_rx, _gpio_tx);
}

bool ACAN2040::send_message(struct can2040_msg *msg) {
	int ret = can2040_transmit(&_cbus, msg);
	return (ret == 0);
}

bool ACAN2040::ok_to_send(void) {
	return can2040_check_transmit(&_cbus);
}

void ACAN2040::stop(void) {
	can2040_stop(_cbusp);
}

void ACAN2040::get_statistics(struct can2040_stats *can_stats) {

	can2040_get_statistics(_cbusp, can_stats);
	return;
}
