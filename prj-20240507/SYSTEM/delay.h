/*
 * @Author         : Belief
 * @Date           : 2022-08-02 10:57:10
 * @LastEditTime   : 2022-08-03 10:03:13
 * @FilePath       : \PRJf:\codefield\CODE_C\C_Work\prj\SYSTEM\delay.h
 * Copyright       : GPL
 */

#ifndef __DELAY_H
#define __DELAY_H 		

#include "stm32f10x.h"


/**
 * @description: 
 * @param      :{u8} SYSCLK
 * @return     :{*}
 * @note      : SYSCLK为系统时钟
 */
void delay_init(u8 SYSCLK);



/**
 * @description: 延迟nms函数
 * @param      :{u16} nms
 * @return     :{*}
 * @note      : 注意nms的范围
                SysTick->LOAD为24位寄存器,所以最大延时为:
                nms<=0xffffff*8*1000/SYSCLK
                SYSCLK单位为Hz,nms单位为ms
                对72M条件下,nms<=1864 
 */
void delay_ms(u16 nms);



/**
 * @description: 延迟nus函数
 * @param      :{u32} nus
 * @return     :{*}
 * @note      : 
 */
void delay_us(u32 nus);


#endif



