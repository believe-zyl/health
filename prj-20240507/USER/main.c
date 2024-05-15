#include "delay.h"
#include "sys.h"
#include "rtc.h"
#include "lcd.h"
#include "lcd_init.h"
#include "work.h"
#include "myiic.h"
#include "max30102.h"
#include "algorithm.h"
#include "ct1711.h"
#include "usart.h"
#include "key.h"



int main(void)
{
	u8 uch_dummy;

	SystemInit();
	NVIC_Configuration();
	delay_init(36);

 	IIC_Init();
	LCD_Init();
	LCD_Fill(0, 0, 240, 300, WHITE);	//清屏填充背景色
	CT1711_Init();

	LCD_show_struc();
	RTC_Init();

	max30102_reset();	//resets the MAX30102
	max30102_read_reg(0, &uch_dummy);	//read and clear status register
	max30102_init();	//initializes the MAX30102

	uart_init(9600);	//串口初始化为9600

	KEY_Init();

	while(1)
	{
		show_health(1);
		show_health(2);
		show_temperature();

		// GPIO_SetBits(GPIOB, GPIO_Pin_12);
		// delay_ms(200);
		// GPIO_ResetBits(GPIOB, GPIO_Pin_12);
		// delay_ms(200);
	}
}


