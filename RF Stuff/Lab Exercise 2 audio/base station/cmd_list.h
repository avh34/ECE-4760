//CMD_LIST.h
//Contains Definition for allt he commands used in the chip
#define R_REGISTER 0b00000000	//Read command and status registers
#define W_REGISTER 0b00100000  //Write command and status registers, Executable in power down or standby mode only
#define R_RX_PAYLOAD 0b01100001	//Read RX-payload, starts from LSByte first
#define W_TX_PAYLOAD 0b10100000    //Write TX=Payload, starts from LSByte first
#define FLUSH_TX 0b11100001	//Flush TX FIFO
#define FLUSH_RX 0b11100010	//FLUSH RX_FIFO. Should not be executed during transmission of acknoweledge
#define REUSE_TX_PL 0b11100011	//Reuse last transmitted payload. Active untill W_TX_PAYLOAD or FLUSH_TX is executed
								//Not to be used during packet transmission
#define R_RX_PL_WID 0b01100000 //Read RX payload width for the top R_RX_PAYLOAD in RX FIFO, Flush FIFO if read value 
								//is larger than 32bytes
#define W_ACK_PAYLOAD 0b10101000 //Write payload to be transmitted with ACK packet on PIPE PPP
#define W_TX_PAYLOAD_NOACK 0b10110000 //Used in TX mode. Disables AUTOACK on this specific packet.
#define NOP 0b11111111	//No operation. Used to read status register.
