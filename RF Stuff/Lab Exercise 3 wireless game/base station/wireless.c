#include "cmd_list.h"
#include "register_map.h"
#include "wireless.h"
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>

/** initialize the nRF24L01+ transceiver
	enhanced_shockburst_enable is set to 0 to disable enhanced shockburst and 1 to enable
	the transceiver starts out in RX mode*/
void init_RF(unsigned char enhanced_shockburst_enable) {
	if (PAYLOAD_LENGTH > 32 || PAYLOAD_LENGTH < 1) {
		fprintf(stdout,"PAYLOAD_LENGTH must be in range 1 to 32\n");
		return;
	}
	//SPI settings for ATmega644
	DDR_SPI = (1<<DD_MOSI) | (1<<DD_SCK) | (1<<DD_CSN) | (0<<DD_MISO) | (1<<DD_CE);
	SPCR = (1<<SPE) | (0<<DORD) | (1<<MSTR) | (0<<CPOL) | (0<<CPHA) | (0<<SPR1) | (0<<SPR0);
	set_CE(0);
	_delay_ms(100); //delay is needed for module to power on
	write_RF_register(SETUP_AW, 0x02); //set address width to 4 bytes
	write_RF_register(RF_CH, 0x00); //set RF channel
	write_RF_register(RF_SETUP, 1<<RF_DR_HIGH | 0x3<<RF_PWR); //data rate to 2 Mbps and output power to 0dBm
	write_RF_register(RX_PW_P0, PAYLOAD_LENGTH); //set payload length
	write_RF_register(RX_PW_P1, PAYLOAD_LENGTH); //set payload length
	write_RF_register(FEATURE, 0<<EN_DPL | 0<<EN_ACK_PAY);
	write_RF_register(DYNPD, 0x0);
	if (enhanced_shockburst_enable == 1) 
		write_RF_register(EN_AA, 0x3F); //enable Enhanced ShockBurst on all pipes
	else 
		write_RF_register(EN_AA, 0x0); //disable Enhanced ShockBurst
	write_RF_register(CONFIG, (1<<EN_CRC)|(1<<PWR_UP)|(1<<PRIM_RX));
	//clear IRQs and FIFOs
	clear_IRQs();
	flush_TX_FIFO();
	flush_RX_FIFO();
	_delay_ms(5); //delay for module to power up
	set_CE(1); //CE is kept at 1 in RX mode
}

/** Enables the pipe specified by pipenumber, which can be from 0 to 5*/
void enable_pipe(unsigned char pipenumber) {
	if (pipenumber < 0 || pipenumber > 5) {
		fprintf(stdout,"Pipe number must be in range 0 to 5\n");
		return;
	}
	if (PAYLOAD_LENGTH > 32 || PAYLOAD_LENGTH < 1) {
		fprintf(stdout,"PAYLOAD_LENGTH must be in range 1 to 32\n");
		return;
	}
	unsigned char enrxaddr = read_RF_register(EN_RXADDR);
	switch (pipenumber) {
		case 0:
			enrxaddr |= 1<<ERX_P0;
			write_RF_register(RX_PW_P0, PAYLOAD_LENGTH); //set payload length
			break;
		case 1:
			enrxaddr |= 1<<ERX_P1;
			write_RF_register(RX_PW_P1, PAYLOAD_LENGTH);
			break;
		case 2:
			enrxaddr |= 1<<ERX_P2;
			write_RF_register(RX_PW_P2, PAYLOAD_LENGTH);
			break;
		case 3:
			enrxaddr |= 1<<ERX_P3;
			write_RF_register(RX_PW_P3, PAYLOAD_LENGTH);
			break;
		case 4:
			enrxaddr |= 1<<ERX_P4;
			write_RF_register(RX_PW_P4, PAYLOAD_LENGTH);
			break;
		case 5:
			enrxaddr |= 1<<ERX_P5;
			write_RF_register(RX_PW_P5, PAYLOAD_LENGTH);
			break;
	}
	write_RF_register(EN_RXADDR, enrxaddr);
}

/**	Sets the transciever to TX mode if mode == TX_MODE
	else set it to RX mode*/
void set_mode(unsigned char mode) {
	unsigned char config = read_RF_register(CONFIG);
	if (mode == TX_MODE) { //changing to TX mode
		set_CE(0); //CE is 0 in TX mode except when transmitting
		config &= ~(1<<PRIM_RX);
		write_RF_register(CONFIG, config);
		_delay_us(130);
	} else { //changing to RX mode
		config |= 1<<PRIM_RX;
		write_RF_register(CONFIG, config);
		set_CE(1); //CE is kept at 1 in RX mode
		_delay_us(130);
	}
}

/** transmit up to PAYLOAD_LENGTH bytes of the packet.
	returns 1 if the transmission is successful.
	returns 2 if maximum number of retries reached without receiving ACK,
	this only occurs if enhanced shockburst is enabled.
	With enhanced shockburst disabled, the function always returns 1 unless the transciever is connected wrongly*/
unsigned char transmit_packet(char* packet) {
	unsigned char i;
	char TX_status;
	set_CSN(0); //start sending packet to TX FIFO
	status = SPI_transmit(W_TX_PAYLOAD);
	for(i = 0; i < PAYLOAD_LENGTH; i++)
		SPI_transmit(packet[i]);
	set_CSN(1);
	set_CE(1); //set CE to 1 to begin transmission
	_delay_us(10); //CE must be high for at least 10us to switch to the TX settling state
	_delay_us(130); //130us in the TX settling state
	while (1) { //loop until transmission successful or MAX_RT high
		TX_status = check_TX_status();
		if (TX_status == 1) return 1; //transmission successful
		if (TX_status == 2) return 2; //MAX_RT high, meaning maximum number of retries reached without receiving ACK
	}
	set_CE(0);
}

