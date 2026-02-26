////////////////////////////////////////////////////////////////////////////////
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,//
// EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED	  //
// WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.	  //
//																			  //
//							Copyright (c) UIROBOT.							  //
//							 All rights reserved.							  //
////////////////////////////////////////////////////////////////////////////////
#define _BLOCK_
#pragma once

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//                                               Static Library Prototype

//////////////////////////////////////////////////////// Notes //////////////////////////////////////////////////////

// 1) 1 CAN network = 1 gateway + N members; [ 1 个 CAN 网络 = 1 网关 + N 成员 ]
// 2) StartCanNet() = open hardware port + open gateway + get gateway information + get members' information + complete all preparatory work;
//    [ StartCanNet() = 打开硬件端口 + 打开网关 + 统计网关信息 + 统计全部成员信息 + 完成一切准备工作 ]
// 3) After StartCanNet(), the SDK will assign a unique GtwyHandle to the Gateway. This GtwyHandle should be provided for any operation on the Gateway.
//    [ 调用 StartCanNet(), SDK 会分配唯一的 GtwyHandle 给到用户指定的 Gtwy；之后对该网关的操作，都应提供该 GtwyHandle ]
// 4) Up to 32 "232CAN" (UIM2512 / UIM2513) + 64 "EthCAN" (UIM2522/UIM2523) can be useed at the same time; Each CAN network contains 1 gateway and up to 100 members;
//    [ 最多同时支持32个UIM2512/UIM2513 + 64个 UIM2522/UIM2523; 每个网络包含1个网关和最多100个成员 ]
// 5) Function return value: 0 = success, !0 = error code(refer to UIError.h).
//    [ 函数返回值 : 0 = 成功, !0 = 错误代码（参考 UIError.h)]
// 6) When using GroupID, if you want to receive ACK from all devices within the group, you need to set CANnid(NodeID) = GroupID | 0x80.
//    [ 使用 GroupID 时，如果希望收到组内各器件的 ACK, 需要设置  CANnid = GroupID | 0x80 ]

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//========    System    ========

/// [ CAN Network    ]
#ifdef _BLOCK_

