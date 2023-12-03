#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile int state = 0;

ISR(INT0_vect){
	state = !state;		//상태 토글
}

void INIT_PORT(void){
	DDRB = 0x01;		//pb0핀 출력
	PORTB = 0x00;		//led 꺼진상태
	
	DDRD = 0x00;		//pd0핀 입력
	PORTD = 0x04;		//pd0 풀업저항 사용
}

void INIT_INT0(void){
	EIMSK |= (1<<INT0);			//인터럽트0 허용
	EICRA |= (1<<ISC01);		//하강엣지일때 인터럽트 발생
	sei();
}

int main(){
	INIT_PORT();
	INIT_INT0();
	
	while(1){
		if(state == 1){
			PORTB = 0x01;		//LED ON
		}
		else{
			PORTB = 0x00;		//LED OFF
		}
	}
	return 0;
}
