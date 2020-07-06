#include "main_app.h"
#include "SX127X_Driver.h"
#include "SX127X_Hal.h"
#include "stdio.h"
#include "bsp_eeprom.h"
#include "string.h"
#include "key_hal.h"
#include "speak_hal.h"
//业务层
#define join_network   0x01    //入网请求
#define join_network_state  0x02 //入网状态发送
#define Power_report    0x03    //设备电池电量上报
#define Equipment_tamper    0x04   //设备防拆上报
#define device_number     0x05     //设备号上报
#define xiaojing_pass_num      0x06     //消警密码上报
#define user_password_fun     0x07    //用户密码上报
#define dev_run_state     0x08    //正常状态上报
#define set_xiaojing_password  0x09   //设备消警密码
#define set_userpassword         0x10  //后台设置用户密码
#define rang_fun              0x12 //设备报警上报
#define xiaojing_num          0x13   //消警指令下发
uint32_t Fre[5] = {470800000, 494600000, 510000000, 868000000, 915000000};   //收发频率
uint8_t communication_states; //业务状态
uint8_t TXbuffer[50] = {0};
uint8_t RXbuffer[50] = {0};
uint16_t T_Cnt = 0;
uint8_t factory_parameter_flag=0;    //出厂是否设置
uint8_t user_password[10]={1,2,3,1,2,3};        //用户密码
uint8_t user_temp_password[10]={0};        //用户密码
uint8_t xiaojing_password[10]={1,2,3,2,2,2};    //消警密码
uint8_t dev_num[2]={0};     //设备号
uint8_t dev_runing_flag=0;  //设备状态
uint8_t pack_len = 0;
uint8_t lora_data_state=0;
uint8_t scan_key_flag=0;
uint8_t password_key=0;  //按键检查标志
uint8_t sleep_flag=0;
extern uint8_t vol_conv_flag;
extern uint8_t adc_temp;
extern uint8_t password_key_value;
void set_txrx_datalen(uint8_t datalen);
void sleep_open(void);
extern uint8_t key_state_value;
 uint8_t  runing_state_flag=0;
	 uint8_t  rang_key_flag=0;
		uint8_t  fangchai_flag=0;
uint8_t pin_pack(uint8_t *dev,uint8_t dev_type,uint8_t function_num,uint8_t *data_pack);
 uint8_t  check_key_password();
enum DemoInternalStates
{
    APP_RNG = 0, // nothing to do (or wait a radio interrupt)
    RX_DONE,
    TX_DONE,
    TX_ING,
    APP_IDLE,
};
//LORA 初始化配置
void app_lora_config_init()
{
	 uint8_t astate=0;
    G_LoRaConfig.LoRa_Freq = Fre[0];   //中心频率470MHz
    G_LoRaConfig.BandWidth = BW125KHZ;    //BW = 125KHz  BW125KHZ
    G_LoRaConfig.SpreadingFactor = SF09;  //SF = 9
    G_LoRaConfig.CodingRate = CR_4_6;     //CR = 4/6
    G_LoRaConfig.PowerCfig = 15;          //19±1dBm
    G_LoRaConfig.MaxPowerOn = true;       //最大功率开启
    G_LoRaConfig.CRCON = true;            //CRC校验开启?
    G_LoRaConfig.ExplicitHeaderOn = true; //Header开启
    G_LoRaConfig.PayloadLength = 10;      //数据包长度
     astate= SX127X_Lora_init();
	   printf("astate=%d\r\n",astate);
    if(astate!= NORMAL)	 //无线模块初始化
    {
        while(1)
        {
           printf("lora init fail\r\n");
					HAL_Delay(500);
        }
    }
		
		 communication_states=APP_IDLE;
} 
 uint8_t flag=0;
 uint8_t rang_state=0;
 uint8_t fagchai_state=0;