/**
 * @brief Start the CAN network (启动 CAN 网络)
 *
 * @param pGtwy   Pointer to Gateway information object (网关信息对象，详见 uirSDKgen3.h)
 * @param pMember Pointer to Member information object (成员信息对象，详见 uirSDKgen3.h)
 * @param Config  Gateway configuration (网关附加配置):
 *                - 0: Use default parameters (默认参数)
 *                - !=0: Use user-defined configuration (自定义参数), format:
 *                  - Byte3: Wait ACK Timeout (单机应答等待超时), 1=10ms, ... 255=2550ms
 *                  - Byte2: Inter-ACK Timeout (多机应答间隔超时), 1=10ms, ... 255=2550ms
 *                  - Byte1: Reserved, must be 0x00
 *                  - Byte0: Flags (标志位)
 *                    - Bit0 = 1: Keep TCP connection indefinitely (保持 TCP 连接)
 *                    - Bit0 = 0: Close after 3 sniffing cycles (结束连接)
 *
 * @remarks
 * Wait ACK Timeout: Maximum waiting time (ms) for an ACK from a single slave after sending a command.
 * Inter-ACK Timeout: Maximum interval (ms) to receive ACKs from multiple slaves after broadcasting a command.
 *
 * @return 0: Success (成功), !=0: Error code (错误代码)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkStartCanNet(GATEWAY_INFO_OBJ* pGtwy, MEMBER_INFO_OBJ* pMember, uint Config = 0);

/**
 * @brief Close the CAN network (关闭 CAN 网络)
 *
 * @param GtwyHandle Gateway handle (网关句柄)
 * @return 0: Success (成功), !=0: Error code (错误代码)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkCloseCanNet(uint GtwyHandle);

/**
 * @brief Get CAN gateway information (查询 CAN 网关信息)
 *
 * @param pGtwy   Pointer to Gateway information object (网关信息对象，详见 uirSDKgen3.h)
 * @param pMember Pointer to Member information object (成员信息对象，详见 uirSDKgen3.h)
 * @return 0: Success (成功), !=0: Error code (错误代码)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetGtwyInfo(GATEWAY_INFO_OBJ* pGtwy, MEMBER_INFO_OBJ* pMember);

/**
 * @brief Get CAN member list information (查询 CAN 成员列表信息)
 *
 * @param GtwyHandle Gateway handle (网关句柄)
 * @param pMember    Pointer to Member information object (成员信息对象，详见 uirSDKgen3.h)
 * @return 0: Success (成功), !=0: Error code (错误代码)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetMemberList(uint GtwyHandle, MEMBER_INFO_OBJ* pMember);

/**
 * @brief Set Eth-CAN gateway IP address (适用于型号 2522 / 2523)
 *
 * @param GtwyHandle Gateway handle (网关句柄)
 * @param IPaddr     IP address string (IP 地址字符串)
 * @return 0: Success (成功), !=0: Error code (错误代码)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetIPaddr(uint GtwyHandle, BYTE* IPaddr);

#endif

/// [ ProtocolParam  ]
#ifdef _BLOCK_

/**
 * @brief Set communication parameters (设置通讯参数)
 *
 * Configure the communication parameters of the gateway device, such as system update,
 * baud rate, parity, CAN bit rate, etc. (配置网关设备的通讯参数，包括系统升级、波特率、奇偶校验、CAN 比特率等)
 *
 * @param GtwyHandle Gateway handle (网关句柄)
 * @param CANid      Device node ID (设备站点 ID)
 * @param Index      Parameter index (参数下标), valid values are:
 *                   - PPS_SysUpdate   = 0   System update (系统升级)
 *                   - PPS_BDR         = 1   Serial baud rate (串口波特率)
 *                   - PPS_Parity      = 3   Parity (奇偶校验)
 *                   - PPS_BTR         = 5   CAN bit rate (CAN 比特率)
 *                   - PPS_TargetID    = 6   Target ID (目标 ID)
 *                   - PPS_NodeID      = 7   Node ID (站点号)
 *                   - PPS_GroupID     = 8   Group ID (组号)
 *                   - PPS_GatewayACK  = 11  Gateway acknowledgment (网关应答)
 * @param iData      Parameter value to set (要设定的参数值)
 * @param pRxdata    Pointer to receive feedback, can be NULL (用于接收反馈的指针，可为 NULL)
 *
 * @return 0 on success, non-zero on failure (0 表示成功，非 0 表示失败)
 *
 * @note This function is used to set device communication parameters (此函数用于设定设备通讯参数)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetProtocolPara(uint GtwyHandle, uint CANid, uint Index, uint iData, uint* pRxdata = NULL);

/**
 * @brief Get communication parameters (查询通讯参数)
 *
 * Retrieve current communication parameters of the device, such as system update status,
 * baud rate, parity, CAN bit rate, etc. (读取设备当前通讯参数，包括系统升级状态、波特率、奇偶校验、CAN 比特率等)
 *
 * @param GtwyHandle Gateway handle (网关句柄)
 * @param CANid      Device node ID (设备站点 ID)
 * @param Index      Parameter index (参数下标), valid values are:
 *                   - PPS_SysUpdate   = 0   System update (系统升级)
 *                   - PPS_BDR         = 1   Serial baud rate (串口波特率)
 *                   - PPS_Parity      = 3   Parity (奇偶校验)
 *                   - PPS_BTR         = 5   CAN bit rate (CAN 比特率)
 *                   - PPS_TargetID    = 6   Target ID (目标 ID)
 *                   - PPS_NodeID      = 7   Node ID (站点号)
 *                   - PPS_GroupID     = 8   Group ID (组号)
 *                   - PPS_GatewayACK  = 11  Gateway acknowledgment (网关应答)
 * @param pRxdata    Pointer to receive parameter value (接收参数值的指针)
 *
 * @return 0 on success, non-zero on failure (0 表示成功，非 0 表示失败)
 *
 * @note This function is used to query device communication parameters (此函数用于查询设备通讯参数)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetProtocolPara(uint GtwyHandle, uint CANid, uint Index, uint* pRxdata);

#endif

/// [ System Info.   ]
#ifdef _BLOCK_

/**
 * @brief Set device serial number (设置设备序列号)
 *
 * @param GtwyHandle       Gateway handle (网关句柄)
 * @param CANid            Device's ID (站点)
 * @param iSerialNumberIn  Serial number to set (设置的序列号)
 * @param pSerialNumberOut Pointer to store the feedback result (设置结果反馈), can be NULL
 *
 * @note This function sets the serial number of the specified CAN device (设置指定 CAN 设备的序列号).
 *
 * @return 0: Success (设置序列号成功), !=0: Failure (设置序列号失败)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetSerial(uint GtwyHandle, uint CANid, uint iSerialNumberIn, uint* pSerialNumberOut = NULL);

/**
 * @brief Get device serial number (查询设备序列号)
 *
 * @param GtwyHandle     Gateway handle (网关句柄)
 * @param CANid          Device's ID (站点)
 * @param pSerialNumber  Pointer to receive the serial number (序列号)
 *
 * @note This function queries the serial number of the specified CAN device (查询指定 CAN 设备的序列号).
 *
 * @return 0: Success (查询序列号成功), !=0: Failure (查询序列号失败)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetSerial(uint GtwyHandle, uint CANid, uint* pSerialNumber);

/**
 * @brief Get device model information (查询器件型号)
 *
 * @param GtwyHandle   Gateway handle (网关句柄)
 * @param CANid        Device's ID (站点)
 * @param pModelInfo   Pointer to receive the device model info (器件型号)
 *
 * @note This function queries the model information of the specified CAN device (查询指定 CAN 设备的型号信息).
 *
 * @return 0: Success (查询器件型号成功), !=0: Failure (查询器件型号失败)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetModel(uint GtwyHandle, uint CANid, MODEL_INFO_OBJ* pModelInfo);

/**
 * @brief Get historical error report (查询历史错误报告)
 *
 * @param GtwyHandle  Gateway handle (网关句柄)
 * @param CANid       Device's ID (站点)
 * @param ErrIdx      Index of historical error record (历史错误记录序号, 取值: 10...17; 10=latest, 17=oldest)
 * @param pRxData     Pointer to error report data (错误报告数据, 结构体 ERR_REPORT_OBJ，详见 uirSDKgen3.h)
 *
 * @note This function retrieves a historical error record from the device (获取设备的历史错误记录).
 *
 * @return 0: Success (查询错误报告成功), !=0: Failure (查询错误报告失败)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetErrReport(uint GtwyHandle, uint CANid, uint ErrIdx, ERR_REPORT_OBJ* pRxData);

/**
 * @brief Clear all error reports (清除所有错误报告)
 *
 * @param GtwyHandle  Gateway handle (网关句柄)
 * @param CANid       Device's ID (站点)
 * @param pRxData     Pointer to feedback error report structure (反馈错误报告结构体，详见 uirSDKgen3.h)
 *
 * @note This function clears all stored error reports from the specified device (清除指定设备的所有错误记录).
 *
 * @return 0: Success (清除所有错误成功), !=0: Failure (清除所有错误失败)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkClrErrReport(uint GtwyHandle, uint CANid, ERR_REPORT_OBJ* pRxData);

/**
 * @brief Get the latest error report (获取最新错误报告)
 *
 * @param GtwyHandle  Gateway handle (网关句柄)
 * @param CANid       Device's ID (站点)
 * @param pRxData     Pointer to latest error report (最新错误报告结构体，详见 uirSDKgen3.h)
 *
 * @note This function queries the most recent error record from the device (查询设备最新错误记录).
 *
 * @return 0: Success (查询最新错误成功), !=0: Failure (查询最新错误失败)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetLastErr(uint GtwyHandle, uint CANid, ERR_REPORT_OBJ* pRxData);

/**
 * @brief Get device power-on duration (查询设备上电时长)
 *
 * @param GtwyHandle   - Gateway handle (网关句柄)
 * @param CANid        - Device's ID (站点)
 * @param pSystemTime  - Pointer to store power-on duration in microseconds (上电时长, 单位: us)
 *
 * @note This function queries the time elapsed since the device was powered on (查询设备上电至今的时长).
 *
 * @return 0: Success (查询上电时长成功), !=0: Failure (查询上电时长失败)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetSystemTime(uint GtwyHandle, uint CANid, uint* pSystemTime);

/**
 * @brief Reset device power-on duration (重置设备上电时长)
 *
 * @param GtwyHandle  - Gateway handle (网关句柄)
 * @param CANid       - Device's ID (站点)
 *
 * @note This function resets the recorded power-on time for the specified device (重置指定设备的上电时间记录).
 *
 * @return 0: Success (重置成功), !=0: Error code (错误代码)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkRstSystemTime(uint GtwyHandle, uint CANid);

#define _SY_ULK_SYS 0 // System Unlock (系统解锁)
#define _SY_RST_SYS 1 // System Restart (重启系统)
#define _SY_RST_DFL 2 // Restore Factory Settings (恢复出厂设置)
#define _SY_SYN_ENA 4 // Enable synchronization signal (使能同步信号,仅适用 2503/MMC900/MMC901).

/**
 * @brief Set a system operation parameter (设置系统操作参数)
 *
 * Use the `Index` parameter to select the target system operation:
 * - `_SY_ULK_SYS` : Unlock system (系统解锁)
 * - `_SY_RST_SYS` : Restart system (重启系统)
 * - `_SY_RST_DFL` : Restore factory settings (恢复出厂设置)
 * - `_SY_SYN_ENA` : Enable/disable sync signal (使能/禁用同步信号)
 *
 * @param GtwyHandle  Gateway handle (网关句柄)
 * @param CANid       Device's ID (站点)
 * @param Index       System operation parameter index (系统操作参数下标)
 * @param CfgData     Parameter value/content (系统操作参数内容)
 * @param pRxData     Optional pointer to receive result feedback (设置结果反馈)，可为 NULL
 *
 * @note This API configures specific system operations for the target device.
 *       Use according to supported models.
 *
 * @return 0: Success (设置成功), !=0: Failure (设置失败)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetSystem(uint GtwyHandle, uint CANid, uint Index, uint CfgData, BYTE* pRxData = NULL);

/**
 * @brief Get a system operation parameter (查询系统操作参数)
 *
 * @param GtwyHandle  Gateway handle (网关句柄)
 * @param CANid       Device's ID (站点)
 * @param Index       System operation parameter index (系统操作参数下标)
 * @param pRxData     Pointer to buffer storing the parameter value (系统操作参数内容)
 *
 * @note This API queries the value of specific system operations based on Index.
 *
 * @return 0: Success (查询成功), !=0: Failure (查询失败)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetSystem(uint GtwyHandle, uint CANid, uint Index, BYTE* pRxData);

/**
 * @brief Set the temperature alarm threshold (设置温度报警阈值)
 *
 * @param GtwyHandle  Gateway handle (网关句柄)
 * @param CANid       Device's ID (站点)
 * @param TempLimit   Temperature threshold for alarm (温度警报值)
 * @param pRxData     Optional pointer to receive feedback (结果反馈)，可为 NULL
 *
 * @note This API sets the motor/device temperature alarm trigger point.
 *
 * @return 0: Success (设置成功), !=0: Failure (设置失败)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetHeatLimit(uint GtwyHandle, uint CANid, uint TempLimit, uint* pRxData = NULL);

/**
 * @brief Get the temperature alarm threshold (查询温度报警阈值)
 *
 * @param GtwyHandle  Gateway handle (网关句柄)
 * @param CANid       Device's ID (站点)
 * @param pRxData     Pointer to store threshold value (温度警报值)
 *
 * @note Retrieves the configured temperature alarm threshold of the device.
 *
 * @return 0: Success (查询成功), !=0: Failure (查询失败)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetHeatLimit(uint GtwyHandle, uint CANid, uint* pRxData);

/**
 * @brief Get the current device temperature (查询当前温度)
 *
 * @param GtwyHandle  Gateway handle (网关句柄)
 * @param CANid       Device's ID (站点)
 * @param pRxData     Pointer to store current temperature (当前温度值)
 *
 * @note This API queries the real-time temperature reading from the device.
 *
 * @return 0: Success (查询成功), !=0: Failure (查询失败)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetTemperature(uint GtwyHandle, uint CANid, uint* pRxData);

/**
 * @brief Set the date/time in MMC device (设置 MMC 日期时间)
 *
 * @param GtwyHandle  Gateway handle (网关句柄)
 * @param CANid       Device's ID (站点)
 * @param yr          Year (年)
 * @param mo          Month (月)
 * @param da          Day (日)
 * @param hr          Hour (时)
 * @param mi          Minute (分)
 * @param se          Second (秒)
 *
 * @note Adjusts the internal date/time of MMC devices.
 *
 * @return 0: Success (设置成功), !=0: Failure (设置失败)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetMmcDate(uint GtwyHandle, uint CANid, uint yr, uint mo, uint da, uint hr, uint mi, uint se);

/**
 * @brief Get the date/time from MMC device (查询 MMC 日期时间)
 *
 * @param GtwyHandle  Gateway handle (网关句柄)
 * @param CANid       Device's ID (站点)
 * @param yr          Pointer to store year (年)
 * @param mo          Pointer to store month (月)
 * @param da          Pointer to store day (日)
 * @param hr          Pointer to store hour (时)
 * @param mi          Pointer to store minute (分)
 * @param se          Pointer to store second (秒)
 *
 * @note Reads the internal date/time from MMC devices.
 *
 * @return 0: Success (查询成功), !=0: Failure (查询失败)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetMmcDate(uint GtwyHandle, uint CANid, uint* yr, uint* mo, uint* da, uint* hr, uint* mi, uint* se);

/**
 * @brief Execute a built-in motor function (运行内建功能)
 *
 * @param GtwyHandle  Gateway handle (网关句柄)
 * @param CANid       Device's ID (站点)
 * @param pSet        Pointer to structure containing command and parameters for the motor (发送指令和参数)
 * @param pRxData     Pointer to receive returned data from the motor (接收电机返回数据)
 *
 * @note Use this API to run device-specific embedded functions.
 *       Command format is defined in `OPR_DATA_OBJ`.
 *
 * @return 0: Success (执行成功), !=0: Failure code (失败代码)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkBuildinFunc(uint GtwyHandle, uint CANid, OPR_DATA_OBJ* pSet, OPR_DATA_OBJ* pRxData);

#endif

/// [ Initial Config ]
#ifdef _BLOCK_

/**
 * @enum INITIAL_CONFIG_INDEX
 * @brief Power-on configuration parameter index (上电配置参数下标)
 *
 * Use these values in `Index` when calling SdkSetInitialConfig / SdkGetInitialConfig.
 *
 * | Value | Description | 中文 
 * |-------|-------------------------------------------------------------------------
 * | 0     | Power-on Enable (0=Disable, 1=Enable) | 上电使能 (0=禁止, 1=使能) |
 * | 1     | Positive turn direction (0=CW, 1=CCW) | 正转向设定 (0=顺时针, 1=逆时针) |
 * | 2     | UPG Enable (0=Disable, 1=Enable) | UPG使能 (0=禁止, 1=使能) |
 * | 3     | Input trigger lock (0=Disable, 1=Enable) | 输入触发锁定 (0=禁止, 1=使能) |
 * | 4     | Acceleration/Deceleration mode (0=Value, 1=Time) | 加减速方式 (0=数值, 1=时间) |
 * | 5     | Encoder type (0=Incremental, 1=Absolute; 720 by servo drive) | 编码器类型 (0=增量, 1=绝对值; 720由伺服驱动器决定) |
 * | 6     | Open/Closed loop (0=Open loop, 1=Closed loop) | 开闭环使能 (0=开环, 1=闭环) |
 * | 7     | Soft limit enable (0=Disable, 1=Enable) | 软限位使能 (0=禁止, 1=使能) |
 * | 8     | Brake port control (0=Normal, 1=Enable output OP01) | 刹车端口 (0=正常, 1=使能刹车控制) |
 * | 9     | Brake closed loop monitoring (0=Normal, 1=Enable input IN02) | 刹车闭环 (0=正常, 1=使能刹车监控) |
 * | 10    | Forced hibernation on power-up (0=Normal, 1=Hibernate) | 强制休眠 (0=正常, 1=上电休眠) |
 * | 255   | Overall query/settings (.uiValue) | 整体查询/设置 (.uiValue) |
 */

