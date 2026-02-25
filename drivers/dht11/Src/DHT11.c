/**
 * @file DHT11.c
 * @brief DHT11温湿度传感器驱动源文件
 * @author xiaoyvxie ("xiaoyvxie@gmail.com" or "xiaoyvxie@yeah.net")
 * @version V1.1
 * @date 2026-02-25
 * @details 实现了DHT11温湿度传感器的初始化、数据读取等功能。
 */

#include "DHT11.h"

/**
 * @brief 初始化DHT11 GPIO为输出模式
 * @param 无
 * @return 无
 * @note 配置DHT11传感器的GPIO引脚为输出模式
 */
void DHT11_Gpio_Write_Init() {

}


/**
 * @brief 初始化DHT11 GPIO为输入模式
 * @param 无
 * @return 无
 * @note 配置DHT11传感器的GPIO引脚为输入模式
 */
void DHT11_Gpio_Read_Init() {

}


/**
 * @brief 初始化DHT11模块
 * @param 无
 * @return 无
 * @note 初始化DHT11传感器，准备进行数据读取
 * @details 执行以下初始化步骤:
 *          1. 延时1秒，等待DHT11模块启动
 *          2. 初始化引脚为输出模式
 *          3. 释放总线，将GPIO设置为高电平
 */
void DHT11_Init() {
    DHT11_DELAY_1S();           // 启动延迟，等待DHT11模块启动
    DHT11_Gpio_Write_Init();    // 初始化引脚为输出模式
    DHT11_GPIO_WRITE(DHT11_GPIO_PIN_SET);   // 释放总线
}


/**
 * @brief 发送DHT11启动信号
 * @param 无
 * @return 无
 * @note 向DHT11传感器发送启动信号，触发数据传输
 * @details 执行以下操作:
 *          1. 拉低总线20毫秒
 *          2. 释放总线，等待DHT11响应
 */
void DHT11_StartSignal() {
    DHT11_GPIO_WRITE(DHT11_GPIO_PIN_RESET);     
    DHT11_DELAY_20MS();                         // 总线拉低20ms
    DHT11_GPIO_WRITE(DHT11_GPIO_PIN_SET);       // 释放总线
}


/**
 * @brief 等待DHT11响应信号
 * @param 无
 * @return uint8_t - 操作结果
 * @retval DHT11_OK - 响应成功
 * @retval DHT11_ERROR - 响应失败
 * @note 等待并检测DHT11传感器的响应信号
 * @details 执行以下操作:
 *          1. 初始化引脚为输入模式
 *          2. 等待DHT11低电平响应信号
 *          3. 等待DHT11响应信号结束
 *          4. 等待DHT11高电平响应信号结束
 */
uint8_t DHT11_WaitResponse() {
    uint8_t wait_count = 0;
    DHT11_Gpio_Read_Init();
    
    // 等待DHT11低电平响应信号
    while (DHT11_GPIO_READ() == DHT11_GPIO_PIN_SET) {
        DHT11_DELAY_20US();
        if (++wait_count > 1) {
            return DHT11_ERROR;
        }
    }
    
    wait_count = 0;
    // 等待dht11响应信号结束
    while (DHT11_GPIO_READ() == DHT11_GPIO_PIN_RESET) {
        DHT11_DELAY_20US();
        if (++wait_count > 5) {
            return DHT11_ERROR;
        }
    }
    
    wait_count = 0;
    // 等待dht11高电频响应信号结束
    while(DHT11_GPIO_READ() == DHT11_GPIO_PIN_SET) {
        DHT11_DELAY_20US();
        if (++wait_count > 5) {
            return DHT11_ERROR;
        }
    }
    
    return DHT11_OK;
}


/**
 * @brief 读取DHT11数据位
 * @param 无
 * @return uint8_t - 读取的数据位
 * @retval DHT11_GPIO_PIN_SET - 数据位为1
 * @retval DHT11_GPIO_PIN_RESET - 数据位为0
 * @retval DHT11_ERROR - 读取失败
 * @note 读取DHT11传感器的单个数据位
 * @details 根据高电平持续时间判断数据位:
 *          - 高电平持续时间 <= 30us 表示数据位0
 *          - 高电平持续时间 > 30us 表示数据位1
 */
