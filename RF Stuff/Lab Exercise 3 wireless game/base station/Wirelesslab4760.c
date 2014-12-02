#define PIPE_ADDRESS_0 0xb4b5b600
#define PIPE_ADDRESS_1 0xb4b5b601
#define PIPE_ADDRESS_2 0xb4b5b602
#define PIPE_ADDRESS_3 0xb4b5b603
#define PIPE_ADDRESS_4 0xb4b5b604
#define PIPE_ADDRESS_5 0xb4b5b605


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
	PAYLOAD_LENGTH = 10; //set the payload length of each packet
	init_RF(1); //init transceiver, enable enhanced shockburst
	//set_RF_channel(8); can be used to change the RF channel to reduce interference
	set_RX_addr(PIPE_ADDRESS_0, 0);
	set_RX_addr(PIPE_ADDRESS_1, 1);
	set_RX_addr(PIPE_ADDRESS_2, 2);
	set_RX_addr(PIPE_ADDRESS_3, 3);
	set_RX_addr(PIPE_ADDRESS_4, 4);
	set_RX_addr(PIPE_ADDRESS_5, 5);
	enable_pipe(0);
	enable_pipe(1);
	enable_pipe(2);
	enable_pipe(3);
	enable_pipe(4);
	enable_pipe(5);
	unsigned char pipe_received;
	uart_init();
	stdout = stdin = stderr = &uart_str;
	char* packet = (char *) malloc(PAYLOAD_LENGTH);
	while (1) {
		if (RX_FIFO_status() == 1) {
			pipe_received = read_RX_payload(packet);
			fprintf(stdout,"<%s>", packet);
		}
	}
}
