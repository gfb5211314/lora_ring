#include <stdint.h>

#ifndef SX127X_DRIVER_H
#define SX127X_DRIVER_H

#include "SX127X_Hal.h"

#define RF_BUFFER_SIZE_MAX                          128
#define RF_BUFFER_SIZE                              80

/*!
 * RF state machine
 */
//LoRa
typedef enum
{
    RFLR_STATE_IDLE,
    RFLR_STATE_RX_INIT,
    RFLR_STATE_RX_RUNNING,
    RFLR_STATE_RX_DONE,
    RFLR_STATE_RX_TIMEOUT,
    RFLR_STATE_TX_INIT,
    RFLR_STATE_TX_RUNNING,
    RFLR_STATE_TX_DONE,
    RFLR_STATE_TX_TIMEOUT,
    RFLR_STATE_CAD_INIT,
    RFLR_STATE_CAD_RUNNING,
}tRFLRStates;

/*!
 * SX1276 definitions
 */
#define XTAL_FREQ                                   32000000
#define FREQ_STEP                                   61.03515625

/*!
 * SX1276 Internal registers Address
 */
#define REG_LR_FIFO                                 0x00 
// Common settings
#define REG_LR_OPMODE                               0x01 
//#define REG_LR_BANDSETTING                          0x04
#define REG_LR_FRFMSB                               0x06 
#define REG_LR_FRFMID                               0x07
#define REG_LR_FRFLSB                               0x08 
// Tx settings
#define REG_LR_PACONFIG                             0x09 
#define REG_LR_PARAMP                               0x0A 
#define REG_LR_OCP                                  0x0B 
// Rx settings
#define REG_LR_LNA                                  0x0C 
// LoRa registers
#define REG_LR_FIFOADDRPTR                          0x0D 
#define REG_LR_FIFOTXBASEADDR                       0x0E 
#define REG_LR_FIFORXBASEADDR                       0x0F 
#define REG_LR_FIFORXCURRENTADDR                    0x10 
#define REG_LR_IRQFLAGSMASK                         0x11 
#define REG_LR_IRQFLAGS                             0x12 
#define REG_LR_NBRXBYTES                            0x13 
#define REG_LR_RXHEADERCNTVALUEMSB                  0x14 
#define REG_LR_RXHEADERCNTVALUELSB                  0x15 
#define REG_LR_RXPACKETCNTVALUEMSB                  0x16 
#define REG_LR_RXPACKETCNTVALUELSB                  0x17 
#define REG_LR_MODEMSTAT                            0x18 
#define REG_LR_PKTSNRVALUE                          0x19 
#define REG_LR_PKTRSSIVALUE                         0x1A 
#define REG_LR_RSSIVALUE                            0x1B 
#define REG_LR_HOPCHANNEL                           0x1C 
#define REG_LR_MODEMCONFIG1                         0x1D 
#define REG_LR_MODEMCONFIG2                         0x1E 
#define REG_LR_SYMBTIMEOUTLSB                       0x1F 
#define REG_LR_PREAMBLEMSB                          0x20 
#define REG_LR_PREAMBLELSB                          0x21 
#define REG_LR_PAYLOADLENGTH                        0x22 
#define REG_LR_PAYLOADMAXLENGTH                     0x23 
#define REG_LR_HOPPERIOD                            0x24 
#define REG_LR_FIFORXBYTEADDR                       0x25
#define REG_LR_MODEMCONFIG3                         0x26
// end of documented register in datasheet
// I/O settings
#define REG_LR_DIOMAPPING1                          0x40
#define REG_LR_DIOMAPPING2                          0x41
// Version
#define REG_LR_VERSION                              0x42
// Additional settings
#define REG_LR_PLLHOP                               0x44
#define REG_LR_TCXO                                 0x4B
#define REG_LR_PADAC                                0x4D
#define REG_LR_FORMERTEMP                           0x5B
#define REG_LR_BITRATEFRAC                          0x5D
#define REG_LR_AGCREF                               0x61
#define REG_LR_AGCTHRESH1                           0x62
#define REG_LR_AGCTHRESH2                           0x63
#define REG_LR_AGCTHRESH3                           0x64


/*!
 * SX1276 LoRa bit control definition
 */

/*!
 * RegFifo
 */

/*!
 * RegOpMode
 */
#define RFLR_OPMODE_LONGRANGEMODE_MASK              0x7F 
#define RFLR_OPMODE_LONGRANGEMODE_OFF               0x00 // Default
#define RFLR_OPMODE_LONGRANGEMODE_ON                0x80 

#define RFLR_OPMODE_ACCESSSHAREDREG_MASK            0xBF 
#define RFLR_OPMODE_ACCESSSHAREDREG_ENABLE          0x40 
#define RFLR_OPMODE_ACCESSSHAREDREG_DISABLE         0x00 // Default

#define RFLR_OPMODE_FREQMODE_ACCESS_MASK            0xF7
#define RFLR_OPMODE_FREQMODE_ACCESS_LF              0x08 // Default
#define RFLR_OPMODE_FREQMODE_ACCESS_HF              0x00 

#define RFLR_OPMODE_MASK                            0xF8 
#define RFLR_OPMODE_SLEEP                           0x00  //睡眠模式
#define RFLR_OPMODE_STANDBY                         0x01 // Default
#define RFLR_OPMODE_SYNTHESIZER_TX                  0x02 
#define RFLR_OPMODE_TRANSMITTER                     0x03 
#define RFLR_OPMODE_SYNTHESIZER_RX                  0x04 
#define RFLR_OPMODE_RECEIVER                        0x05 
// LoRa specific modes
#define RFLR_OPMODE_RECEIVER_SINGLE                 0x06 
#define RFLR_OPMODE_CAD                             0x07 

/*!
 * RegBandSetting 
 */
#define RFLR_BANDSETTING_MASK                    0x3F 
#define RFLR_BANDSETTING_AUTO                    0x00 // Default
#define RFLR_BANDSETTING_DIV_BY_1                0x40
#define RFLR_BANDSETTING_DIV_BY_2                0x80
#define RFLR_BANDSETTING_DIV_BY_6                0xC0


