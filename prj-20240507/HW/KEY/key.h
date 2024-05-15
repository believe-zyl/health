#ifndef __KEY_H
#define __KEY_H	 

#include "sys.h" 


#define KEY PAin(11)    //PA11

	 
void KEY_Init(void);    //IO初始化
u8   KEY_Scan(void);    //按键扫描函数					    
#endif
