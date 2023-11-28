#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

// PD0  출력 라인

#define BTN 0x01
#define input_value PINB

void main(){
	unsigned char pattern_table[16] = {0xC0, 0x86, 0xA4, 0xD0, 0x99, 0x92, 0x82, 0xD8,
										// pattern 0123457
										0x80, 0x90, 0x88, 0x83, 0xA7, 0xA1, 0x86, 0x87};
										// pattern 89AbcdEF
	unsigned char index = 0;
	DDRB = BTN;		// 버튼 출력 설정
	
	DDRD = 0xff; // 모두 출력
	PORTD = 0x00;	// LED 초기화
	
	while(1){
		if((input_value & BTN) == BTN){
			PORTD = pattern_table[(index++)&0x0F];
			_delay_ms(100);
		}
	
	}
}