//检测电池电压
void check_vol_task()
{
	    if(vol_conv_flag==1)
			{
			        SX127X_StandbyMode();   //待机模式   
		           
		     	pack_len=pin_pack(dev_num,0x01,0x03,&adc_temp); //
			    printf("pack_len=%d\r\n",pack_len);
			    set_txrx_datalen(pack_len);//数据包长度
           SX127X_TxPacket(TXbuffer); 
				   vol_conv_flag=0;
				   HAL_Delay(1000);
			  	  HAL_Delay(1000);
				   sleep_open(); 
				
			}
	
	
}
void rang_runing()
{
	  printf("rang_state=%d",rang_state);
	 switch(rang_state)
	 {
		 case 0 :
			       SX127X_StandbyMode();   //待机模式   
		           	lora_data_state=1;
		     	pack_len=pin_pack(dev_num,0x01,0x12,&lora_data_state); //
			    printf("pack_len=%d\r\n",pack_len);
			    set_txrx_datalen(pack_len);//数据包长度
           SX127X_TxPacket(TXbuffer); 
   		         rang_state=1;
			         
		                 break;
		 case 1 :    
		       	          printf("一键启动报警\r\n");					
					              Line_1A_WT588S(10);//
				               HAL_Delay(1000);  //延迟播放音乐
										   HAL_Delay(1000);
		                      rang_state=2;
		                 break;
		 case  2 :    
				
		                   HAL_GPIO_TogglePin( led_en_GPIO_Port,led_en_Pin);
                       Line_1A_WT588S(16);//			      
				               HAL_Delay(500);  //延迟播放音乐
		
    			  
		        break;			      
	 }	
}
void fangchai_runing()
{
	
	 switch(fagchai_state)
	 {
		 case 0 :
			         printf("防拆启动报警\r\n");
					            Line_1A_WT588S(12);//报警成功
				               HAL_Delay(1000);  //延迟播放音乐
										   HAL_Delay(1000);
//		                   if(play_music_com(9)==1)
//											 {
//		                 fagchai_state=2;
//											 }
		                 break;
		 case 1 :    
		               printf("防拆声音\r\n");
		                   HAL_GPIO_TogglePin( led_en_GPIO_Port,led_en_Pin);      
					            Line_1A_WT588S(16);//报警成功				      
				               HAL_Delay(500);  //延迟播放音乐
//										   HAL_Delay(1000);
//		                if(play_music_com(0)==1)
//											 {
////		                 fagchai_state=2;
//											 }
		                 break;
		 case  2 :      SX127X_StandbyMode();   //待机模式   
		         	lora_data_state=0;
		     	pack_len=pin_pack(dev_num,0x01,0x04,&lora_data_state); //
			    printf("pack_len=%d\r\n",pack_len);
			    set_txrx_datalen(pack_len);//
            SX127X_TxPacket(TXbuffer); 
   		         fagchai_state=1;
		        break;			      
	 }	
}
void check_rung_state()
{
      	if(rang_key_flag==1)
				 {
					 rang_runing();
   									            
				 }
				  if(fangchai_flag==1)
					{
						  fangchai_runing();
									
					}
					//检查按键输入是否正确
	      if(password_key_value!=0)
				{
					if(password_key_value==1)
					{
						    Line_1A_WT588S(13);//
						  HAL_Delay(1000);  //延迟播放音乐
						password_key_value=0;
						printf("12345");
						HAL_NVIC_SystemReset();
						  rang_key_flag=0;   						
					}
					//密码消警
					if(password_key_value==2)
					{
						password_key_value=0;
						printf("8888");
							    SX127X_StandbyMode();   //待机模式   	
                      lora_data_state=1;								 
			    pack_len=pin_pack(dev_num,0x01,0x13,&lora_data_state); 
			       set_txrx_datalen(pack_len);//数据包长度
                 SX127X_TxPacket(TXbuffer);  
								       Line_1A_WT588S(11);//消警成功	
								 HAL_GPIO_WritePin(led_en_GPIO_Port, led_en_Pin, GPIO_PIN_SET); 
								 
				               HAL_Delay(1000);  //延迟播放音乐
//										   HAL_Delay(1000);
									          rang_key_flag=0;
								             rang_state=0;
					         	 rang_key_flag=0;  
                     fangchai_flag=0; 	
						
							           	 sleep_open();
						 
  						
					}
				}
}
	
