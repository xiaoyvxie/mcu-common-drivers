# DHT11 温湿度传感器库使用说明

## 1. 库概述

DHT11 温湿度传感器库是一个专为 51 单片机设计的驱动库，用于与 DHT11 数字温湿度传感器进行通信。该库提供了简洁的 API 接口，支持温湿度数据的读取和处理。

### 1.1 功能特点

- 支持 DHT11 传感器的初始化和数据读取
- 提供完整的通信协议实现
- 内置校验和验证，确保数据准确性
- 模块化设计，易于移植到不同硬件平台
- 详细的中文注释，便于理解和使用

### 1.2 文件结构

```
├── Inc/
│   └── DHT11.h       # 头文件，包含宏定义和函数声明
└── Src/
    └── DHT11.c       # 源文件，包含函数实现
```

## 2. 快速开始

### 2.1 硬件连接

DHT11 传感器与单片机的连接非常简单：

| DHT11 引脚 | 单片机引脚 | 说明 |
|------------|---------------|------|
| VCC        | 5V            | 电源正极 |
| GND        | GND           | 电源负极 |
| DATA       | P3.0          | 数据信号线（可通过宏定义修改） |

### 2.2 库的集成

1. 将 `DHT11.h` 文件复制到项目的 `Inc` 目录
2. 将 `DHT11.c` 文件复制到项目的 `src` 目录
3. 在需要使用 DHT11 的源文件中包含头文件：
   ```c
   #include "DHT11.h"
   ```

### 2.3 基本使用示例

```c
#include "config.h"
#include "DHT11.h"
#include "uart.h"
#include "interrupt_mgr.h"
#include "Delay.h"

/**
 * @brief 系统初始化函数
 * @param 无
 * @return 无
 * @note 初始化系统所需的各个模块
 * @details 执行以下初始化步骤:
 *          1. 配置 UART 中断为高优先级
 *          2. 配置 UART 为 8 位可变波特率模式，波特率 9600
 *          3. 初始化 UART 模块
 *          4. 启用 UART 接收功能
 *          5. 初始化中断系统
 *          6. 全局启用中断
 *          7. 初始化 DHT11 温湿度传感器
 */
void Init() {
    // 配置中断参数：UART 中断源，高优先级
    INTERRUPT_CONFIG interrupt_config = {
        INTERRUPT_SOURCE_UART, 
        INTERRUPT_PRIORITY_HIGH};
    
    // 配置 UART 参数：8 位可变波特率模式，波特率 9600
    UART_CONFIG uart_config = {
        UART_MODE_8BIT_VARIABLE_BAUD,
        0xF3// 波特率9600  
    };
    
    UART_Init(&uart_config);         ///< 初始化 UART 模块
    uart_receive_enable(__ON);       ///< 启用 UART 接收
    interrupt_init(&interrupt_config); ///< 初始化中断系统
    interrupt_global_enable(__ON);   ///< 全局启用中断
    DHT11_Init();                    ///< 初始化 DHT11 温湿度传感器
}



/**
 * @brief 主函数
 * @param 无
 * @return 无
 * @note 程序的入口点，实现 DHT11 温湿度数据的读取和串口发送
 * @details 执行以下操作:
 *          1. 定义温湿度变量
 *          2. 初始化系统
 *          3. 进入主循环，不断读取温湿度数据并通过串口发送
 *          4. 读取成功时发送温湿度数据
 *          5. 读取失败时发送错误码
 *          6. 每秒钟读取一次数据
 */
void main() {
    uint8_t temperature_int;    ///< 温度整数部分
    uint8_t temperature_dec;    ///< 温度小数部分
    uint8_t humidity_int;       ///< 湿度整数部分
    uint8_t humidity_dec;       ///< 湿度小数部分
    
    Init();                     ///< 初始化系统
    
    while(1) {
        // 读取 DHT11 温湿度数据
        if (DHT11_ReadData(&temperature_int, &temperature_dec, &humidity_int, &humidity_dec) == DHT11_OK) {
            // 读取成功，发送温湿度数据（湿度小数、湿度整数、温度小数、温度整数）
            uart_send_byte(humidity_dec);
            uart_send_byte(humidity_int);
            uart_send_byte(temperature_dec);
            uart_send_byte(temperature_int);
        } 
        else {
            // 读取失败，发送错误码 0xFF
            uart_send_byte(0xFF);
            Delay_ms(1);        ///< 短暂延时
        }
        Delay_ms(1000);         ///< 延时 1 秒，控制采样频率
    }
}
```

## 3. API 参考

### 3.1 初始化函数

#### `void DHT11_Init()`
- **功能**：初始化 DHT11 传感器
- **参数**：无
- **返回值**：无
- **说明**：该函数会延时 1 秒等待传感器启动，然后初始化 GPIO 引脚为输出模式并释放总线

### 3.2 数据读取函数

#### `uint8_t DHT11_ReadData(uint8_t *temperature_int, uint8_t *temperature_dec, uint8_t *humidity_int, uint8_t *humidity_dec)`
- **功能**：读取 DHT11 传感器的温湿度数据
- **参数**：
  - `temperature_int`：温度整数部分的指针
  - `temperature_dec`：温度小数部分的指针
  - `humidity_int`：湿度整数部分的指针
  - `humidity_dec`：湿度小数部分的指针
- **返回值**：
  - `DHT11_OK`：读取成功
  - `DHT11_ERROR`：读取失败
- **说明**：该函数会发送启动信号，等待响应，然后读取 5 个字节的数据（湿度小数、湿度整数、温度小数、温度整数、校验和），最后验证校验和的正确性

### 3.3 内部函数

以下函数为内部使用函数，一般不需要直接调用：

