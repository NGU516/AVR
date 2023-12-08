#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define BTN 0x01
#define RED 0x02
#define LED 0x20
#define input_value PINB

// volatile 지정 시 컴파일러가 최적화를 할 때 무시하지 않음.
volatile unsigned char Switch_flag = 0;
volatile unsigned char Timer_flag = 0;

void setup() {
    DDRB = 0xFF;
    PORTB = 0xFF;

    DDRD = 0x04;
}

void Timer1_init() {        // 분주비가 1024로 설정하고 1초로 설정하려고 하면 제대로 인식하지 않음.
    TCCR1A = 0x00;        // WGM11, WGM10 -> 0
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);  // WGM13, WGM12 -> 0100 (CTC 모드), CS12, CS11, CS10 -> 011 (분주비 64)
    OCR1A = 0x3D08;        // 0x3D08 = 15624 (1초), 16MHz / 1024 / 15624 = 약 1Hz
    TIMSK1 = (1 << OCIE1A);      // Timer 1 Output Compare A Interrupt Enable
}

void EXTINT_init() {     // External interrupt 초기화
    EIMSK = 0x01;   // INTF0 허용
    EICRA = 0x02;   // falling edge 일 때 interrupt 발생
}

ISR(INT0_vect) {        // INT0이 falling edge일 때 인터럽트 발생
    Switch_flag = 1;
}

ISR(TIMER1_COMPA_vect){
    Timer_flag = 1;
}


int main() {
    setup();    // PORT 초기화
    Timer1_init();      // 타이머 초기화
    EXTINT_init();      // 외부 인터럽트 초기화
    sei();      // 모든 interrupt 허용

    while (1) {
        PORTB = 0x00;

        if(Timer_flag){
            Timer_flag = 0; // 타이머 인터럽트 상태 초기화
            PORTB |= LED;
            //_delay_ms(1000);
        }
           
        if ((input_value & BTN) == BTN) {  // 버튼이 눌렸을 때
            PORTB = 0x00;
            PORTD |= (1 << PORTD2);  // PD2, INT0 인터럽트 발생
            if(Switch_flag == 1){
                Switch_flag = 0;  // 상태 초기화
                PORTB |= RED;  // RED만 켜기
                _delay_ms(1000);
            }
            PORTD &= ~(1 << PORTD2);  // PD2, INT0 다시 LOW로 설정
            Switch_flag = 0;  // 스위치 인터럽트 상태 초기화
        }
        
    }
}