void lora_process()
{
//	 printf("communication_states=%d\r\n",communication_states);
	  switch(communication_states)
        {
        case APP_IDLE:
          if(DIO0_GetState() == GPIO_PIN_SET)
         {      
        SX127X_Read(REG_LR_IRQFLAGS, &flag);
        SX127X_Write(REG_LR_IRQFLAGS, 0xff); //clear flags
        if(flag & RFLR_IRQFLAGS_TXDONE)
        {
            communication_states = TX_DONE;
					  printf("send sucess\r\n");
        }
        else if(flag & RFLR_IRQFLAGS_RXDONE)
        {
            communication_states = RX_DONE;
					printf("rx sucess\r\n");
        }
       }      
      
            break;
     
        case TX_DONE:

            SX127X_StandbyMode();   //待机模式

            SX127X_StartRx();
   communication_states = APP_IDLE;
            break;
					
				case RX_DONE :
					 SX127X_Read(REG_LR_NBRXBYTES, &G_LoRaConfig.PayloadLength); //获取数据长度
					 set_txrx_datalen(G_LoRaConfig.PayloadLength);
				   SX127X_RxPacket(RXbuffer);		
			   //   SX127X_SleepMode(); //睡眠模式
		       SX127X_StandbyMode();  //切换状态清空FIFO，要不收到250个字节，会出错				
				 for(uint8_t i=0;i<G_LoRaConfig.PayloadLength;i++)
			 	 {
					printf("RXbuffer[%d]=%02x\r\n",i,RXbuffer[i]);
									
				 }
				 printf("当前设备地址:%d-%d\r\n",dev_num[0],dev_num[1]);
				 //接收数据处理
				   if(RXbuffer[0]==0x5a&&RXbuffer[1]==0xa5&&RXbuffer[2]==dev_num[0]&&RXbuffer[3]==dev_num[1]&&RXbuffer[4]==0x01)
					 {
						   switch(RXbuffer[5])
							 {
								    //后台接收消警指令
								 case  0x12 : 
									           //第一步
								          //关闭报警声音和LED闪烁
								          //2 播放消警成功
								          //正常运行
								   printf("后台收到报警指令\r\n");
								       
							            rang_key_flag=0;
								           rang_state=0;
								 break;
						
								         
								       //后台接收防拆报警指令
								 case  0x04 : 
									         HAL_GPIO_WritePin(led_en_GPIO_Port, led_en_Pin, GPIO_PIN_SET);    
//								   printf("后台收到防拆指令报警\r\n");
							            fangchai_flag=0;
								 fagchai_state=0;
								 break;
					
								 //后台设置消警密码
								   //接收消警指令
								 case  0x13 : 
									           //第一步
								          //关闭报警声音和LED闪烁
								         //2 播放消警成功
								          //正常运行
								    SX127X_StandbyMode();   //待机模式   	
                      lora_data_state=1;								 
			    pack_len=pin_pack(dev_num,0x01,0x13,&lora_data_state); 
			       set_txrx_datalen(pack_len);//数据包长度
                 SX127X_TxPacket(TXbuffer);  
								       Line_1A_WT588S(11);//消警成功	
								 HAL_GPIO_WritePin(led_en_GPIO_Port, led_en_Pin, GPIO_PIN_SET); 
								 
				               HAL_Delay(1000);  //延迟播放音乐
										   HAL_Delay(1000);
									          rang_key_flag=0;
								             rang_state=0;
								           password_key=0;
							           	 sleep_open();
								 break;
								 //后台设置消警密码
								 case  0x09 :
									 // 第一步  解包
								     for(uint8_t i=0;i<6;i++)
								 {
									  xiaojing_password[i]=RXbuffer[i+6];
								 }
								 EEPROM_WriteBytes(xiaojing_password, 20,6);  //把用户密码写进去
								 break;
							 }
						 
						 
					 }
					 else
					 {
						 //qingling
						 
					 }
					    SX127X_StartRx();
					 	 communication_states = APP_IDLE;
					break;
				
			}
}
uint8_t chuchang_lora_process()
{
	      uint8_t  value=0;
	     static uint8_t lora_com=APP_IDLE;
	  switch(lora_com)
        {
        case APP_IDLE:
         if(DIO0_GetState() == GPIO_PIN_SET)
       {
        uint8_t flag=0;
        SX127X_Read(REG_LR_IRQFLAGS, &flag);
        SX127X_Write(REG_LR_IRQFLAGS, 0xff); //clear flags
        if(flag & RFLR_IRQFLAGS_TXDONE)
        {
            lora_com = TX_DONE;
//					  printf("send sucess\r\n");
        }
        else if(flag & RFLR_IRQFLAGS_RXDONE)
        {
				
            lora_com = RX_DONE;
//					printf("rx sucess\r\n");
        }
        }     
            break;

//        case TX_ING:			  
//            SX127X_TxPacket(TXbuffer);
//            communication_states = APP_IDLE;
//            break;
     
        case TX_DONE:
					SX127X_StandbyMode();   //待机模式
            SX127X_StartRx();
				     lora_com = APP_IDLE;
            break;				
				case RX_DONE :
					 SX127X_Read(REG_LR_NBRXBYTES, &G_LoRaConfig.PayloadLength); //获取数据长度
//					 set_txrx_datalen(G_LoRaConfig.PayloadLength);
				   SX127X_RxPacket(RXbuffer);		
				     SX127X_StandbyMode();  //切换状态清空FIFO，要不收到250个字节，会出错
			   //   SX127X_SleepMode(); //睡眠模式		     		
//				 for(uint8_t i=0;i<G_LoRaConfig.PayloadLength;i++)
//			 	 {
//					printf("RXbuffer[%d]=%02x\r\n",i,RXbuffer[i]);
//									
//				 }
				 //接收lora数据处理
				   if(RXbuffer[0]==0x5a&&RXbuffer[1]==0xa5&&RXbuffer[4]==0x01)
					 {
						   switch(RXbuffer[5])
							 {
							 
								 //接入后台分配的设备号
								  case  0x01 : 
										//拿出设备号
										       for(uint8_t i=0;i<2;i++)
									{
										dev_num[i]=RXbuffer[6+i];
										
									}
				      	EEPROM_WriteBytes(dev_num, 30,2);  //把设备号写进去     
			    	   for(uint8_t i=0;i<G_LoRaConfig.PayloadLength;i++)
				       {
					
					       RXbuffer[i]=0;
					
				       }
							       value=1;
									break;
							 //接收网关入网信息
							  case  0x02 : 
  
//                 printf("设备入网成功\r\n");								
			    	   for(uint8_t i=0;i<G_LoRaConfig.PayloadLength;i++)
				       {
					
					       RXbuffer[i]=0;
					
				       }
							    value=2;
									break;
							 //接收密码应答
							  case 	0x07 :
//									  printf("后台收到用户密码\r\n");								
			    	   for(uint8_t i=0;i<G_LoRaConfig.PayloadLength;i++)
				       {
					
					       RXbuffer[i]=0;
					
				       }
                 value=3;
                   break;	
                 	 //接收消警密码应答
							  case 	0x06 :
//									  printf("后台收到消警密码上报\r\n");								
			    	   for(uint8_t i=0;i<G_LoRaConfig.PayloadLength;i++)
				       {
					
					       RXbuffer[i]=0;
					
				       }
                        value=4;
                   break;	 							 
							
						 
					 }
				 }
					   SX127X_StartRx();
					  lora_com = APP_IDLE;
				 
				    break;
			}
					 return value;
}
//检查出厂参数
uint8_t check_factory_parameter()
{
	       uint8_t   value=0;
	       if(EEPROM_ReadBytes(&factory_parameter_flag, 1, 1))
				 {
					   printf("factory_parameter_flag=%d",factory_parameter_flag);
					           if(factory_parameter_flag ==0xaa)
										 {
											     
											 value=1;
										 }
										 else
										 {
											 
											value=0;
										 }
				 }
				 else
				 {
					 
//					   printf("read data fail\r\n");
				 }
				 return value;
}
void set_txrx_datalen(uint8_t datalen)
{
  G_LoRaConfig.PayloadLength = datalen;      //数据包长度
}
//组包
uint8_t pin_pack(uint8_t *dev,uint8_t dev_type,uint8_t function_num,uint8_t *data_pack)
{
	  uint8_t index=0;
	  TXbuffer[index++]=0x5a;
	  TXbuffer[index++]=0xa5;  //厂家识别码
	  TXbuffer[index++]=dev[0];  //设备码
	  TXbuffer[index++]=dev[1];  //设备码
	  TXbuffer[index++]=dev_type;  //设备类型
  	TXbuffer[index++]=function_num; //功能码
	   for(uint8_t i=0;i<strlen((const char *)data_pack);i++)
	{
	  TXbuffer[index++]=data_pack[i]; //功能码
	}
	  return index;
}
//用户密码按键设置
uint8_t key_password_set(uint8_t *password)
{
	       uint8_t value=0;
                uint8_t key=0 ;
	     static	   uint8_t key_count=0 ; //按键计数
     	 static	   uint8_t key_value=0 ;//按键的值
	             key = scan_key();
	           switch(key)
			{
				case 1:
				   
    			       	Line_1A_WT588S(1);
				            key_count++;
				            key_value=1;
				 
				break;
				case 2:     
					          key_count++;Line_1A_WT588S(1);
                        key_value=2;
				break;
				case 3: 
				                	key_value=3;
					         key_count++;Line_1A_WT588S(1);
				break;
			}
			if(key_count<7)
			{
	        password[key_count-1]=key_value;
				    
			}
		  if(key_count==6)
			{
			
	
//	  
//	     EEPROM_ReadBytes(userpass, 10, 6);
//				for(uint8_t i=0;i<6;i++)
//				{
//      printf("%d",password[i]);
//				}
				value=1;
				  key_count=0;
				  key_value=0;
	

			 } 
return value ;			
 }
