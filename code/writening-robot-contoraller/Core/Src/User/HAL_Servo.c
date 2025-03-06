#include "HAL_Servo.h"
#include "usart.h"
uint8_t servo_sdata[10] = {0x7b, 0x79, 0, 0, 0, 0, 0x10, 0x10, 0x10, 0x7d};       // 发送数组  10bit
uint8_t servo_sdata_short[4] = {0x7E, 0, 0, 0};                                   // 发送数组  4bit,原名：servo_sdata1
uint8_t servo_rdata[16] = {0x7b, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x7d}; // 舵机返回值存放数组 16bit
extern uint8_t RxBuff[64];                                                         // 接收数组
// 添加全局变量用于中断接收
volatile uint8_t u_numm = 0;                     // 接收数据计数器
volatile uint8_t receive_flag = 0;               // 接收状态标志（0:未接收，1:正在接收，2:接收完成）
volatile float servo_rpara[5] = {0, 0, 0, 0, 0}; // 存储解析后的数据数组
volatile float id_number = 0;                    // 舵机ID号
volatile float cur_angle = 0;                    // 当前角度
volatile float exp_angle = 0;                    // 期望角度
volatile float run_time = 0;                     // 运行时间

// 接收中断回调函数
// void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
// {
//   if (huart = &huart1)
//   {
//     // 检测帧头
//     if (RxBuff[0] == 0x7B)
//     {
//       for (u_numm = 0; u_numm < 16; u_numm++)
//       {
//         servo_rdata[u_numm] = RxBuff[u_numm];
//       }
//     }
//   }
//     // 在解析完一字节后，再次开启下一次接收
//     HAL_UARTEx_ReceiveToIdle_IT(huart, &RxBuff, 64);
// }

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
HAL_StatusTypeDef DaRan_HAL_set_angle(UART_HandleTypeDef *huart, uint8_t id_num, float angle, uint16_t step, uint32_t timeout)
{
  HAL_StatusTypeDef status; // 用于存储HAL库函数返回值

  /* 设置timeout缺省值 */
  if (timeout == 0)
    timeout = 1000; // 默认60秒超时

  /* 参数范围检查 */
  if (step <= 0)
    step = 1; // 步数最小为1
  if (angle < 0)
    angle = 0; // 角度最小为0
  if (angle > 285)
    angle = 285; // 角度最大为285

  /* 构建命令数据包 */
  servo_sdata[0] = 0x7B;                    // 帧头
  servo_sdata[1] = id_num;                  // 舵机ID
  servo_sdata[2] = ((int)angle / 10) % 100; // 角度高位
  servo_sdata[3] = ((int)angle * 10) % 100; // 角度低位
  servo_sdata[4] = (int)step / 100;         // 步数高位
  servo_sdata[5] = (int)step % 100;         // 步数低位

  servo_sdata[7] = 16;   // 功能选择,0x10
  servo_sdata[8] = 0x1;  // 模式选择
  servo_sdata[9] = 0x7D; // 帧尾

  /* 计算校验和 */
  servo_sdata[6] = (servo_sdata[1] + servo_sdata[2] + servo_sdata[3] +
                    servo_sdata[4] + servo_sdata[5] + servo_sdata[7] +
                    servo_sdata[8]) %
                   100; // 校验和

  /* 发送第一个命令包 */
  status = HAL_UART_Transmit(huart, (uint8_t *)servo_sdata, sizeof(servo_sdata), timeout);

  /* 修改为第二个命令包并发送 */
  servo_sdata[7] = 17; // 功能选择,0x11
  servo_sdata[6] = (servo_sdata[1] + servo_sdata[2] + servo_sdata[3] +
                    servo_sdata[4] + servo_sdata[5] + servo_sdata[7] +
                    servo_sdata[8]) %
                   100; // 更新校验和
  return HAL_UART_Transmit(huart, (uint8_t *)servo_sdata, sizeof(servo_sdata), timeout);
}

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
HAL_StatusTypeDef DaRan_HAL_set_angles(UART_HandleTypeDef *huart, int id_list[20], float angle_list[20], int step, int n, uint32_t timeout)
{
  HAL_StatusTypeDef status = HAL_OK;
  int i;

  /* 设置timeout缺省值 */
  if (timeout == 0)
    timeout = 60000; // 默认60秒超时

  if (n == 0)
    n = 1; // 默认使用长协议

  if (n == 1)
  {
    for (i = 0; i < 20; i++)
    {
      if (i != 0)
      {
        if (id_list[i] != 0)
        {
          status = DaRan_HAL_preset_angle(huart, id_list[i], angle_list[i], step, n, timeout);
          if (status != HAL_OK)
            return status;
          HAL_Delay(2);
        }
      }
      else
      {
        status = DaRan_HAL_preset_angle(huart, id_list[i], angle_list[i], step, 0, timeout);
        if (status != HAL_OK)
          return status;
        HAL_Delay(2);
      }
    }

    // 多舵机同时开始转动
    servo_sdata[1] = 121; // 广播ID:0x79
    servo_sdata[7] = 17;  // 功能选择:0x11
    servo_sdata[8] = 1;   // 模式选择:0x01
    servo_sdata[6] = (servo_sdata[1] + servo_sdata[2] + servo_sdata[3] +
                      servo_sdata[4] + servo_sdata[5] + servo_sdata[7] +
                      servo_sdata[8]) %
                     100; // 更新校验和

    return HAL_UART_Transmit(huart, (uint8_t *)servo_sdata, sizeof(servo_sdata), timeout);
  }
  else if (n == 2)
  {
    for (i = 0; i < 20; i++)
    {
      if (id_list[i] != 0)
      {
        servo_sdata_short[0] = 0x7E;                            // ID:0x7E,广播ID的短协议
        servo_sdata_short[1] = id_list[i];                      // 舵机ID
        servo_sdata_short[2] = ((int)angle_list[i] / 10) % 100; // 角度高位
        servo_sdata_short[3] = ((int)angle_list[i] * 10) % 100; // 角度低位

        status = HAL_UART_Transmit(huart, (uint8_t *)servo_sdata_short, sizeof(servo_sdata_short), timeout);
        if (status != HAL_OK)
          return status;
        HAL_Delay(2);
      }
    }

    // 设置步数
    servo_sdata_short[0] = 0x7E;
    servo_sdata_short[1] = (int)(step / 100);
    servo_sdata_short[2] = (int)(step % 100);
    servo_sdata_short[3] = 0x7F;

    return HAL_UART_Transmit(huart, (uint8_t *)servo_sdata_short, sizeof(servo_sdata), timeout);
  }

  return HAL_ERROR; // 如果n不是1或2，返回错误
}

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
HAL_StatusTypeDef DaRan_HAL_preset_angle(UART_HandleTypeDef *huart, int id_num, float angle, int step, int rn, uint32_t timeout)
{
  HAL_StatusTypeDef status;

  /* 设置timeout缺省值 */
  if (timeout == 0)
    timeout = 60000; // 默认60秒超时

  if (step <= 0)
    step = 1; // 步数最小为1
  if (angle < 0)
    angle = 0; // 角度最小为0
  if (angle > 285)
    angle = 285; // 角度最大为285

  servo_sdata[0] = 0x7B;                    // 帧头
  servo_sdata[1] = id_num;                  // 舵机ID
  servo_sdata[2] = ((int)angle / 10) % 100; // 角度高位
  servo_sdata[3] = ((int)angle * 10) % 100; // 角度低位
  servo_sdata[4] = (int)step / 100;         // 步数高位
  servo_sdata[5] = (int)step % 100;         // 步数低位

  servo_sdata[7] = 16;   // 功能选择,0x10
  servo_sdata[8] = 0x1;  // 模式选择
  servo_sdata[9] = 0x7D; // 帧尾
  servo_sdata[6] = (servo_sdata[1] + servo_sdata[2] +
                    servo_sdata[3] + servo_sdata[4] + servo_sdata[5] +
                    servo_sdata[7] + servo_sdata[8]) %
                   100; // 校验和

  status = HAL_UART_Transmit(huart, (uint8_t *)servo_sdata, sizeof(servo_sdata), timeout);
  if (status != HAL_OK)
    return status;

  if (rn == 0)
  {
    servo_sdata[7] = 17; // 功能选择,0x11
    servo_sdata[6] = (servo_sdata[1] + servo_sdata[2] +
                      servo_sdata[3] + servo_sdata[4] + servo_sdata[5] +
                      servo_sdata[7] + servo_sdata[8]) %
                     100; // 更新校验和

    return HAL_UART_Transmit(huart, (uint8_t *)servo_sdata, sizeof(servo_sdata), timeout);
  }

  return HAL_OK;
}

