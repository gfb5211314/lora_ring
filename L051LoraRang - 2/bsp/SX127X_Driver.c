/**
  ******************************************************************************
  * 文件名 ：   SX127X_Driver.c
  * 作者   ：   LSD RF Team
  * 版本   ：   V1.0.0
  * 时间   ：   15-Aug-2018
  * 文件描述：
  *     该文件为SX127X模块的驱动层，包含对SX127X模块的工作模式控制，FIFO操作，发
  *射功率、频率等设置，以及一些数据处理的公共函数
  *    客户在使用SX127X模块时候需要移植该文件，如果SX127X_HAL.c中各个函数功能都已
  *实现且函数名等都没有改动，那本文件可以不用做更改直接使用，客户在应用层直接调
  *用本文件函数就可以实现各种操作。
*******************************************************************************/

#include "SX127X_Driver.h"
#include "stm32l0xx_hal.h"
#include "main.h"
/*
  注意，本文件的函数实现了从RF的驱动与MCU的兼容

*/
float G_BandWidthKHz = 500.0;//本地计算Symbol周期使用
float G_TsXms = 1.024;//1.024ms

/**
  * @简介：SX127X 配置参数
  * @参数：无
  * @返回值：无
  */
S_LoRaConfig G_LoRaConfig = {
    470000000,       //收发频率
    BW500KHZ,      //带宽
    SF08,         //因子
    CR_4_5,        //循环纠错编码
    15,        //发射功率
    true,
    true,
    true,
    64,  //负载长度
};
S_LoRaPara G_LoRaPara;

//-------------------------SX127X 寄存器------------------------//
//该部分函数为MCU对SX127X模块寄存器进行读写及FIFO的读写
//--------------------------------------------------------------//

/**
  * @简介：RF  向寄存器地址写1字节数据
  * @参数：uint8_t addr,寄存器地址 uint8_t data数据
  * @返回值：无
  */
void SX127X_Write( uint8_t addr, uint8_t data )
{
    SX127X_WriteBuffer( addr, &data, 1 );
}
/**
  * @简介：RF  向寄存器地址读1字节数据
  * @参数：uint8_t addr,寄存器地址 uint8_t *data读数据存储地址
  * @返回值：无
  */
void SX127X_Read( uint8_t addr, uint8_t *data )
{
    SX127X_ReadBuffer( addr, data, 1 );
}
/**
  * @简介：SX127X  向FIFO写数据
  * @参数：uint8_t *buffer,数组指针 uint8_t size长度
  * @返回值：无
  */
void SX127X_WriteFifo( uint8_t *buffer, uint8_t size )
{
    SX127X_WriteBuffer( 0, buffer, size );
}
/**
  * @简介：SX127X  向FIFO读数据
  * @参数：uint8_t *buffer,数组指针 uint8_t size长度
  * @返回值：uint8_t *buffer 存储读取内容
  */
void SX127X_ReadFifo( uint8_t *buffer, uint8_t size )
{
    SX127X_ReadBuffer( 0, buffer, size );
}

//-------------------------SX127X 基本设置-----------------------//
//该部分函数为对SX127X模块复位、发射功率、发射频率等参数设置，以
//及SX127X工作模式设置、数据包读写等
//--------------------------------------------------------------//

/**
  * @简介：SX127X  复位设置
  * @参数：无
  * @返回值：无
  */
void SX127X_Reset(void)
{
  HAL_GPIO_WritePin(lora_rst_GPIO_Port, lora_rst_Pin, GPIO_PIN_RESET);//set LoRa RESET(PA_1) to low
	HAL_Delay(200);
	HAL_GPIO_WritePin(lora_rst_GPIO_Port, lora_rst_Pin, GPIO_PIN_SET);
	HAL_Delay(200);
}
/**
  * @简介：SX127X  IO初始化及配置
  * @参数：无
  * @返回值：无
  */