uint8_t DHT11_ReadBit() {
    uint8_t high_time = 0;
    
    // 等待DHT11数据位开始（低电平结束）
    while (DHT11_GPIO_READ() == DHT11_GPIO_PIN_RESET) {
        DHT11_DELAY_10US();
        if (++high_time > 6) {
            return DHT11_ERROR;
        }
    }
    
    high_time = 0;
    // 计算高电平持续时间
    while (DHT11_GPIO_READ() == DHT11_GPIO_PIN_SET) {
        DHT11_DELAY_10US();
        if (++high_time > 10) {
            return DHT11_ERROR;
        }
    }
    
    // 根据高电平持续时间判断数据位
    // 30微秒位于数据位 "0" 的最大时间(27微秒)和数据位"1"的最小时间(68微秒)之间
    return (high_time > 3) ? DHT11_GPIO_PIN_SET : DHT11_GPIO_PIN_RESET;
}


/**
 * @brief 读取DHT11数据字节
 * @param 无
 * @return uint8_t - 读取的数据字节
 * @note 读取DHT11传感器的单个数据字节（8位）
 * @details 连续读取8个数据位，组成一个完整的字节
 */
uint8_t DHT11_ReadByte() {
    uint8_t bit_index;
    uint8_t DHT11_data = 0;
    for ( bit_index = 0; bit_index < 8; bit_index++) {
        DHT11_data <<= 1; // 左移一位，为下一位数据做准备
        if (DHT11_ReadBit() == DHT11_GPIO_PIN_SET) {
            DHT11_data |= 0x01;
        }
        
    }
    return DHT11_data;
}


/**
 * @brief 读取DHT11温湿度数据
 * @param[out] temperature_sign 温度符号（0为正，1为负）
 * @param[out] temperature_int 温度整数部分
 * @param[out] temperature_dec 温度小数部分
 * @param[out] humidity_int 湿度整数部分
 * @return uint8_t - 操作结果
 * @retval DHT11_OK - 读取成功
 * @retval DHT11_ERROR - 读取失败
 * @note 读取DHT11传感器的完整温湿度数据
 * @details 执行以下操作:
 *          1. 发送启动信号
 *          2. 等待响应信号
 *          3. 读取湿度整数部分
 *          4. 读取湿度小数部分（内部处理，不输出）
 *          5. 读取温度整数部分
 *          6. 读取温度小数部分和符号
 *          7. 读取校验和并验证
 *          8. 处理温度符号和小数部分
 */
uint8_t DHT11_ReadData(uint8_t *temperature_sign, uint8_t *temperature_int, uint8_t *temperature_dec, uint8_t *humidity_int) {
    uint8_t check;               ///< 校验和
    uint8_t humidity_dec;        ///< 湿度小数部分（内部使用）
    uint8_t temperature_dec_sign; ///< 温度小数部分和符号
    
    DHT11_StartSignal();    // 发送启动信号
    
    // 等待DHT11响应信号
    if (DHT11_WaitResponse() != DHT11_OK) {
        return DHT11_ERROR;
    }
    
    *humidity_int = DHT11_ReadByte();        // 读取湿度整数部分
    humidity_dec = DHT11_ReadByte();         // 读取湿度小数部分
    *temperature_int = DHT11_ReadByte();     // 读取温度整数部分
    temperature_dec_sign = DHT11_ReadByte(); // 读取温度小数部分和符号
    check = DHT11_ReadByte();                // 读取校验和
    
    // 验证校验和
    if (check != ((*humidity_int + humidity_dec + *temperature_int + temperature_dec_sign) & 0xFF)) {
        return DHT11_ERROR;
    }
    
    // 温度符号处理：如果温度小数部分的最高位为1，则表示负温度
    *temperature_sign = (temperature_dec_sign & 0x80) ? 1 : 0;
    // 温度小数部分处理：去掉最高位的符号位
    *temperature_dec = temperature_dec_sign & 0x7F;
    
    return DHT11_OK;
}