/*!
 * RegPaConfig
 */
#define RFLR_PACONFIG_PASELECT_MASK                 0x7F 
#define RFLR_PACONFIG_PASELECT_PABOOST              0x80 
#define RFLR_PACONFIG_PASELECT_RFO                  0x00 // Default

#define RFLR_PACONFIG_MAX_POWER_MASK                0x8F

#define RFLR_PACONFIG_OUTPUTPOWER_MASK              0xF0 
 
/*!
 * RegPaRamp
 */
#define RFLR_PARAMP_TXBANDFORCE_MASK                0xEF 
#define RFLR_PARAMP_TXBANDFORCE_BAND_SEL            0x10 
#define RFLR_PARAMP_TXBANDFORCE_AUTO                0x00 // Default

#define RFLR_PARAMP_MASK                            0xF0 
#define RFLR_PARAMP_3400_US                         0x00 
#define RFLR_PARAMP_2000_US                         0x01 
#define RFLR_PARAMP_1000_US                         0x02
#define RFLR_PARAMP_0500_US                         0x03 
#define RFLR_PARAMP_0250_US                         0x04 
#define RFLR_PARAMP_0125_US                         0x05 
#define RFLR_PARAMP_0100_US                         0x06 
#define RFLR_PARAMP_0062_US                         0x07 
#define RFLR_PARAMP_0050_US                         0x08 
#define RFLR_PARAMP_0040_US                         0x09 // Default
#define RFLR_PARAMP_0031_US                         0x0A 
#define RFLR_PARAMP_0025_US                         0x0B 
#define RFLR_PARAMP_0020_US                         0x0C 
#define RFLR_PARAMP_0015_US                         0x0D 
#define RFLR_PARAMP_0012_US                         0x0E 
#define RFLR_PARAMP_0010_US                         0x0F 

/*!
 * RegOcp
 */
#define RFLR_OCP_MASK                               0xDF 
#define RFLR_OCP_ON                                 0x20 // Default
#define RFLR_OCP_OFF                                0x00   

#define RFLR_OCP_TRIM_MASK                          0xE0
#define RFLR_OCP_TRIM_045_MA                        0x00
#define RFLR_OCP_TRIM_050_MA                        0x01   
#define RFLR_OCP_TRIM_055_MA                        0x02 
#define RFLR_OCP_TRIM_060_MA                        0x03 
#define RFLR_OCP_TRIM_065_MA                        0x04 
#define RFLR_OCP_TRIM_070_MA                        0x05 
#define RFLR_OCP_TRIM_075_MA                        0x06 
#define RFLR_OCP_TRIM_080_MA                        0x07  
#define RFLR_OCP_TRIM_085_MA                        0x08
#define RFLR_OCP_TRIM_090_MA                        0x09 
#define RFLR_OCP_TRIM_095_MA                        0x0A 
#define RFLR_OCP_TRIM_100_MA                        0x0B  // Default
#define RFLR_OCP_TRIM_105_MA                        0x0C 
#define RFLR_OCP_TRIM_110_MA                        0x0D 
#define RFLR_OCP_TRIM_115_MA                        0x0E 
#define RFLR_OCP_TRIM_120_MA                        0x0F 
#define RFLR_OCP_TRIM_130_MA                        0x10
#define RFLR_OCP_TRIM_140_MA                        0x11   
#define RFLR_OCP_TRIM_150_MA                        0x12 
#define RFLR_OCP_TRIM_160_MA                        0x13 
#define RFLR_OCP_TRIM_170_MA                        0x14 
#define RFLR_OCP_TRIM_180_MA                        0x15 
#define RFLR_OCP_TRIM_190_MA                        0x16 
#define RFLR_OCP_TRIM_200_MA                        0x17  
#define RFLR_OCP_TRIM_210_MA                        0x18
#define RFLR_OCP_TRIM_220_MA                        0x19 
#define RFLR_OCP_TRIM_230_MA                        0x1A 
#define RFLR_OCP_TRIM_240_MA                        0x1B

/*!
 * RegLna
 */
#define RFLR_LNA_GAIN_MASK                          0x1F 
#define RFLR_LNA_GAIN_G1                            0x20 // Default
#define RFLR_LNA_GAIN_G2                            0x40 
#define RFLR_LNA_GAIN_G3                            0x60 
#define RFLR_LNA_GAIN_G4                            0x80 
#define RFLR_LNA_GAIN_G5                            0xA0 
#define RFLR_LNA_GAIN_G6                            0xC0 

#define RFLR_LNA_BOOST_LF_MASK                      0xE7 
#define RFLR_LNA_BOOST_LF_DEFAULT                   0x00 // Default
#define RFLR_LNA_BOOST_LF_GAIN                      0x08 
#define RFLR_LNA_BOOST_LF_IP3                       0x10 
#define RFLR_LNA_BOOST_LF_BOOST                     0x18 

#define RFLR_LNA_RXBANDFORCE_MASK                   0xFB 
#define RFLR_LNA_RXBANDFORCE_BAND_SEL               0x04
#define RFLR_LNA_RXBANDFORCE_AUTO                   0x00 // Default

#define RFLR_LNA_BOOST_HF_MASK                      0xFC 
#define RFLR_LNA_BOOST_HF_OFF                       0x00 // Default
#define RFLR_LNA_BOOST_HF_ON                        0x03 

/*!
 * RegFifoAddrPtr
 */
#define RFLR_FIFOADDRPTR                            0x00 // Default

/*!
 * RegFifoTxBaseAddr
 */
#define RFLR_FIFOTXBASEADDR                         0x80 // Default

/*!
 * RegFifoTxBaseAddr
 */
#define RFLR_FIFORXBASEADDR                         0x00 // Default

/*!
 * RegFifoRxCurrentAddr (Read Only)
 */

/*!
 * RegIrqFlagsMask
 */
