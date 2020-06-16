#include "main_app.h"
#include "SX127X_Driver.h"
#include "SX127X_Hal.h"
#include "stdio.h"
#include "bsp_eeprom.h"
#include "string.h"
#include "key_hal.h"
#include "speak_hal.h"
//ҵ���
#define join_network   0x01    //��������
#define join_network_state  0x02 //����״̬����
#define Power_report    0x03    //�豸��ص����ϱ�
#define Equipment_tamper    0x04   //�豸�����ϱ�
#define device_number     0x05     //�豸���ϱ�
#define xiaojing_pass_num      0x06     //���������ϱ�
#define user_password_fun     0x07    //�û������ϱ�
#define dev_run_state     0x08    //����״̬�ϱ�
#define set_xiaojing_password  0x09   //�豸��������
#define set_userpassword         0x10  //��̨�����û�����
#define rang_fun              0x12 //�豸�����ϱ�
#define xiaojing_num          0x13   //����ָ���·�
uint32_t Fre[5] = {470800000, 494600000, 510000000, 868000000, 915000000};   //�շ�Ƶ��
uint8_t communication_states; //ҵ��״̬
uint8_t TXbuffer[50] = {0};
uint8_t RXbuffer[50] = {0};
uint16_t T_Cnt = 0;
uint8_t factory_parameter_flag=0;    //�����Ƿ�����
uint8_t user_password[10]={0};        //�û�����
uint8_t user_temp_password[10]={0};        //�û�����
uint8_t xiaojing_password[10]={1,2,3,2,2,2};    //��������
uint8_t dev_num[2]={0};     //�豸��
uint8_t dev_runing_flag=0;  //�豸״̬
uint8_t pack_len = 0;
uint8_t lora_data_state=0;
uint8_t scan_key_flag=0;
uint8_t sleep_flag=0;
void set_txrx_datalen(uint8_t datalen);
void sleep_open(void);
extern uint8_t key_state_value;
 uint8_t  runing_state_flag=0;
	 uint8_t  rang_key_flag=0;
		uint8_t  fangchai_flag=0;
