#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

// 8bit Timer0
// TOV0(Timer Overflow 0)

// atmega328p TCCR0A,B datasheet 읽어볼 것
// TCCR0A mode 설정 레지스터
// TCCR0B 분주기 설정 레지스터

void setup();
void Timer_init();
void Timer_10ms_delay();
void Timer_1s_delay();

void setup(){
	DDRB = 0xFF;
	PORTB = 0xFF;
}

void Timer_init(){	// TCCR(Timer Counter Control Register), TCNT(Timer Count0)
	TCCR0A = 0x00;  // Normal mode(기본)
	TCCR0B = (1 << CS02) | (1 << CS00);  // 1024분주
	TCNT0 = 0x64;  // 100부터 255까지, 256은 overflow 순간 count
	// count수 = 255-TCNT0+1 -> 156, 156*64us = 9.984ms	
}

void Timer_10ms_delay(){	
	while(!(TIFR0 & 0x01));	// Flag가 생겼을 때(9.984ms가 지났을 때)
	TIFR0 |= 0x01;  // TIFR(Timer Interrupt Flag Register), TOV0(Timer overflow) flag 초기화
					// 실제로 레지스터에 1이 기록되는 것이 아닌 초기화하는 동작을 실행(제조사가 정함)
	TCNT0 = 0x64;	// Timer count 100으로 다시 초기화
}

void Timer_1s_delay(){
	unsigned char count = 100;
	
	while(count){
		while(!(TIFR0 & 0x01));
		TIFR0 |= 0x01; 
		TCNT0 = 0x64;
		count--;
	}
}

void main(){
	setup();
	Timer_init();
	
	while(1){
		PORTB = 0xFF;
		Timer_1s_delay();
		
		PORTB = 0x00;
		Timer_1s_delay();
	}
}