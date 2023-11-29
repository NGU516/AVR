#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

// atmega328p TCCR0A,B datasheet 읽어볼 것

#define LED 1<<5 // LED pin

unsigned char count = 0;

void setup(){
	PORTB = 0xFF;
	DDRB = 0xFF;
}

void main(void)
{
	setup();
	
	// TCCR(Timer Counter Control Register), TCNT(Timer Count0)
	TCCR0A = 0x00;  // Normal mode
	TCCR0B = (1 << CS02) | (1 << CS00);  // prescaler 1024
	TCNT0 = 0x64;  // 100부터 255까지, 256은 overflow 순간 count
	// 255-TCNT0+1 -> 156, 156*64us = 9.984ms
	
	while (1)
	{
		count = 100;
		
		PORTB = 0xFF;
		
		while(count){
			while(!(TIFR0 & 0x01));
			TCNT0 = 0x64;
			TIFR0 |= 0x01;  // TIFR(Timer Interrupt Flag Register), TOV0(Timer overflow) flag 초기화
			count--;
		}
		
		PORTB = 0x00;
		
		count = 100;
		while(count){
			while(!(TIFR0 & 0x01));
			TCNT0 = 0x64;
			TIFR0 |= 0x01;
			count--;
		}
		
	}
}
