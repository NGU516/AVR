#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

// 일정 시간(1s)을 두고 ADC0, ADC7채널에 아날로그 전압을 두 개 중 한개의 값만 led에 2진수 표현
////////////////////////////////////////////////////////////////////////// 

// Single Conversion Mode(변환 시작 완료 후 대기, 한번만 변환)
// ADMUX(ADC multiplexing selection register)
// ADCSRA(ADC control and status register A)

void setup(){
	PORTB = 0x01;
	DDRB = 0x01;
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

void ADC_init(){
	ADMUX = 0x60;	//	0b01100000, REFS0~1, ADLAR 1일 시 최상위 8비트만 사용, 0일 시 최하위 8비트만 사용
	ADCSRA = 0x87;	// 0b10000111, ADEN, ADPS0~2
}

unsigned char ADC_read(unsigned char ch){
	ADMUX |= (ch & 0b00001111);	// ch 7일 경우
		ADCSRA |= 0x40;		// 6번 비트, ADSC(ADC start conversion)
		
	while(!(ADIF & 0x10));	// flag 초기화, ADIF(ADC Interrupt Flag)
	return ADC;		// 16bit 변환 레지스터 반환, 앞에 10자리 ADLAR로 왼쪽에 10bit로 해상도(resolution) 표시
}

void main(void)
{
	unsigned int Value_ADC0;		// 0번 채널
	unsigned int Value_ADC7;		// 7번 채널
	unsigned char ch;
	
	setup();
	ADC_init();
	Timer1_init();
	
    while (1) {
		ch = 0x00;
		Value_ADC0 = ADC_read(ch);
		
		PORTB = (unsigned char) Value_ADC0;			// 하위 bit 정보 출력
		PORTD = (unsigned char) (Value_ADC0 >> 8);	// 상위 bit 정보 출력
		
		Timer1_1s_delay();
		
		ch = 0x07;
		Value_ADC7 = ADC_read(ch);
		
		PORTB = (unsigned char) Value_ADC7;
		PORTD = (unsigned char) (Value_ADC7 >> 8);	
		
		Timer1_1s_delay();
    }
}

