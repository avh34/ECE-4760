#define RX_MODE 1
#define TX_MODE 0

#define DDR_SPI DDRB
#define DD_MOSI DD5
#define DD_MISO DD6
#define DD_SCK DD7
#define DD_CSN DD4
#define CSN PORTB4
#define DD_CE DD2
#define CE PORTB2

unsigned PAYLOAD_LENGTH; //the length of the packets to be transmitted or received

unsigned char status; //contains status returned by the RF module

/** initialize the nRF24L01+ transceiver
	enhanced_shockburst_enable is set to 0 to disable enhanced shockburst and 1 to enable
	the transceiver starts out in RX mode*/
void init_RF(unsigned char enhanced_shockburst_enable);

/** Enables the pipe specified by pipenumber, which can be from 0 to 5*/
void enable_pipe(unsigned char pipenumber);

/**	Sets the transciever to TX mode if mode == TX_MODE
	else set it to RX mode*/
void set_mode(unsigned char mode);

/** transmit up to PAYLOAD_LENGTH bytes of the packet.
	returns 1 if the transmission is successful.
	returns 2 if maximum number of retries reached without receiving ACK,
	this only occurs if enhanced shockburst is enabled.
	With enhanced shockburst disabled, the function always returns 1 unless the transciever is connected wrongly*/
unsigned char transmit_packet(char* packet);

/** return 0 if RX_FIFO is empty. 1 otherwise */
unsigned char RX_FIFO_status(void);

/** Reads up to PAYLOAD_LENGTH bytes of a received packet from the RX FIFO into buffer 
	returns the pipe number that the packet was received from*/
unsigned char read_RX_payload(char* buffer);

/** Sets the TX address of the transceiver */
void set_TX_addr(unsigned long address);

/** Sets the RX address of the transceiver */
void set_RX_addr(unsigned long address, unsigned char pipe);

/** Sets the RF channel to channel, channel is a 7 bit number */
void set_RF_channel(unsigned char channel);

///////////////////////////////////////////////////////////////////////////////
/////Helper functions/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/** Reads the contents of the register specified by addr */
unsigned char read_RF_register(unsigned char addr);
/** Writes value to the register specified by addr */
void write_RF_register(unsigned char addr, unsigned char value);
/** Writes unsigned long longvalue to the register specified by addr */
void write_RF_register_long(unsigned char addr, unsigned char longvalue);
/**	CSN pin is used to initiate and end the communication between the SPI module of mega644 and the RF chip*/
void set_CSN(unsigned char value);
/**	CE enables and disables the transceiver from transmitting and receiving*/
void set_CE(unsigned char value);
/** helper function for transmitting data by SPI*/
unsigned char SPI_transmit(unsigned char data);
/** clear the RX_DR, TX_DS and MAX_RT IRQs by writing to 1 to bits corresponding to them in the STATUS register*/
void clear_IRQs(void);
/**	return 1 if TX_DS is high meaning that ACK was received and packet was transmitted successfully
	return 2 if MAX_RT is high meaning that the max number of retries has been reached
	return 0 if tranmission is still in progress*/
unsigned char check_TX_status(void);
/** returns the value of the status register */
unsigned char read_status(void);
/** Flushes the TX_FIFO */
void flush_TX_FIFO(void);
/** Flushes the RX_FIFO */
void flush_RX_FIFO(void);
/** returns true if the RX_DR flag is set, indicating that a packet was received*/
unsigned char check_RX_DR(void);