void SX127X_InitIo(void)
{
    SX127X_DIO0_INPUT();
    SX127X_DIO1_INPUT();
    SX127X_DIO2_INPUT();
//    SX127X_SPIGPIO_Init();
    SX127X_RESET_OUTPUT(GPIO_PIN_SET);
    //以下两句是LSD4RF-2F717N10以及LSD4RF-2F717N01需要用到。
    SX127X_TXE_OUTPUT(GPIO_PIN_RESET);
    SX127X_RXE_OUTPUT(GPIO_PIN_RESET);
}
/**
  * @简介：SX127X  TX/RX的PA切换
  * @参数：bool txEnable  切换逻辑；真：作为TX。假：作为RX   为硬件两个PA控制IO口
  * @返回值:无
  */
void SX127XWriteRxTx( bool txEnable )
{
    if( txEnable != 0 )       //如果为真，为TX
    {
        SX127X_RXE_OUTPUT(GPIO_PIN_RESET);
        SX127X_TXE_OUTPUT(GPIO_PIN_SET);
    }
    else  //为假，为RX
    {
        SX127X_RXE_OUTPUT(GPIO_PIN_SET);
        SX127X_TXE_OUTPUT(GPIO_PIN_RESET);
    }
}
/**
  * @简介：DIO0中断开启使能
  * @参数：无
  * @返回值：无
  */
void DIO0_EnableInterrupt(void)
{
    SX127X_DIO0_INTENABLE();
}
/**
  * @简介：DIO0中断关闭使能
  * @参数：无
  * @返回值：无
  */
void DIO0_DisableInterrupt(void)
{
    SX127X_DIO0_INTDISABLE();
}
/**
  * @简介：DIO0引脚状态获取
  * @参数：无
  * @返回值：State表示DIO0获取的电平，高电平"1",低电平"0"
  */
GPIO_PinState DIO0_GetState(void)
{
    GPIO_PinState State;
    State = SX127X_DIO0_GetState();
    return State;
}
/**
  * @简介：SX127X 配置频率检查判断
  * @参数：无
  * @返回值：true or false
  */
bool LoRaConfig_Check(void)
{
    if((G_LoRaConfig.LoRa_Freq < 430000000) || (G_LoRaConfig.LoRa_Freq > 920000000))
        return false;
    G_LoRaConfig.BandWidth = (t_BandWidth)(G_LoRaConfig.BandWidth & 0xF0);
    if(G_LoRaConfig.BandWidth > BW500KHZ)
        return false;
    //计算BandWidth
    switch(G_LoRaConfig.BandWidth) {
    case BW500KHZ:
        G_BandWidthKHz = 500.0;
        break;
    case BW250KHZ:
        G_BandWidthKHz = 250.0;
        break;
    case BW125KHZ:
        G_BandWidthKHz = 125.0;
        break;
    case BW62_50KHZ:
        G_BandWidthKHz = 62.5;
        break;
    case BW41_66KHZ:
        G_BandWidthKHz = 41.66;
        break;
    case BW31_25KHZ:
        G_BandWidthKHz = 31.25;
        break;
    case BW20_83KHZ:
        G_BandWidthKHz = 20.83;
        break;
    case BW15_62KHZ:
        G_BandWidthKHz = 15.62;
        break;
    case BW10_41KHZ:
        G_BandWidthKHz = 10.41;
        break;
    case BW7_81KHZ:
        G_BandWidthKHz = 7.81;
        break;
    }
    G_LoRaConfig.SpreadingFactor = (t_SpreadingFactor)(G_LoRaConfig.SpreadingFactor & 0xF0);
    if((G_LoRaConfig.SpreadingFactor > SF12) || (G_LoRaConfig.SpreadingFactor < SF06))
        return false;
    //计算LoRa码元周期，单位ms
    G_TsXms = (2 << ((G_LoRaConfig.SpreadingFactor >> 4) - 1)) / G_BandWidthKHz;

    G_LoRaConfig.CodingRate = (t_CodingRate)(G_LoRaConfig.CodingRate & 0x0E);
    if((G_LoRaConfig.CodingRate > CR_4_8) || (G_LoRaConfig.CodingRate < CR_4_5))
        return false;
    if(G_LoRaConfig.PowerCfig > 15)
        return false;
    if(G_LoRaConfig.PayloadLength > 127)
        return false;
    return true;
}
/**
* @简介:SX127X初始化
  * @参数：无
  * @返回值：tSX127xError   错误枚举内容
  */