#define RFLR_IRQFLAGS_RXTIMEOUT_MASK                0x80 
#define RFLR_IRQFLAGS_RXDONE_MASK                   0x40 
#define RFLR_IRQFLAGS_PAYLOADCRCERROR_MASK          0x20 
#define RFLR_IRQFLAGS_VALIDHEADER_MASK              0x10 
#define RFLR_IRQFLAGS_TXDONE_MASK                   0x08 
#define RFLR_IRQFLAGS_CADDONE_MASK                  0x04 
#define RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL_MASK       0x02 
#define RFLR_IRQFLAGS_CADDETECTED_MASK              0x01 

/*!
 * RegIrqFlags
 */
#define RFLR_IRQFLAGS_RXTIMEOUT                     0x80 
#define RFLR_IRQFLAGS_RXDONE                        0x40 
#define RFLR_IRQFLAGS_PAYLOADCRCERROR               0x20 
#define RFLR_IRQFLAGS_VALIDHEADER                   0x10 
#define RFLR_IRQFLAGS_TXDONE                        0x08 
#define RFLR_IRQFLAGS_CADDONE                       0x04 
#define RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL            0x02 
#define RFLR_IRQFLAGS_CADDETECTED                   0x01 



/*!
 * RegFifoRxNbBytes (Read Only)    //
 */

 
 /*!
 * RegRxHeaderCntValueMsb (Read Only)    //
 */
 
 
  /*!
 * RegRxHeaderCntValueLsb (Read Only)    //
 */
 
 
/*!
 * RegRxPacketCntValueMsb (Read Only)    //
 */
 
 
 /*!
 * RegRxPacketCntValueLsb (Read Only)    //
 */
 
 
 /*!
 * RegModemStat (Read Only)    //
 */
#define RFLR_MODEMSTAT_RX_CR_MASK                   0x1F 
#define RFLR_MODEMSTAT_MODEM_STATUS_MASK            0xE0 
 
/*!
 * RegPktSnrValue (Read Only)    //
 */

 
 /*!
 * RegPktRssiValue (Read Only)    //
 */
 
 
/*!
 * RegRssiValue (Read Only)    //
 */

 
 /*!
 * RegModemConfig1
 */
#define RFLR_MODEMCONFIG1_BW_MASK                   0x0F 

#define RFLR_MODEMCONFIG1_BW_7_81_KHZ               0x00 
#define RFLR_MODEMCONFIG1_BW_10_41_KHZ              0x10 
#define RFLR_MODEMCONFIG1_BW_15_62_KHZ              0x20 
#define RFLR_MODEMCONFIG1_BW_20_83_KHZ              0x30 
#define RFLR_MODEMCONFIG1_BW_31_25_KHZ              0x40 
#define RFLR_MODEMCONFIG1_BW_41_66_KHZ              0x50 
#define RFLR_MODEMCONFIG1_BW_62_50_KHZ              0x60 
#define RFLR_MODEMCONFIG1_BW_125_KHZ                0x70 // Default
#define RFLR_MODEMCONFIG1_BW_250_KHZ                0x80 
#define RFLR_MODEMCONFIG1_BW_500_KHZ                0x90 
                                                    
#define RFLR_MODEMCONFIG1_CODINGRATE_MASK           0xF1 
#define RFLR_MODEMCONFIG1_CODINGRATE_4_5            0x02
#define RFLR_MODEMCONFIG1_CODINGRATE_4_6            0x04 // Default
#define RFLR_MODEMCONFIG1_CODINGRATE_4_7            0x06 
#define RFLR_MODEMCONFIG1_CODINGRATE_4_8            0x08 
                                                    
#define RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK       0xFE 
#define RFLR_MODEMCONFIG1_IMPLICITHEADER_ON         0x01 
#define RFLR_MODEMCONFIG1_IMPLICITHEADER_OFF        0x00 // Default

 /*!
 * RegModemConfig2
 */
#define RFLR_MODEMCONFIG2_SF_MASK                   0x0F 
#define RFLR_MODEMCONFIG2_SF_6                      0x60 
#define RFLR_MODEMCONFIG2_SF_7                      0x70 // Default
#define RFLR_MODEMCONFIG2_SF_8                      0x80 
#define RFLR_MODEMCONFIG2_SF_9                      0x90 
#define RFLR_MODEMCONFIG2_SF_10                     0xA0 
#define RFLR_MODEMCONFIG2_SF_11                     0xB0 
#define RFLR_MODEMCONFIG2_SF_12                     0xC0 

#define RFLR_MODEMCONFIG2_TXCONTINUOUSMODE_MASK     0xF7 
#define RFLR_MODEMCONFIG2_TXCONTINUOUSMODE_ON       0x08 
#define RFLR_MODEMCONFIG2_TXCONTINUOUSMODE_OFF      0x00 

#define RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK         0xFB 
#define RFLR_MODEMCONFIG2_RXPAYLOADCRC_ON           0x04 
#define RFLR_MODEMCONFIG2_RXPAYLOADCRC_OFF          0x00 // Default
 
#define RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK       0xFC 
#define RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB            0x00 // Default
                                                    
                                                    
/*!                                                 
 * RegHopChannel (Read Only)                        
 */                                                 
#define RFLR_HOPCHANNEL_PLL_LOCK_TIMEOUT_MASK       0x7F 
#define RFLR_HOPCHANNEL_PLL_LOCK_FAIL               0x80 
#define RFLR_HOPCHANNEL_PLL_LOCK_SUCCEED            0x00 // Default
                                                    
#define RFLR_HOPCHANNEL_PAYLOAD_CRC16_MASK          0xBF
#define RFLR_HOPCHANNEL_PAYLOAD_CRC16_ON            0x40
#define RFLR_HOPCHANNEL_PAYLOAD_CRC16_OFF           0x00 // Default

#define RFLR_HOPCHANNEL_CHANNEL_MASK                0x3F 


/*!
 * RegSymbTimeoutLsb
 */
#define RFLR_SYMBTIMEOUTLSB_SYMBTIMEOUT             0x64 // Default

/*!
 * RegPreambleLengthMsb
 */
