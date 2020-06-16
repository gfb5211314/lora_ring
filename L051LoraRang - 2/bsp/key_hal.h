#ifndef  __KEY_HAL_H__
#define  __KEY_HAL_H__



#include "main.h"

uint8_t get_key1(void);

//获得key2键值
uint8_t get_key2(void);

//获得key3键值
uint8_t get_key3(void);

//获得rang键值
uint8_t get_rang_key(void);


//扫描按键驱动层
uint8_t scan_key(void);

#endif

