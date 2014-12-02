#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "uart.h"
#include "wireless.h"

// set up serial for debugging
FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

#define begin {
#define end   } 

//wireless variables
#define PIPE_ADDRESS_0 0xb4b5b600
#define PIPE_ADDRESS_1 0xb4b5b601
#define PIPE_ADDRESS_2 0xb4b5b602
#define PIPE_ADDRESS_3 0xb4b5b603
#define PIPE_ADDRESS_4 0xb4b5b604
#define PIPE_ADDRESS_5 0xb4b5b605

char NETID[] = "abc234";

int time;
char pushed;
char turning_action;
char accelerating_action;
char Ain; //reading from ADC

int main(void)
begin
	//init the A to D converter 
   	//channel zero/ left adj /EXTERNAL Aref
   	//!!!CONNECT Aref jumper!!!!
   	ADMUX = (1<<ADLAR) ;  
   	//enable ADC and set prescaler to 1/128*16MHz=125,000
   	//and clear interupt enable
   	//and start a conversion
   	ADCSRA = (1<<ADEN) | (1<<ADSC) + 7 ;

	uart_init();
	stdout = stdin = stderr = &uart_str;
	fprintf(stdout,"starting\n");

	PAYLOAD_LENGTH = 10;
	init_RF(1); //init transceiver, enable enhanced shockburst
	//set_RF_channel(8); //can be used to change the RF channel to reduce interference
	set_mode(TX_MODE);
	set_RX_addr(PIPE_ADDRESS_0, 0);
	set_TX_addr(PIPE_ADDRESS_0);

	char previous_Ain = 0;
	char command = 'N';
	time = 0;
	pushed = 0;
	accelerating_action = 'N';
	char send = 0;
	char* packet = (char *) malloc(PAYLOAD_LENGTH);
	while(1) 
	begin
		if (time >= 200) begin
			//get the sample  
 			Ain = ADCH;
 			//start another conversion
 			ADCSRA |= (1<<ADSC);
			if ((~PINC & 0x0F) != 0x00 && pushed == 0) 
			begin
				 pushed = 1;
				 switch (~PINC & 0x0F)
				 begin
					case 0x01:
						accelerating_action = 'F'; //forward
						break;
					case 0x02:
						accelerating_action = 'B'; //backward
						break;
					default:
						accelerating_action = 'N';
				 end
				 send = 1;
 			end
			if ((~PINC & 0x0F) == 0x00 && pushed == 1) 
			begin
				pushed = 0;
				accelerating_action = 'N';
				send = 1;
			end
			if (Ain != previous_Ain) 
			begin
				previous_Ain = Ain;
				send = 1;
			end
			if (send == 1) 
			begin
				sprintf(packet, "%6s%c%3d", NETID, accelerating_action, Ain);
				fprintf(stdout, "%s", packet);
				unsigned char transmission_result = transmit_packet(packet);
				fprintf(stdout, "\ntransmission result: %d\n", transmission_result);
				send = 0;
			end
			time = 0;
		end
		time++;
   end // while(1)
end  //end main
