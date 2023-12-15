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

volatile unsigned char state = 0;

void setup() {
    DDRB = 0xFF;
    PORTB = 0xFF;

    DDRD = 0x04;
}

void Timer1_init() {
    TCCR1A = 0x00;        // WGM11, WGM10 -> 0
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);  // WGM13, WGM12 -> 0100 (CTC 모드), CS12, CS11, CS10 -> 011 (분주비 64)
    OCR1A = 0x09C4;        // 0x09C4 = 2500 1ms, 16MHz / 64 / 2500 = 10Hz, 10ms 주기
}

void Timer1_1s_delay() {
    _delay_ms(1000);  // 1초 지연
}

void EXTINT_init() {     // External interrupt 초기화
    EIMSK = 0x01;   // INTF0 허용
    EICRA = 0x02;   // falling edge 일 때 interrupt 발생
}

ISR(INT0_vect) {        // INT0이 falling edge일 때 인터럽트 발생
    state = 1;
}

int main() {
    setup();    // PORT 초기화
    Timer1_init();      // 타이머 초기화
    EXTINT_init();
    sei();      // 모든 interrupt 허용

    while (1) {
        PORTB = 0x00;

        if ((input_value & BTN) == BTN) {  // 버튼이 눌렸을 때
            PORTD |= (1 << PORTD2);  // PD2, INT0 인터럽트 발생
            if(state == 1){
                PORTB |= RED;  // RED만 켜기
            Timer1_1s_delay();
            }
            PORTD &= ~(1 << PORTD2);  // PD2, INT0 다시 LOW로 설정
        }
        state = 0;  // 상태 초기화
    }
}