//校验密码
 uint8_t  check_key_password()
  {
	              uint8_t  value=0;
//	           scan_key_flag=1;
					if(key_password_set(user_temp_password)==1)
					  {
//							  scan_key_flag=0;
							     HAL_Delay(500); //播放语音
						      				 uint8_t b=0;
							             uint8_t b1=0;

				         for(uint8_t i=0;i<6;i++)
				       {
					          if(user_temp_password[i]==user_password[i])
										{
											        b++ ;
										}				       
		           }
							  for(uint8_t i=0;i<6;i++)
				       {
					          if(user_temp_password[i]==xiaojing_password[i])
										{
											        b1++ ;
										}
		           }
//				printf("b=%d",b);
//									printf("b1=%d",b1);	
				      if((b==6)||(b1==6))
					    {				
                    //撤防成功						
                if(b==6)
								{
										Line_1A_WT588S(9);//密码正确
				               HAL_Delay(1000);
//										   HAL_Delay(1000);	
									  	b=0;
							     	value=1;
								}
								//消警成功
								 else if(b1==6)
								 {
									 printf("xiaojingcheng");
									 	Line_1A_WT588S(9);//密码正确
				               HAL_Delay(1000);
//										   HAL_Delay(1000);	
									  	b1=0;
							     	value=2;
									 
								 }
					      
				      }
			        else
			       {

									   Line_1A_WT588S(8);//密码错误
				               HAL_Delay(1000);
							        HAL_Delay(1000);
//										   HAL_Delay(1000);	
									  	b=0;
                      b1=0;	
                     value=0;							 
			        }
						
						
					  }
	  return  value;
 }
