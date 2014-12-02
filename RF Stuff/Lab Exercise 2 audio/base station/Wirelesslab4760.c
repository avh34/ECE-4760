#define PIPE_ADDRESS_0 0xc4c5c600
#define PIPE_ADDRESS_1 0xc4c5c601
#define PIPE_ADDRESS_2 0xc4c5c602
#define PIPE_ADDRESS_3 0xc4c5c603
#define PIPE_ADDRESS_4 0xc4c5c604
#define PIPE_ADDRESS_5 0xc4c5c605

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
	PAYLOAD_LENGTH = 1; //set the payload length of each packet
	init_RF(0); //disable enhanced shockburst
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
	DDRB = DDRB | (1<<PORTB3); // this has to be done after calling init_RF, because init_RF also writes to DDRB
	unsigned char pipe_received;
	unsigned char j;
	uart_init();
	stdout = stdin = stderr = &uart_str;
	// timer 0 runs at full rate
   	TCCR0B = 1 ;  
   	//turn off timer 0 overflow ISR
   	TIMSK0 = 0 ;
	// turn on PWM
   	// turn on fast PWM and OC0A output
   	// at full clock rate, toggle OC0A (pin B3) 
   	// 16 microsec per PWM cycle sample time
   	TCCR0A = (1<<COM0A0) | (1<<COM0A1) | (1<<WGM00) | (1<<WGM01) ; 
   	OCR0A = 128 ; // set PWM to half full scale
	fprintf(stdout,"starting\n");
	char* packet = (char *) malloc(PAYLOAD_LENGTH);
	while (1) {
		if (RX_FIFO_status() == 1) {
			pipe_received = read_RX_payload(packet);
			OCR0A = packet[0];
		}
	}
}