tSX127xError SX127X_Lora_init(void)
{
    if(false == LoRaConfig_Check()) //如果输入参数错误
    {
        return PARAMETER_INVALID;  
    }
//    SX127X_InitIo();               
     SX127X_Reset();              
//    SX127X_SPI_Init();       

    SX127X_Write( REG_LR_OPMODE, RFLR_OPMODE_SLEEP );
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_SLEEP );
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );

    //SPI 验证
    uint8_t test = 0;
    SX127X_Write( REG_LR_HOPPERIOD, 0x91 ); //选一个用不到的寄存器来做验证
    SX127X_Read( REG_LR_HOPPERIOD, &test);
    if(test != 0x91)
        return SPI_READCHECK_WRONG;
    SX127X_Write( REG_LR_DIOMAPPING2, RFLR_DIOMAPPING2_DIO4_01);
    SX127X_Write(REG_LR_LNA, 0x23);
    SX127X_FreqSet(); 
    SX127X_PoutSet();
    SX127X_Write( REG_LR_PARAMP, RFLR_PARAMP_0100_US);
    //  ↑PA Ramp的时间，如果用户LDO不能快速输出大电流（泵能力），适当增加PA Ramp时间
    //  ↑如果Ramp时间过短超过了LDO的能力时，会出现进入TX后，系统电流为发射电流，但是RF信号不出现的现象
    SX127X_Write( REG_LR_OCP, 0x20 | RFLR_OCP_TRIM_240_MA); //电流过载保护 Over Current Protection
    SX127X_Write( REG_LR_PAYLOADLENGTH, G_LoRaConfig.PayloadLength);
    //注意，无头模式（Implicit Header）时，必须提前规定好收发双方的PL

    //BW、CR、Header有无，初始化
    SX127X_Write( REG_LR_MODEMCONFIG1, \
                  (((uint8_t)G_LoRaConfig.BandWidth) | ((uint8_t)G_LoRaConfig.CodingRate)) | (\
                          (true == G_LoRaConfig.ExplicitHeaderOn) ? 0x00 : 0x01));

    //SF、CRC初始化
    SX127X_Write( REG_LR_MODEMCONFIG2, \
                  ((uint8_t)G_LoRaConfig.SpreadingFactor) | (\
                          (true == G_LoRaConfig.CRCON) ? 0x04 : 0x00));
    if(SF06 == G_LoRaConfig.SpreadingFactor) { //慎用SF = 6，需要的配置很特殊
        uint8_t temp = 0;
        SX127X_Read( 0x31, &temp);
        SX127X_Write(  0x31, (temp & 0xF8) | 0x05);
        SX127X_Write(  0x37, 0x0C);
    }

    //低速率优化功能是否开启、AutoAGC默认开启
    SX127X_Write( REG_LR_MODEMCONFIG3, ((G_TsXms > 16.0f) ? \
                                        RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_ON : RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_OFF\
                                       ) | RFLR_MODEMCONFIG3_AGCAUTO_ON);
    return NORMAL;
}
/**
  * @简介：RF发送数据包
  * @参数：uint8_t*data：发送数据指针
  * @返回值：无
  */
