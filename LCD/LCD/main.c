#include "main.h"
#include "twi.h"
#include "lcd.h" 

int main(void) {
	DDRC |= 0b00110000;
	
	TWI_Init();
	_delay_ms(50);
	LCD_Init();
	
	while (1) {
		LCD_clear();
		LCD_setPosition(0,0);
		LCD_sendString("TestLCD 23.12.29");
		LCD_setPosition(1,1);
		LCD_sendString(" Code Test ");
		_delay_ms(5000);
	}

	return 0;
}
