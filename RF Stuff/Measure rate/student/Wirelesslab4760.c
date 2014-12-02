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
volatile unsigned long time1; 

ISR (TIMER0_COMPA_vect) 
{
  if (time1 < 10000)	time1++;
}

int main(void) {
	initialize();
	PAYLOAD_LENGTH = 32; //set the payload length of each packet
	unsigned char pipe_received;
	unsigned char j;
	init_RF(1);
	set_RX_addr(PIPE_ADDRESS_0, 0);
	char* packet = (char *) malloc(PAYLOAD_LENGTH);
	unsigned int packets_received = 0;
	while (1) {
		if (RX_FIFO_status() == 1) {
			pipe_received = read_RX_payload(packet);
			packets_received++;
		}
		if (time1 == 10000) {
			packets_received = packets_received / 10; //per second
			fprintf(stdout,"packets per second = %d \n", packets_received);
			packets_received = 0;
			time1 = 0;
		}
	}
}

//********************************************************** 
//Set up timer
void initialize(void)
{        
  //set up timer 0 for 1 mSec timebase 
  TIMSK0= (1<<OCIE0A);	//turn on timer 0 cmp match ISR 
  OCR0A = 249;  		//set the compare re to 250 time ticks
  //set prescalar to divide by 64 
  TCCR0B= 3; //0b00001011;	
  // turn on clear-on-match
  TCCR0A= (1<<WGM01) ;
  
  //init the timer
  time1 = 0;
      
  //init the UART -- uart_init() is in uart.c
  uart_init();
  stdout = stdin = stderr = &uart_str;
  fprintf(stdout,"Starting...\n\r");
      
  //crank up the ISRs
  sei();
}
