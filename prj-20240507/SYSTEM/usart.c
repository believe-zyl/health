#include "sys.h"
#include "usart.h"
// #include "ct1711.h"
#include "work.h"
#include "stdio.h"


//加入以下代码,支持printf函数,而不需要选择use MicroLIB
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*使用microLib的方法*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
u8 USART_RX_BUF[64];     //接收缓冲,最大64个字节.
//接收状态
//bit7，接收完成标志
//bit6，接收到0x0d
//bit5~0，接收到的有效字节数目
u8 USART_RX_STA=0;       //接收状态标记

void uart_init(u32 bound){
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
     //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

   //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART1
  
   //USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);
   
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
   
    USART_Cmd(USART1, ENABLE);		//使能串口 
}


//串口1中断服务程序
void USART1_IRQHandler(void)	
{
	u16 Res;
	float t = 0.00;
	u8 a = 1;
	u8 b = 2;
	char strings[10];

	int HR, SP;

	t = show_temperature();

	HR = show_health(a);
	SP = show_health(b);


	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {	//接收中断
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		Res = USART_ReceiveData(USART1);					//读取接收到的数据

		switch(Res) {

			case 0X0031 :
				// USART_SendString(USART1, "\n Temp = ");
				// t = CT1711_Read_Temp_Degree();
				sprintf(strings, "Temp = %6.1f ℃\n", t);
				USART_SendString(USART1, strings);

				// USART_SendData(USART1, t);
				// while(USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET);	//判断发送完成
				break;

			case 0X0032 :
				if(HR) {
					sprintf(strings, "HR = %6d\n", HR);
					USART_SendString(USART1, strings);
					// while(USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET);	//判断发送完成
				} else {
					USART_SendString(USART1, "HR = Wait...\n");
				}
				break;

			case 0X0033 :
				if(SP) {
					// USART_SendString(USART1, "\n Spo2 = ");
					// USART_SendData(USART1, b);
					sprintf(strings, "SpO2 = %6d\n", SP);
					USART_SendString(USART1, strings);
					// while(USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET);	//判断发送完成
				} else {
					USART_SendString(USART1, "SpO2 = Wait...\n");
				}
				break;

			// default :
			// 	USART_SendData(USART1, 6);
			// 	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET);	//判断发送完成
		}
    }
}


void USART_SendString(USART_TypeDef* USARTx, char *DataString)
{
	int i = 0;
	USART_ClearFlag(USARTx, USART_FLAG_TC);						//发送字符前清空标志位（否则缺失字符串的第一个字符）
	while(DataString[i] != '\0')								//字符串结束符
	{
		USART_SendData(USARTx, DataString[i]);					//每次发送字符串的一个字符
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == 0);	//等待数据发送成功
		USART_ClearFlag(USARTx, USART_FLAG_TC);					//发送字符后清空标志位
		i++;
	}
}