/**
 * @brief Set device power-on configuration (设置上电配置)
 *
 * @param GtwyHandle  Gateway handle (网关句柄)
 * @param CANid       Device's ID (站点)
 * @param Index       Power-on configuration index (上电配置参数下标) — see @ref INITIAL_CONFIG_INDEX
 * @param TxData      Parameter value to set (上电配置参数内容)
 * @param pRxData     Optional pointer to receive feedback result (结果反馈)，可为 NULL
 *
 * @note This API configures the device's behavior at power-up, such as enabling/disabling, direction, braking, etc.
 *
 * @return 0: Success (设置成功)
 *         !=0: Failure (设置失败)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetInitialConfig(uint GtwyHandle, uint CANid, uint Index, uint TxData, uint* pRxData = NULL);

/**
 * @brief Get device power-on configuration (查询上电配置)
 *
 * @param GtwyHandle  Gateway handle (网关句柄)
 * @param CANid       Device's ID (站点)
 * @param Index       Power-on configuration index (上电配置参数下标) — see @ref INITIAL_CONFIG_INDEX
 * @param pRxData     Pointer to store retrieved parameter value (对应上电配置参数的内容)
 *
 * @note This API queries a specific power-on configuration parameter from the device.
 *
 * @return 0: Success (查询成功); !=0: Failure (查询失败)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetInitialConfig(uint GtwyHandle, uint CANid, uint Index, uint* pRxData);

#endif

/// [ Inform Enable  ]
#ifdef _BLOCK_

/**
 * @enum INFORM_ENABLE_INDEX
 * @brief Notification enable parameter index (通知使能参数下标)
 *
 * Use these values in `Index` when calling SdkSetInformEnable / SdkGetInformEnable.
 *
 * | Value | Description (英文)                                | 中文说明 |
 * |-------|---------------------------------------------------|----------|
 * | 0     | P1 port change notification (0=Disable, 1=Enable) | P1端口变化通知 (0=禁止, 1=使能) |
 * | 1     | P2 port change notification (0=Disable, 1=Enable) | P2端口变化通知 (0=禁止, 1=使能) |
 * | 2     | P3 port change notification (0=Disable, 1=Enable) | P3端口变化通知 (0=禁止, 1=使能) |
 * | 3     | P4 port change notification (0=Disable, 1=Enable) | P4端口变化通知 (0=禁止, 1=使能) |
 * | 4     | P5 port change notification (0=Disable, 1=Enable) | P5端口变化通知 (0=禁止, 1=使能) |
 * | 5     | P6 port change notification (0=Disable, 1=Enable) | P6端口变化通知 (0=禁止, 1=使能) |
 * | 6     | P7 port change notification (0=Disable, 1=Enable) | P7端口变化通知 (0=禁止, 1=使能) |
 * | 7     | P8 port change notification (0=Disable, 1=Enable) | P8端口变化通知 (0=禁止, 1=使能) |
 * | 8     | PTP movement in-place notification (0=Disable, 1=Enable) | PTP运动到位通知 (0=禁止, 1=使能) |
 * | 9     | Motor gridlock notification (0=Disable, 1=Enable) | 电机堵转通知 (0=禁止, 1=使能) |
 * | 10    | PVT data buffer dry notification (0=Disable, 1=Enable) | PVT数据缓冲区水位全干通知 (0=禁止, 1=使能) |
 * | 11    | PVT data buffer low-level notification (0=Disable, 1=Enable) | PVT数据缓冲区水位很低通知 (0=禁止, 1=使能) |
 * | 255   | Overall query/settings (.uiValue) | 整体查询/设置 (.uiValue) |
 */

/**
 * @brief Set device notification enable status (设置通知使能)
 *
 * Configures whether specific notification events are enabled, such as port changes,
 * PTP movement complete, motor stall, or PVT buffer alerts.
 *
 * @param GtwyHandle  Gateway handle (网关句柄)
 * @param CANid       Device's ID (站点)
 * @param Index       Notification parameter index — see @ref INFORM_ENABLE_INDEX
 * @param TxData      Notification enable value (通知使能参数内容)
 *                    - 0: Disable / 禁止
 *                    - 1: Enable / 使能
 * @param pRxData     Optional pointer to receive the configuration result (结果反馈)，可为 NULL
 *
 * @return 0: Success (设置成功)
 *         !=0: Failure (设置失败)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetInformEnable(uint GtwyHandle, uint CANid, uint Index, uint TxData, uint* pRxData = NULL);

/**
 * @brief Get device notification enable status (查询通知使能)
 *
 * Retrieves the enabled/disabled status for a specific notification event.
 *
 * @param GtwyHandle  Gateway handle (网关句柄)
 * @param CANid       Device's ID (站点)
 * @param Index       Notification parameter index — see @ref INFORM_ENABLE_INDEX
 * @param pRxData     Pointer to store the retrieved enable status (对应通知使能参数的内容)
 *                    - 0: Disabled / 禁止
 *                    - 1: Enabled / 使能
 *
 * @return 0: Success (查询成功)
 *         !=0: Failure (查询失败)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetInformEnable(uint GtwyHandle, uint CANid, uint Index, uint* pRxData);

#endif

//======== Motion & I/O ========

/// [Motion.Static ]
#ifdef _BLOCK_

/**
 * @enum MOTOR_CONFIG_INDEX
 * @brief Motor driver parameter index (驱动参数下标)
 *
 * Used in SdkSetMotorConfig / SdkGetMotorConfig.
 *
 * | Value | Description (英文)                           | 中文说明 |
 * |-------|----------------------------------------------|----------|
 * | 0     | Micro-stepping resolution [1,2,4,8,16]       | 细分 [1,2,4,8,16] |
 * | 1     | Working current [10...80]                    | 电流 [10...80] |
 * | 2     | Idle current throttling [0...100%]           | 待机节流 [0...100(%)] |
 * | 3     | Auto driver ON delay [0...60000 ms]          | 上电自动使能延时 [0...60000(ms)] |
 * | 4     | Custom / Reserved                            | 保留 |
 * | 5     | Brake control                                | 刹车控制 |
 */
#define MTS_MCS_IDX 0
#define MTS_CUR_IDX 1
#define MTS_PSV_IDX 2
#define MTS_ENA_IDX 3
#define MTS_CUC_IDX 4
#define MTS_BRK_IDX 5

/**
 * @brief Set motor driver parameter (设置驱动参数)
 * @param GtwyHandle Gateway handle (网关句柄)
 * @param CANid      Device's ID (站点)
 * @param Index      Motor driver parameter index — see @ref MOTOR_CONFIG_INDEX
 * @param iData      Parameter value (驱动参数内容)
 * @param pRxData    Optional feedback pointer (结果反馈指针，可为 NULL)
 * @return 0 Success / 成功; !=0 Failure / 失败
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetMotorConfig(uint GtwyHandle, uint CANid, uint Index, uint iData, uint* pRxData = NULL);

/**
 * @brief Get motor driver parameter (查询驱动参数)
 * @param GtwyHandle Gateway handle (网关句柄)
 * @param CANid      Device's ID (站点)
 * @param Index      Motor driver parameter index — see @ref MOTOR_CONFIG_INDEX
 * @param pRxData    Pointer to store parameter value (驱动参数内容)
 * @return 0 Success / 成功; !=0 Failure / 失败
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetMotorConfig(uint GtwyHandle, uint CANid, uint Index, uint* pRxData);

/**
 * @brief Set acceleration (设置加速度)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetAcceleration(uint GtwyHandle, uint CANid, uint iData, uint* pRxData = NULL);

/**
 * @brief Get acceleration (查询加速度)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetAcceleration(uint GtwyHandle, uint CANid, uint* pRxData);

/**
 * @brief Set deceleration (设置减速度)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetDeceleration(uint GtwyHandle, uint CANid, uint iData, uint* pRxData = NULL);

/**
 * @brief Get deceleration (查询减速度)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetDeceleration(uint GtwyHandle, uint CANid, uint* piDeceleration);

/**
 * @brief Set cut-in speed (设置切入速度)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetCutInSpeed(uint GtwyHandle, uint CANid, uint iData, uint* pRxData = NULL);

/**
 * @brief Get cut-in speed (查询切入速度)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetCutInSpeed(uint GtwyHandle, uint CANid, uint* pRxData);

/**
 * @brief Set emergency stop deceleration (设置急停减速度)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetStopDeceleration(uint GtwyHandle, uint CANid, uint iData, uint* pRxData = NULL);

/**
 * @brief Get emergency stop deceleration (查询急停减速度)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetStopDeceleration(uint GtwyHandle, uint CANid, uint* pRxData);

/**
 * @enum ENCODER_CONFIG_INDEX
 * @brief Encoder parameter index (编码器参数下标)
 *
 * | Value | Description (英文)                                                   | 中文说明 |
 * |-------|----------------------------------------------------------------------|----------|
 * | 0     | Encoder lines (after x4 = pulses per turn) [10..16000]               | 编码器线数（x4 后 = 每圈脉冲数）范围 [10..16000] |
 * | 1     | Stall tolerance (must >= 100)                                        | 堵转容差（必须 >= 100） |
 * | 2     | Absolute encoder counts per turn                                     | 绝对编码器单圈位数 |
 * | 3     | Absolute encoder battery status (0 = low battery, 1 = OK)            | 绝对编码器电池状态（0 = 低电量，1 = 正常） |
 * | 4     | INC encoder lines (actual value, e.g., magnetic encoder 90 lines)    | INC 编码器线数（实际值，如磁编码器 90 线） |
 * | 5     | Clear residual position error (usually after stop)                   | 清除残留位置误差（一般在停止后设置） |
 */
