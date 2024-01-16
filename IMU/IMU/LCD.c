#include "LCD.h"
#include "main.h"
#include "TWI.h"

#define LCD_ADDRESS 0x27 

volatile unsigned char PortLCD = 0;  // Higher 4bit(0xF0) == data bit, Lower 4bit(0x0F) == Control bit
uint8_t LCD_ADDR = LCD_ADDRESS << 1;  // I2C 주소를 변수로 지정

void LCD_Init(void) {
	sendHalfByte(0b00000011);
	_delay_ms(5);
	sendHalfByte(0b00000011);
	_delay_us(100);
	sendHalfByte(0b00000011);
	_delay_ms(1);
	sendHalfByte(0b00000010);
	_delay_ms(1);

	sendByte(0b00101000, 0);  // Data 4bit, Line 2, Font 5x8
	_delay_ms(1);
	sendByte(0b00001111, 0);  // Display ON, Cursor ON, Blink ON
	_delay_ms(1);

	TWI_Transmit_Addr(PortLCD |= 0x08, LCD_ADDR);  // BackLight ON
	TWI_Transmit_Addr(PortLCD &= ~0x02, LCD_ADDR);  // LCD Write ON
}

void LCD_BackLight(unsigned char mode) {
	switch (mode) {
		case 0:
		TWI_Transmit_Addr(PortLCD &= ~0x08, LCD_ADDR);  // BackLight OFF
		break;
		case 1:
		TWI_Transmit_Addr(PortLCD |= 0x08, LCD_ADDR);  // BackLight ON
		break;
	}
}

void sendHalfByte(unsigned char c) { // 4 bit
	// current Higher 4 Bit = data lower 4bit
	c <<= 4;

	TWI_Transmit_Addr(PortLCD |= 0x04, LCD_ADDR); // Enable E(0x04)
	_delay_us(50);

	TWI_Transmit_Addr(PortLCD | c, LCD_ADDR);

	TWI_Transmit_Addr(PortLCD &= ~0x04, LCD_ADDR); // Disable E(0x04)
	_delay_us(50);
}

void sendByte(unsigned char c, unsigned char mode) {
	if (mode == 0)
	TWI_Transmit_Addr(PortLCD &= ~0x01, LCD_ADDR); // PortLCD : 0xX0, RS
	else
	TWI_Transmit_Addr(PortLCD |= 0x01, LCD_ADDR); // PortLCD : 0xX1, RS

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