#define RFLR_PREAMBLELENGTHMSB                      0x00 // Default

/*!
 * RegPreambleLengthLsb
 */
#define RFLR_PREAMBLELENGTHLSB                      0x08 // Default

/*!
 * RegPayloadLength
 */
#define RFLR_PAYLOADLENGTH                          0x0E // Default

/*!
 * RegPayloadMaxLength
 */
#define RFLR_PAYLOADMAXLENGTH                       0xFF // Default

/*!
 * RegHopPeriod
 */
#define RFLR_HOPPERIOD_FREQFOPPINGPERIOD            0x00 // Default


/*!
 * RegDioMapping1
 */
#define RFLR_DIOMAPPING1_DIO0_MASK                  0x3F
#define RFLR_DIOMAPPING1_DIO0_00                    0x00  // Default
#define RFLR_DIOMAPPING1_DIO0_01                    0x40
#define RFLR_DIOMAPPING1_DIO0_10                    0x80
#define RFLR_DIOMAPPING1_DIO0_11                    0xC0

#define RFLR_DIOMAPPING1_DIO1_MASK                  0xCF
#define RFLR_DIOMAPPING1_DIO1_00                    0x00  // Default
#define RFLR_DIOMAPPING1_DIO1_01                    0x10
#define RFLR_DIOMAPPING1_DIO1_10                    0x20
#define RFLR_DIOMAPPING1_DIO1_11                    0x30

#define RFLR_DIOMAPPING1_DIO2_MASK                  0xF3
#define RFLR_DIOMAPPING1_DIO2_00                    0x00  // Default
#define RFLR_DIOMAPPING1_DIO2_01                    0x04
#define RFLR_DIOMAPPING1_DIO2_10                    0x08
#define RFLR_DIOMAPPING1_DIO2_11                    0x0C

#define RFLR_DIOMAPPING1_DIO3_MASK                  0xFC
#define RFLR_DIOMAPPING1_DIO3_00                    0x00  // Default
#define RFLR_DIOMAPPING1_DIO3_01                    0x01
#define RFLR_DIOMAPPING1_DIO3_10                    0x02
#define RFLR_DIOMAPPING1_DIO3_11                    0x03

/*!
 * RegDioMapping2
 */
#define RFLR_DIOMAPPING2_DIO4_MASK                  0x3F
#define RFLR_DIOMAPPING2_DIO4_00                    0x00  // Default
#define RFLR_DIOMAPPING2_DIO4_01                    0x40
#define RFLR_DIOMAPPING2_DIO4_10                    0x80
#define RFLR_DIOMAPPING2_DIO4_11                    0xC0

#define RFLR_DIOMAPPING2_DIO5_MASK                  0xCF
#define RFLR_DIOMAPPING2_DIO5_00                    0x00  // Default
#define RFLR_DIOMAPPING2_DIO5_01                    0x10
#define RFLR_DIOMAPPING2_DIO5_10                    0x20
#define RFLR_DIOMAPPING2_DIO5_11                    0x30

#define RFLR_DIOMAPPING2_MAP_MASK                   0xFE
#define RFLR_DIOMAPPING2_MAP_PREAMBLEDETECT         0x01
#define RFLR_DIOMAPPING2_MAP_RSSI                   0x00  // Default

/*!
 * RegVersion (Read Only)
 */

/*!
 * RegAgcRef
 */

/*!
 * RegAgcThresh1
 */

/*!
 * RegAgcThresh2
 */

/*!
 * RegAgcThresh3
 */
 
/*!
 * RegFifoRxByteAddr (Read Only)
 */
 
/*!
 * RegPllHop
 */
#define RFLR_PLLHOP_FASTHOP_MASK                    0x7F
#define RFLR_PLLHOP_FASTHOP_ON                      0x80
#define RFLR_PLLHOP_FASTHOP_OFF                     0x00 // Default

/*!
 * RegTcxo
 */
#define RFLR_TCXO_TCXOINPUT_MASK                    0xEF
#define RFLR_TCXO_TCXOINPUT_ON                      0x10
#define RFLR_TCXO_TCXOINPUT_OFF                     0x00  // Default

/*!
 * RegPaDac
 */
#define RFLR_PADAC_20DBM_MASK                       0xF8
#define RFLR_PADAC_20DBM_ON                         0x07
#define RFLR_PADAC_20DBM_OFF                        0x04  // Default

/*!
 * RegPll
 */
#define RFLR_PLL_BANDWIDTH_MASK                     0x3F
#define RFLR_PLL_BANDWIDTH_75                       0x00
#define RFLR_PLL_BANDWIDTH_150                      0x40
#define RFLR_PLL_BANDWIDTH_225                      0x80
#define RFLR_PLL_BANDWIDTH_300                      0xC0  // Default

/*!
 * RegPllLowPn
 */
#define RFLR_PLLLOWPN_BANDWIDTH_MASK                0x3F
#define RFLR_PLLLOWPN_BANDWIDTH_75                  0x00
#define RFLR_PLLLOWPN_BANDWIDTH_150                 0x40
#define RFLR_PLLLOWPN_BANDWIDTH_225                 0x80
#define RFLR_PLLLOWPN_BANDWIDTH_300                 0xC0  // Default

/*!
 * RegModemConfig3
 */
#define RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_MASK  0xF7 
#define RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_ON    0x08 
#define RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_OFF   0x00 // Default

#define RFLR_MODEMCONFIG3_AGCAUTO_MASK              0xFB 
#define RFLR_MODEMCONFIG3_AGCAUTO_ON                0x04 // Default 
#define RFLR_MODEMCONFIG3_AGCAUTO_OFF               0x00 

/*!
 * RegFormerTemp
 */

