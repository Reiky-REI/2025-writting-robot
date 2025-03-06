#ifndef __SERVO_H
#define __SERVO_H

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_uart.h"
#include "stdio.h"
#include <stdint.h>

/** 1
 * @fn HAL_StatusTypeDef DaRan_HAL_set_angle(UART_HandleTypeDef *huart, uint8_t id_num, float angle, uint16_t step, uint32_t timeout)
 * @brief 使单个舵机转动到指定角度 (HAL库风格)
 * 
 * @param huart    UART句柄，指定使用的UART外设
 * @param id_num   舵机编号（0~255）
 * @param angle    目标角度（0~270度之间）
 * @param step     转动过程中分步的步数，步数为1表示最快速度转动
 * @param timeout  UART发送超时时间(毫秒)
 * @return HAL_StatusTypeDef 操作结果状态
 */
HAL_StatusTypeDef DaRan_HAL_set_angle(UART_HandleTypeDef *huart, uint8_t id_num, float angle, uint16_t step, uint32_t timeout);

/** 2
 * @fn HAL_StatusTypeDef DaRan_HAL_set_angles(UART_HandleTypeDef *huart, int id_list[20], float angle_list[20], int step, int n)
 * @brief 控制多个舵机同时转动到指定角度 (HAL库风格)
 * 
 * 根据传入的舵机编号数组和对应角度数组，该函数将通过统一的步数参数使多个舵机同时开始转动，
 * 从而保证转动过程同步。
 * 
 * @param huart      UART句柄，指定使用的UART外设
 * @param id_list    舵机编号数组，每个元素对应一个舵机编号
 * @param angle_list 舵机角度数组，每个角度的取值范围为 0~270 度
 * @param step       分步转动的步数，建议保证所有舵机同时开始和结束
 * @param n          协议版本，支持两种协议。n=1时为长协议，n=2时为短协议；默认值一般为 1
 *                   长协议控制：[0x7B, ID, angle_H, angle_L, step_H, step_L, Check, Cmd, CmdData, 0x7D]
 *                   协议格式为：[126, ID1, angle0, angle1] [126, ID2, angle0, angle1], .....[126, step0, step1, 127]
 * @param timeout    UART发送超时时间(毫秒)
 * @return HAL_StatusTypeDef 操作结果状态
 */
HAL_StatusTypeDef DaRan_HAL_set_angles(UART_HandleTypeDef *huart, int id_list[20], float angle_list[20], int step, int n, uint32_t timeout);

/** 3
 * @fn HAL_StatusTypeDef DaRan_HAL_preset_angle(UART_HandleTypeDef *huart, int id_num, float angle, int step, int rn, uint32_t timeout)
 * @brief 预设舵机角度（内部函数）
 * 
 * @details 此函数准备并发送命令以设置舵机角度。它构造
 *          适合舵机运动的数据包并通过UART发送。
 *          此函数不应被用户代码直接调用，而应通过set_angles()函数调用。
 *          
 *          函数格式化的数据包结构如下：
 *          - 头字节 (0x7B)
 *          - 舵机ID
 *          - 角度数据（十位和小数部分）
 *          - 步进数据（百位和余数）
 *          - 命令字节 (0x10)
 *          - 模式字节 (0x01)
 *          - 校验和（由其他字节计算得出）
 *          - 结束字节 (0x7D)
 *          
 *          当rn=0时，会发送一个额外的命令，命令字节为0x11（17）
 * 
 * @param huart 使用的UART句柄
 * @param id_num 要控制的舵机ID号
 * @param angle 目标角度（度）（范围：0-285）
 * @param step 舵机运动的速度/步进值（最小值：1）
 * @param rn 模式选择：0发送两个命令，1发送一个命令
 * @param timeout UART发送超时时间(毫秒)
 * 
 * @return HAL_StatusTypeDef 操作结果状态
 * 
 * @note 此函数应仅由set_angles()调用，不应直接使用
 * @note 角度值会自动限制在0到285度之间
 * @note 如果步进值小于等于0，将被设为1
 */
HAL_StatusTypeDef DaRan_HAL_preset_angle(UART_HandleTypeDef *huart, int id_num, float angle, int step, int rn, uint32_t timeout);

/** 4
 * @fn HAL_StatusTypeDef DaRan_HAL_set_id(UART_HandleTypeDef *huart, int id_num, int id_new, uint32_t timeout)
 * @brief 设置舵机控制模式函数
 * 
 * 该函数将根据传入参数设置所控舵机的控制模式。
 * 
 * @param huart     UART句柄，指定使用的UART外设
 * @param id_num    舵机编号（0~255），注意预留广播编号使用
 * @param mode_num  控制模式编号，支持四种模式
 *                  mode_num=1, 阻尼模式	0x01;
 * 					mode_num=2, 锁死模式	0x02;
 * 					mode_num=3, 掉电模式	0x03;
 * 					mode_num=4, 轮子模式	0x04;
 * @param timeout   UART发送超时时间(毫秒)
 * @return HAL_StatusTypeDef 操作结果状态
 */
