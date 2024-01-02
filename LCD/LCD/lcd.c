#include "LCD.h"
#include "main.h"
#include "TWI.h"

volatile unsigned char PortLCD = 0;		// Higher 4bit(0xF0) == data bit, Lower 4bit(0x0F) == Control bit

void LCD_Init(void) {
	// sendByte(0b00110011, 0);
	sendHalfByte(0b00000011);
	_delay_ms(5);
	sendHalfByte(0b00000011);
	_delay_us(100);
	// sendByte(0b00110010, 0);
	sendHalfByte(0b00000011);
	_delay_ms(1);
	sendHalfByte(0b00000010);
	_delay_ms(1);
	
	sendByte(0b00101000, 0); // Data 4bit, Line 2, Font 5x8
	_delay_ms(1);
	sendByte(0b00001110, 0); //Display ON, Cursor ON, Blink OFF
	_delay_ms(1);

	TWI_Transmit_Addr(PortLCD |= 0x08, 0x4E);	// BackLight ON
	
	TWI_Transmit_Addr(PortLCD &= ~0x02, 0x4E);	// LCD Write ON
}

void LCD_BackLight(unsigned char mode) {
	switch(mode) {
		case 0: 
			TWI_Transmit_Addr(PortLCD &= 0x08, 0x4E);	// BackLight OFF
			break;
		case 1: 
			TWI_Transmit_Addr(PortLCD |= 0x08, 0x4E);	// BackLight ON
			break;
	}
}

void sendHalfByte(unsigned char c) { // 4 bit
	// current Higher 4 Bit = data lower 4bit
	c <<= 4;
	
	TWI_Transmit_Addr(PortLCD |= 0x04, 0x4E); // Enable E(0x04), 0x4E(address)
	_delay_us(50);

	TWI_Transmit_Addr(PortLCD | c, 0x4E);

	TWI_Transmit_Addr(PortLCD &= ~0x04, 0x4E); // Disable E(0x04)
	_delay_us(50);
}

void sendByte(unsigned char c, unsigned char mode) {
	if(mode == 0) TWI_Transmit_Addr(PortLCD &= ~0x01, 0x4E);	// PortLCD : 0xX0, RS
	else TWI_Transmit_Addr(PortLCD |= 0x01, 0x4E);				// PortLCD : 0xX1, RS
	unsigned char half_c = 0;
	half_c = c >> 4;
	sendHalfByte(half_c);
	sendHalfByte(c);
}

void LCD_sendString(char s[]) {
	char n;
	for(n=0; s[n]!='\0'; n++)
		sendByte(s[n], 1);
}

void LCD_setPosition(unsigned char x, unsigned char line) {	// HD44780 data sheet for reference
	switch(line) {
		case 0: 
			sendByte(x | 0x80, 0);
			break;
		case 1: 
			sendByte((x+0x40) | 0x80, 0);
			break;
	}
}

void LCD_clear(void) {
	sendByte(0x01, 0);
	_delay_ms(5);
}