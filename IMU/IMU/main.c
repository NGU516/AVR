#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL
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
	char buffer[17];	// register length 16bit
	sprintf(buffer, "%s %d", label, data);
	UART_Transmit_string(buffer);
}

void I2C_INIT() {
	TWSR = 0;  // Set prescaler 1
	TWBR = ((F_CPU / 100000UL) - 16) / 2;  // Set bit rate 100kHz
}

void I2C_START() {
	TWCR = (1 << TWSTA) | (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
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
	while (!(TWCR & (1 << TWINT)));
	return TWDR;	// Higher 8bit
}

uint8_t I2C_READ_NACK() {
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	return TWDR;	// Lower 8bit
}

int16_t read_mpu9250_register16(uint8_t reg_addr) {
	I2C_START();
	I2C_WRITE((MPU9250_ADDRESS << 1) | 0);	// LSB 0: WRITE
	I2C_WRITE(reg_addr);
	I2C_START();
	I2C_WRITE((MPU9250_ADDRESS << 1) | 1);	// LSB 1: READ
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

	while (1) {
		if (Data_Received) {
			int16_t acc_x = read_mpu9250_register16(ACCEL_XOUT);
			int16_t acc_y = read_mpu9250_register16(ACCEL_YOUT);
			int16_t acc_z = read_mpu9250_register16(ACCEL_ZOUT);			
			int16_t gyro_x = read_mpu9250_register16(GYRO_XOUT);
			int16_t gyro_y = read_mpu9250_register16(GYRO_YOUT);
			int16_t gyro_z = read_mpu9250_register16(GYRO_ZOUT);
			
			UART_Transmit_data("ACC_X:", acc_x);
			UART_Transmit_char('\t');
			UART_Transmit_data("ACC_Y:", acc_x);
			UART_Transmit_char('\t');
			UART_Transmit_data("ACC_Z:", acc_x);
			UART_Transmit_char('\n');

			UART_Transmit_data("Gyro_X:", gyro_x);
			UART_Transmit_char('\t');
			UART_Transmit_data("Gyro_Y:", gyro_y);
			UART_Transmit_char('\t');
			UART_Transmit_data("Gyro_Z:", gyro_z);
			UART_Transmit_char('\n');
			
			_delay_ms(100);
			Data_Received = 0;
		}
	}
}