#define QES_LPR_IDX 0
#define QES_PQE_IDX 1
#define QES_AER_IDX 2
#define QES_ABW_IDX 3
#define QES_CPR_IDX 4
#define QES_SYN_IDX 5

UISIMCANFUNC_API ERRO STDCALLAPI SdkSetEncoderConfig(uint GtwyHandle, uint CANid, uint Index, uint iData, uint* pRxData = NULL);

UISIMCANFUNC_API ERRO STDCALLAPI SdkGetEncoderConfig(uint GtwyHandle, uint CANid, uint Index, uint* pRxData);

/**
 * @brief Calibrate magnetic encoder (校准磁编码器)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkCalibrateEncoder(uint GtwyHandle, uint CANid);

/**
 * @brief Get auxiliary position info (查询辅助位置信息)
 *
 * Closed-loop mode: returns open-loop pulse value
 * Open-loop mode: returns encoder value
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetAuxPosition(uint GtwyHandle, uint CANid, int* pRxData);

/**
 * @brief Set backlash compensation value (设置回程间隙补偿值)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetBacklashComp(uint GtwyHandle, uint CANid, uint iData, uint* pRxData = NULL);

/**
 * @brief Get backlash compensation value (查询回程间隙补偿值)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetBacklashComp(uint GtwyHandle, uint CANid, uint* pRxData);

/**
 * @brief Set motion limits (设置运动限位)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetMotionLimits(uint GtwyHandle, uint CANid, MOTION_LIMIT_OBJ* pData, MOTION_LIMIT_OBJ* pRxData = NULL);

/**
 * @brief Get motion limits (查询运动限位)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetMotionLimits(uint GtwyHandle, uint CANid, MOTION_LIMIT_OBJ* pRxData);

/**
 * @brief Reset motion limits (复位行程限位)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkRstMotionLimits(uint GtwyHandle, uint CANid, MOTION_LIMIT_OBJ* pRxData = NULL);

/**
 * @brief Enable or disable limit check (设置限位检查使能/禁止)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetLimitCheck(uint GtwyHandle, uint CANid, uint iData, uint* pRxData = NULL);

/**
 * @brief Get limit check status (查询限位检查使能/禁止状态)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetLimitCheck(uint GtwyHandle, uint CANid, uint* pRxData);

/**
 * @enum DESIRED_VALUE_INDEX
 * @brief Desired value parameter index (目标值参数下标)
 *
 * | Value | Description (英文)                                   | 中文说明 |
 * |-------|------------------------------------------------------|----------|
 * | 0     | Control mode: JV / PTP / PVT                         | 控制模式: JV / PTP / PVT |
 * | 1     | Working current                                      | 工作电流 |
 * | 2     | Target speed                                         | 目标速度 |
 * | 3     | Position relative                                    | 位移 |
 * | 4     | Position absolute                                    | 位置 |
 * | 5     | Reserved                                              | 保留 |
 */
#define DVR_MOD_IDX 0
#define DVR_CUR_IDX 1
#define DVR_SPD_IDX 2
#define DVR_PRM_IDX 3
#define DVR_PAM_IDX 4
#define DVR_TIS_IDX 5

