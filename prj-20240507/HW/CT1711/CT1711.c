#include "ct1711.h"
#include "delay.h"


void CT1711_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); //使能PORTA,PORTB时钟
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);    //关闭jtag，使能SWD

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_SetBits(GPIOB, GPIO_Pin_4);
}


void CT1711_reset(void)
{
  	GPIO_ResetBits(GPIOB, GPIO_Pin_4);
  	delay_us(500); // 500 us
 	GPIO_SetBits(GPIOB, GPIO_Pin_4);	
}


char CT1711_Read_Bit(void)
{
	char bi_data;
	GPIO_ResetBits(GPIOB, GPIO_Pin_4);
	delay_us(5); // 5us 
	GPIO_SetBits(GPIOB, GPIO_Pin_4);
  	delay_us(10); // 10us
  	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4))
  	{
    	bi_data = 1;
  	} else {
    	bi_data = 0;
}
	GPIO_SetBits(GPIOB, GPIO_Pin_4);
  	delay_us(30); // 30us
  
  	return bi_data;
}


unsigned char CT1711_Read_Byte(void)
{
  	unsigned char byte = 0;
  	int i;
  	for(i=8; i>0; i--) {
		byte <<= 1;
		byte |= CT1711_Read_Bit();
	}
	return byte;
}


float CT1711_Read_Temp_Degree(void)
{
	float temp = 0.00;
  	unsigned char bit_cc0, bit_cc1, bit_sign;
  	char temp_byte0, temp_byte1;
  	int temp_val;
  
  	CT1711_reset();
  	delay_ms(155);//150
  	bit_cc0 = CT1711_Read_Bit();
  	delay_us(10);
  	bit_cc1 = CT1711_Read_Bit();
  	delay_us(10);
	bit_cc0 = bit_cc0 & 0x01;
  	bit_cc1 = bit_cc1 & 0x01;
  	if((bit_cc0 == 0x00) && (bit_cc1 == 0x00)) {
    	bit_sign = CT1711_Read_Bit();
    	delay_us(10);
    	temp_byte0 = CT1711_Read_Byte();
    	delay_us(10);
    	temp_byte1 = CT1711_Read_Byte();
    	delay_us(10);
    	temp_val = (temp_byte0 << 8) + temp_byte1;
    	if(bit_sign == 0x01) {
      		temp_val = ~temp_val;
      		temp_val &= 0xffff;
      		temp_val++;
      		temp = (-3.90625 * temp_val / 1000.00);
    	} else
      		temp = ((3.90625 * (float)temp_val) / 1000.00);
    	return temp;
  	}
	return 0;
}