typedef struct sSX1276LR
{
    uint8_t RegFifo;                                // 0x00 
    // Common settings
    uint8_t RegOpMode;                              // 0x01 
    
    uint8_t RegTestReserved02[0x06 - 0x02];         // 0x02-0x05
   // uint8_t RegRes02;                               // 0x02 
   // uint8_t RegRes03;                               // 0x03 
   // uint8_t RegBandSetting;                         // 0x04 
   // uint8_t RegRes05;                               // 0x05 
    
    uint8_t RegFrfMsb;                              // 0x06 
    uint8_t RegFrfMid;                              // 0x07 
    uint8_t RegFrfLsb;                              // 0x08 
    // Tx settings
    uint8_t RegPaConfig;                            // 0x09 
    uint8_t RegPaRamp;                              // 0x0A 
    uint8_t RegOcp;                                 // 0x0B 
    // Rx settings
    uint8_t RegLna;                                 // 0x0C 
    
    // LoRa registers
    uint8_t RegFifoAddrPtr;                         // 0x0D 
    uint8_t RegFifoTxBaseAddr;                      // 0x0E 
    uint8_t RegFifoRxBaseAddr;                      // 0x0F 
    uint8_t RegFifoRxCurrentAddr;                   // 0x10 
    uint8_t RegIrqFlagsMask;                        // 0x11 
    uint8_t RegIrqFlags;                            // 0x12 
    uint8_t RegNbRxBytes;                           // 0x13 
    uint8_t RegRxHeaderCntValueMsb;                 // 0x14 
    uint8_t RegRxHeaderCntValueLsb;                 // 0x15 
    uint8_t RegRxPacketCntValueMsb;                 // 0x16 
    uint8_t RegRxPacketCntValueLsb;                 // 0x17 
    uint8_t RegModemStat;                           // 0x18 
    uint8_t RegPktSnrValue;                         // 0x19 
    uint8_t RegPktRssiValue;                        // 0x1A 
    uint8_t RegRssiValue;                           // 0x1B 
    uint8_t RegHopChannel;                          // 0x1C 
    uint8_t RegModemConfig1;                        // 0x1D 
    uint8_t RegModemConfig2;                        // 0x1E 
    uint8_t RegSymbTimeoutLsb;                      // 0x1F 
    uint8_t RegPreambleMsb;                         // 0x20 
    uint8_t RegPreambleLsb;                         // 0x21 
    uint8_t RegPayloadLength;                       // 0x22 
    uint8_t RegMaxPayloadLength;                    // 0x23 
    uint8_t RegHopPeriod;                           // 0x24 
    uint8_t RegFifoRxByteAddr;                      // 0x25
    uint8_t RegModemConfig3;                        // 0x26
    uint8_t RegTestReserved27[0x31 - 0x27];         // 0x27-0x30
    //void SX1276LoRaSetNbTrigPeaks( uint8_t value )用到
    uint8_t RegTestReserved31;                      // 0x31
    uint8_t RegTestReserved32[0x40 - 0x32];         // 0x32-0x3F
    // I/O settings                
    uint8_t RegDioMapping1;                         // 0x40 
    uint8_t RegDioMapping2;                         // 0x41 
    // Version
    uint8_t RegVersion;                             // 0x42
    
    uint8_t RegTestReserved43[0x4B - 0x43];         // 0x43-0x4A 
    uint8_t RegTcxo;                                // 0x4B
    uint8_t RegTestReserved4C;                      // 0x4C
    uint8_t RegPaDac;                               // 0x4D
    uint8_t RegTestReserved4E[0x5B - 0x4E];         // 0x4E-0x5A
    uint8_t RegFormerTemp;                          // 0x5B
    uint8_t RegTestReserved5C[0x61 - 0x5C];         // 0x5C-0x60
    // Additional settings
    uint8_t RegAgcRef;                              // 0x61                             
    uint8_t RegAgcThresh1;                          // 0x62                          
    uint8_t RegAgcThresh2;                          // 0x63
    uint8_t RegAgcThresh3;                          // 0x64
    uint8_t RegTestReserved65[0x70 - 0x65];         // 0x65-0x6F
    uint8_t RegPll;                                 // 0x70
}tSX1276LR;

extern tSX1276LR* SX1276LR;

typedef enum
{
    NORMAL,                //正常      
    PARAMETER_INVALID,    //参数不可用
    SPI_READCHECK_WRONG,  //SPI出错
}tSX127xError;           //定义出错枚举

typedef enum
{
  // LORA [0: 7.8 kHz, 1: 10.4 kHz, 2: 15.6 kHz, 3: 20.8 kHz, 4: 31.2 kHz,
        // 5: 41.6 kHz, 6: 62.5 kHz, 7: 125 kHz, 8: 250 kHz, 9: 500 kHz, other: Reserved]
    BW500KHZ = 0x90,      
    BW250KHZ = 0x80,
    BW125KHZ = 0x70,
    BW62_50KHZ = 0x60,
    BW41_66KHZ = 0x50,
    BW31_25KHZ = 0x40,
    BW20_83KHZ = 0x30,
    BW15_62KHZ = 0x20,
    BW10_41KHZ = 0x10,
    BW7_81KHZ = 0x00,
}t_BandWidth;      //定义带宽枚举
typedef enum{
    // LORA [6: 64, 7: 128, 8: 256, 9: 512, 10: 1024, 11: 2048, 12: 4096  chips]
    SF12 = 0xC0,
    SF11 = 0xB0,
    SF10 = 0xA0,
    SF09 = 0x90,
    SF08 = 0x80,
    SF07 = 0x70,
    SF06 = 0x60,
}t_SpreadingFactor;
typedef enum{
    // LORA [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8] 
    CR_4_8 = 0x08,
    CR_4_7 = 0x06,
    CR_4_6 = 0x04,
    CR_4_5 = 0x02,
}t_CodingRate;
typedef struct S_LoRaConfig
{
    uint32_t LoRa_Freq;          //收发频率       
    t_BandWidth BandWidth;              //带宽        
    t_SpreadingFactor SpreadingFactor;            
    t_CodingRate CodingRate;
    int8_t PowerCfig; //0~15,输出功率 = 2+PowerReg,具体参考数据手册
                     //如果开启MaxPower，PowerReg = 15时，输出功率 = 19±1dBm
    bool MaxPowerOn;                    // [false: OFF, true: ON]
    bool CRCON;                         // [false: OFF, true: ON]
    bool ExplicitHeaderOn;              // [false: OFF, true: ON]
    uint8_t PayloadLength;              //1~127  
}S_LoRaConfig;