void SX127X_TxPacket(uint8_t*data)
{
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
    SX127X_Write( REG_LR_PREAMBLEMSB, 0);
    SX127X_Write( REG_LR_PREAMBLELSB, 10);
    SX127X_Write( REG_LR_PAYLOADLENGTH, G_LoRaConfig.PayloadLength);

    SX127XWriteRxTx(true);//LSD4RF-2F717N10以及LSD4RF-2F717N01时需要用到，高频切换；

    SX127X_Write( REG_LR_FIFOADDRPTR, 0x80);
    SX127X_WriteBuffer(REG_LR_FIFO, data, G_LoRaConfig.PayloadLength);
    SX127X_Write(REG_LR_IRQFLAGS, 0xff);
    SX127X_Write( REG_LR_IRQFLAGSMASK, ~(RFLR_IRQFLAGS_TXDONE));
    SX127X_Write( REG_LR_DIOMAPPING1, RFLR_DIOMAPPING1_DIO0_01 );
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_TRANSMITTER );
}
/**
  * @简介：RF进入接收状态
  * @参数：无
  * @返回值：无
  */
void SX127X_StartRx(void)
{
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
    SX127X_Write( REG_LR_PREAMBLEMSB, 0);
    SX127X_Write( REG_LR_PREAMBLELSB, 10);
    SX127X_Write( REG_LR_PAYLOADLENGTH, G_LoRaConfig.PayloadLength);

    SX127XWriteRxTx(false);//LSD4RF-2F717N10以及LSD4RF-2F717N01时需要用到，高频切换；

    SX127X_Write( REG_LR_FIFOADDRPTR, 0x00);
    SX127X_Write(REG_LR_IRQFLAGS, 0xff);
    SX127X_Write( REG_LR_IRQFLAGSMASK, ~(RFLR_IRQFLAGS_RXDONE));
    SX127X_Write( REG_LR_DIOMAPPING1, RFLR_DIOMAPPING1_DIO0_00 );
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_RECEIVER );
}
/**
  * @简介：RF接收数据包
  * @参数：uint8_t*cbuf接收数组指针
  * @返回值：无
  */
void SX127X_RxPacket(uint8_t*cbuf)
{
    unsigned char Reg_PKTRSSI, Reg_PKTSNR;
    SX127X_Read( REG_LR_PKTSNRVALUE, &Reg_PKTSNR);

    if((Reg_PKTSNR & 0x80) != 0) {
        Reg_PKTSNR = ((~Reg_PKTSNR + 1) & 0xff) >> 2;
        //SNR前显示加“-”
        G_LoRaPara.Packet_SNR = -Reg_PKTSNR;
    }
    else
    {
        Reg_PKTSNR = (Reg_PKTSNR & 0xff) >> 2;
        G_LoRaPara.Packet_SNR = Reg_PKTSNR;
    }

    SX127X_Read( REG_LR_PKTRSSIVALUE, &Reg_PKTRSSI);

    if(G_LoRaPara.Packet_SNR < 0)
    {
        G_LoRaPara.Packet_RSSI = -164 + Reg_PKTRSSI - G_LoRaPara.Packet_SNR;
    }
    else
    {
        G_LoRaPara.Packet_RSSI = -164 + Reg_PKTRSSI * 16 / 15;
    }

    if(true == G_LoRaConfig.ExplicitHeaderOn) {
        //有头那么从寄存器中读，否则按照G_LoRaConfig设置长度读取FIFO
        SX127X_Read(REG_LR_NBRXBYTES, &G_LoRaConfig.PayloadLength);
        SX127X_Write( REG_LR_FIFOADDRPTR, 0x00);
    }
    SX127X_ReadFifo(cbuf, G_LoRaConfig.PayloadLength);
    SX127X_Write(REG_LR_IRQFLAGS, 0xff);
}
/**
  * @简介：获取当前信道中的RSSI值
  * @参数：无
  * @返回值：RSSI值
  */
int16_t SX127X_Current_RSSI(void)
{
    unsigned char Reg_RSSIValue;
    ///int16_t temp;
    SX127X_Read( REG_LR_RSSIVALUE, &Reg_RSSIValue);
    G_LoRaPara.Current_RSSI = -164 + Reg_RSSIValue; 
    return G_LoRaPara.Current_RSSI;
}
/**
  * @简介：SX127X进入LORA睡眠模式
  * @参数：无
  * @返回值：无
  */
