#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

// 16bit Timer1
// CTC(Clear Timer on Compare)
// TCCR1B의 WGM(Waveform Generation Mode)12, CTC설정 비트 0100(13,12,11,10)

// TCCR1B의 CS12,11,10 제어
// 8분주 -> 010

// OCR1A(Output Compare Register)

// TIFR1 2번 비트 OCF1A flag

void setup();
void Timer1_init();
void Timer1_10ms_delay();
void Timer1_1s_delay();

void setup(){
	DDRB = 0xFF;
	PORTB = 0xFF;
}

void Timer1_init() {
	TCCR1A = 0x00;        // WGM11, WGM10 -> 0
	TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);  // WGM13, WGM12 -> 0100 (CTC 모드), CS12, CS11, CS10 -> 011 (분주비 64)
	OCR1A = 0x09C4;        // 0x09C4 = 2500 1ms, 16MHz / 64 / 2500 = 10Hz, 10ms 주기
}

void Timer1_10ms_delay() {
	while (!(TIFR1 & (1 << OCF1A)));
	TIFR1 |= (1 << OCF1A);  // Output Compare Flag 초기화
}

void Timer1_1s_delay(){
	unsigned char count = 0;
	
	count = 100;
	while(count){
		while (!(TIFR1 & (1 << OCF1A)));
		TIFR1 |= (1 << OCF1A);  // Output Compare Flag 초기화
		count--;
	}
}

void main(){
		setup();
		Timer1_init();
		
		while(1){
			PORTB = 0xFF;
			Timer1_1s_delay();
			
			PORTB = 0x00;
			Timer1_1s_delay();
		}
}