/** 4
 * @fn HAL_StatusTypeDef DaRan_HAL_change_mode(UART_HandleTypeDef *huart, int id_num, int mode_num, uint32_t timeout)
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
HAL_StatusTypeDef DaRan_HAL_change_mode(UART_HandleTypeDef *huart, int id_num, int mode_num, uint32_t timeout)
{
  /* 设置timeout缺省值 */
  if (timeout == 0)
    timeout = 60000; // 默认60秒超时
  if (mode_num == 0)
    mode_num = 1; // 默认阻尼模式

  servo_sdata[0] = 0x7B;   // 帧头:123
  servo_sdata[1] = id_num; // 舵机ID
  // servo_sdata[2];        // 注释掉未使用
  // servo_sdata[3];        // 注释掉未使用
  // servo_sdata[4];        // 注释掉未使用
  // servo_sdata[5];        // 注释掉未使用

  servo_sdata[7] = 0x10;            // 功能选择:16
  servo_sdata[8] = 0x10 + mode_num; // 模式选择
  servo_sdata[9] = 0x7D;            // 帧尾:125
  /* 校验 */
  servo_sdata[6] = (servo_sdata[1] + 0 + 0 + 0 + 0 + servo_sdata[7] + servo_sdata[8]) % 100;

  return HAL_UART_Transmit(huart, (uint8_t *)servo_sdata, sizeof(servo_sdata), timeout);
}