/**
 * @brief Get desired value (获取目标值参数)
 * @param Index Desired value index — see @ref DESIRED_VALUE_INDEX
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetDesiredValue(uint GtwyHandle, uint CANid, uint Index, void* pRxData);

#endif

/// [Motion.Dynamic]
#ifdef _BLOCK_

/**
 * @brief Query motor status (查询电机状态)
 *
 * @param GtwyHandle [in] Gateway handle (网关句柄)
 * @param CANid      [in] Device's ID (站点)
 * @param pStatus    [out] Pointer to MOTION_STATUS_OBJ structure (电机状态对象，结构定义见 uirSDKgen3.h)
 * @param pVe        [out] Motor speed (电机速度)
 * @param pPr        [out] Motor displacement / Relative position (电机位移，相对位置)
 * @param pPa        [out] Motor position / Absolute position (电机位置，绝对位置)
 * @return 0 Success (成功); !0 Failure (失败)  
 *
 * @note This function retrieves both status and real-time motion data.
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetMotionStatus(uint GtwyHandle, uint CANid, MOTION_STATUS_OBJ* pStatus, int* pVe, int* pPr, int* pPa);

/**
* @brief Reset motor status (重置电机状态)
*
* Only clears PAIF / PSIF / TCIF flags and returns updated status.
*
* @param GtwyHandle [in] Gateway handle (网关句柄)
* @param CANid      [in] Device's ID (站点)
* @param pStatus    [out] Updated motor status feedback (MOTION_STATUS_OBJ)
* @return 0 Success (成功); !0 Failure (失败)
*
* @note 状态复位不会改变位置或速度，仅清除中断/状态位。
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkRstMotionStatus(uint GtwyHandle, uint CANid, MOTION_STATUS_OBJ* pStatus);

/**
 * @brief Enable or disable motor driver (设置电机使能/脱机)
 *
 * @param GtwyHandle [in]
 * @param CANid      [in]
 * @param bMotorOn   [in] 0: OFF / 脱机; 1: ON / 使能
 * @param pbEnable   [out] Enable state feedback (结果反馈，可为 NULL)
 * @return 0 Success (成功); !0 Failure (失败)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetMotorOn(uint GtwyHandle, uint CANid, uint bMotorOn, uint* pbEnable = NULL);

/**
 * @brief Get motor enable/offline status (获取电机使能/脱机状态)
 *
 * @param pbEnable [out] 0: OFF (脱机); 1: ON (使能)
 * @return 0 Success (成功); !0 Failure (失败)  
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetMotorOn(uint GtwyHandle, uint CANid, uint* pbEnable);

/**
 * @brief Start motion immediately (设置开始运动)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetBeginMxn(uint GtwyHandle, uint CANid, uint* pRxData = NULL);

/**
 * @brief Set timed motion start (设置开始运动的时间)
 *
 * @param time [in] Delay time in ms before motion starts (延迟启动时间 ms)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetBeginMxnTime(uint GtwyHandle, uint CANid, uint time, uint* pRxData = NULL);

/**
 * @brief Emergency stop motion (设置紧急停止)
 *
 * @return 0 Success (成功); !0 Failure (失败)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetStopMxn(uint GtwyHandle, uint CANid, uint* pRxData = NULL);

/**
 * @brief Set current position as origin (当前位置设为原点)
 * @return 0 Success (成功); !0 Failure (失败)
 * @note Sets absolute position PA = 0
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetOrigin(uint GtwyHandle, uint CANid);

/// ========================
//  JOG Motion
/// ========================

/**
 * @brief Set JOG speed mode + target speed (设定速度模式 + 目标速度)
 *
 * @param Velo [in] Target speed
 * @param pRxVelo [out] Speed feedback
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetJogMxn(uint GtwyHandle, uint CANid, int Velo, int* pRxVelo = NULL);

/**
 * @brief Get current JOG speed (查询当前速度)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetJogMxn(uint GtwyHandle, uint CANid, int* pRxVelo);

/// ========================
//  PTP Motion (Absolute)
/// ========================

/**
 * @brief Set absolute position motion (设定位置运动)
 *
 * Sets both target speed and absolute target position.
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetPtpMxnA(uint GtwyHandle, uint CANid, int Velo, int Pa, int* pRxVelo = NULL, int* pRxPa = NULL);

/**
* @brief Get current speed & absolute position (查询当前速度 + 当前位置)
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetPtpMxnA(uint GtwyHandle, uint CANid, int* pRxVelo, int* pRxPa);

/**
 * @brief Get absolute position only (查询当前位置)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetAbsolutePos(uint GtwyHandle, uint CANid, int* pRxPa);

/// ========================
//  PTP Motion (Relative)
/// ========================

/**
 * @brief Set displacement motion (设定位移运动)
 *
 * Sets both target speed and relative displacement.
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetPtpMxnR(uint GtwyHandle, uint CANid, int Velo, int Pr, int* pRxVelo = NULL, int* pRxPr = NULL);

/**
* @brief Get current speed & displacement (查询当前速度 + 当前位移)
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetPtpMxnR(uint GtwyHandle, uint CANid, int* pRxVelo, int* pRxPr);

/**
 * @brief Get displacement only (查询当前位移)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetRelativePos(uint GtwyHandle, uint CANid, int* pRxPr);








///=============================
// PVT Motion Control Functions
///=============================

/**
 * @brief Reset PVT buffer (复位 PVT 缓存区)
 *
 * 清空轨迹点缓存区、水位、执行点。
 * 建议：发送新的 FIFO 序列前执行该操作。
 * 前提：必须退出 PVT 模式（等待水位干或发送 SD 指令）。
 *
 * @param GtwyHandle [in] Gateway handle 网关句柄
 * @param CANid      [in] Device’s ID    站点
 * @return 0 成功 / Success; 非零 失败 / Failure
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetPvtRstBuf(uint GtwyHandle, uint CANid);

/**
 * @brief Get PVT buffer water level (查询 PVT 缓存水位)
 *
 * @param pWaterLevel [out] Usable PVT data sets in buffer (可用的 PVT 数据集数量)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetPvtWaterLevel(uint GtwyHandle, uint CANid, uint* pWaterLevel);

/**
 * @brief Set PVT configuration (设置 PVT 参数)
 *
 * @param pData   [in]  Pointer to PVT_CONFIG_OBJ (参数结构体, 见 uirSDKgen3.h)
 * @param pRxData [out] Feedback PVT_CONFIG_OBJ  (反馈结构体)
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetPvtCfg(uint GtwyHandle, uint CANid, PVT_CONFIG_OBJ* pData, PVT_CONFIG_OBJ* pRxData);

/**
* @brief Get PVT configuration (查询 PVT 参数)
*
* @param pRxData [out] Pointer to PVT_CONFIG_OBJ receiving parameters
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetPvtCfg(uint GtwyHandle, uint CANid, PVT_CONFIG_OBJ* pRxData);

/**
* @brief Set PVT motion mode and starting label (设定 PVT 模式 + 运动起点标签)
*
* @param iData   [in]  Start motion label (运动起点标号)
* @param pRxData [out] Feedback label (反馈值)
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetPvtMxn(uint GtwyHandle, uint CANid,uint iData, uint* pRxData);

/**
* @brief Get current PVT execution point label (查询当前执行点标签)
*
* @param pRxData [out] Execution point label
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetPvtExeIdx(uint GtwyHandle, uint CANid, uint* pRxData);

/**
* @brief Set PVT track point (设置 PVT 轨迹点)
*
* 用于 32-bit 数据格式的轨迹点配置。
*
* @param Index   [in]  Track point index      (轨迹点下标)
* @param pData   [in]  PVT_QPVT_OBJ data      (轨迹点数据)
* @param pRxData [out] Feedback of set point  (轨迹点反馈值)
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetPvtQPVT(uint GtwyHandle, uint CANid, uint Index, PVT_QPVT_OBJ* pData, PVT_QPVT_OBJ* pRxData);

/**
* @brief Get PVT track point (查询 PVT 轨迹点)
*
* @param Index   [in]  Track point index
* @param pRxData [out] Pointer to PVT_QPVT_OBJ receiving data
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetPvtQPVT(uint GtwyHandle, uint CANid, uint Index, PVT_QPVT_OBJ* pRxData);

/**
* @brief Set quick feed QP/QV/QT (设置快速输入的 QP/QV/QT)
*
* 可用于批量/高速更新轨迹点数据。
*
* @param pData   [in]  Pointer to PVT_QPVT_OBJ (数据结构)
* @param pRxData [out] Feedback PVT_QPVT_OBJ
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetPvtQF(uint GtwyHandle, uint CANid, PVT_QPVT_OBJ* pData, PVT_QPVT_OBJ* pRxData);

///=============================
// P-T Motion Control Functions
///=============================

/**
 * @brief Set PT motion time interval (设置 PT 运动时间间隔)
 *
 * 将 PT 运动的时间间隔发送到电机控制器，并返回设备端反馈的时间间隔。
 *
 * @param GtwyHandle [in] Gateway handle 网关句柄
 * @param CANid      [in] Device’s ID    站点
 * @param PTtime     [in] Time interval for PT motion (PT运动时间间隔, 单位取决于系统配置)
 * @param pRxPTtime  [out] Feedback time interval from motor
 * @return 0 成功 / Success; 非零 失败 / Failure
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetPtTime(uint GtwyHandle, uint CANid, uint PTtime, uint* pRxPTtime);

/**
* @brief Get PT motion time interval (查询 PT 运动时间间隔)
*
* 从电机获取当前的 PT 运动时间间隔。
*
* @param GtwyHandle [in] Gateway handle 网关句柄
* @param CANid      [in] Device’s ID    站点
* @param pRxPTtime  [out] Feedback time interval from motor
* @return 0 成功 / Success; 非零 失败 / Failure
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetPtTime(uint GtwyHandle, uint CANid, uint* pRxPTtime);

/**
* @brief Set PT motion position (设置 PT 运动位置)
*
* 将指定轨迹点的 PT 运动位置发送到电机控制器。
*
* @param GtwyHandle  [in] Gateway handle   网关句柄
* @param CANid       [in] Device’s ID      站点
* @param Index       [in] Track point index 轨迹点下标
* @param PTposition  [in] Position value for PT motion 位置值
* @param pRxPos      [out] Feedback position from motor
* @return 0 成功 / Success; 非零 失败 / Failure
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetPtPosition(uint GtwyHandle, uint CANid, uint Index, int PTposition, int* pRxPos);

/**
* @brief Get PT motion position (查询 PT 运动位置)
*
* 读取指定轨迹点的 PT 运动位置。
*
* @param GtwyHandle [in] Gateway handle 网关句柄
* @param CANid      [in] Device’s ID    站点
* @param Index      [in] Track point index 轨迹点下标
* @param pRxPos     [out] Feedback position from motor
* @return 0 成功 / Success; 非零 失败 / Failure
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetPtPosition(uint GtwyHandle, uint CANid, uint Index, int* pRxPos);

///===================================
// Internal PVT Extended Functions
///===================================

/**
* @brief Set I/O device address for PVT sequence (设置 PVT 序列使用的 I/O 输出设备地址)
*
* 内部接口，用于指定 PVT 序列运行时的输出设备 CAN-ID。
*
* @param IODcanid     [in] I/O device CAN ID
* @param pRxIODcanid  [out] Feedback CAN ID from device
* @return 0 成功 / Success; 非零 失败 / Failure
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetPvtIODevice(uint GtwyHandle, uint CANid, BYTE IODcanid, BYTE* pRxIODcanid);

/**
* @brief Get I/O device address for PVT sequence (查询 PVT 序列使用的 I/O 输出设备地址)
*
* 内部接口，从设备读取当前绑定的 I/O 输出设备 CAN-ID。
*
* @param pRxIODcanid  [out] Feedback CAN ID from device
* @return 0 成功 / Success; 非零 失败 / Failure
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetPvtIODevice(uint GtwyHandle, uint CANid, BYTE* pRxIODcanid);

/**
* @brief Set PVT track point (24-bit length data) (设置 PVT 轨迹点 - 24位数据)
*
* 用于发送 24-bit 长度的轨迹点数据到设备。
*
* @param Index   [in]  Track point index            轨迹点下标
* @param pData   [in]  Pointer to PVT_QPVT_OBJ      数据结构 (详见 uirSDKgen3.h)
* @param pRxData [out] Feedback track point data    反馈数据结构
* @return 0 成功 / Success; 非零 失败 / Failure
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetPvtUcode(uint GtwyHandle, uint CANid, uint Index, PVT_QPVT_OBJ* pData, PVT_QPVT_OBJ* pRxData);

/**
	* @brief Set PVT track point (K-code format) (设置 PVT 轨迹点 - K码格式)
	*
	* 用于发送 K-code 格式的轨迹点数据到设备（特殊编码格式）。
	*
	* @param pData   [in]  Pointer to PVT_KCODE_OBJ     数据结构 (详见 uirSDKgen3.h)
	* @param pRxData [out] Feedback track point data    反馈数据结构
	* @return 0 成功 / Success; 非零 失败 / Failure
	*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetPvtKcode(uint GtwyHandle, uint CANid, PVT_KCODE_OBJ* pData, PVT_KCODE_OBJ* pRxData);

#endif

/// [I/O Control ]
#ifdef _BLOCK_

//[Port Function value] CfgCode Use the following parameters
#define IOC_DOU_IDX 0 // Digital output 数字输出
#define IOC_DIN_IDX 1 // Digital input 数字输入
#define IOC_AIN_IDX 2 // Analog input 模拟输入
#define IOC_QEI_IDX 3 // QEI

//===================================
// I/O Port Management Functions
//===================================

/**
 * @brief Set I/O port function (设置 I/O 端口功能)
 *
 * 配置指定 I/O 端口的功能值，并返回设备回馈的功能设定。
 *
 * @param GtwyHandle [in] Gateway handle 网关句柄
 * @param CANid      [in] Device’s ID    站点
 * @param PortIx     [in] IO port number IO端口号
 * @param CfgCode    [in] Functional value 功能值
 * @param pRxData    [out,opt] Feedback functional value 功能数据反馈值 (可为 NULL)
 * @return 0 成功 / Success; 非零 失败 / Failure
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetIOConfig(uint GtwyHandle, uint CANid, uint PortIx, uint CfgCode, uint* pRxData = NULL);

/**
* @brief Get I/O port function (查询 I/O 端口功能)
*
* 读取指定 I/O 端口当前配置的功能值。
*
* @param GtwyHandle [in] Gateway handle 网关句柄
* @param CANid      [in] Device’s ID   站点
* @param PortIx     [in] IO port number IO端口号
* @param pRxData    [out] Functional value 功能值
* @return 0 成功 / Success; 非零 失败 / Failure
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetIOConfig(uint GtwyHandle, uint CANid, uint PortIx, uint* pRxData);

/**
* @brief Set digital input trigger delay (设置数字输入触发延时)
*
* 用于设定数字输入的触发延时（消抖作用）。
* 如果 TrigDelay > 60000，认作单次触发，原触发动作会被清除。
*
* @param GtwyHandle [in] Gateway handle 网关句柄
* @param CANid      [in] Device’s ID    站点
* @param PortIx     [in] IO port number IO端口号
* @param TrigDelay  [in] Delay time in ms 延时时间（单位：ms）
* @param pRxData    [out,opt] Feedback functional value 功能数据反馈值 (可为 NULL)
* @return 0 成功 / Success; 非零 失败 / Failure
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetTriggerMode(uint GtwyHandle, uint CANid, uint PortIx, uint TrigDelay, uint* pRxData = NULL);

/**
* @brief Query digital input trigger delay (查询数字输入触发延时)
*
* 获取指定 I/O 端口的数字输入延时设定。
*
* @param GtwyHandle [in] Gateway handle 网关句柄
* @param CANid      [in] Device’s ID   站点
* @param PortIx     [in] IO port number IO端口号
* @param pRxData    [out] Delay time in ms 延时时间（单位：ms）
* @return 0 成功 / Success; 非零 失败 / Failure
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetTriggerMode(uint GtwyHandle, uint CANid, uint PortIx, uint* pRxData);

/**
* @brief Get analog input value (查询模拟输入值)
*
* 读取指定 I/O 端口的模拟量输入值。
*
* @param GtwyHandle [in] Gateway handle             网关句柄
* @param CANid      [in] Device’s ID                站点
* @param PortIx     [in] IO port number             IO端口号
* @param pRxData    [out] Analog value 模拟量 (整数表示，单位和范围依设备规格)
* @return 0 成功 / Success; 非零 失败 / Failure
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetAnalogInput(uint GtwyHandle, uint CANid, uint PortIx, int* pRxData);

/**
* @brief Set digital output value (设置数字输出)
*
* 修改指定 I/O 端口的数字输出值。
*
* @param GtwyHandle [in] Gateway handle 网关句柄
* @param CANid      [in] Device’s ID    站点
* @param pDOValue   [in] Pointer to DIGITAL_OUTPUT_OBJ  IO端口输出值 (定义详见 uirSDKgen3.h)
* @param pRxData    [out,opt] Pointer to UIM_DIO_OBJ  IO端口输出值反馈 (可为 NULL)
* @return 0 成功 / Success; 非零 失败 / Failure
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetDigitalOutput(uint GtwyHandle, uint CANid, DIGITAL_OUTPUT_OBJ* pDOValue, UIM_DIO_OBJ* pRxData = NULL);

/**
* @brief Get digital port levels (查询数字端口电平)
*
* 读取设备数字端口的输入和输出电平。
*
* @param GtwyHandle [in] Gateway handle 网关句柄
* @param CANid      [in] Device’s ID    站点
* @param pRxData    [out] Pointer to UIM_DIO_OBJ 数字端口电平(包括输入端口和输出端口)
* @return 0 成功 / Success; 非零 失败 / Failure
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetDIOport(uint GtwyHandle, uint CANid, UIM_DIO_OBJ* pRxData);


UISIMCANFUNC_API ERRO STDCALLAPI SdkSetPWM(uint GtwyHandle, uint CANid, uint PortIx, DIGITAL_PWMCFG_OBJ pPwmCfg, DIGITAL_PWMCFG_OBJ* pRxPwmCfg);

UISIMCANFUNC_API ERRO STDCALLAPI SdkGetPWM(uint GtwyHandle, uint CANid, uint PortIx, DIGITAL_PWMCFG_OBJ* pRxPwmCfg);


///[Input trigger action parameters]
#define IOL_CAT_GENERAL 0 // indicates the parameter applied by the received instruction; 收到指令应用的参数 
#define IOL_CAT_STALLED 1 // Application after motor Stall detection; 电机堵转 检出后应用
#define IOL_CAT_IN1RISE 2 // Apply after IN1 rising edge is detected; IN1上升沿检出后应用  
#define IOL_CAT_IN1FALL 3 // Apply after IN1 falling edge is detected;IN1下降沿检出后应用
#define IOL_CAT_IN2RISE 4 // Apply after IN2 rising edge is detected; IN2上升沿检出后应用
#define IOL_CAT_IN2FALL 5 // Apply after IN2 falling edge is detected;IN2下降沿检出后应用
#define IOL_CAT_IN3RISE 6 // Apply after IN3 rising edge is detected; IN3上升沿检出后应用
#define IOL_CAT_IN3FALL 7 // Apply after IN3 falling edge is detected;IN3下降沿检出后应用
#define IOL_CAT_IN4RISE 8 // Apply after IN4 rising edge is detected; IN4上升沿检出后应用
#define IOL_CAT_IN4FALL 9 // Apply after IN4 falling edge is detected;IN4下降沿检出后应用

//===================================
// Input Trigger Logic Functions
//===================================
// @ingroup input_logic

/**
 * @brief Set input trigger action - AC (Acceleration)
 *        设置输入触发动作 - AC（加速度）
 *
 * 向设备配置在指定触发分类下的加速度参数，并返回回馈值。
 *
 * @param GtwyHandle [in] Gateway handle 网关句柄
 * @param CANid      [in] Device’s ID    站点
 * @param Catagory   [in] Trigger classification 触发分类
 * @param iData      [in] Acceleration 加速度
 * @param pRxData    [out,opt] Acceleration feedback value 加速度反馈值 (可为 NULL)
 * @return 0 成功 / Success; 非零 失败 / Failure
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetInputLogicAC(uint GtwyHandle, uint CANid, uint Catagory, uint iData, uint* pRxData = NULL);

/**
* @brief Get input trigger action - AC (Acceleration)
*        查询输入触发动作 - AC（加速度）
*
* @param GtwyHandle [in] Gateway handle 网关句柄
* @param CANid      [in] Device’s ID    站点
* @param Catagory   [in] Trigger classification 触发分类
* @param pRxData    [out] Acceleration 加速度
* @return 0 成功 / Success; 非零 失败 / Failure
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetInputLogicAC(uint GtwyHandle, uint CANid, uint Catagory, uint* pRxData);

/**
* @brief Set input trigger action - DC (Deceleration)
*        设置输入触发动作 - DC（减速度）
*
* @param GtwyHandle [in] Gateway handle 网关句柄
* @param CANid      [in] Device’s ID    站点
* @param Catagory   [in] Trigger classification 触发分类
* @param iData      [in] Deceleration 减速度
* @param pRxData    [out,opt] Deceleration feedback value 减速度反馈值 (可为 NULL)
* @return 0 成功 / Success; 非零 失败 / Failure
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetInputLogicDC(uint GtwyHandle, uint CANid, uint Catagory, uint iData, uint* pRxData = NULL);

/**
* @brief Get input trigger action - DC (Deceleration)
*        查询输入触发动作 - DC（减速度）
*
* @param GtwyHandle     [in] Gateway handle 网关句柄
* @param CANid          [in] Device’s ID    站点
* @param Catagory       [in] Trigger classification 触发分类
* @param piDeceleration [out] Deceleration 减速度
* @return 0 成功 / Success; 非零 失败 / Failure
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetInputLogicDC(uint GtwyHandle, uint CANid, uint Catagory, uint* piDeceleration);

/**
* @brief Set input trigger action - SP (Speed)
*        设置输入触发动作 - SP（速度）
*
* 配置指定触发分类下的 JV / PTP 速度参数。
*
* @param GtwyHandle  [in] Gateway handle 网关句柄
* @param CANid       [in] Device’s ID    站点
* @param Catagory    [in] Trigger classification 触发分类
* @param Velocity    [in] JV Velocity / PTP Velocity 速度
* @param pRxVelocity [out,opt] Velocity feedback value 速度反馈值 (可为 NULL)
* @return 0 成功 / Success; 非零 失败 / Failure
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetInputLogicSP(uint GtwyHandle, uint CANid, uint Catagory, int Velocity, int* pRxVelocity = NULL);

/**
* @brief Get input trigger action - SP (Speed)
*        查询输入触发动作 - SP（速度）
*
* @param GtwyHandle  [in] Gateway handle 网关句柄
* @param CANid       [in] Device’s ID    站点
* @param Catagory    [in] Trigger classification 触发分类
* @param pRxVelocity [out] JV Velocity / PTP Velocity 速度
* @return 0 成功 / Success; 非零 失败 / Failure
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetInputLogicSP(uint GtwyHandle, uint CANid, uint Catagory, int* pRxVelocity);

/**
* @brief Set input trigger action - PR (Displacement)
*        设置输入触发动作 - PR（位移）
*
* @param GtwyHandle [in] Gateway handle 网关句柄
* @param CANid      [in] Device’s ID    站点
* @param Catagory   [in] Trigger classification 触发分类
* @param iData      [in] Displacement 位移
* @param pRxData    [out,opt] Displacement feedback value 位移反馈值 (可为 NULL)
* @return 0 成功 / Success; 非零 失败 / Failure
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetInputLogicPR(uint GtwyHandle, uint CANid, uint Catagory, int iData, int* pRxData = NULL);

/**
* @brief Get input trigger action - PR (Displacement)
*        查询输入触发动作 - PR（位移）
*
* @param GtwyHandle [in] Gateway handle 网关句柄
* @param CANid      [in] Device’s ID    站点
* @param Catagory   [in] Trigger classification 触发分类
* @param pRxData    [out] Displacement feedback value 位移反馈值
* @return 0 成功 / Success; 非零 失败 / Failure
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetInputLogicPR(uint GtwyHandle, uint CANid, uint Catagory, int* pRxData);

/**
* @brief Set input trigger action - PA (Position)
*        设置输入触发动作 - PA（位置）
*
* @param GtwyHandle [in] Gateway handle 网关句柄
* @param CANid      [in] Device’s ID    站点
* @param Catagory   [in] Trigger classification 触发分类
* @param iData      [in] Position 位置
* @param pRxData    [out,opt] Position feedback value 位置反馈值 (可为 NULL)
* @return 0 成功 / Success; 非零 失败 / Failure
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetInputLogicPA(uint GtwyHandle, uint CANid, uint Catagory, int iData, int* pRxData = NULL);

/**
* @brief Get input trigger action - PA (Position)
*        查询输入触发动作 - PA（位置）
*
* @param GtwyHandle [in] Gateway handle 网关句柄
* @param CANid      [in] Device’s ID    站点
* @param Catagory   [in] Trigger classification 触发分类
* @param pRxData    [out] Position 位置
* @return 0 成功 / Success; 非零 失败 / Failure
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetInputLogicPA(uint GtwyHandle, uint CANid, uint Catagory, int* pRxData);


//[Input trigger action] Input Logic Action Code
#define ILG_NULL_0000_IDX 0		// No action (i.e. prohibited) 无动作(即禁止) 
#define ILG_MOFF_0000_IDX 1		// Offline 脱机
#define ILG_JVSD_0000_IDX 2		// Stop quickly 急速停止
#define ILG_JVDC_0000_IDX 3		// Slow down and stop 减速停止
#define ILG_SORG_PTPR_IDX 4		// Origin + commutation + motion according to displacement abs(PR) 原点+换向+按位移abs(PR)运动
#define ILG_SORG_JVSD_IDX 5		// Origin + quick stop 原点+急速停止
#define ILG_SORG_JVDC_IDX 6		// Origin + Slow down to stop 原点+减速停止
#define ILG_NDIR_JOGV_IDX 7		// Reversing + Motion according to preset abs(JV) 换向+按预设abs(JV)运动
#define ILG_JOGV_0000_IDX 8		// Move as preset +/-JV 按预设 +/-JV 运动
#define ILG_NDIR_PTPR_IDX 9		// Commutation + Motion according to preset displacement abs(PR) 换向+按预设位移abs(PR)运动
#define ILG_PTPR_0000_IDX 10	// Move according to preset displacement (+/-PR) 按预设位移(+/-PR)运动
#define ILG_PTPA_0000_IDX 11	// Move at preset position (+/-PA) 按预设位置(+/-PA)运动
#define ILG_PVTA_0000_IDX 15	// Execute the PVT movement according to the preset parameters 按预设参数执行 PVT 运动

/**
 * @brief Set input trigger action logic
 *        设置输入触发动作逻辑
 *
 * 配置指定触发端口的默认触发动作逻辑，并可返回设置后的反馈值。
 *
 * @param GtwyHandle    [in] Gateway handle                 网关句柄
 * @param CANid         [in] Device’s ID                    站点
 * @param InputEdgeIdx  [in] Trigger port index / number     触发端口号
 * @param MxnCode       [in] Default action of Input trigger 输入触发的预设动作
 * @param pRxData       [out,opt] Default action feedback    输入触发的预设动作反馈值 (可为 NULL)
 * @return 0 成功 / Success; 非零 失败 / Failure
 *
 * @note This function sets the default input trigger action logic.
 *       本函数用于设置输入触发的默认逻辑。
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetInputLogic(uint GtwyHandle, uint CANid, uint InputEdgeIdx, INPUT_LOGIC_OBJ MxnCode, INPUT_LOGIC_OBJ* pRxData = NULL);

/**
* @brief Get input trigger action logic
*        查询输入触发动作逻辑
*
* 获取指定触发端口的默认输入触发动作逻辑。
*
* @param GtwyHandle [in] Gateway handle                 网关句柄
* @param CANid      [in] Device’s ID                    站点
* @param PortIx     [in] Trigger port index / number    触发端口号
* @param pRxData    [out] Default action of Input trigger 输入触发的预设动作
* @return 0 成功 / Success; 非零 失败 / Failure
*
* @note This function queries the default input trigger action logic.
*       本函数用于查询输入触发的默认逻辑。
*/
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetInputLogic(uint GtwyHandle, uint CANid, uint PortIx, INPUT_LOGIC_OBJ* pRxData);

