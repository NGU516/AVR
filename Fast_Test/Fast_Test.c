#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define RED 0x02   // D9, PB1
#define BLUE 0x08  // D11, PB3
#define LED 0x20   // D13, PB5

volatile uint32_t ReceivedNumber = 0;
volatile uint8_t NewDataReceived = 0;

void setup() {
    DDRB = 0xFF;
    PORTB = 0x00;

    DDRD = 0x02;  // PD0 = RXD 로 입력 설정, PD1 = TXD 로 출력 설정
    PORTD = 0xFF; // 포트 초기화
}

void USART0_init(void) {
    UBRR0H = 0x00; // USART Baud Rate Register High, 9600 baud rate
    UBRR0L = 103;  // USART Baud Rate Register Low, 9600 baud rate

    UCSR0A = 0x20;                                      // 0b00100000, USART Data Reister Empty, 초기화시 비어있음.
    UCSR0B = (1 << RXEN0) | (1 << RXCIE0); // 수신 인터럽트 설정
    UCSR0C = 0x06;                                      // 0b00000110, UCSZ01, UCSZ00 '1' 설정
                                                        ///////////// 한 번에 8bit를 보내야 함 {UCSZ02, UCSZ01, UCSZ00} = {0, 1, 1} 설정, 쓰게되면 100 -> 8bit
}

ISR(USART_RX_vect) {
    char receivedChar = UDR0;

    if (receivedChar >= '0' && receivedChar <= '9') {
        ReceivedNumber = ReceivedNumber * 10 + (receivedChar - '0');
    } else if (receivedChar == '\n') {
        NewDataReceived = 1;
    }
}

void loop() {
    if (NewDataReceived) {
        // 적절한 동작을 위한 ReceivedNumber 값 비교
        if (ReceivedNumber == 1) {
            // 빨간색 LED를 켬
            PORTB |= RED;
            PORTB |= LED;
            _delay_ms(1000);
        } else {
            // 파란색 LED를 켬
            PORTB |= BLUE;
            PORTB |= LED;
            _delay_ms(1000);
        }

        // 수신 및 처리가 끝난 후 초기화
        ReceivedNumber = 0;
        NewDataReceived = 0;
    }
}

int main() {
    setup();
    USART0_init();
    sei(); // 전역 인터럽트 활성화

    while (1) {
        loop();
    }

    return 0;
}
