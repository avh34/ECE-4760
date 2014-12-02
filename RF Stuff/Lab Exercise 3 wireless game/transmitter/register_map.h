//register_map.h
//Look at nRF24L01+ datasheet pg 57 for better description
#define CONFIG 0x00 //Configuration register.
	#define MASK_RX_DR 6
	#define MASK_TX_DS 5
	#define MASK_MAX_RT 4
	#define EN_CRC 3//Enables CRC, Forced high if one of the bits in the EN_AA is high
	#define CRCO 2//'0' = 1byte CRC, '1' = 2 bytes CRC
	#define PWR_UP 1
	#define PRIM_RX 0
#define EN_AA 0x01		//Enhanced Shock burst
	#define ENAA_P5 5 //Enable auto acknowledgement data pipe5
	#define ENAA_P4 4
	#define ENAA_P3 3
	#define ENAA_P2 2
	#define ENAA_P1 1
	#define ENAA_P0 0
#define EN_RXADDR 0x02		//Enabled RX addresses(Data PIPE)
	#define ERX_P5 5
	#define ERX_P4 4
	#define ERX_P3 3
	#define ERX_P2 2
	#define ERX_P1 1
	#define ERX_P0 0
#define SETUP_AW 0x03	//Setup of Address Width(common for all data pipes)
#define SETUP_RETR 0x04	//Setup automatic retransmission
	#define ARD 4	//Auto retransmit delay 0000(250uS)~1111(4000uS)
	#define ARC 0	//Auto retransmit count(0~15)
#define RF_CH 0x05	//RF Channel frequency
#define RF_SETUP 0x06 //Setup RF
	#define CONT_WAVE 7
	#define RF_DR_LOW 5	//Set RF DATA Rate to 250Kbps
	#define RF_DR_HIGH 3//
	#define RF_PWR 1
#define STATUS 0x07//In the event of an interrupt, you look at this register to see 
					//The source of interrupt, Write one to clear bit
	#define RX_DR 6
	#define TX_DS 5
	#define MAX_RT 4
	#define RX_P_NO 1//Which data pipe(of 6
	#define STATUS_TX_FULL 0
#define OBSERVE_TX 0x08 //Transmit Observer
	#define PLOS_CNT 4
	#define ARC_CNT 0
#define RPD 0x09	//Received Power detector
#define RX_ADDR_P0 0x0A//Receive Address data pipe(5bytes)
#define RX_ADDR_P1 0x0B//Receive Address data pipe(5bytes)
#define RX_ADDR_P2 0x0C
#define RX_ADDR_P3 0x0D
#define RX_ADDR_P4 0x0E
#define RX_ADDR_P5 0x0F
#define TX_ADDR 0x10//Transmit address(5byte). Used for PTX device only
#define RX_PW_P0 0x11 //Number of bytes in RX payload in data pipe 0
#define RX_PW_P1 0x12
#define RX_PW_P2 0x13
#define RX_PW_P3 0x14
#define RX_PW_P4 0x15
#define RX_PW_P5 0x16
#define FIFO_STATUS 0x17
	#define TX_REUSE 6
	#define FIFO_STATUS_TX_FULL 5
	#define TX_EMPTY 4
	#define RX_FULL 1
	#define RX_EMPTY 0
#define DYNPD 0x1C	//Enable Dynamic payload length
	#define DPL_P5 5
	#define DPL_P4 4
	#define DPL_P3 3
	#define DPL_P2 2
	#define DPL_P1 1
	#define DPL_P0 0
#define FEATURE 0x1D
	#define EN_DPL 2
	#define EN_ACK_PAY 1
	#define EN_DYN_ACK 0


























	