typedef struct S_LoRaPara//RF???,???????
{
  uint8_t* BufferPointer;
  uint16_t PreambleLength;
  uint8_t PayloadLength;              //1~127
  int16_t Packet_RSSI;
	int16_t Packet_SNR;
  int16_t Current_RSSI;
}S_LoRaPara;

extern S_LoRaConfig G_LoRaConfig;
extern S_LoRaPara G_LoRaPara;
extern float G_BandWidthKHz;//计算Symbol周期使用
extern float G_TsXms;//Symbol周期，单位ms

/***********************************************FSK****************************************************/
#define REG_FSK_FIFO                                 0x00 
// Common settings
#define REG_FSK_OPMODE                               0x01 
#define REG_FSK_BITRATEMSB                           0x02
#define REG_FSK_BITRATELSB                           0x03
#define REG_FSK_FDEVMSB                              0x04
#define REG_FSK_FDEVLSB                              0x05
#define REG_FSK_FRFMSB                               0x06 
#define REG_FSK_FRFMID                               0x07
#define REG_FSK_FRFLSB                               0x08 
// Tx settings
#define REG_FSK_PACONFIG                             0x09 
#define REG_FSK_PARAMP                               0x0A 
#define REG_FSK_OCP                                  0x0B 
// Rx settings
#define REG_FSK_LNA                                  0x0C 
// LoRa registers
#define REG_FSK_RXCONFIG                             0x0D 
#define REG_FSK_RSSICONFIG                           0x0E 
#define REG_FSK_RSSICOLLISION                        0x0F 
#define REG_FSK_RSSITHRESH                           0x10 
#define REG_FSK_RSSIVALUE                            0x11 
#define REG_FSK_RXBW                                 0x12 
#define REG_FSK_AFCBW                                0x13 
#define REG_FSK_OOKPEAK                              0x14 
#define REG_FSK_OOKFIX                               0x15 
#define REG_FSK_OOKAVG                               0x16 
#define REG_FSK_RES17                                0x17 
#define REG_FSK_RES18                                0x18 
#define REG_FSK_RES19                                0x19 
#define REG_FSK_AFCFEI                               0x1A 
#define REG_FSK_AFCMSB                               0x1B 
#define REG_FSK_AFCLSB                               0x1C 
#define REG_FSK_FEIMSB                               0x1D 
#define REG_FSK_FEILSB                               0x1E 
#define REG_FSK_PREAMBLEDETECT                       0x1F 
#define REG_FSK_RXTIMEOUT1                           0x20 
#define REG_FSK_RXTIMEOUT2                           0x21 
#define REG_FSK_RXTIMEOUT3                           0x22 
#define REG_FSK_RXDELAY                              0x23 
#define REG_FSK_OSC                                  0x24 
#define REG_FSK_PREAMBLEMSB                          0x25
#define REG_FSK_PREAMBLELSB                          0x26

#define REG_FSK_SYNCCONFIG                           0x27 
#define REG_FSK_SYNCVALUE1                           0x28 
#define REG_FSK_SYNCVALUE2                           0x29 
#define REG_FSK_SYNCVALUE3                           0x2A 
#define REG_FSK_SYNCVALUE4                           0x2B 
#define REG_FSK_SYNCVALUE5                           0x2C
#define REG_FSK_SYNCVALUE6                           0x2D
#define REG_FSK_SYNCVALUE7                           0x2E 
#define REG_FSK_SYNCVALUE8                           0x2F 
#define REG_FSK_PACKETCONFIG1                        0x30 
#define REG_FSK_PACKETCONFIG2                        0x31 
#define REG_FSK_PAYLOADLENGTH                        0x32 
#define REG_FSK_NODEADRS                             0x33
#define REG_FSK_BROADCASTADRS                        0x34
#define REG_FSK_FIFOTHRESH                           0x35 
#define REG_FSK_SEQCONFIG1                           0x36 
#define REG_FSK_SEQCONFIG2                           0x37 
#define REG_FSK_TIMERRESOL                           0x38 
#define REG_FSK_TIMER1COEF                           0x39 
#define REG_FSK_TIMER2COEF                           0x3A
#define REG_FSK_IMAGECAL                             0x3B
#define REG_FSK_TEMP                                 0x3C
#define REG_FSK_LOWBAT                               0x3D 
#define REG_FSK_IRQFLAGS1                            0x3E 
#define REG_FSK_IRQFLAGS2                            0x3F 


