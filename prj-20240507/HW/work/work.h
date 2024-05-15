#ifndef __WORK_H
#define __WORK_H		


#define MAX_BRIGHTNESS 255
#define START 100
#define DATA_LENGTH 500

#define EPISON 1e-7


void LCD_show_struc(void);
void show_real_time(void);
int show_health(int x);
float show_temperature(void);


#endif

