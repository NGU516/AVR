#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>

#define RED   0x02   // D9, PB1
#define GREEN 0x04   // D10, PB2
#define BLUE  0x08   // D11, PB3
#define LED   0x20   // D13, PB5

volatile uint32_t Data = 0;        // 4BYTE 버퍼
volatile unsigned char Count_Data = 0; // 4개받으면 초기화

void setup() {
    DDRB = 0xFF;
    PORTB = 0x00;

    DDRD = 0x02;  // PD0 = RXD 로 입력 설정, PD1 = TXD 로 출력 설정
    PORTD = 0xFF; // 포트 초기화
}

void USART0_init(void) {
    UBRR0H = 0x00; // USART Baud Rate Register High, 9600 baud rate
    UBRR0L = 103;  // USART Baud Rate Register Low, 9600 baud rate

    UCSR0A = 0x20;  // 0b00100000, USART Data Reister Empty, 초기화시 비어있음.
    UCSR0B = (1 << RXEN0) | (1 << RXCIE0); // 수신 인터럽트 설정
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8-bit 데이터 포맷
}

void USART0_send(uint32_t Data) {
    while (!(UCSR0A & (1 << UDRE0)))
        ; // 데이터가 빌 때까지 대기
    UDR0 = Data;
}

void Timer1_init() {
    TCCR1A = 0x00;                                    // WGM11, WGM10 -> 0
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10); // WGM13, WGM12 -> 0100 (CTC 모드), CS12, CS11, CS10 -> 011 (분주비 64)
    OCR1A = 0x09C4;                                    // 0x09C4 = 2500 1ms, 16MHz / 64 / 2500 = 10Hz, 10ms 주기
}

void Timer1_10ms_delay() {
    while (!(TIFR1 & (1 << OCF1A)))
        ;
    TIFR1 |= (1 << OCF1A); // Output Compare Flag 초기화
}

void Timer1_100ms_delay() {
    unsigned char count = 0;

    count = 10;
    while (count) {
        while (!(TIFR1 & (1 << OCF1A)));
        TIFR1 |= (1 << OCF1A); // Output Compare Flag 초기화
        count--;
    }
}

void Timer1_1s_delay() {
    unsigned char count = 0;

    count = 100;
    while (count) {
        while (!(TIFR1 & (1 << OCF1A)));
        TIFR1 |= (1 << OCF1A); // Output Compare Flag 초기화
        count--;
    }
}

ISR(USART_RX_vect) { // 수신 인터럽트
    for (unsigned int i = 0; i < 4; ++i) {
        _delay_us(10);
        Data |= ((uint32_t)UDR0) << (i * 8);
        Count_Data++;
    }

}

//////////////////////////////////////////////////////////////////////////
int main() {
    setup();
    sei(); // 전역 인터럽트 활성화
    USART0_init();
    Timer1_init();
    Timer1_1s_delay();
    

    while (1) {
        PORTB = 0x00;

        if (Count_Data % 4 == 0) {
            if (Data > 0) { // 빨강
                PORTB |= RED;
                PORTB |= LED;
                Timer1_1s_delay();
            } else if (Data > 20000 && Data < 40000) { // 파랑
                PORTB |= BLUE;
                PORTB |= LED;
                Timer1_1s_delay();
            } else if (Data > 40000) { // 초록
                PORTB |= GREEN;
                PORTB |= LED;
                Timer1_1s_delay();
            } else {
                USART0_send('E'); // 입력 오류 전송
                continue;
            }
            USART0_send('C'); // 제어 완료 전송
            Data = 0;        // 버퍼 초기화
            Count_Data = 0;   // 카운트 초기화
        }
    }
}
//////////////////////////////////////////////////////////////////////////
