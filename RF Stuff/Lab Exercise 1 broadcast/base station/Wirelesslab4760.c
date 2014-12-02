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

char data[] = "The purpose of this course is to enable students to carry out sophisticated designs of the modern digital systems which now appear in products such as automobiles, appliances and industrial tools.\n";

int main(void) {
	unsigned char i;
	unsigned char j;
	char transmission_result;
	uart_init();
	stdout = stdin = stderr = &uart_str;
	fprintf(stdout,"starting\n");
	PAYLOAD_LENGTH = 4; //set the payload length of each packet
	init_RF(0); //init transceiver, disable enhanced shockburst
	set_mode(TX_MODE);
	set_RX_addr(PIPE_ADDRESS_0, 0);
	set_TX_addr(PIPE_ADDRESS_0);
	char* packet = (char *) malloc(PAYLOAD_LENGTH);
	while (1) {
		for (i = 0; i < strlen(data); i += PAYLOAD_LENGTH) {
			strncpy(packet, data + i, PAYLOAD_LENGTH);
			for (j = 0; j < PAYLOAD_LENGTH; j++)
				fprintf(stdout,"%c", packet[j]);
			transmission_result = transmit_packet(packet);
			if (transmission_result == 2) fprintf(stdout,"transmission failed\n");
		}
	}
}
