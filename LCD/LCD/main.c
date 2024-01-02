#include "main.h"
#include "TWI.h"
#include "LCD.h" 

int main(void) {
	DDRC |= 0b00110000;		// A4(SDA), A5(SCL)
	
	TWI_Init();
	_delay_ms(50);
	LCD_Init();
	
	while (1) {
		LCD_clear();
		LCD_setPosition(0,0);
		LCD_sendString("TestLCD 24.01.02");
		LCD_setPosition(0,1);
		LCD_sendString(" Code Test 9999");
		_delay_ms(5000);
	}

	return 0;
}
