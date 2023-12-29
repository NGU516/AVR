#include "twi.h"
#include "main.h"

void TWI_Init(void){
	// TWI Bit Rate Register
	// TWBR = 0x48;		// 0b01001000
	TWBR = (1<<TWBR5) | (1<<TWBR3);		// SCL Frequency, Datasheet for reference
}

void TWI_Start(void) {
	// TWI Control Register
	// Interrupt, Start bit, Enable Set
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));		// TWI Setup complete -> Interrupt flag set
}

void TWI_Transmit(unsigned char data) {
	// TWI Data Register(Data buffer, 8bit)
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN);		// Transmit -> Interrupt
	while(!(TWCR & (1<<TWINT)));
}

void TWI_Transmit_Addr(unsigned char data, unsigned char addr){
	TWI_Start();
	TWI_Transmit(addr);
	TWI_Transmit(data);
	TWI_Stop();
}

void TWI_Stop(void) {
	// TWCR Register init
	TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);
}