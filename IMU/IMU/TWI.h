#ifndef TWI_H_
#define TWI_H_

void TWI_Start(void);
void TWI_Init(void);
void TWI_Stop(void);
void TWI_Transmit(unsigned char data);
void TWI_Transmit_Addr(unsigned char data, unsigned char addr);

#endif /* TWI_H_ */