void SX127X_SleepMode(void)
{
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_SLEEP );
}
/**
  * @简介：SX127X进入LORA的STANDBY状态
  * @参数：无
  * @返回值：无
  */
void SX127X_StandbyMode(void)
{
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
}
/**
  * @简介：SX127X配置频率
  * @参数：无
  * @返回值：tSX127xError   错误枚举内容
  */
tSX127xError SX127X_FreqSet(void)
{

    if((G_LoRaConfig.LoRa_Freq > 920000000) || (G_LoRaConfig.LoRa_Freq < 430000000))
        return PARAMETER_INVALID;
    uint32_t freq_reg = (uint32_t)(G_LoRaConfig.LoRa_Freq / FREQ_STEP);
    uint8_t test_FRFMSB = 0, test_FRFMID = 0, test_FRFLSB = 0;
    SX127X_StandbyMode();
    SX127X_Write( REG_LR_FRFMSB, (uint8_t)(freq_reg >> 16)); 
    SX127X_Write( REG_LR_FRFMID, (uint8_t)(freq_reg >> 8) );
    SX127X_Write( REG_LR_FRFLSB, (uint8_t)(freq_reg) );

    SX127X_Read(REG_LR_FRFMSB, &test_FRFMSB);
    SX127X_Read(REG_LR_FRFMID, &test_FRFMID);
    SX127X_Read(REG_LR_FRFLSB, &test_FRFLSB);

    if(test_FRFMSB != (uint8_t)(freq_reg >> 16))
        return SPI_READCHECK_WRONG;
    if(test_FRFMID != (uint8_t)(freq_reg >> 8))
        return SPI_READCHECK_WRONG;
    if(test_FRFLSB != (uint8_t)(freq_reg))
        return SPI_READCHECK_WRONG;
    return NORMAL;
}
/**
  * @简介：SX127X配置功率
  * @参数：无
  * @返回值：tSX127xError   错误枚举内容
  */
tSX127xError SX127X_PoutSet(void)
{
    if(G_LoRaConfig.PowerCfig > 15)
        return PARAMETER_INVALID;
    SX127X_StandbyMode();
    SX127X_Write( REG_LR_PACONFIG, 0xf0 | G_LoRaConfig.PowerCfig);
    uint8_t test = 0;
    SX127X_Read(REG_LR_PACONFIG, &test);
    if((0xf0 | G_LoRaConfig.PowerCfig) != test)
        return SPI_READCHECK_WRONG;
    if(true == G_LoRaConfig.MaxPowerOn)
        SX127X_Write( REG_LR_PADAC, 0x80 | RFLR_PADAC_20DBM_ON );
    else
        SX127X_Write( REG_LR_PADAC, 0x80 | RFLR_PADAC_20DBM_OFF );
    return NORMAL;
}

/**
  * @简介：SX127X CAD初始化
  * @参数：无
  * @返回值：无
  */
void SX127X_CADinit(void)
{
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
    SX127X_Write( REG_LR_PREAMBLEMSB, 0xf0);
    SX127X_Write( REG_LR_PREAMBLELSB, 0xff);
    SX127X_Write( REG_LR_IRQFLAGSMASK, \
                  ~(RFLR_IRQFLAGS_CADDONE | RFLR_IRQFLAGS_CADDETECTED));
    SX127X_Write( REG_LR_DIOMAPPING1, RFLR_DIOMAPPING1_DIO0_10);
    SX127XWriteRxTx(false);     //set RF switch to RX path
    SX127X_Write(REG_LR_IRQFLAGS, 0xff);
}
/**
  * @简介：SX127X 启动CAD，采样信道情况一次   采样时间约为(2^SF+32)/BW
  * @参数：无
  * @返回值：无
  */
void SX127X_CAD_Sample(void)
{
    SX127XWriteRxTx(false);     //set RF switch to RX path
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_CAD );
}
/**
  * @简介：WOR初始化
  * @参数：无
  * @返回值：无
  */
