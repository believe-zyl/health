#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h"

   	   		   
//IO方向设置
#define SDA_IN()  {GPIOB->CRL &= 0X0FFFFFFF; GPIOB->CRL |= (u32)8<<28;}  //上下拉输入模式
#define SDA_OUT() {GPIOB->CRL &= 0X0FFFFFFF; GPIOB->CRL |= (u32)3<<28;}  //通用推挽50MHz输出模式

//IO操作函数	 
#define IIC_SCL    PBout(6) //SCL    12
#define IIC_SDA    PBout(7) //SDA	 11
#define READ_SDA   PBin(7)  //输入SDA 


//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口				 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号

void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
// u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_Read_Byte(void);             //IIC读取一个字节

u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号


#endif