// end of documented register in datasheet
// I/O settings
#define REG_FSK_DIOMAPPING1                          0x40
#define REG_FSK_DIOMAPPING2                          0x41
// Version
#define REG_FSK_VERSION                              0x42
#define REG_FSK_TEST43                               0x43
// Additional settings
#define REG_FSK_PLLHOP                               0x44
#define REG_FSK_TEST45                               0x45
#define REG_FSK_TEST46                               0x46
#define REG_FSK_TEST47                               0x47
#define REG_FSK_TEST48                               0x48
#define REG_FSK_TEST49                               0x49
#define REG_FSK_TEST4A                               0x4A
#define REG_FSK_TCXO                                 0x4B
#define REG_FSK_TEST4C                               0x4C
#define REG_FSK_PADAC                                0x4D
#define REG_FSK_TEST4E                               0x4E
#define REG_FSK_TEST4F                               0x4F
#define REG_FSK_TEST50                               0x50
#define REG_FSK_TEST51                               0x51
#define REG_FSK_TEST52                               0x52
#define REG_FSK_TEST53                               0x53
#define REG_FSK_TEST54                               0x54
#define REG_FSK_TEST55                               0x55
#define REG_FSK_TEST56                               0x56
#define REG_FSK_TEST57                               0x57
#define REG_FSK_TEST58                               0x58
#define REG_FSK_TEST59                               0x59
#define REG_FSK_TEST5A                               0x5A
#define REG_FSK_FORMERTEMP                           0x5B
#define REG_FSK_TEST5C                               0x5C
#define REG_FSK_BITRATEFRAC                          0x5D
#define REG_FSK_TEST5E                               0x5E
#define REG_FSK_TEST5F                               0x5F
#define REG_FSK_TEST60                               0x60
#define REG_FSK_AGCREF                               0x61
#define REG_FSK_AGCTHRESH1                           0x62
#define REG_FSK_AGCTHRESH2                           0x63
#define REG_FSK_AGCTHRESH3                           0x64
#define REG_FSK_TEST65                               0x65
#define REG_FSK_TEST66                               0x66
#define REG_FSK_TEST67                               0x67
#define REG_FSK_TEST68                               0x68
#define REG_FSK_TEST69                               0x69
#define REG_FSK_TEST6A                               0x6A
#define REG_FSK_TEST6B                               0x6B
#define REG_FSK_TEST6C                               0x6C
#define REG_FSK_TEST6D                               0x6D
#define REG_FSK_TEST6E                               0x6E
#define REG_FSK_TEST6F                               0x6F
#define REG_FSK_PLL                                  0x70
#define REG_FSK_TEST71                               0x71
#define REG_FSK_TEST72                               0x72
#define REG_FSK_TEST73                               0x73
#define REG_FSK_TEST74                               0x74
#define REG_FSK_TEST75                               0x75
#define REG_FSK_TEST76                               0x76
#define REG_FSK_TEST77                               0x77
#define REG_FSK_TEST78                               0x78
#define REG_FSK_TEST79                               0x79
#define REG_FSK_TEST7A                               0x7A
#define REG_FSK_TEST7B                               0x7B
#define REG_FSK_TEST7C                               0x7C
#define REG_FSK_TEST7D                               0x7D
#define REG_FSK_TEST7E                               0x7E
#define REG_FSK_TEST7F                               0x7F


#define RFFSK_OPMODE_MASK                            0xF8 
#define RFFSK_OPMODE_SLEEP                           0x00 
#define RFFSK_OPMODE_STANDBY                         0x01 // Default
#define RFFSK_OPMODE_SYNTHESIZER_TX                  0x02 
#define RFFSK_OPMODE_TRANSMITTER                     0x03 
#define RFFSK_OPMODE_SYNTHESIZER_RX                  0x04 
#define RFFSK_OPMODE_RECEIVER                        0x05 

//#define RFLR_DIOMAPPING1_DIO0_MASK                  0x3F
#define RFFSK_DIOMAPPING1_DIO0_00                    0x00  // Default
#define RFFSK_DIOMAPPING1_DIO0_01                    0x40
#define RFFSK_DIOMAPPING1_DIO0_10                    0x80
#define RFFSK_DIOMAPPING1_DIO0_11                    0xC0
//
//#define RFLR_DIOMAPPING1_DIO1_MASK                  0xCF
#define RFFSK_DIOMAPPING1_DIO1_00                    0x00  // Default
#define RFFSK_DIOMAPPING1_DIO1_01                    0x10
#define RFFSK_DIOMAPPING1_DIO1_10                    0x20
#define RFFSK_DIOMAPPING1_DIO1_11                    0x30
//
//#define RFLR_DIOMAPPING1_DIO2_MASK                  0xF3
#define RFFSK_DIOMAPPING1_DIO2_00                    0x00  // Default
#define RFFSK_DIOMAPPING1_DIO2_01                    0x04
#define RFFSK_DIOMAPPING1_DIO2_10                    0x08
#define RFFSK_DIOMAPPING1_DIO2_11                    0x0C
//
//#define RFLR_DIOMAPPING1_DIO3_MASK                  0xFC
#define RFFSK_DIOMAPPING1_DIO3_00                    0x00  // Default
#define RFFSK_DIOMAPPING1_DIO3_01                    0x01
#define RFFSK_DIOMAPPING1_DIO3_10                    0x02
#define RFFSK_DIOMAPPING1_DIO3_11                    0x03
//
///*!
// * RegDioMapping2
// */
//#define RFLR_DIOMAPPING2_DIO4_MASK                  0x3F
#define RFFSK_DIOMAPPING2_DIO4_00                    0x00  // Default
#define RFFSK_DIOMAPPING2_DIO4_01                    0x40
#define RFFSK_DIOMAPPING2_DIO4_10                    0x80
#define RFFSK_DIOMAPPING2_DIO4_11                    0xC0
//
//#define RFLR_DIOMAPPING2_DIO5_MASK                  0xCF
#define RFFSK_DIOMAPPING2_DIO5_00                    0x00  // Default
#define RFFSK_DIOMAPPING2_DIO5_01                    0x10
#define RFFSK_DIOMAPPING2_DIO5_10                    0x20
#define RFFSK_DIOMAPPING2_DIO5_11                    0x30
//


typedef struct {
    unsigned char addr;
    unsigned char val;
}_SX12XX_REG;