void SX127X_WORInit(void)
{
    SX127X_CADinit();        //CAD功能初始化
}
/**
  * @简介：执行WOR操作
  * @参数：uint8_t cclen  0：进入睡眠。1：进入CAD检测模式
  * @返回值：无
  */
void SX127X_WOR_Execute(uint8_t cclen)
{
    switch(cclen)
    {
    case 0:   //启动睡眠
    {
        SX127X_Write(REG_LR_IRQFLAGS, 0xff); //clear flags
        SX127X_SleepMode();      //进入睡眠模式
    }
    break;
    case 1:   //进入CAD检测模式
    {
        SX127X_CAD_Sample();     //启动CAD一次
    }
    break;
    default:
        break;
    }
}
/**
  * @简介：WOR到RX
  * @参数：无
  * @返回值：无
  */
void SX127X_WOR_Exit(void)
{
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
    SX127X_Write( REG_LR_PREAMBLEMSB, 0xf0);
    SX127X_Write( REG_LR_PREAMBLELSB, 0xff);
    SX127X_Write( REG_LR_PAYLOADLENGTH, G_LoRaConfig.PayloadLength);
    SX127X_Write( REG_LR_IRQFLAGSMASK, ~(RFLR_IRQFLAGS_RXDONE));
    SX127X_Write( REG_LR_DIOMAPPING1, RFLR_DIOMAPPING1_DIO0_00 );
    SX127XWriteRxTx(false);                                    
    SX127X_Write( REG_LR_FIFOADDRPTR, 0x00);
    SX127X_Write(REG_LR_IRQFLAGS, 0xff);
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_RECEIVER );
}


/**
  * @简介：SX127X发送唤醒包
  * @参数：uint8_t*data：发送数据指针,前导长度
  * @返回值：无
  */
void SX127X_Awake(uint8_t*cbuf, uint16_t Preamble_Length)
{
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
    SX127X_Write( REG_LR_PAYLOADLENGTH, G_LoRaConfig.PayloadLength);
    SX127XWriteRxTx(true);
    SX127X_Write( REG_LR_FIFOADDRPTR, 0x80);
    SX127X_WriteBuffer(REG_LR_FIFO, cbuf, G_LoRaConfig.PayloadLength);
    SX127X_Write(REG_LR_IRQFLAGS, 0xff);
    SX127X_Write( REG_LR_IRQFLAGSMASK, ~(RFLR_IRQFLAGS_TXDONE));
    SX127X_Write( REG_LR_DIOMAPPING1, RFLR_DIOMAPPING1_DIO0_01 );
    SX127X_Write( REG_LR_PREAMBLEMSB, (uint8_t)(Preamble_Length >> 8)); //set preamble length
    SX127X_Write( REG_LR_PREAMBLELSB, (uint8_t)Preamble_Length); //set preamble length
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_TRANSMITTER );

}


/***********************************************FSK********************************************************************/

/**
  * @简介：SX127X  FSK初始化
  * @参数：无
  * @返回值：无
  */
unsigned char SX127x_Fsk_init(void)
{
    SX127X_InitIo();                // PAIO口初始化
    SX127X_Reset();                 //复位RF
//    SX127X_SPI_Init();              //SPI初始化

    LSD_RF_SleepMode_FSK();
    LSD_RF_StandbyMode_FSK();
    LSD_RF_Config();                    
    // to test SPI
    uint8_t test = 0;
    SX127X_Write(REG_FSK_SYNCVALUE8, 0x55);
    SX127X_Read(REG_FSK_SYNCVALUE8, &test);
    if(test != 0x55)
    {
        return SPI_READCHECK_WRONG;// something wrong with SPI
    }
    SX127X_Write(REG_FSK_SYNCVALUE8, 0xD3);
    SX127X_Read(REG_FSK_SYNCVALUE8, &test);
    if(test != 0xD3)
    {
        return SPI_READCHECK_WRONG;// something wrong with SPI
    }
    return NORMAL;

}

