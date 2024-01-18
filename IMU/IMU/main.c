#include "main.h"
#include "LCD.h"
#include "TWI.h"
#include <avr/interrupt.h>

#define BAUD_RATE 9600
#define UBRR_VALUE ((F_CPU / (BAUD_RATE * 16UL)) - 1)

// MPU9250 Register Address
#define MPU9250_ADDRESS 0x68
#define ACCEL_XOUT 0x3B
#define ACCEL_YOUT 0x3D
#define ACCEL_ZOUT 0x3F
#define GYRO_XOUT 0x43
#define GYRO_YOUT 0x45
#define GYRO_ZOUT 0x47

volatile uint8_t Data_Received = 0;
volatile char Received_char;

//////////////////////////////////////////////////////////////////////////
void UART_INIT(void);
void UART_Transmit_char(char data);
void UART_Transmit_string(const char* str);
void UART_Transmit_data(const char* label, int16_t data);
void I2C_INIT(void);
void I2C_START(void);
void I2C_STOP(void);
void I2C_WRITE(uint8_t data);
uint8_t I2C_READ_ACK(void);
uint8_t I2C_READ_NACK(void);
int16_t read_mpu9250_register16(uint8_t reg_addr);
void handleCommunicationError(void);

ISR(USART_RX_vect);

//////////////////////////////////////////////////////////////////////////

void UART_INIT() {
	UBRR0H = (uint8_t)(UBRR_VALUE >> 8);
	UBRR0L = (uint8_t)UBRR_VALUE;
	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8-bit format
}

void UART_Transmit_char(char data) {
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

void UART_Transmit_string(const char* str) {
	while (*str) {
		UART_Transmit_char(*str);
		str++;
	}
}

void UART_Transmit_data(const char* label, int16_t data) {
	char buffer[20];
	float float_data = (float)data;
	snprintf(buffer, sizeof(buffer), "%s %.3f\n", label, float_data);
	UART_Transmit_string(buffer);
}

void I2C_INIT() {
	TWSR = 0;  // Set prescaler 1
	TWBR = ((F_CPU / 100000UL) - 16) / 2;  // Set bit rate 100kHz
	TWCR |= (1 << TWEN);
}

void I2C_START() {
	TWCR = (1 << TWSTA) | (1 << TWINT) | (1 << TWEN);	// | (1 << TWIE), if use TWI interrupt(통신 완료마다 인터럽트 발생)
	while (!(TWCR & (1 << TWINT)));
	TWCR &= ~(1 << TWINT);
}

void I2C_STOP() {
	TWCR = (1 << TWSTO) | (1 << TWINT) | (1 << TWEN);
	_delay_us(50);
}

void I2C_WRITE(uint8_t data) {
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
}

uint8_t I2C_READ_ACK() {
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);

	TCNT1 = 0;  // Time out Counter
	TCCR1B |= (1 << CS12) | (1 << CS10);  // Timer 1024 prescaler

	while (!(TWCR & (1 << TWINT))) {
		if (TCNT1 > 1000) {	// 1sec
			break;
		}
	}
	
	TCCR1B = 0;
	return TWDR;    // Higher 8bit
}

uint8_t I2C_READ_NACK() {
	TWCR = (1 << TWINT) | (1 << TWEN);
	
	TCNT1 = 0;
	TCCR1B |= (1 << CS12) | (1 << CS10);

	while (!(TWCR & (1 << TWINT))) {
		if (TCNT1 > 1000) {
			break;
		}
	}

	TCCR1B = 0;
	return TWDR;    // Lower 8bit
}

int16_t read_mpu9250_register16(uint8_t reg_addr) {
	I2C_START();
	I2C_WRITE((MPU9250_ADDRESS << 1) | 0);    // LSB 0: WRITE
	I2C_WRITE(reg_addr);
	I2C_START();
	I2C_WRITE((MPU9250_ADDRESS << 1) | 1);    // LSB 1: READ
	int16_t data = (I2C_READ_ACK() << 8) | I2C_READ_NACK();
	I2C_STOP();
	return data;
}

ISR(USART_RX_vect) {
	Received_char = UDR0;
	Data_Received = 1;
}

int main(void) {
	sei();
	UART_INIT();
	I2C_INIT();
	LCD_Init();
	cli();
	LCD_clear();
	_delay_ms(20);
	sei();

	while (1) {
		// MPU9250 raw data Print LCD
		LCD_setPosition(0, 0);
		int16_t gyro_x_raw = read_mpu9250_register16(GYRO_XOUT);
		float gyro_x_data = gyro_x_raw / 250.0;  // set scale ±250°/sec
		char gyro_x_str[7];
		snprintf(gyro_x_str, sizeof(gyro_x_str), "%+05.3f", gyro_x_data);
		LCD_sendString(gyro_x_str);

	
		LCD_setPosition(8, 0);
		int16_t gyro_y_raw = read_mpu9250_register16(GYRO_YOUT);
		float gyro_y_data = gyro_y_raw / 250.0;
		char gyro_y_str[7];
		snprintf(gyro_y_str, sizeof(gyro_y_str), "%+05.3f", gyro_y_data);
		LCD_sendString(gyro_y_str);
		
		LCD_setPosition(0, 1);
		int16_t gyro_z_raw = read_mpu9250_register16(GYRO_ZOUT);
		float gyro_z_data = gyro_z_raw / 250.0;
		char gyro_z_str[8];
		snprintf(gyro_z_str, sizeof(gyro_z_str),"%+05.3f", gyro_z_data);
		LCD_sendString(gyro_z_str);
		
		LCD_setPosition(8, 1);
		LCD_sendString("GYRO_XYZ");

		if (Data_Received) {
			UART_Transmit_data("GYRO_X_RAW: ", gyro_x_raw);UART_Transmit_char('\t');
			UART_Transmit_data("GYRO_X_Data: ", gyro_x_data);UART_Transmit_char('\t');
			UART_Transmit_char('\n');

			UART_Transmit_data("GYRO_Y_RAW: ", gyro_y_raw);UART_Transmit_char('\t');
			UART_Transmit_data("GYRO_Y_Data: ", gyro_y_data);UART_Transmit_char('\t');
			UART_Transmit_char('\n');

			UART_Transmit_data("GYRO_Z_RAW: ", gyro_z_raw);UART_Transmit_char('\t');
			UART_Transmit_data("GYRO_Z_Data: ", gyro_z_data);UART_Transmit_char('\t');
			UART_Transmit_char('\n');
			Data_Received = 0;
		}
		_delay_ms(500);
	}
}