/** 5
 * @fn HAL_StatusTypeDef DaRan_HAL_set_id(UART_HandleTypeDef *huart, int id_num, int id_new, uint32_t timeout)
 * @brief 设置舵机编号函数
 *
 * @param huart    UART句柄，指定使用的UART外设
 * @param id_num   需要重新设置编号的舵机当前编号。其中121为广播编号使用
 * @param id_new   舵机的新编号
 * @param timeout  UART发送超时时间(毫秒)
 * @return HAL_StatusTypeDef 操作结果状态
 */
HAL_StatusTypeDef DaRan_HAL_set_id(UART_HandleTypeDef *huart, int id_num, int id_new, uint32_t timeout)
{
  HAL_StatusTypeDef status;

  /* 设置timeout缺省值 */
  if (timeout == 0)
    timeout = 60000; // 默认60秒超时
  if (id_new == 0)
    id_new = id_num; // 默认不改变ID

  servo_sdata[0] = 0x7B;   // 帧头:123
  servo_sdata[1] = id_num; // 舵机ID
  // servo_sdata[2];         // 注释掉未使用
  // servo_sdata[3];         // 注释掉未使用
  // servo_sdata[4];         // 注释掉未使用
  // servo_sdata[5];         // 注释掉未使用

  servo_sdata[7] = 0x42; // 功能选择
  servo_sdata[8] = 0;    // 模式选择
  servo_sdata[9] = 0x7D; // 帧尾:125
  /* 校验 */
  servo_sdata[6] = (servo_sdata[1] + 0 + 0 + 0 + 0 + servo_sdata[7] + servo_sdata[8]) % 100;

  /* 发送数据 */
  status = HAL_UART_Transmit(huart, (uint8_t *)servo_sdata, sizeof(servo_sdata), timeout);
  if (status != HAL_OK)
    return status;

  HAL_Delay(2);
  /* 设置新ID */
  servo_sdata[0] = 123;    // 帧头
  servo_sdata[1] = id_num; // 舵机ID
  servo_sdata[2] = id_new; // 新ID
  // servo_sdata[3];         // 注释掉未使用
  // servo_sdata[4];         // 注释掉未使用
  // servo_sdata[5];         // 注释掉未使用

  servo_sdata[7] = 0x44; // 功能选择
  servo_sdata[8] = 0;    // 模式选择
  servo_sdata[9] = 125;  // 帧尾
  /* 校验 */
  servo_sdata[6] = (servo_sdata[1] + servo_sdata[2] + 0 + 0 + 0 + servo_sdata[7] + servo_sdata[8]) % 100;

  /* 发送数据 */
  return HAL_UART_Transmit(huart, (uint8_t *)servo_sdata, sizeof(servo_sdata), timeout);
}

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
HAL_StatusTypeDef DaRan_HAL_set_pid(UART_HandleTypeDef *huart, int id_num, char pid, uint32_t value, uint32_t timeout)
{
  HAL_StatusTypeDef status;
  uint8_t mode_select;

  switch (pid)
  {
  case 'P':
  case 'p':
    mode_select = 3;
    break;
  case 'I':
  case 'i':
    mode_select = 4;
    break;
  case 'D':
  case 'd':
    mode_select = 5;
    break;
  default:
    return HAL_ERROR;
  }

  /* 设置timeout缺省值 */
  if (timeout == 0)
    timeout = 60000; // 默认60秒超时

  /* 第一步：进入PID设置模式 */
  servo_sdata[0] = 0x7B;   // 帧头
  servo_sdata[1] = id_num; // 舵机ID
  // servo_sdata[2];              // 注释掉未使用
  // servo_sdata[3];              // 注释掉未使用
  // servo_sdata[4];              // 注释掉未使用
  // servo_sdata[5];              // 注释掉未使用

  servo_sdata[7] = 0x42; // 功能选择
  servo_sdata[8] = 0;    // 模式选择
  servo_sdata[9] = 0x7D; // 帧尾

  servo_sdata[6] = (servo_sdata[1] + 0 + 0 + 0 + 0 + servo_sdata[7] + servo_sdata[8]) % 100;

  status = HAL_UART_Transmit(huart, (uint8_t *)servo_sdata, sizeof(servo_sdata), timeout);
  if (status != HAL_OK)
    return status;
  HAL_Delay(2);

  /* 第二步：发送新的PID参数值 */
  servo_sdata[0] = 123;    // 帧头
  servo_sdata[1] = id_num; // 舵机ID
  servo_sdata[2] = value % 100;
  servo_sdata[3] = value / 100;
  // servo_sdata[4];             // 注释掉未使用
  // servo_sdata[5];             // 注释掉未使用

  servo_sdata[7] = 0x44;        // 功能选择
  servo_sdata[8] = mode_select; // 模式选择
  servo_sdata[9] = 125;         // 帧尾

  servo_sdata[6] = (servo_sdata[1] + servo_sdata[2] + servo_sdata[3] + 0 + 0 + servo_sdata[7] + servo_sdata[8]) % 100;
  status = HAL_UART_Transmit(huart, (uint8_t *)servo_sdata, sizeof(servo_sdata), timeout);
  HAL_Delay(2);

  return status;
}

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
 * @param o_m      用来指明多个舵机(o_m=0)还是一个舵机，如果只有一个舵机可以采用广播模式，此时 o_m=1
 * @param timeout  UART发送超时时间(毫秒)
 * @return float   返回值会根据para_num的值相应改变
 * @note 如果多个舵机连接时，查询指令使用了广播模式，会return 0,不执行查询指令
 */