#endif

/// [Servo Motor ]
#ifdef _BLOCK_

///===================================
// Servo Control and Parameters
///===================================
// @ingroup servo_control

/**
 * @brief Set general servo parameters
 *        设置伺服通用设置
 *
 * 配置伺服的通用参数，包括主码、辅码及数据。
 * @note 结构体 `SERVO_PARAM_OBJ` 定义详见 `uirSDKgen3.h`
 *
 * @param GtwyHandle [in] Gateway handle                    网关句柄
 * @param CANid      [in] Device’s ID                       站点
 * @param SrvoData   [in] Servo setting content             伺服设置内容（主码+辅码+数据）
 * @param pRxData    [out,opt] Feedback of setting content  伺服设置内容反馈值 (可为 NULL)
 * @return 0 成功 / Success; 非零 失败 / Failure
 *
 * @note This function sets the general servo parameters.
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetServoParam(uint GtwyHandle, uint CANid, SERVO_PARAM_OBJ* SrvoData, SERVO_PARAM_OBJ* pRxData = NULL);

/**
 * @brief Get general servo parameters
 *        查询伺服通用设置
 *
 * 获取伺服的通用参数（主码+辅码+数据，调用前需事先填写主码与辅码）。
 *
 * @param GtwyHandle [in] Gateway handle                    网关句柄
 * @param CANid      [in] Device’s ID                       站点
 * @param pRxData    [in,out] Servo setting content         伺服设置内容（需先填主码+辅码）
 * @return 0 成功 / Success; 非零 失败 / Failure
 *
 * @note This function queries the general servo parameters.
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetServoParam(uint GtwyHandle, uint CANid, SERVO_PARAM_OBJ* pRxData);

/// Servo Quick Function Codes
#define _SV_QKF_STF 0xF6 // Stiffness / 刚度设定
#define _SV_QKF_ADP 0xF7 // Parameter Auto Adaption / 自适应功能
#define _SV_QKF_FLT 0xF8 // Filter / 指令滤波
#define _SV_CLR_ABS 0xF9 // Origin / 编码器清零
#define _SV_CLR_ALM 0xFA // Clear Current Alarm / 清除当前报警
#define _SV_GET_ALM 0xFC // Get Current Alarm / 读取当前报警
#define _SV_SET_EEP 0xFD // Store parameters to EEPROM / 固化参数EEP
#define _SV_RST_AUT 0xFE // reset/release Authentication / 释放修改权
#define _SV_GET_AUT 0xFF // get Authentication / 取得修改权

/**
 * @brief Quickly set servo parameters using function code 快捷设置伺服参数
 *
 * 根据功能码快速设置伺服参数，并可返回设置后的反馈值。
 *
 * @param GtwyHandle [in] Gateway handle                    网关句柄
 * @param CANid      [in] Device’s ID                       站点
 * @param FuncCode   [in] Function code                     功能码（如 _SV_QKF_STF 等）
 * @param TxData     [in] Set content                       设置内容
 * @param pRxData    [out,opt] Feedback of setting          设置内容反馈值 (可为 NULL)
 * @return 0 成功 / Success; 非零 失败 / Failure
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetServoFunc(uint GtwyHandle, uint CANid, uint FuncCode, uint TxData, uint* pRxData = NULL);

/**
 * @brief Quickly read servo parameters using function code 快捷读取伺服参数
 * 根据功能码快速读取伺服参数。
 *
 * @param GtwyHandle [in] Gateway handle                    网关句柄
 * @param CANid      [in] Device’s ID                       站点
 * @param FuncCode   [in] Function code                     功能码
 * @param pRxData    [out] Servo parameter content          伺服参数内容
 * @return 0 成功 / Success; 非零 失败 / Failure
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetServoFunc(uint GtwyHandle, uint CANid, uint FuncCode, uint* pRxData);

/**
 * @brief Set torque control 设置力矩控制
 *
 * 设置力矩控制的超限动作及阈值，并可返回当前设置反馈。
 * 阈值为 最大力矩/额定力矩 × 100%
 *
 * @param GtwyHandle [in] Gateway handle                    网关句柄
 * @param CANid      [in] Device’s ID                       站点
 * @param ActionCode [in] Action code triggered by overlimit 超限触发动作代码
 * @param Threshold  [in] Torque threshold percentage       最大力矩/额定力矩 × 100%
 * @param pRxData    [out,opt] Feedback value               反馈值（RxData.b0:ActionCode, b1:Threshold, b2=0, b3=0）
 * @return 0 成功 / Success; 非零 失败 / Failure
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetTorqueCtrl(uint GtwyHandle, uint CANid, uint ActionCode, uint Threshold, uint* pRxData = NULL);

/**
 * @brief Get torque control 查询力矩控制
 * 查询当前力矩控制设置。
 *
 * @param GtwyHandle [in] Gateway handle                    网关句柄
 * @param CANid      [in] Device’s ID                       站点
 * @param pRxData    [out] Torque control value             力矩控制值（RxData.b0:ActionCode, b1:Threshold, b2=0, b3=0）
 * @return 0 成功 / Success; 非零 失败 / Failure
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetTorqueCtrl(uint GtwyHandle, uint CANid, int* pRxData);

#endif

/// [Aux Encoder ] UIM0808/1616 Only
#ifdef _BLOCK_

///===================================
// Auxiliary Encoder Control
///===================================
// @ingroup encoder_control

/**
 * @brief Set auxiliary encoder counting period
 *        设置辅助编码器计数周期
 *
 * 设置辅助编码器的增量计数周期（单位：毫秒）。
 * 如果周期值为 0，则关闭辅助编码器功能。
 *
 * @param GtwyHandle [in] Gateway handle                    网关句柄
 * @param CANid      [in] Device’s ID                       站点
 * @param Period     [in] Encoder counting period (ms)      编码器计数周期（毫秒）
 * @param pRxData    [out,opt] Result feedback              结果反馈，可为 NULL
 * @return 0 成功 / Success; 非零 失败 / Failure
 *
 * @note `Period=0` 表示禁用辅助编码器
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSetEncoderPeriod(uint GtwyHandle, uint CANid, uint Period, uint* pRxData = NULL);

/**
 * @brief Reset auxiliary encoder counter
 *        复位辅助编码器计数器
 *
 * 将辅助编码器计数器清零（=0）。
 *
 * @param GtwyHandle [in] Gateway handle    网关句柄
 * @param CANid      [in] Device’s ID       站点
 * @return 0 成功 / Success; 非零 失败 / Failure
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkRstEncoderCount(uint GtwyHandle, uint CANid);

/**
 * @brief Get auxiliary encoder speed
 *        获取辅助编码器速度
 *
 * 读取辅助编码器在 1 秒内的计数值，用于计算当前转速或线速度。
 *
 * @param GtwyHandle [in] Gateway handle                    网关句柄
 * @param CANid      [in] Device’s ID                       站点
 * @param pRxData    [out] Counts within 1 second           1 秒内的计数
 * @return 0 成功 / Success; 非零 失败 / Failure
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetEncoderSpeed(uint GtwyHandle, uint CANid, int* pRxData);

/**
 * @brief Get auxiliary encoder counter
 *        获取辅助编码器计数器
 *
 * 查询当前辅助编码器的累积计数值。
 *
 * @param GtwyHandle [in] Gateway handle            网关句柄
 * @param CANid      [in] Device’s ID               站点
 * @param pRxCount   [out] Read-out value           当前累积计数
 * @return 0 成功 / Success; 非零 失败 / Failure
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetEncoderCount(uint GtwyHandle, uint CANid, int* pRxCount);

/**
 * @brief Get auxiliary encoder counts within period
 *        获取周期内编码器计数
 *
 * 读取指定周期内辅助编码器的计数值。周期通过 `SdkSetEncoderPeriod()` 设定。
 *
 * @param GtwyHandle [in] Gateway handle                    网关句柄
 * @param CANid      [in] Device’s ID                       站点
 * @param pRxData    [out] Counts within period             周期内的计数
 * @return 0 成功 / Success; 非零 失败 / Failure
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkGetEncoderIncrement(uint GtwyHandle, uint CANid, int* pRxData);

#endif

//======== RealTime Notification/Inform ========

/**
 * @brief Get real-time notification message 获取实时通知报文
 *
 * 从网关设备读取实时事件通知消息（RTCN）。
 * 可用于获取设备状态变化、故障报警等即时反馈。
 *
 * @param GtwyHandle [in] Gateway handle        网关句柄
 * @param pUiMsg     [out] Received UI message  接收到的 UI 消息结构体指针
 * @return 消息数量 / Number of messages retrieved
 *
 * @note 结构体 `UI_MSG_OBJ` 的定义详见 `uirSDKgen3.h`
 * @see UI_MSG_OBJ
 */
