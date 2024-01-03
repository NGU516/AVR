#include "LCD.h"
#include "TWI.h"
#include "main.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define BTN 0x01     // D8, PB0
#define RED   0x02   // D9, PB1
#define GREEN 0x04   // D10, PB2
#define BLUE  0x08   // D11, PB3
#define LED   0x20   // D13, PB5

volatile unsigned char DataBuffer[16] = {};  // 16BYTE ����
volatile unsigned char BufferIndex = 0; // ���� �Է� ��ġ
volatile unsigned char state = 0;       // ����

void setup() {
	DDRB = 0xFF;
	PORTB = 0x00;

	DDRD = 0x02;  // PD0 = RXD �� �Է� ����, PD1 = TXD �� ��� ����
	PORTD = 0xFF; // ��Ʈ �ʱ�ȭ

	DDRC |= 0b00110000; // A4(SDA), A5(SCL)
	
	
    PCICR |= (1 << PCIE2);  // PCINT2 �� ���� ���ͷ�Ʈ Ȱ��ȭ
	PCMSK2 |= (1 << PCINT23);  // D7 �ɿ� ���� ��ġ ���� ���ͷ�Ʈ Ȱ��ȭ
	
}

void USART0_init(void) {
	UBRR0H = 0x00; // USART Baud Rate Register High, 9600 baud rate
	UBRR0L = 103;  // USART Baud Rate Register Low, 9600 baud rate

	UCSR0A = 0x20;  // 0b00100000, USART Data Register Empty, �ʱ�ȭ�� �������.
	UCSR0B = 0x98;  // 0b00011000, RXEN TXEN '1' ���� �� UCSZ02 '0' ����
	UCSR0C = 0x06;  // 0b00000110, UCSZ01, UCSZ00 '1' ����
	// UCSR0B = (1 << RXEN0) | (1 << RXCIE0) | (0 << UCSZ02); // ���� ���ͷ�Ʈ ����
	// UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8-bit ������ ����
}

void USART0_send(unsigned char Data) {
	while (!(UCSR0A & (1 << UDRE0))); // �����Ͱ� �� ������ ���
	UDR0 = Data;
}

void EXTINT_init() { // External interrupt �ʱ�ȭ
	EIMSK = 0x01;    // INTF0 ���
	EICRA = 0x02;    // falling edge �� �� interrupt �߻�
}

ISR(USART_RX_vect) { // ���� ���ͷ�Ʈ
	unsigned char ReceivedData = UDR0;
	
	if (ReceivedData == 0x08) { // loof ���� �ȵ�
		USART0_send('a');
		sendByte(0x10, 0);

		if (BufferIndex > 0) {
			BufferIndex--;
			DataBuffer[BufferIndex] = '\0';
		}

		LCD_setPosition(0, 0);
		LCD_sendString("ReceivedData:");
		LCD_setPosition(0, 1);
		LCD_sendString(DataBuffer);
		} 
		
	else {
		if (BufferIndex < 15) {
			DataBuffer[BufferIndex] = ReceivedData;
			BufferIndex++;
			LCD_setPosition(0, 0);
			LCD_sendString("ReceivedData:");
			LCD_setPosition(0, 1);
			LCD_sendString(DataBuffer);
		}
	}
}


ISR(PCINT2_vect) { // D0~D7 Pin change Interrupt
	memset(DataBuffer, 0, sizeof(DataBuffer));	// arr set
	BufferIndex = 0;	
	LCD_clear();
	_delay_ms(2);
}


int main() {
	setup();
	USART0_init();
	EXTINT_init();
	sei(); // ���� ���ͷ�Ʈ Ȱ��ȭ

	TWI_Init();
	_delay_ms(20);
	LCD_Init();
	cli();
	LCD_clear();
	_delay_ms(20);
	sei();
	
	while (1) {
	}
}
