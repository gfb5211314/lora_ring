#include "key_hal.h"
#include "main.h"
//移植只需要更改宏定义GPIO端口和引脚
#define   key1_port    key1_GPIO_Port
#define   key1_pin       key1_Pin

#define   key2_port    key2_GPIO_Port
#define   key2_pin       key2_Pin

#define   key3_port    key3_GPIO_Port
#define   key3_pin       key3_Pin

#define   rang_key_port    rang_key_GPIO_Port
#define   rang_Pin       rang_key_Pin

#define   FANGCHAI_key_Port    FANGCHAI_GPIO_Port
#define   fangchai_key_pin       FANGCHAI_Pin
uint8_t key_state_value=0;
uint8_t init_key_flag=0;
 extern uint8_t  scan_key_flag;
  extern uint8_t  runing_state_flag;
	  extern uint8_t  rang_key_flag;
		  extern uint8_t  fangchai_flag;
			  extern uint8_t  sleep_flag;
  extern		uint8_t password_key;  //按键检查标志
	extern uint8_t password_key_value;
				uint8_t xingling_flag=0;
		uint8_t  reset_rang_flag=0;
		uint8_t  reset_rang_key=0;
//HAL接口层函数
//获得key1键值
uint8_t get_key1()
{
	return  HAL_GPIO_ReadPin(key1_port, key1_pin);
}
//获得key2键值
uint8_t get_key2()
{
	return  HAL_GPIO_ReadPin(key2_port, key2_Pin);
}
//获得key3键值
uint8_t get_key3()
{
	return  HAL_GPIO_ReadPin(key3_port, key3_Pin);
}

//获得rang键值
uint8_t get_rang_key()
{
		return  HAL_GPIO_ReadPin(rang_key_port, rang_Pin);
	
	
}

//扫描按键驱动层
uint8_t  scan_key()
{
	  
	  switch(key_state_value)
		{
       case 1 :  
               key_state_value=0;		 
              return 1;
			 break;
       case 2 :   key_state_value=0;	  
              return 2;
			 break;
			 case 3 :  key_state_value=0;	   
              return 3;
			 break;



		}			

}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
   
     HAL_Delay(10);/* 延时一小段时间，消除抖动 */
	if((scan_key_flag==1)||(runing_state_flag==1))
	{
     if(GPIO_Pin==key1_Pin)
    {
	   
    HAL_Delay(10);/* 延时一小段时间，消除抖动 */
    if(HAL_GPIO_ReadPin(key1_GPIO_Port,key1_Pin)==0)
    {
			    printf("1");
	        password_key=1;
          key_state_value=1;
	
		  
    }

    }
  else if(GPIO_Pin==key2_Pin)
  {
    HAL_Delay(10);/* 延时一小段时间，消除抖动 */
    if(HAL_GPIO_ReadPin(key2_GPIO_Port,key2_Pin)==0)
    {
//			   printf("2");
          password_key=1;
          key_state_value=2;
	
    }
    __HAL_GPIO_EXTI_CLEAR_IT(key2_Pin);
  }
	  else if(GPIO_Pin==key3_Pin)
  {
    HAL_Delay(10);/* 延时一小段时间，消除抖动 */
    if(HAL_GPIO_ReadPin(key3_GPIO_Port,key3_Pin)==0)
    {
        password_key=1;
          key_state_value=3;
	

    }
    __HAL_GPIO_EXTI_CLEAR_IT(key3_Pin);
  }
    if((password_key==1)&&(runing_state_flag==1)&&((rang_key_flag==1)||(fangchai_flag==1)))
			{
				        password_key_value =check_key_password();
           if(password_key_value!=0)
					 {			
        
//				printf("password_key_value=%d\r\n",password_key_value);					 
						   password_key=0;
					 }
					 else
					 {
						 key_state_value=0;
					 }
			}
			else
			{
				
				password_key_value=0;
			}
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
  }
		 if(GPIO_Pin==rang_key_Pin)
  {
    HAL_Delay(10);/* 延时一小段时间，消除抖动 */
    if(HAL_GPIO_ReadPin(rang_key_GPIO_Port,rang_key_Pin)==0)
    {
			//初始化功能按键，8.31增加代码
      if(reset_rang_flag==0)
			{
         reset_rang_key=1;
			}	
//			
			if(runing_state_flag==1)
			{
         rang_key_flag=1;
				key_state_value=0;
			}
			else
			{
				  rang_key_flag=0;
			}
    }
    __HAL_GPIO_EXTI_CLEAR_IT(rang_key_Pin);
  }
			  else if(GPIO_Pin==fangchai_key_pin)
  {
      HAL_Delay(10);/* 延时一小段时间，消除抖动 */
    if(HAL_GPIO_ReadPin(FANGCHAI_key_Port,fangchai_key_pin)==0)
    {
           	if(runing_state_flag==1)
			{
            fangchai_flag=1;
				   key_state_value=0;
			}
			else
			{
			  	fangchai_flag=0;
			}
    }
    __HAL_GPIO_EXTI_CLEAR_IT(fangchai_key_pin);
  }
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
	 if((sleep_flag==1)&&(runing_state_flag==1))
		{
			key_state_value=0;
			      sleep_init();
		     	sleep_flag=0;
//		     	init_key_flag=1;
//			printf("醒来\r\n");
		}

}
//