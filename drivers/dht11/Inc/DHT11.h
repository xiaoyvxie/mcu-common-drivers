/**
 * @file DHT11.h
 * @brief DHT11温湿度传感器驱动头文件
 * @author xiaoyvxie ("xiaoyvxie@gmail.com" or "xiaoyvxie@yeah.net")
 * @version V1.1
 * @date 2026-02-25
 * @details 提供了DHT11温湿度传感器的初始化、数据读取等功能的函数和宏定义。
 */
#ifndef __DHT11_H__
#define __DHT11_H__

/**
 * @brief 头文件包含
 * @note 包含必要的头文件以支持DHT11驱动
 */
// ------------------------------------------------------------------>

#include "config.h"
#include "Delay.h"
#include "gpio.h"

// ------------------------------------------------------------------<

/**
 * @brief DHT11延时函数定义
 * @note 用于DHT11操作时序控制
 */
// ------------------------------------------------------------------>

#define DHT11_DELAY_1S()            Delay_ms(1000)      ///< 延时1秒
#define DHT11_DELAY_20MS()          Delay_ms(20)        ///< 延时20毫秒
#define DHT11_DELAY_20US()          Delay_20us()        ///< 延时20微秒
#define DHT11_DELAY_10US()          Delay_10us()        ///< 延时10微秒

// ------------------------------------------------------------------<

/**
 * @brief DHT11 GPIO函数定义
 * @note 定义了DHT11传感器的GPIO操作函数
 */
// ------------------------------------------------------------------>

void DHT11_Gpio_Write_Init();     
void DHT11_Gpio_Read_Init();     
#define DHT11_GPIO_WRITE(value)     gpio_write(GPIO_B, PIN_0, value)    ///< GPIO写操作
#define DHT11_GPIO_READ()           gpio_read(GPIO_B, PIN_0)            ///< GPIO读操作
#define DHT11_GPIO_PIN_SET          GPIO_PIN_SET                        ///< 高电平
#define DHT11_GPIO_PIN_RESET        GPIO_PIN_RESET                      ///< 低电平

// ------------------------------------------------------------------<

/**
 * @brief DHT11返回值定义
 * @note 定义了DHT11传感器操作的返回状态
 */
// ------------------------------------------------------------------>

#define DHT11_OK                    0       ///< 操作成功
#define DHT11_ERROR                 0xff    ///< 操作失败

// ------------------------------------------------------------------<

/**
 * @brief DHT11函数声明
 * @note 声明了DHT11传感器的所有操作函数
 */
// ------------------------------------------------------------------>

void DHT11_Init();                                                                  
void DHT11_StartSignal();                                                               
uint8_t DHT11_WaitResponse();                                                            
uint8_t DHT11_ReadBit();                                                                     
uint8_t DHT11_ReadByte();                                                              
uint8_t DHT11_ReadData(uint8_t *temperature_sign,  uint8_t *temperature_int, uint8_t *temperature_dec, uint8_t *humidity_int);

// ------------------------------------------------------------------<

#endif