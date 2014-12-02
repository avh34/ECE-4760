#define PIPE_ADDRESS_0 0xb4b5b600

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>

#include "uart.h"
#include "wireless.h"


// UART file descriptor
// putchar and getchar are in uart.c
FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

int main(void) {
	char transmission_result;
	uart_init();
	stdout = stdin = stderr = &uart_str;
	fprintf(stdout,"starting\n");
	PAYLOAD_LENGTH = 32; //set the payload length of each packet
	init_RF(1);
	set_mode(TX_MODE);
	set_RX_addr(PIPE_ADDRESS_0, 0);
	set_TX_addr(PIPE_ADDRESS_0);
	char* packet = (char *) malloc(PAYLOAD_LENGTH);
	int i;
	for (i = 0; i < PAYLOAD_LENGTH; i++) {
		packet[i] = i;
	}
	while (1) {
		transmission_result = transmit_packet(packet);
	}
}