//////////////////////////////////////////////////////////////////////////////////
//// ???? : ????
//// ??     : 
//////////////////////////////////////////////////////////////////////////////////
static const _SX12XX_REG LSD_RFregConfig[] = {         // 
     
    {REG_FSK_FIFO,          0x00},           
    {REG_FSK_OPMODE,        0x09},           
    {REG_FSK_BITRATEMSB,    0X0D},//9600bps
    {REG_FSK_BITRATELSB,    0X05}, 
    {REG_FSK_FDEVMSB,       0x01},// 19KDEV
    {REG_FSK_FDEVLSB,       0x37},    
    {REG_FSK_FRFMSB,        0X75}, //471MHz
    {REG_FSK_FRFMID,        0XC0},           
    {REG_FSK_FRFLSB,        0X00},           
    {REG_FSK_PACONFIG,      0X8F},           
    {REG_FSK_PARAMP,        0x19},
    {REG_FSK_OCP,           0x3B},  //?????? 240mA         
    {REG_FSK_LNA,           0X20},  //????        
    {REG_FSK_RXCONFIG,      0X08},           
    {REG_FSK_RSSICONFIG,    0X02},                      
    {REG_FSK_RSSICOLLISION, 0x0A},           
    {REG_FSK_RSSITHRESH ,   0XFF},           
    {REG_FSK_RSSIVALUE,     0X00},           
    {REG_FSK_RXBW,          0x03},  //0X03,RXBW=62.5K
    {REG_FSK_AFCBW,         0x0B},
    {REG_FSK_OOKPEAK,       0x28},
    {REG_FSK_OOKFIX,        0x0C},      
    {REG_FSK_OOKAVG ,       0X12},
    {REG_FSK_AFCFEI,        0x00}, //  
    {REG_FSK_AFCMSB,        0X00},           
    {REG_FSK_AFCLSB,        0X00},           
    {REG_FSK_FEIMSB,        0X00},           
    {REG_FSK_FEILSB,        0X00},           
    {REG_FSK_PREAMBLEDETECT,0X40},        
    {REG_FSK_RXTIMEOUT1,    0X00},     
    {REG_FSK_RXTIMEOUT2,    0X00},                      
    {REG_FSK_RXTIMEOUT3 ,   0X00},         
    {REG_FSK_RXDELAY ,      0X00},           
    {REG_FSK_OSC ,          0X05},           
    {REG_FSK_PREAMBLEMSB,   0X00}, //???????        
    {REG_FSK_PREAMBLELSB,   0X04},            
    {REG_FSK_SYNCCONFIG,    0X33}, //?????:0x55,???????,?????3+1             
    {REG_FSK_SYNCVALUE1,    0XD3}, //???????
    {REG_FSK_SYNCVALUE2,    0X91}, //???????
    {REG_FSK_SYNCVALUE3,    0XD3}, //???????    
    {REG_FSK_SYNCVALUE4,    0X91}, //???????     
    {REG_FSK_SYNCVALUE5,    0X00},           
    {REG_FSK_SYNCVALUE6,    0X00},           
    {REG_FSK_SYNCVALUE7,    0X00},           
    {REG_FSK_SYNCVALUE8,    0X00},           
    {REG_FSK_PACKETCONFIG1, 0X90},    //0X10 ?????+CRC   ,0x90 ?????+CRC  0x51 ??+CRC(IBM)+??  0x11 ??+CRC(IBM),//0x09??CRC
    {REG_FSK_PACKETCONFIG2, 0X40},            
    {REG_FSK_PAYLOADLENGTH, 0X20},   //??????    0X20????????       
    {REG_FSK_NODEADRS,      0X00},           
    {REG_FSK_BROADCASTADRS, 0X00},           
    {REG_FSK_FIFOTHRESH,    0X8F},           
    {REG_FSK_SEQCONFIG1,    0X00},        
    {REG_FSK_SEQCONFIG2,    0X00},                       
    {REG_FSK_TIMERRESOL,    0X00},                       
    {REG_FSK_TIMER1COEF,    0XF5},                       
    {REG_FSK_TIMER2COEF,    0X20},                       
    {REG_FSK_IMAGECAL,      0X82},                       
    {REG_FSK_TEMP,          0X00},                       
    {REG_FSK_LOWBAT,        0X02},                       
    {REG_FSK_IRQFLAGS1,     0X80},                       
    {REG_FSK_IRQFLAGS2,     0X40},                       
    {REG_FSK_DIOMAPPING1,   0X00},                       
    {REG_FSK_DIOMAPPING2,   0X00}, 
    {REG_FSK_PLLHOP,        0X2D}, 
    {REG_FSK_TCXO,          0X09},                       
    {REG_FSK_PADAC,         0X87},                       
    {REG_FSK_FORMERTEMP,    0X00},                       
    {REG_FSK_BITRATEFRAC,   0X00},    //???????                    
    {REG_FSK_AGCREF,        0X19}, 
    {REG_FSK_AGCTHRESH1,    0X0C},         
    {REG_FSK_AGCTHRESH2,    0x4B},
    {REG_FSK_AGCTHRESH3,    0xCC},
    {REG_FSK_PLL,           0xD0}, 
};
/*******************************************FSK End*********************************************************************/

void SX127X_Reset(void);
void SX127X_InitIo(void);
void SX127XWriteRxTx( bool txEnable );
void DIO0_EnableInterrupt(void);
void DIO0_DisableInterrupt(void);
GPIO_PinState DIO0_GetState(void);
bool LoRaConfig_Check(void);
tSX127xError SX127X_Lora_init(void);
void SX127X_TxPacket(uint8_t*data);
void SX127X_StartRx(void);
int16_t SX127X_Current_RSSI(void);
void SX127X_RxPacket(uint8_t*cbuf);
void SX127X_SleepMode(void);
void SX127X_StandbyMode(void);
tSX127xError SX127X_FreqSet(void);
tSX127xError SX127X_PoutSet(void);
void SX127X_Write( uint8_t addr, uint8_t data );
void SX127X_Read( uint8_t addr, uint8_t *data );
void SX127X_WriteFifo( uint8_t *buffer, uint8_t size );
void SX127X_ReadFifo( uint8_t *buffer, uint8_t size );

void SX127X_CADinit(void);
void SX127X_WORInit(void);
void SX127X_CAD_Sample(void);
void SX127X_WOR_Execute(uint8_t cclen);
void SX127X_WOR_Exit(void);
void SX127X_Awake(uint8_t*cbuf, uint16_t Preamble_Length);

/*******************FSK*********************************/
unsigned char SX127x_Fsk_init(void);
void LSD_RF_StandbyMode_FSK(void);
void LSD_RF_SleepMode_FSK(void);
void LSD_RF_Config(void);
void LSD_RF_ClearFIFO(void);
void LSD_RF_SendPacket_FSK(uint8_t *txBuffer, uint8_t size);
void LSD_RF_RXmode_FSK(void);
void LSD_RF_RxPacket_FSK(uint8_t *cRxBuf, uint8_t *cLength);

//*****************************************************************************************
#endif
