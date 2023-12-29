/*
 * Tilt_sensor.c
 *
 * Created: 2023-12-26 오전 11:50:03
 * Author : cdm05
 */ 
#define F_CPU 16000000UL	// 16MHz,크리스탈 주파수를 지정해주어야 올바른 시간 사용 가능
#include <avr/io.h>
#include <util/delay.h>

#define input 0x10   // D12, PB4
#define LED   0x20   // D13, PB5
#define input_value PINB

void setup(){
	PORTB = 0x00;		
	DDRB = (1 << DDB5);		// PB5만 출력
}

int main(){
	setup();
	
	while(1){
		PORTB = 0x00;
		
		if(input_value & input){		// Tilt sensor data 1
			PORTB |= LED;	//	 		
			_delay_ms(100);	
		}
	}
}