/** return 0 if RX_FIFO is empty. 1 otherwise */
unsigned char RX_FIFO_status(void) {
	read_status();
	if ((status & 0xE)>>1 == 7) //RX FIFO is empty
		return 0;
	else return 1;
}

/** Reads up to PAYLOAD_LENGTH bytes of a received packet from the RX FIFO into buffer 
	returns the pipe number that the packet was received from*/
unsigned char read_RX_payload(char* buffer) {
	unsigned char i;
	set_CE(0);
	set_CSN(0);
	SPI_transmit(R_RX_PAYLOAD);
	status = SPDR;
	for(i = 0; i < PAYLOAD_LENGTH; i++){
		SPI_transmit(0x00);	//Send null data
		buffer[i] = SPDR;
	}
	set_CSN(1);//end transmission
	set_CE(1);
	//returns RX_P_NO in the STATUS register, which indicates the pipe that the payload was received from,
	//RX_P_NO = 6 indicates not used and RX_P_NO = 7 indicates RX FIFO empty
	return (status & 0xE) >> 1;
}

/** Sets the TX address of the transceiver */
void set_TX_addr(unsigned long address) {
	write_RF_register_long(TX_ADDR, address);
}

/** Sets the RX address for the specified pipe 
	for transmitter, RX address for pipe 0 should be the same as the TX address*/
void set_RX_addr(unsigned long address, unsigned char pipe) {
	if (pipe == 0 || pipe == 1) {
		write_RF_register_long(RX_ADDR_P0 + pipe, address);
	} else {
		unsigned char temp8 = address;
		write_RF_register(RX_ADDR_P0 + pipe, temp8);
	}
}

/** Sets the RF channel to channel, channel is a 7 bit number */
void set_RF_channel(unsigned char channel) {
	write_RF_register(RF_CH, channel);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Helper functions, users do not need to access these ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**	CSN pin is used to initiate and end the communication between the SPI module of mega644 and the RF chip*/
void set_CSN(unsigned char value){
	if (value == 1) PORTB |= (1<<CSN);//Start the SPI communication
	else if (value == 0) PORTB &= ~(1<<CSN);//end transmission
}

/**	CE enables and disables the transceiver from transmitting and receiving*/
void set_CE(unsigned char value){
	if (value == 1) PORTB |= (1<<CE);
	else if (value == 0) PORTB &= ~(1<<CE);
}

/** helper function for transmitting data by SPI*/
unsigned char SPI_transmit(unsigned char data){
	SPDR = data;
	//wait for the data to be transmitted
	while(!(SPSR&(1<<SPIF))); //Bit 7 of SPSR is set high when serial transfer is complete
	//flag is cleared by reading SPDR
	return SPDR;
}

/** Reads the contents of the register specified by addr */
unsigned char read_RF_register(unsigned char addr){
	unsigned char temp8;
	if(addr > 0x17 && addr < 0x1C || addr > 0x1D) return 0xFFFF;
	set_CSN(0);
	temp8 = R_REGISTER|addr;
	SPI_transmit(temp8);
	status = SPDR;
	SPI_transmit(0x00);
	set_CSN(1);
	temp8 = SPDR;
	return temp8;
}

/** Writes unsigned char value to the register specified by addr */
void write_RF_register(unsigned char addr, unsigned char value){
	unsigned char temp8;
	set_CSN(0);
	temp8 = W_REGISTER | addr;
	SPI_transmit(temp8);
	SPI_transmit(value);
	set_CSN(1);
}

/** Writes unsigned long longvalue to the register specified by addr */
void write_RF_register_long(unsigned char addr, unsigned char longvalue){
	unsigned char temp8;
	set_CSN(0);
	temp8 = W_REGISTER | addr;
	SPI_transmit(temp8);
	status = SPDR;
	temp8 = longvalue;
	SPI_transmit(temp8);
	temp8 = longvalue >> 8;
	SPI_transmit(temp8);
	temp8 = longvalue >> 16;
	SPI_transmit(temp8);
	temp8 = longvalue >> 24;
	SPI_transmit(temp8);
	set_CSN(1);
}

/** clear the RX_DR, TX_DS and MAX_RT IRQs by writing to 1 to bits corresponding to them in the STATUS register*/
void clear_IRQs(void){
	write_RF_register(STATUS, (1<<RX_DR) | (1<<TX_DS) | (1<<MAX_RT));
}

/**	return 1 if TX_DS is high meaning that ACK was received and packet was transmitted successfully
	return 2 if MAX_RT is high meaning that the max number of retries has been reached
	return 0 if tranmission is still in progress*/
unsigned char check_TX_status(void){
	read_status();
	if(status & (1<<TX_DS)){
		clear_IRQs();
	 	return 1;
	}
	if(status & (1<<MAX_RT)){
		clear_IRQs();
	 	return 2;
	}
	else return 0;
}

/** returns the value of the status register */
unsigned char read_status(void){
	set_CSN(0);
	SPI_transmit(NOP);
	status = SPDR;
	set_CSN(1);
	return status;
}

/** Flushes the TX_FIFO */
void flush_TX_FIFO(void){
	set_CSN(0);
	SPI_transmit(FLUSH_TX);
	status = SPDR;
	set_CSN(1);
}

/** Flushes the RX_FIFO */
void flush_RX_FIFO(void){
	set_CSN(0);
	SPI_transmit(FLUSH_RX);
	status = SPDR;
	set_CSN(1);
}

/** returns true if the RX_DR flag is set, indicating that a packet was received*/
unsigned char check_RX_DR(void){
	read_status();
	if(status&(1<<RX_DR)){
		clear_IRQs();
	 	return 1;
	}
	else return 0;
}