float DaRan_HAL_get_state(UART_HandleTypeDef *huart, int id_num, int para_num, int o_m, uint32_t timeout)
{
  /* 多舵机连接时如果使用广播模式，返回0且不执行查询指令 */
  if ((id_num == 121 && o_m == 0) || (id_num == 121 && o_m > 1))
    return 0;

  /* 准备接收数据 */
  u_numm = 0;
  receive_flag = 0;

  /* 启用UART接收中断 */

  /* 构建查询指令数据包 */
  servo_sdata[0] = 0x7B;   // 帧头
  servo_sdata[1] = id_num; // 舵机ID
  servo_sdata[2] = 0;
  servo_sdata[3] = 0;
  servo_sdata[4] = 0;
  servo_sdata[5] = 0;
  servo_sdata[7] = 0x13; // 功能码
  servo_sdata[8] = 0;
  servo_sdata[9] = 0x7D; // 帧尾

  /* 计算校验和 */
  servo_sdata[6] = (servo_sdata[1] + servo_sdata[2] + servo_sdata[3] +
                    servo_sdata[4] + servo_sdata[5] + servo_sdata[7] +
                    servo_sdata[8]) %
                   100;

  /* 发送查询指令 */
  if (HAL_UART_Transmit(huart, servo_sdata, sizeof(servo_sdata), timeout) != HAL_OK)
  {
    return -2;
  }
  // __HAL_UART_DISABLE_IT(huart,UART_IT_RXNE);
  if(HAL_UART_Receive(huart,servo_rdata,sizeof(servo_rdata),timeout) != HAL_OK)
  {
    __HAL_UART_ENABLE_IT(huart,UART_IT_RXNE);
    return -1;
  }
  // __HAL_UART_ENABLE_IT(huart,UART_IT_RXNE);
  // /* 等待接收完成或超时 */
  // uint32_t startTick = HAL_GetTick();
  // while (receive_flag != 2)
  // {
  //   /* 检查是否超时 */
  //   if (HAL_GetTick() - startTick > timeout)
  //   {
  //     __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);
  //     return -1;
  //   }
  //   HAL_Delay(1); // 短暂延时，避免过度占用CPU
  // }
 
  // /* 禁用UART接收中断，防止干扰 */
  // __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);

  /* 数据接收完成，解析数据 */
    receive_flag = 0;
    id_number = servo_rdata[1];
    cur_angle = (float)servo_rdata[2] * 10.0f + (float)servo_rdata[3] * 0.1f;
    exp_angle = (float)servo_rdata[4] * 10.0f + (float)servo_rdata[5] * 0.1f;
    run_time = servo_rdata[6] * 100.0f + servo_rdata[7];

    /* 存储解析后的数据到数组，方便后续调用 */
    servo_rpara[0] = id_number; // 舵机ID
    servo_rpara[1] = cur_angle; // 当前角度
    servo_rpara[2] = exp_angle; // 期望角度
    servo_rpara[3] = run_time;  // 运行时间

    /* 根据para_num返回相应参数 */
    if (para_num == 1)
      return id_number;
    else if (para_num == 2)
      return cur_angle;
    else if (para_num == 3)
      return exp_angle;
    else if (para_num == 4)
      return run_time;
    else if (para_num == 0)
    {
      servo_rpara[4] = servo_rdata[8]; // 存储工作模式
    }
    return 666666.66; // 返回成功标志
}

