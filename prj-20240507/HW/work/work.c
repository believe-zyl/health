#include "work.h"
#include "delay.h"
#include "lcd.h"
#include "rtc.h"
#include "max30102.h"
#include "algorithm.h"
#include "ct1711.h"



const u8 *COMPILED_DATE=__DATE__;//获得编译日期
const u8 *COMPILED_TIME=__TIME__;//获得编译时间

const u8* Week[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


void LCD_show_struc(void)
{
	// POINT_COLOR=BLACK;//设置字体为黑色 
	LCD_ShowString(60, 50, "Health Monitoring", BLACK, WHITE, 16, 0);	
	LCD_ShowString(60, 70, "MH", BLACK, WHITE, 16, 0);	

	//显示时间
	// POINT_COLOR=RED;//设置字体为红色
	LCD_ShowString(62, 110, "    -  -", RED, WHITE, 16, 0);
	LCD_ShowString(60, 160, "  :  :  ", RED, WHITE, 16, 0);

	//显示心率和血氧
	// POINT_COLOR=BLUE;//设置字体为蓝色
	LCD_ShowString(60, 180, "HR=   ", BLUE, WHITE, 16, 0);
	LCD_ShowString(60, 200, "SpO2=   ", BLUE, WHITE, 16, 0);

	//显示温度
	// POINT_COLOR=BLUE;//设置字体为蓝色
	LCD_ShowString(60, 220, "T=    .  C", BLUE, WHITE, 16, 0);
}


void show_real_time(void)
{
	// u8 t=0;
	// if(t != timer.sec) {
	// 	t = timer.sec;
		LCD_ShowIntNum(52, 110, timer.w_year, 5, BLACK, WHITE, 16);									  
		LCD_ShowIntNum(105, 110, timer.w_month, 2, BLACK, WHITE, 16);									  
		LCD_ShowIntNum(135, 110, timer.w_date, 2, BLACK, WHITE, 16);
		switch(timer.week) {
			case 0:
				LCD_ShowString(60, 130, "Sunday   ", BLACK, WHITE, 16, 0);
				break;
			case 1:
				LCD_ShowString(60, 130, "Monday   ", BLACK, WHITE, 16, 0);
				break;
			case 2:
				LCD_ShowString(60, 130, "Tuesday  ", BLACK, WHITE, 16, 0);
				break;
			case 3:
				LCD_ShowString(60, 130, "Wednesday", BLACK, WHITE, 16, 0);
				break;
			case 4:
				LCD_ShowString(60, 130, "Thursday ", BLACK, WHITE, 16, 0);
				break;
			case 5:
				LCD_ShowString(60, 130, "Friday   ", BLACK, WHITE, 16, 0);
				break;
			case 6:
				LCD_ShowString(60, 130, "Saturday ", BLACK, WHITE, 16, 0);
				break;  
		}
		LCD_ShowIntNum(60, 160, timer.hour, 2, BLACK, WHITE, 16);									  
		LCD_ShowIntNum(84, 160, timer.min, 2, BLACK, WHITE, 16);									  
		LCD_ShowIntNum(108, 160, timer.sec, 2, BLACK, WHITE, 16);
	// }	
	delay_ms(10);
}


uint32_t aun_ir_buffer[DATA_LENGTH];	//IR LED sensor data
int32_t n_ir_buffer_length;				//data length
uint32_t aun_red_buffer[DATA_LENGTH];	//Red LED sensor data
int32_t n_sp02;			//SPO2 value
int8_t ch_spo2_valid;   //indicator to show if the SP02 calculation is valid
int32_t n_heart_rate;   //heart rate value
int8_t  ch_hr_valid;    //indicator to show if the heart rate calculation is valid

int j=0, k=0, a=0, b=0;
int HR_BUF = 0;
int SP_BUF = 0;

int show_health(int x)
{
	uint32_t un_min, un_max, un_prev_data;  //variables to calculate the on-board LED brightness that reflects the heartbeats
	int i;
	int32_t n_brightness;
	float f_temp;  

	n_ir_buffer_length = DATA_LENGTH; //buffer length of 100 stores 5 seconds of samples running at 100sps

	i=0;
	un_min=0x3FFFF;
	un_max=0;

	// dumping the first 100 sets of samples in the memory and shift the last 400 sets of samples to the top
	for(i=START; i<DATA_LENGTH; i++) {
		aun_red_buffer[i-START] = aun_red_buffer[i];
		aun_ir_buffer[i-START] = aun_ir_buffer[i];
				
		//update the signal min and max
		if(un_min > aun_red_buffer[i])
			un_min = aun_red_buffer[i];
		if(un_max < aun_red_buffer[i])
			un_max = aun_red_buffer[i];
	}
	//take 100 sets of samples before calculating the heart rate.
	for(i=400; i<DATA_LENGTH; i++) {
		un_prev_data = aun_red_buffer[i-1];
		// while(PAin(5)==1);
		max30102_read_fifo((aun_red_buffer+i), (aun_ir_buffer+i));
		
		if(aun_red_buffer[i] > un_prev_data)//just to determine the brightness of LED according to the deviation of adjacent two AD data
		{
			f_temp = aun_red_buffer[i] - un_prev_data;
			f_temp /= (un_max-un_min);
			f_temp *= MAX_BRIGHTNESS;
			n_brightness -= (int)f_temp;
			if(n_brightness < 0)
				n_brightness = 0;
		} else {
			f_temp = un_prev_data - aun_red_buffer[i];
			f_temp /= (un_max-un_min);
			f_temp *= MAX_BRIGHTNESS;
			n_brightness += (int)f_temp;
			if(n_brightness > MAX_BRIGHTNESS)
				n_brightness = MAX_BRIGHTNESS;
		}
	}
	maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid); 

	switch(x) {
		case 1 :
			if(ch_hr_valid) {
				LCD_ShowString(88, 180, "          ", BLACK, WHITE, 16, 0);
				LCD_ShowIntNum(88, 180, n_heart_rate, 3, BLACK, WHITE, 16);

				a = a + n_heart_rate;
				j++;
				HR_BUF = a / j;
				if(j == 10) {
					j = 0;
					a = 0;
				}
				// return HR_BUF;
			} else {
				LCD_ShowString(88, 180, "Wait...", BLACK, WHITE, 16, 0);
				// return HR_BUF;
			}
			return HR_BUF;

		case 2 :
			if(ch_spo2_valid) {
				LCD_ShowString(105, 200, "          ", BLACK, WHITE, 16, 0);
				LCD_ShowIntNum(105, 200, n_sp02, 3, BLACK, WHITE, 16);

				b = b + n_sp02;
				k++;
				SP_BUF = b / k;
				if(k == 10) {
					k = 0;
					b = 0;
				}
				// return n_sp02;
			} else {
				LCD_ShowString(105, 200, "Wait...", BLACK, WHITE, 16, 0);
				// return 0;
			}
			return SP_BUF;
	}
	return 0;


	// switch(x) {
	// 	case 1 :
	// 		if(ch_hr_valid) {
	// 			LCD_ShowString(88, 180, "          ");
	// 			LCD_ShowNum(88, 180, n_heart_rate, 3, 16);
	// 			return n_heart_rate;
	// 		} else {
	// 			LCD_ShowString(88, 180, "Wait...");
	// 			return 0;
	// 		}

	// 	case 2 :
	// 		if(ch_spo2_valid) {
	// 			LCD_ShowString(105, 200, "          ");
	// 			LCD_ShowNum(105, 200, n_sp02, 3, 16);
	// 			return n_sp02;
	// 		} else {
	// 			LCD_ShowString(105, 200, "Wait...");
	// 			return 0;
	// 		}
	// }
	// return 0;
}


float show_temperature(void)
{
	float t = 0.00;
	u32 s = 0;
	float r = 0.00;

	t = CT1711_Read_Temp_Degree();

	s = t;
	r = t - s;
	r = r * 100;
	r = r / 10;

	if(t <= -EPISON) {
		LCD_ShowString(80, 220, "-", BLACK, WHITE, 16, 0);
		LCD_ShowIntNum(90, 220, t, 2, BLACK, WHITE, 16);
		LCD_ShowIntNum(120, 220, r, 1, BLACK, WHITE, 16);
	} else {
		LCD_ShowIntNum(90, 220, t, 2, BLACK, WHITE, 16);
		LCD_ShowIntNum(120, 220, r, 1, BLACK, WHITE, 16);
	}
	return t;
}

