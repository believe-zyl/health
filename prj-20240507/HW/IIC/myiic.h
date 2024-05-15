#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h"

   	   		   
//IO��������
#define SDA_IN()  {GPIOB->CRL &= 0X0FFFFFFF; GPIOB->CRL |= (u32)8<<28;}  //����������ģʽ
#define SDA_OUT() {GPIOB->CRL &= 0X0FFFFFFF; GPIOB->CRL |= (u32)3<<28;}  //ͨ������50MHz���ģʽ

//IO��������	 
#define IIC_SCL    PBout(6) //SCL    12
#define IIC_SDA    PBout(7) //SDA	 11
#define READ_SDA   PBin(7)  //����SDA 


//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�

void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
// u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Read_Byte(void);             //IIC��ȡһ���ֽ�

u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�


#endif