/**
  * @简介：SX127X  写FSK配置表
  * @参数：无
  * @返回值：无
  */
void LSD_RF_Config(void)
{
    _SX12XX_REG const *p;
    unsigned char i;
    p = LSD_RFregConfig;
    for(i = sizeof(LSD_RFregConfig) / 2; i > 0; i--)
    {
        SX127X_Write(p->addr, p->val);
        p++;
    }
}
/**
  * @简介：SX127X  进入FSK的standby模式
  * @参数：无
  * @返回值：无
  */
void LSD_RF_StandbyMode_FSK(void)
{
    unsigned char cData;
    unsigned int nTimes = 65535;
    SX127X_Write(REG_FSK_OPMODE, 0x08 | RFFSK_OPMODE_STANDBY );  
    do
    {
        SX127X_Read(REG_FSK_OPMODE, &cData);
        nTimes--;
    }
    while(((cData & 0x07) != RFFSK_OPMODE_STANDBY) && nTimes);
}
/**
  * @简介：SX127X  进入FSK的Sleep模式
  * @参数：无
  * @返回值：无
  */
void LSD_RF_SleepMode_FSK(void)
{
    unsigned char cData;
    unsigned int nTimes = 65535;
    SX127X_Write(REG_FSK_OPMODE, 0x08 | RFFSK_OPMODE_SLEEP );  
    do
    {
        SX127X_Read(REG_FSK_OPMODE, &cData);
        nTimes--;
    }
    while(((cData & 0x07) != RFFSK_OPMODE_SLEEP) && nTimes);
}
/**
  * @简介：SX127X  FSK模式下晴FIFO
  * @参数：无
  * @返回值：无
  */
void LSD_RF_ClearFIFO(void)
{
    SX127X_Write(REG_FSK_IRQFLAGS2, 0x10);
}
/**
  * @简介：SX127X FSK发送数据包
  * @参数：txBuffer发送数据存储数据的首地址，size数据包长度
  * @返回值：无
  */
void LSD_RF_SendPacket_FSK(uint8_t *txBuffer, uint8_t size)
{
    LSD_RF_StandbyMode_FSK();
    SX127X_Write(REG_FSK_DIOMAPPING1, RFFSK_DIOMAPPING1_DIO0_00); 
    SX127XWriteRxTx(true);
    LSD_RF_ClearFIFO();    
    SX127X_Write(REG_FSK_FIFO, size);
    for(unsigned int i = 0; i < size; i++)
    {
        SX127X_Write(REG_FSK_FIFO, txBuffer[i]);
    }
    SX127X_Write(REG_FSK_OPMODE, 0x08 | RFFSK_OPMODE_TRANSMITTER );
}
/**
  * @简介：SX127X  进入FSK的RX模式
  * @参数：无
  * @返回值：无
  */
void LSD_RF_RXmode_FSK(void)
{
    LSD_RF_StandbyMode_FSK();                     
    SX127X_Write(REG_FSK_DIOMAPPING1, RFFSK_DIOMAPPING1_DIO0_00);
    LSD_RF_ClearFIFO();        
    SX127XWriteRxTx(false);
    SX127X_Write(REG_FSK_OPMODE, 0x08 | RFFSK_OPMODE_RECEIVER); 
}

/**
  * @简介：SX127X  FSK读取数据包
  * @参数：cRxBuf发送数据存储数据的首地址，c数据包长度
  * @返回值：无
  */
void LSD_RF_RxPacket_FSK(uint8_t *cRxBuf, uint8_t *cLength)
{
    uint8_t i;
    uint8_t test = 0;
    LSD_RF_StandbyMode_FSK();     
    SX127X_Read(REG_FSK_FIFO, &test);
    *cLength = test;
    for(i = 0; i < *cLength; i++)
        SX127X_Read(REG_FSK_FIFO, &cRxBuf[i]);
    LSD_RF_ClearFIFO();   

}

/***********************************************FSK End********************************************************************/