UISIMCANFUNC_API NUMB STDCALLAPI SdkGetRTCN(uint GtwyHandle, UI_MSG_OBJ* pUiMsg);

//========   Sending Universal Message  ========

/**
 * @brief Send and receive a versatile instruction
 *        发送并接收万能指令
 *
 * 将一条指令报文发送到指定网关/设备，并等待其反馈报文。
 *
 * @param GtwyHandle [in] Gateway handle           网关句柄
 * @param TxMsg      [in] Message to send          待发送的报文，结构体 UI_MSG_OBJ 指针
 * @param RxMsg      [out] Feedback message        反馈报文，结构体 UI_MSG_OBJ 指针
 * @return 0 成功 / Success; 非零 失败 / Failure
 *
 * @note
 * - 此方法为通用指令接口，可直接封装并发送自定义命令。
 * - 结构体 `UI_MSG_OBJ` 的定义详见 `uirSDKgen3.h`
 * @see UI_MSG_OBJ
 */
UISIMCANFUNC_API ERRO STDCALLAPI SdkSendMsg(uint GtwyHandle, UI_MSG_OBJ* TxMsg, UI_MSG_OBJ* RxMsg);


//======= Following are for Factory Only =======

/// [UFQ Operation]
#ifdef _BLOCK_

UISIMCANFUNC_API ERRO STDCALLAPI GetUIRobotSDKUFQ(void);
UISIMCANFUNC_API void STDCALLAPI SetUIRobotSDKUFQ(int iSDKUFQ);

