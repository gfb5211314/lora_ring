#include "adc_bat.h"
#include "rtc.h"
//HAL_ADCEx_Calibration_Start(&hadcx);
#define  num_vol_bat   3.75
float  adc_value=0;
float   adc_vol=0;
uint8_t adc_temp=0;
uint8_t rtc_flag=0;
uint8_t vol_conv_flag=0;
extern uint8_t sleep_flag;
void adc_start()
{
	
	HAL_ADC_Start_IT(&hadc);
	
	
}
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
	//处于睡眠状态，由RTC唤醒的
	
		    if(sleep_flag==1)
				{
					sleep_flag=0;
					sleep_init();
//					adc_start();		
          adc_start();						
				}
	
}

//void chesfff()
//{
//	  adc_value=HAL_ADC_GetValue(&hadc);
//	  adc_vol=adc_value/4096*3.3*2.85/num_vol_bat*100;
//    	adc_temp=adc_vol;
//	 printf("当前电压=%d\r\n",adc_temp);
//	   vol_conv_flag=1;
//		adc_start();
//}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	  adc_value=HAL_ADC_GetValue(hadc);
	  adc_vol=adc_value/4096*3.3*2.85/num_vol_bat*100;
    	adc_temp=adc_vol;
	 printf("当前电压=%d\r\n",adc_temp);
	   vol_conv_flag=1;
//		adc_start();
}