HAL_StatusTypeDef DaRan_HAL_change_mode(UART_HandleTypeDef *huart, int id_num, int mode_num, uint32_t timeout);

/** 5
 * @fn HAL_StatusTypeDef DaRan_HAL_set_id(UART_HandleTypeDef *huart, int id_num, int id_new, uint32_t timeout)
 * @brief 设置舵机编号
 * @param id_num  需要重新设置编号的舵机当前编号。其中121为广播编号，多机器串联不建议使用。
 * @param id_new  舵机的新编号。
 */
HAL_StatusTypeDef DaRan_HAL_set_id(UART_HandleTypeDef *huart, int id_num, int id_new, uint32_t timeout);

/** 6
 * @fn HAL_StatusTypeDef DaRan_HAL_set_pid(UART_HandleTypeDef *huart, int id_num, char pid, int value, uint32_t timeout)
 * @brief 设置PID参数函数，仅调整指定的PID参数
 * 
 * @param huart    UART句柄，指定使用的UART外设
 * @param id_num   舵机编号（0~255）
 * @param pid      指定要调整的PID参数，'P'（或'p'）代表P参数，'I'（或'i'）代表I参数，'D'（或'd'）代表D参数
 * @param value    要设置的参数值
 * @param timeout  UART发送超时时间(毫秒)
 * @return HAL_StatusTypeDef 操作结果状态
 */
HAL_StatusTypeDef DaRan_HAL_set_pid(UART_HandleTypeDef *huart, int id_num, char pid, uint32_t value, uint32_t timeout);

/** 7
 * @fn float DaRan_HAL_get_state(UART_HandleTypeDef *huart, int id_num, int para_num, int o_m, uint32_t timeout)
 * @brief 获取当前舵机状态信息，例如当前角度等
 * 
 * 获取当前舵机状态，包括
 *  - 舵机编号
 *  - 当前实际角度
 *  - 当前期望角度
 *  - 到达期望角度剩余的步数
 * 
 * @param huart    UART句柄，指定使用的UART外设
 * @param id_num   舵机编号,查询第几号舵机的模式，这里不可以用广播模式
 * @param para_num 想要查询的参数编号
 *                 para_num=0, 返回所有信息组成的列表
 *                 para_num=1, 返回当前舵机编号
 *                 para_num=2, 返回当前角度
 *                 para_num=3, 返回当前期望角度
 *                 para_num=4, 返回运行时长
 * @param o_m      用来指明多个舵机(o_m=0)还是一个舵机，如果只有一个舵机可以采用广播模式，此时o_m=1
 * @param timeout  UART发送超时时间(毫秒)
 * @return float   返回值会根据para_num的值相应改变
 *                 para_num=0, 返回所有信息组成的列表
 *                 para_num=1, 返回当前舵机编号
 *                 para_num=2, 返回当前角度
 *                 para_num=3, 返回当前期望角度
 *                 para_num=4, 返回运行时长
 * @note 如果多个舵机连接时，查询指令使用了广播模式，会return 0,不执行查询指令
 */
float DaRan_HAL_get_state(UART_HandleTypeDef *huart, int id_num, int para_num, int o_m, uint32_t timeout);

/** 8
 * @brief 解锁舵机的堵转保护状态
 * 
 * 当舵机进入堵转保护状态（如过载保护）后，不会响应转动命令。
 * 排除导致保护状态的原因（如负载过大）后，可调用此函数或重新上电
 * 解除保护状态，使舵机重新正常工作。
 * 
 * @param huart   指定使用的UART外设的句柄
 * @param id_num  要解锁的舵机编号
 * @param timeout UART发送的超时时间（毫秒）
 * @return HAL_StatusTypeDef 如果成功则返回HAL_OK，否则返回错误状态
 */
HAL_StatusTypeDef unlock_stall(UART_HandleTypeDef *huart, int id_num, uint32_t timeout);

/** 9
 * @fn HAL_StatusTypeDef DaRan_HAL_set_speed(UART_HandleTypeDef *huart, int id_num, int speed, uint32_t timeout)
 * @brief 轮子模式下设置舵机的转动速度
 * 
 * @param huart   指定使用的UART外设的句柄
 * @param id_num  要设置的舵机编号
 * @param speed   舵机的转动速度
 * @param timeout UART发送的超时时间（毫秒）
 * @return HAL_StatusTypeDef 如果成功则返回HAL_OK，否则返回错误状态
 */
HAL_StatusTypeDef DaRan_HAL_set_speed(UART_HandleTypeDef *huart, int id_num, int speed, uint32_t timeout);

#endif