UISIMCANFUNC_API ERRO STDCALLAPI  OPRoperation(uint GtwyHandle, uint CANid, OPR_DATA_OBJ* pOPRDataIn, OPR_DATA_OBJ* pOPRDataObjOut);
UISIMCANFUNC_API ERRO STDCALLAPI  OPR_UFQStart(uint GtwyHandle, uint CANid, OPR_DATA_OBJ* pOPRDataObjOut);
UISIMCANFUNC_API ERRO STDCALLAPI  OPR_UFQShutDown(uint GtwyHandle, uint CANid, OPR_DATA_OBJ* pOPRDataObjOut);
UISIMCANFUNC_API ERRO STDCALLAPI  OPR_UFQFreez(uint GtwyHandle, uint CANid, OPR_DATA_OBJ* pOPRDataObjOut);
UISIMCANFUNC_API ERRO STDCALLAPI  OPR_UFQHome(uint GtwyHandle, uint CANid, OPR_DATA_OBJ* pOPRDataObjOut);
//UFQ Set the system working mode, MD=0 debugging mode, MD=1 working mode (power-on default)
 //UFQ 设置系统工作模式，MD=0 调试模式, MD=1 工作模式（上电默认）
UISIMCANFUNC_API ERRO STDCALLAPI  OPR_UFQSetMode(uint GtwyHandle, uint CANid, int iMode, OPR_DATA_OBJ* pOPRDataObjOut);
UISIMCANFUNC_API ERRO STDCALLAPI  OPR_UFQGetODO(uint GtwyHandle, uint CANid, int * pNumber);
UISIMCANFUNC_API ERRO STDCALLAPI  OPR_UFQGetErr(uint GtwyHandle, uint CANid, OPR_DATA_OBJ* pOPRDataObjOut);
UISIMCANFUNC_API ERRO STDCALLAPI  OPR_UFQSetNodeID(uint GtwyHandle, uint CANid, uint dwNewCanNodeId, OPR_DATA_OBJ* pRxData);
UISIMCANFUNC_API ERRO STDCALLAPI  OPR_UFQSetGroupID(uint GtwyHandle, uint CANid, uint dwNewGroupId, OPR_DATA_OBJ* pOPRDataObjOut);
UISIMCANFUNC_API ERRO STDCALLAPI  OPR_UFQGetGroupID(uint GtwyHandle, uint CANid, uint *dwGroupId);
UISIMCANFUNC_API ERRO STDCALLAPI  OPR_UFQSetPVTWarn(uint GtwyHandle, uint CANid, bool bEnabled, OPR_DATA_OBJ* pOPRDataObjOut);
UISIMCANFUNC_API ERRO STDCALLAPI  OPR_UFQStateCheck(uint GtwyHandle, uint CANid, BYTE *pStatus);
UISIMCANFUNC_API ERRO STDCALLAPI  OPR_SystemCheck(uint GtwyHandle, uint CANid, bool bPortAndDecode, OPR_DATA_OBJ* pRxData);
UISIMCANFUNC_API ERRO STDCALLAPI  OPR_SystemShutDown(uint GtwyHandle, uint CANid, unsigned short iDelayTime, OPR_DATA_OBJ* pOPRDataObjOut);
UISIMCANFUNC_API ERRO STDCALLAPI  OPR_SystemSetCurrent(uint GtwyHandle, uint CANid, BYTE iCurrent, OPR_DATA_OBJ* pOPRDataObjOut);
UISIMCANFUNC_API ERRO STDCALLAPI  OPR_SystemGetCurrent(uint GtwyHandle, uint CANid, BYTE *pCurrent);
// Float brake control
 // 浮球刹车控制
UISIMCANFUNC_API ERRO STDCALLAPI  OPR_UFQSetBrake(uint GtwyHandle, uint CANid, uint dwBrake, OPR_DATA_OBJ* pRxData);
// Float Reset command
// 浮球Reset指令
UISIMCANFUNC_API ERRO STDCALLAPI  OPR_ResetCommand(uint GtwyHandle, uint CANid, int iCommand, OPR_DATA_OBJ* pOPRDataObjOut);
UISIMCANFUNC_API ERRO STDCALLAPI  OPR_SystemCommand(uint GtwyHandle, uint CANid, int iCommand, OPR_DATA_OBJ* pOPRDataObjOut);

UISIMCANFUNC_API ERRO STDCALLAPI  UFQSetMotorPara(uint GtwyHandle, uint CANid, MOTOR_PARAM_OBJ* pMotorParaObjIn, MOTOR_PARAM_OBJ* pMotorParaOut);
UISIMCANFUNC_API ERRO STDCALLAPI  UFQGetMotorPara(uint GtwyHandle, uint CANid, MOTOR_PARAM_OBJ* pMotorParaObj);

UISIMCANFUNC_API ERRO STDCALLAPI  SetQickRgb(uint GtwyHandle, uint CANid, QUICKRGB_PARAMETER_OBJ* pQuickRGBCfgObjIn, QUICKRGB_PARAMETER_OBJ* pQuickRGBCfgObjOut);
UISIMCANFUNC_API ERRO STDCALLAPI  SetRgb(uint GtwyHandle, uint CANid, int rgb, int * outRgb);
UISIMCANFUNC_API ERRO STDCALLAPI  GetRgb(uint GtwyHandle, uint CANid, int* rgb);
// Send Float Command (UFQ)
 // 发送浮球指令(UFQ)
UISIMCANFUNC_API ERRO STDCALLAPI SendFloatballCmd(uint GtwyHandle, UI_FLOATSOCKETMSG_OBJ* pPackageObj, UI_FLOATSOCKETMSG_OBJ* pPackageObjOut, int iMaxLen, int *pMsgNum);

#endif