#### `void DHT11_StartSignal()`
- **功能**：发送启动信号
- **参数**：无
- **返回值**：无

#### `uint8_t DHT11_WaitResponse()`
- **功能**：等待 DHT11 响应信号
- **参数**：无
- **返回值**：
  - `DHT11_OK`：响应成功
  - `DHT11_ERROR`：响应失败

#### `uint8_t DHT11_ReadBit()`
- **功能**：读取单个数据位
- **参数**：无
- **返回值**：
  - `DHT11_GPIO_PIN_SET`：数据位为 1
  - `DHT11_GPIO_PIN_RESET`：数据位为 0
  - `DHT11_ERROR`：读取失败

#### `uint8_t DHT11_ReadByte()`
- **功能**：读取单个数据字节
- **参数**：无
- **返回值**：读取的数据字节

## 4. 移植指南

### 4.1 平台移植

要将 DHT11 库移植到不同的硬件平台，需要修改以下部分：

#### 4.1.1 GPIO 操作函数

在 `DHT11.h` 文件中，需要修改以下宏定义：

```c
// GPIO 写操作
#define DHT11_GPIO_WRITE(value)     gpio_write(GPIO_B, PIN_0, value)

// GPIO 读操作
#define DHT11_GPIO_READ()           gpio_read(GPIO_B, PIN_0)
```

根据目标平台的 GPIO 操作方式进行替换.

#### 4.1.2 GPIO 初始化函数

需要实现 `DHT11_Gpio_Write_Init()` 和 `DHT11_Gpio_Read_Init()` 函数，根据目标平台的 GPIO 配置方式

#### 4.1.3 延时函数

在 `DHT11.h` 文件中，需要修改以下宏定义，确保延时函数与目标平台匹配：

```c
#define DHT11_DELAY_1S()            Delay_ms(1000)      // 延时 1 秒
#define DHT11_DELAY_20MS()          Delay_ms(20)        // 延时 20 毫秒
#define DHT11_DELAY_20US()          Delay_20us()        // 延时 20 微秒
#define DHT11_DELAY_10US()          Delay_10us()        // 延时 10 微秒
```

如果目标平台没有这些延时函数，需要自行实现

### 4.2 示例移植

#### 4.2.1 移植到 STC89C52 平台

1. 修改 `DHT11.h` 中的 GPIO 操作宏：
   ```c
   #define DHT11_GPIO_WRITE(value)      P30 = value
   #define DHT11_GPIO_READ()            P30
   ```

2. 实现 GPIO 初始化函数：
   ```c
   void DHT11_Gpio_Write_Init() {
       // STC89C52 无需特别配置，P3.0 默认是准双向口
   }

   void DHT11_Gpio_Read_Init() {
       // STC89C52 无需特别配置，P3.0 默认是准双向口
   }
   ```

3. 实现延时函数（如果需要）：
   ```c
   void Delay_ms(uint16_t ms) {
       uint16_t i, j;
       for (i = 0; i < ms; i++) {
           for (j = 0; j < 110; j++);
       }
   }

   void Delay_10us() {
       uint8_t i;
       for (i = 0; i < 2; i++);
   }

   void Delay_20us() {
       uint8_t i;
       for (i = 0; i < 4; i++);
   }
   ```



## 5. 常见问题与解决方案

### 5.1 读取失败

**症状**：`DHT11_ReadData()` 函数返回 `DHT11_ERROR`

**可能原因**：
1. 硬件连接错误
2. 传感器未正确供电
3. 延时函数精度不够
4. GPIO 初始化不正确

**解决方案**：
1. 检查硬件连接，确保 VCC、GND 和 DATA 引脚连接正确
2. 确保传感器供电稳定（5V）
3. 调整延时函数，确保时序正确
4. 正确实现 GPIO 初始化函数

### 5.2 数据不准确

**症状**：读取到的温湿度数据与实际情况不符

**可能原因**：
1. 校验和验证失败
2. 数据读取顺序错误
3. 传感器老化或损坏

**解决方案**：
1. 检查校验和验证逻辑
2. 确认数据读取顺序正确
3. 更换传感器

### 5.3 移植后无法工作

**症状**：在新平台上无法正常读取数据

**可能原因**：
1. GPIO 操作函数不兼容
2. 延时函数精度不够
3. 平台时钟频率不同

**解决方案**：
1. 根据目标平台修改 GPIO 操作函数
2. 调整延时函数以适应目标平台的时钟频率
3. 参考目标平台的官方文档进行相应调整

## 6. 注意事项

1. **采样频率**：DHT11 传感器的采样频率不应超过 2Hz（即每2秒读取一次）
2. **电源稳定性**：确保传感器供电稳定，避免电压波动
3. **连接线长度**：DATA 信号线长度不宜过长，建议不超过 2 米
4. **抗干扰**：在嘈杂环境中，建议在 DATA 线上添加上拉电阻（4.7KΩ）和滤波电容
5. **温度范围**：DHT11 传感器的工作温度范围为 0-50℃，湿度范围为 20-90%RH

## 7. 版本历史

| 版本 | 日期 | 变更内容 |
|------|------|----------|
| V1.0 | 2026-02-23 | 初始版本，实现基本功能 |

## 8. 许可证

本库采用 MIT 许可证，详情请参考项目文件。

## 9. 联系方式

- 作者：xiaoyvxie
- 邮箱："xiaoyvxie@gmail.com" or "xiaoyvxie@yeah.net"
- 项目地址：https://github.com/xiaoyvxie/mcu-common-drivers

---

**备注**：本使用说明基于 DHT11 传感器的官方 datasheet 和实际测试编写，如有任何疑问或建议，欢迎联系作者。
        