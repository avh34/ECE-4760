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
	PAYLOAD_LENGTH = 4; //set the payload length of each packet
	unsigned char pipe_received;
	unsigned char j;
	uart_init();
	stdout = stdin = stderr = &uart_str;
	fprintf(stdout,"starting\n");
	init_RF(0); //disable enhanced shockburst
	set_RX_addr(PIPE_ADDRESS_0, 0);
	char* packet = (char *) malloc(PAYLOAD_LENGTH);
	while (1) {
		if (RX_FIFO_status() == 1) {
			pipe_received = read_RX_payload(packet);
			for (j = 0; j < PAYLOAD_LENGTH; j++)
				fprintf(stdout,"%c", packet[j]);
		}
	}
}
