#include "sys.h"
#include "usart.h"
// #include "ct1711.h"
#include "work.h"
#include "stdio.h"


//�������´���,֧��printf����,������Ҫѡ��use MicroLIB
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*ʹ��microLib�ķ���*/
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
 
u8 USART_RX_BUF[64];     //���ջ���,���64���ֽ�.
//����״̬
//bit7��������ɱ�־
//bit6�����յ�0x0d
//bit5~0�����յ�����Ч�ֽ���Ŀ
u8 USART_RX_STA=0;       //����״̬���

void uart_init(u32 bound){
    //GPIO�˿�����
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

   //Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART1
  
   //USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);
   
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
   
    USART_Cmd(USART1, ENABLE);		//ʹ�ܴ��� 
}


//����1�жϷ������
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


	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {	//�����ж�
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		Res = USART_ReceiveData(USART1);					//��ȡ���յ�������

		switch(Res) {

			case 0X0031 :
				// USART_SendString(USART1, "\n Temp = ");
				// t = CT1711_Read_Temp_Degree();
				sprintf(strings, "Temp = %6.1f ��\n", t);
				USART_SendString(USART1, strings);

				// USART_SendData(USART1, t);
				// while(USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET);	//�жϷ������
				break;

			case 0X0032 :
				if(HR) {
					sprintf(strings, "HR = %6d\n", HR);
					USART_SendString(USART1, strings);
					// while(USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET);	//�жϷ������
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
					// while(USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET);	//�жϷ������
				} else {
					USART_SendString(USART1, "SpO2 = Wait...\n");
				}
				break;

			// default :
			// 	USART_SendData(USART1, 6);
			// 	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET);	//�жϷ������
		}
    }
}


void USART_SendString(USART_TypeDef* USARTx, char *DataString)
{
	int i = 0;
	USART_ClearFlag(USARTx, USART_FLAG_TC);						//�����ַ�ǰ��ձ�־λ������ȱʧ�ַ����ĵ�һ���ַ���
	while(DataString[i] != '\0')								//�ַ���������
	{
		USART_SendData(USARTx, DataString[i]);					//ÿ�η����ַ�����һ���ַ�
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == 0);	//�ȴ����ݷ��ͳɹ�
		USART_ClearFlag(USARTx, USART_FLAG_TC);					//�����ַ�����ձ�־λ
		i++;
	}
}