//出厂设置
uint8_t  factory_parameter_set()
{
	  uint8_t value=0;
	  static  uint8_t factory_num=0;
	  switch(factory_num)
		 {
			// 检查是否有
			case  0 :    
     //已经设定用户密码
		  	if(check_factory_parameter()==1)
			 {
				    factory_num=13;	
				 EEPROM_ReadBytes(user_password, 10, 6);
				 EEPROM_ReadBytes(xiaojing_password, 20, 6);
         printf("用户密码已经设置\r\n");		           				 
			 }
			 //跳进设置用户密码
      else
			{ 
				  printf("进行用户密码设置\r\n");	
	     factory_num=2;	
			}				
			 break;
			 //把用户密码发送上去
			case 1 :
			break;
						//播放用户密码音乐
			case 2 : 
				              Line_1A_WT588S(2);//播放设定密码声音
				               HAL_Delay(1000);  //延迟播放音乐
										   HAL_Delay(1000);
		              	key_state_value=0;
			               	factory_num=3;		
			
			 break;
			     //用户密码按键输入
			case 3 :  
				            scan_key_flag=1;
				    if( key_password_set(user_password)==1)
						{
							scan_key_flag=0;
							 for(uint8_t i=0;i<6;i++)
							{
							  printf("user_password[%d]=%d",i,user_password[i]);
							}
							
						          	HAL_Delay(1000);										
							 	EEPROM_WriteBytes(user_password, 10,6);  //把用户密码写进去
							        	Line_1A_WT588S(3);//播放设定密码声音
				               HAL_Delay(1000);
										   HAL_Delay(1000);					
							          factory_num=4;	
						}
				 break;
						//播放设置消警密码
			case 4 :
				              	Line_1A_WT588S(4);//播放设定密码声音
				               HAL_Delay(1000);  //延迟播放音乐
										   HAL_Delay(1000);
			               	 factory_num=5;	
                  		
				break;	
			//输入消警密码
	    case 5 :		 
                      scan_key_flag=1;				
				   if(key_password_set(xiaojing_password)==1)
						{
							    scan_key_flag=0;
							     HAL_Delay(1000);
							  	EEPROM_WriteBytes(xiaojing_password, 20,6);  //把用户密码写进去
							        	Line_1A_WT588S(5);//播放设定密码声音
				               HAL_Delay(1000);
										   HAL_Delay(1000);					
							      factory_num=6;	
						}
				break;	
      // 请求入网,后台分配设备号						
			case 6:	
			     printf("请求后台分配设备号\r\n");
               SX127X_StandbyMode();   //待机模式   
		         	lora_data_state=1;
		     	pack_len=pin_pack(dev_num,0x01,0x01,&lora_data_state); //发送入网请求
			  printf("pack_len=%d\r\n",pack_len);
			    set_txrx_datalen(pack_len);//数据包长度
            SX127X_TxPacket(TXbuffer);   				
				          	 factory_num=7;		               		  
				break;
    //接收数据lora数据解包
			case 7:	
				             //
				          if(chuchang_lora_process()==1)
									{
										printf("分配入网成功\r\n");
										   factory_num=9;
									}
								
	              
				break;
			
				//发送入网成功，告知后台
			case 9 : 
				printf("设备号已经拿到\r\n");
				       SX127X_StandbyMode();   //待机模式   
			      memset(TXbuffer, 0, sizeof(TXbuffer));	//qingling
			       lora_data_state=1;
			pack_len=pin_pack(dev_num,0x01,0x02,&lora_data_state); //发送入网请求
			  set_txrx_datalen(pack_len);//数据包长度
            SX127X_TxPacket(TXbuffer);   				
				          	 factory_num=10;		
          break;
			case 10 :
									 if(chuchang_lora_process()==2)
									{
											Line_1A_WT588S(6);//接入网关成功
				               HAL_Delay(1000);
										   HAL_Delay(1000);	
										   factory_num=13;
									}
									
									break;
				//出厂完成
			case 12 :
				      Line_1A_WT588S(7);//
				               HAL_Delay(1000);
										   HAL_Delay(1000);	 
				  printf("出厂设置完成\r\n");
				factory_parameter_flag=0x55;
				EEPROM_WriteBytes(&factory_parameter_flag, 1, 1); //完成出厂设置，设置参数区
                     factory_num=17;  //进入设备绑定状态 
			       
				 break;
			//开机把用户密码发上去
			case 13 :			  
				   SX127X_StandbyMode();   //待机模式   
//			    TXbuffer[0]=1;
		  	pack_len=pin_pack(dev_num,0x01,0x07,user_password); //发送入网请求
			  set_txrx_datalen(pack_len);//数据包长度
            SX127X_TxPacket(TXbuffer);   				
				          	 factory_num=14;		
	
			//				     value=1;   //出厂设置化完成
			 break;
			case 14 :
				     if(chuchang_lora_process()==3)
									{
										   factory_num=15;
									}
			 break;
				 //消防密码上报
			case 15 : 
				
				   SX127X_StandbyMode();   //待机模式   
//			    TXbuffer[0]=1;
		  	pack_len=pin_pack(dev_num,0x01,0x06,xiaojing_password); //发送入网请求
			  set_txrx_datalen(pack_len);//数据包长度
            SX127X_TxPacket(TXbuffer);   				
				          	 factory_num=16;		
	
				break; 
			//消防密码回应
		case 16 :
				     if(chuchang_lora_process()==4)
									{
										   factory_num=12;
									}
			 break;
            				
			
			
				//输入启动密码开始工作
			case 	 17 :
				           scan_key_flag=1;
					if(key_password_set(user_temp_password)==1)
					  {
							scan_key_flag=0;
							     HAL_Delay(1000); //播放语音
						      				 uint8_t b=0;
					  printf("check_password_num\r\n");
				            for(uint8_t i=0;i<6;i++)
				       { 
							    printf("user_temp_password[%d]=%d",i,user_temp_password[i]);
							 }
							       for(uint8_t i=0;i<6;i++)
				       { 
							    printf("user_password[%d]=%d",i,user_password[i]);
							 }
				         for(uint8_t i=0;i<6;i++)
				       {
					          if(user_temp_password[i]==user_password[i])
										{
											        b++ ;
										}
					       
		           	}
				printf("b=%d",b);
				      if(b==6)
					    {				
                    //输入启动密码正确								

								  	Line_1A_WT588S(9);//密码正确
				               HAL_Delay(1000);
										   HAL_Delay(1000);	
									  	b=0;
					         factory_num =18; 
				      }
			       else
			      {

									   Line_1A_WT588S(8);//密码错误
				               HAL_Delay(1000);
										   HAL_Delay(1000);	
									  	b=0;		
			        }
						
						
					  }
				//校验密码   :密码正确   密码错误
				break;
						//
				case  18: 
					//播放语音:设备开始工作 ,绿灯亮起来

                  Line_1A_WT588S(14);//设备开始工作
				               HAL_Delay(1000);
										   HAL_Delay(1000);	  
          			  runing_state_flag=1;	
			                	value=1;
				break;
        				
		}
	
	   return value;
	
}
