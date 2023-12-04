#define F_CPU 16000000UL
#include <avr/io.h>

#define BTN 0x01
#define input_value PINB

void setup() {
	DDRB = 0x20;    
	PORTB = 0x00;   

	DDRD = 0x02;    // PD0 = RXD 로 입력 설정, PD1 = TXD 로 출력 설정
	PORTD = 0xFF;   // 포트 초기화
}

void USART0_init(void){
	UBRR0H = 0x00;	// USART Baud Rate Register High, 9600 baud rate
	UBRR0L = 103;	// USART Baud Rate Register Low, 9600 baud rate
	
	UCSR0A = 0x20;	// 0b00100000, USART Data Reister Empty, 초기화시 비어있음.
	UCSR0B = 0x18;	// 0b00011000, RXEN TXEN '1' 설정 및 UCSZ02 '0' 설정
	UCSR0C = 0x06;	// 0b00000110, UCSZ01, UCSZ00 '1' 설정
	///////////// 한 번에 8bit를 보내야 함 {UCSZ02, UCSZ01, UCSZ00} = {0, 1, 1} 설정, 쓰게되면 100 -> 8bit
}

void USART0_send(unsigned char Data){
	while(!( UCSR0A & (1<<UDRE0)) );
	while(!(UCSR0A & 0x20));
	UDR0 = Data;
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

void Timer1_100ms_delay(){
    unsigned char count = 0;
	
	count = 10;
	while(count){
		while (!(TIFR1 & (1 << OCF1A)));
		TIFR1 |= (1 << OCF1A);  // Output Compare Flag 초기화
		count--;
	}
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

//////////////////////////////////////////////////////////////////////////
int main(){
	unsigned char Data;		// 전송할 문자를 저장하는 버퍼
	
	setup();
	USART0_init();
	Timer1_init();
	Timer1_1s_delay();
	
	while(1){
		Data = 'A';

		PORTB = 0x00; // LED On

        if((input_value & BTN) == BTN){
            USART0_send(Data);

            Timer1_1s_delay();

			if((input_value & BTN) == BTN){		// BTN 누를 시
			    while((input_value & BTN)){         // BTN 지속, 떼기 전까지    
                    PORTB = 0x20;   // LED ON
                    USART0_send(Data);
                    Timer1_100ms_delay();    
                }	
		    }
	    }
	}

}
//////////////////////////////////////////////////////////////////////////
