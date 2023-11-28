#define F_CPU 16000000UL	// 16MHz,크리스탈 주파수를 지정해주어야 올바른 시간 사용 가능

#include <avr/io.h>
#include <util/delay.h>

#define BTN 0x01	// D8, PB0
#define LED 1<<5	// D13, PB5
#define RED 0x02	// D9, PB1
#define GRENN 0x04	// D10, PB2
#define BLUE 0x08	// D11, PB3
#define input_value PINB	// read only register

// unsigned int count = 0;	// 0~65,536
unsigned long count = 0;
unsigned char pattern_table[2] = {LED | RED | BLUE, GRENN | BLUE};
unsigned char index = 0;	// 배열 요소 접근 변수
	
void setup(){
	DDRB = LED | RED | GRENN | BLUE | BTN;	// 출력 설정
	PORTB = 0x00;
}
void loop(){
	PORTB = 0xFF;
	PORTB &= ~(pattern_table[(index++) & 0x01]);	// (index++)&0x배열의 개수(16진수), 순차적으로 인덱스의 요소만 and 연산으로 나옴
	PORTB &= ~(BTN);	// BTN만 0으로 바꾸기
	
	// for(count = 0; count < 10000; count++);	// AVR 시간 측정(발진기의 펄스이용), 0x2710 == 10,000(10진수), 10ms
	if((input_value & BTN) == BTN){
		for(count=0; count < 500000; count++){		// 약 1초
			if((input_value & BTN) == BTN){		// BTN 누를 시
				PORTB = 0x00;
				while((input_value & BTN));	// BTN 지속, 떼기 전까지
			}
		}
	}
	/*PORTB = pattern_table[1];
	_delay_ms(2000);*/
	
}

int main(void)
{
	setup();
	
    while (1) 
    {
		loop();
    }
}