/** 8
 * @fn HAL_StatusTypeDef DaRan_HAL_set_speed(UART_HandleTypeDef *huart, int id_num, int speed, uint32_t timeout)
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
HAL_StatusTypeDef unlock_stall(UART_HandleTypeDef *huart, int id_num, uint32_t timeout)
{
  /* 记录HAL库函数返回状态 */
  HAL_StatusTypeDef status;

  /*
   * cmd数组构造了一条解锁堵转保护的指令包：
   * 0x7B    : 帧头
   * 0x0D,0x32 : 指令数据，用于舵机解锁
   * 0x30    : 功能码
   * 0x7D    : 帧尾
   */
  uint8_t cmd[10] = {0x7B, 0, 0x0D, 0x32, 0, 1, 0, 0x30, 0, 0x7D};

  /* 设置舵机ID */
  cmd[1] = id_num;

  /* 计算校验和（取各数据之和%100） */
  cmd[6] = (cmd[1] + cmd[2] + cmd[3] + cmd[4] + cmd[5] + cmd[7] + cmd[8]) % 100;

  /* 发送解锁指令 */
  status = HAL_UART_Transmit(huart, cmd, sizeof(cmd), timeout);

  /* 为避免指令间干扰，延时20ms */
  HAL_Delay(20);

  return status;
}

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
HAL_StatusTypeDef DaRan_HAL_set_speed(UART_HandleTypeDef *huart, int id_num, int speed, uint32_t timeout)
{
  /* 记录HAL库函数返回状态 */
  HAL_StatusTypeDef status;

  servo_sdata[0] = 123;
	servo_sdata[1] = id_num;
	servo_sdata[7] = 0x16;
	servo_sdata[8] = 0x10 + 4;
	servo_sdata[9] = 125;
	servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
  status = HAL_UART_Transmit(huart, (uint8_t *)servo_sdata, sizeof(servo_sdata), timeout);
  HAL_Delay(2);
  
	servo_sdata[0] = 123;
	servo_sdata[1] = id_num;
	if(speed > 0)
		servo_sdata[2] = 0x21; 
	else if(speed < 0){
    servo_sdata[2] = 0x22;    
    speed = -speed;
  }
	else
		servo_sdata[2] = 0x20;
	
	servo_sdata[4] = (int)(speed / 10);
	servo_sdata[5] = (int)((speed) % 10);
	servo_sdata[7] = 0x20;
	servo_sdata[8] = 1;
	servo_sdata[9] = 125;
	servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
  return HAL_UART_Transmit(huart, (uint8_t *)servo_sdata, sizeof(servo_sdata), timeout);
}