uint8_t pin_pack(uint8_t *dev,uint8_t dev_type,uint8_t function_num,uint8_t *data_pack);
enum DemoInternalStates
{
    APP_RNG = 0, // nothing to do (or wait a radio interrupt)
    RX_DONE,
    TX_DONE,
    TX_ING,
    APP_IDLE,
};
//LORA ��ʼ������
void app_lora_config_init()
{
	 uint8_t astate=0;
    G_LoRaConfig.LoRa_Freq = Fre[0];   //����Ƶ��470MHz
    G_LoRaConfig.BandWidth = BW125KHZ;    //BW = 125KHz  BW125KHZ
    G_LoRaConfig.SpreadingFactor = SF09;  //SF = 9
    G_LoRaConfig.CodingRate = CR_4_6;     //CR = 4/6
    G_LoRaConfig.PowerCfig = 15;          //19��1dBm
    G_LoRaConfig.MaxPowerOn = true;       //����ʿ���
    G_LoRaConfig.CRCON = true;            //CRCУ�鿪��?
    G_LoRaConfig.ExplicitHeaderOn = true; //Header����
    G_LoRaConfig.PayloadLength = 10;      //���ݰ�����
     astate= SX127X_Lora_init();
	   printf("astate=%d\r\n",astate);
    if(astate!= NORMAL)	 //����ģ���ʼ��
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
void rang_runing()
{
	  printf("rang_state=%d",rang_state);
	 switch(rang_state)
	 {
		 case 0 :
			       SX127X_StandbyMode();   //����ģʽ   
		           	lora_data_state=1;
		     	pack_len=pin_pack(dev_num,0x01,0x12,&lora_data_state); //������������
			    printf("pack_len=%d\r\n",pack_len);
			    set_txrx_datalen(pack_len);//���ݰ�����
           SX127X_TxPacket(TXbuffer); 
   		         rang_state=1;
			         
		                 break;
		 case 1 :    
		       	          printf("һ����������\r\n");					
					              Line_1A_WT588S(7);//�����ɹ�	
				               HAL_Delay(1000);  //�ӳٲ�������
										   HAL_Delay(1000);
		                      rang_state=2;
		                 break;
		 case  2 :    
		 
                       Line_1A_WT588S(4);//			      
				               HAL_Delay(1000);  //�ӳٲ�������
    			  
		        break;			      
	 }	
}
void fangchai_runing()
{
	
	 switch(fagchai_state)
	 {
		 case 0 :
			         printf("������������\r\n");
					            Line_1A_WT588S(9);//�����ɹ�
				               HAL_Delay(1000);  //�ӳٲ�������
										   HAL_Delay(1000);
//		                   if(play_music_com(9)==1)
//											 {
//		                 fagchai_state=2;
//											 }
		                 break;
		 case 1 :    
		               printf("��������\r\n");
					            Line_1A_WT588S(0);//�����ɹ�				      
				               HAL_Delay(1000);  //�ӳٲ�������
										   HAL_Delay(1000);
//		                if(play_music_com(0)==1)
//											 {
////		                 fagchai_state=2;
//											 }
		                 break;
		 case  2 :      SX127X_StandbyMode();   //����ģʽ   
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

            SX127X_StandbyMode();   //����ģʽ

            SX127X_StartRx();
   communication_states = APP_IDLE;
            break;
					
				case RX_DONE :
					 SX127X_Read(REG_LR_NBRXBYTES, &G_LoRaConfig.PayloadLength); //��ȡ���ݳ���
					 set_txrx_datalen(G_LoRaConfig.PayloadLength);
				   SX127X_RxPacket(RXbuffer);		
			   //   SX127X_SleepMode(); //˯��ģʽ
		       SX127X_StandbyMode();  //�л�״̬���FIFO��Ҫ���յ�250���ֽڣ������				
				 for(uint8_t i=0;i<G_LoRaConfig.PayloadLength;i++)
			 	 {
					printf("RXbuffer[%d]=%02x\r\n",i,RXbuffer[i]);
									
				 }
				 printf("��ǰ�豸��ַ:%d-%d\r\n",dev_num[0],dev_num[1]);
				 //�������ݴ���
				   if(RXbuffer[0]==0x5a&&RXbuffer[1]==0xa5&&RXbuffer[2]==dev_num[0]&&RXbuffer[3]==dev_num[1]&&RXbuffer[4]==0x01)
					 {
						   switch(RXbuffer[5])
							 {
								    //��̨��������ָ��
								 case  0x12 : 
									           //��һ��
								          //�رձ���������LED��˸
								          //2 ���������ɹ�
								          //��������
								   printf("��̨�յ�����ָ��\r\n");
								       
							            rang_key_flag=0;
								           rang_state=0;
								 break;
						
								         
								       //��̨���շ��𱨾�ָ��
								 case  0x04 : 
									           
//								   printf("��̨�յ�����ָ���\r\n");
							            fangchai_flag=0;
								 fagchai_state=0;
								 break;
					
								 //��̨������������
								   //��������ָ��
								 case  0x13 : 
									           //��һ��
								          //�رձ���������LED��˸
								         //2 ���������ɹ�
								          //��������
								    SX127X_StandbyMode();   //����ģʽ   	
                      lora_data_state=1;								 
			    pack_len=pin_pack(dev_num,0x01,0x13,&lora_data_state); 
			       set_txrx_datalen(pack_len);//���ݰ�����
                 SX127X_TxPacket(TXbuffer);  
								       Line_1A_WT588S(15);//�����ɹ�	

				               HAL_Delay(1000);  //�ӳٲ�������
										   HAL_Delay(1000);
									          rang_key_flag=0;
								             rang_state=0;
							           	 sleep_open();
								 break;
								 //��̨������������
								 case  0x09 :
									 // ��һ��  ���
								     for(uint8_t i=0;i<6;i++)
								 {
									  xiaojing_password[i]=RXbuffer[i+6];
								 }
								 EEPROM_WriteBytes(xiaojing_password, 20,6);  //���û�����д��ȥ
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
					SX127X_StandbyMode();   //����ģʽ
            SX127X_StartRx();
				     lora_com = APP_IDLE;
            break;				
				case RX_DONE :
					 SX127X_Read(REG_LR_NBRXBYTES, &G_LoRaConfig.PayloadLength); //��ȡ���ݳ���
//					 set_txrx_datalen(G_LoRaConfig.PayloadLength);
				   SX127X_RxPacket(RXbuffer);		
				     SX127X_StandbyMode();  //�л�״̬���FIFO��Ҫ���յ�250���ֽڣ������
			   //   SX127X_SleepMode(); //˯��ģʽ		     		
//				 for(uint8_t i=0;i<G_LoRaConfig.PayloadLength;i++)
//			 	 {
//					printf("RXbuffer[%d]=%02x\r\n",i,RXbuffer[i]);
//									
//				 }
				 //����lora���ݴ���
				   if(RXbuffer[0]==0x5a&&RXbuffer[1]==0xa5&&RXbuffer[4]==0x01)
					 {
						   switch(RXbuffer[5])
							 {
							 
								 //�����̨������豸��
								  case  0x01 : 
										//�ó��豸��
										       for(uint8_t i=0;i<2;i++)
									{
										dev_num[i]=RXbuffer[6+i];
										
									}
				      	EEPROM_WriteBytes(dev_num, 30,2);  //���豸��д��ȥ     
			    	   for(uint8_t i=0;i<G_LoRaConfig.PayloadLength;i++)
				       {
					
					       RXbuffer[i]=0;
					
				       }
							       value=1;
									break;
							 //��������������Ϣ
							  case  0x02 : 
  
//                 printf("�豸�����ɹ�\r\n");								
			    	   for(uint8_t i=0;i<G_LoRaConfig.PayloadLength;i++)
				       {
					
					       RXbuffer[i]=0;
					
				       }
							    value=2;
									break;
							 //��������Ӧ��
							  case 	0x07 :
//									  printf("��̨�յ��û�����\r\n");								
			    	   for(uint8_t i=0;i<G_LoRaConfig.PayloadLength;i++)
				       {
					
					       RXbuffer[i]=0;
					
				       }
                 value=3;
                   break;	
                 	 //������������Ӧ��
							  case 	0x06 :
//									  printf("��̨�յ����������ϱ�\r\n");								
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
//����������
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
  G_LoRaConfig.PayloadLength = datalen;      //���ݰ�����
}
//���
uint8_t pin_pack(uint8_t *dev,uint8_t dev_type,uint8_t function_num,uint8_t *data_pack)
{
	  uint8_t index=0;
	  TXbuffer[index++]=0x5a;
	  TXbuffer[index++]=0xa5;  //����ʶ����
	  TXbuffer[index++]=dev[0];  //�豸��
	  TXbuffer[index++]=dev[1];  //�豸��
	  TXbuffer[index++]=dev_type;  //�豸����
  	TXbuffer[index++]=function_num; //������
	   for(uint8_t i=0;i<strlen((const char *)data_pack);i++)
	{
	  TXbuffer[index++]=data_pack[i]; //������
	}
	  return index;
}
//�û����밴������
uint8_t key_password_set(uint8_t *password)
{
	       uint8_t value=0;
                uint8_t key=0 ;
	     static	   uint8_t key_count=0 ; //��������
     	 static	   uint8_t key_value=0 ;//������ֵ
	             key = scan_key();
	           switch(key)
			{
				case 1:
				   
    			     	Line_1A_WT588S(1);
				            key_count++;
				            key_value=1;
				 
				break;
				case 2:     
					          key_count++;Line_1A_WT588S(2);
                        key_value=2;
				break;
				case 3: 
				                	key_value=3;
					         key_count++;Line_1A_WT588S(3);
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
				for(uint8_t i=0;i<6;i++)
				{
      printf("%d",password[i]);
				}
				value=1;
				  key_count=0;
				  key_value=0;
	

			 } 
return value ;			
 }

 void check_key_password()
 {
	           scan_key_flag=1;
					if(key_password_set(user_temp_password)==1)
					  {
							scan_key_flag=0;
							     HAL_Delay(10); //��������
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
                    //��������������ȷ								

								  	Line_1A_WT588S(14);//������ȷ
				               HAL_Delay(1000);
										   HAL_Delay(1000);	
									  	b=0;
					      
				      }
			       else
			      {

									   Line_1A_WT588S(13);//�������
				               HAL_Delay(1000);
										   HAL_Delay(1000);	
									  	b=0;		
			        }
						
						
					  }
	 
 }
//��������
uint8_t  factory_parameter_set()
{
	  uint8_t value=0;
	  static  uint8_t factory_num=0;
	  switch(factory_num)
		 {
			// ����Ƿ���
			case  0 :    
     //�Ѿ��趨�û�����
		  	if(check_factory_parameter()==1)
			 {
				    factory_num=13;	
				 EEPROM_ReadBytes(user_password, 10, 6);
				 EEPROM_ReadBytes(xiaojing_password, 20, 6);
         printf("�û������Ѿ�����\r\n");		           				 
			 }
			 //���������û�����
      else
			{ 
				  printf("�����û���������\r\n");	
	     factory_num=2;	
			}				
			 break;
			 //���û����뷢����ȥ
			case 1 :
			break;
						//�����û���������
			case 2 : 
				              Line_1A_WT588S(9);//�����趨��������
				               HAL_Delay(1000);  //�ӳٲ�������
										   HAL_Delay(1000);
		              	key_state_value=0;
			               	factory_num=3;		
			
			 break;
			     //�û����밴������
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
							 	EEPROM_WriteBytes(user_password, 10,6);  //���û�����д��ȥ
							        	Line_1A_WT588S(19);//�����趨��������
				               HAL_Delay(1000);
										   HAL_Delay(1000);					
							          factory_num=4;	
						}
				 break;
						//����������������
			case 4 :
				              	Line_1A_WT588S(8);//�����趨��������
				               HAL_Delay(1000);  //�ӳٲ�������
										   HAL_Delay(1000);
			               	 factory_num=5;	
                  		
				break;	
			//������������
	    case 5 :		 
                      scan_key_flag=1;				
				   if(key_password_set(xiaojing_password)==1)
						{
							    scan_key_flag=0;
							     HAL_Delay(1000);
							  	EEPROM_WriteBytes(xiaojing_password, 20,6);  //���û�����д��ȥ
							        	Line_1A_WT588S(17);//�����趨��������
				               HAL_Delay(1000);
										   HAL_Delay(1000);					
							      factory_num=6;	
						}
				break;	
      // ��������,��̨�����豸��						
			case 6:	
			     printf("�����̨�����豸��\r\n");
               SX127X_StandbyMode();   //����ģʽ   
		         	lora_data_state=1;
		     	pack_len=pin_pack(dev_num,0x01,0x01,&lora_data_state); //������������
			  printf("pack_len=%d\r\n",pack_len);
			    set_txrx_datalen(pack_len);//���ݰ�����
            SX127X_TxPacket(TXbuffer);   				
				          	 factory_num=7;		               		  
				break;
    //��������lora���ݽ��
			case 7:	
				             //
				          if(chuchang_lora_process()==1)
									{
										printf("���������ɹ�\r\n");
										   factory_num=9;
									}
								
	              
				break;
			
				//���������ɹ�����֪��̨
			case 9 : 
				printf("�豸���Ѿ��õ�\r\n");
				       SX127X_StandbyMode();   //����ģʽ   
			      memset(TXbuffer, 0, sizeof(TXbuffer));	//qingling
			       lora_data_state=1;
			pack_len=pin_pack(dev_num,0x01,0x02,&lora_data_state); //������������
			  set_txrx_datalen(pack_len);//���ݰ�����
            SX127X_TxPacket(TXbuffer);   				
				          	 factory_num=10;		
          break;
			case 10 :
									 if(chuchang_lora_process()==2)
									{
											Line_1A_WT588S(6);//�������سɹ�
				               HAL_Delay(1000);
										   HAL_Delay(1000);	
										   factory_num=13;
									}
									
									break;
				//�������
			case 12 :
				      Line_1A_WT588S(10);//
				               HAL_Delay(1000);
										   HAL_Delay(1000);	 
				  printf("�����������\r\n");
				factory_parameter_flag=0x55;
				EEPROM_WriteBytes(&factory_parameter_flag, 1, 1); //��ɳ������ã����ò�����
                     factory_num=17;  //�����豸��״̬ 
			       
				 break;
			//�������û����뷢��ȥ
			case 13 :			  
				   SX127X_StandbyMode();   //����ģʽ   
//			    TXbuffer[0]=1;
		  	pack_len=pin_pack(dev_num,0x01,0x07,user_password); //������������
			  set_txrx_datalen(pack_len);//���ݰ�����
            SX127X_TxPacket(TXbuffer);   				
				          	 factory_num=14;		
	
			//				     value=1;   //�������û����
			 break;
			case 14 :
				     if(chuchang_lora_process()==3)
									{
										   factory_num=15;
									}
			 break;
				 //���������ϱ�
			case 15 : 
				
				   SX127X_StandbyMode();   //����ģʽ   
//			    TXbuffer[0]=1;
		  	pack_len=pin_pack(dev_num,0x01,0x06,xiaojing_password); //������������
			  set_txrx_datalen(pack_len);//���ݰ�����
            SX127X_TxPacket(TXbuffer);   				
				          	 factory_num=16;		
	
				break; 
			//���������Ӧ
		case 16 :
				     if(chuchang_lora_process()==4)
									{
										   factory_num=12;
									}
			 break;
            				
			
			
				//�����������뿪ʼ����
			case 	 17 :
				           scan_key_flag=1;
					if(key_password_set(user_temp_password)==1)
					  {
							scan_key_flag=0;
							     HAL_Delay(1000); //��������
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
                    //��������������ȷ								

								  	Line_1A_WT588S(14);//������ȷ
				               HAL_Delay(1000);
										   HAL_Delay(1000);	
									  	b=0;
					         factory_num =18; 
				      }
			       else
			      {

									   Line_1A_WT588S(13);//�������
				               HAL_Delay(1000);
										   HAL_Delay(1000);	
									  	b=0;		
			        }
						
						
					  }
				//У������   :������ȷ   �������
				break;
						//
				case  18: 
					//��������:�豸��ʼ���� ,�̵�������

                  Line_1A_WT588S(12);//�豸��ʼ����
				               HAL_Delay(1000);
										   HAL_Delay(1000);	  
          			  runing_state_flag=1;	
			                	value=1;
				break;
        				
		}
	
	   return value;
	
}