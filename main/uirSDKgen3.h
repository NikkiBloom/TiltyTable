/////////////////////////////////////////////////////////////////////////////////
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, //
// EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 	   //
// WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.	   //
// 																			   //
//							                        Copyright (c) UIROBOT. 	   //
// 							                            All rights reserved.   //
/////////////////////////////////////////////////////////////////////////////////
#pragma once
#define _BLOCK_

#ifdef _WIN64
#pragma warning( disable: 4081 4083 4312 4267 4244 4302 4311)
#else
#pragma warning( disable: 4081 4083)
#endif

// Include Files
#ifdef _WINDOWS

	#if !defined(WIN32_LEAN_AND_MEAN)
	#define WIN32_LEAN_AND_MEAN
	#endif

	#define STDCALLAPI
	#undef  UISIMCANFUNC_API
	#define UISIMCANFUNC_API extern "C" __declspec(dllimport)

	#include <windows.h>
	#include "stdio.h"
	#include <queue>
	#include <chrono>
	#include <WinSock2.h>
	#include <WS2tcpip.h>
	#include <mstcpip.h>
	#pragma comment(lib, "Ws2_32.lib")

	#include <assert.h>

#elif defined(_LINUX)

    #define WINAPI
    #define STDCALLAPI 
    #define UISIMCANFUNC_API extern "C"

	#include <pthread.h>
	#include <iomanip> 
	#include <stddef.h>
	#include <fcntl.h>
	#include <termios.h>
	#include <iostream>
	#include <string.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <queue>
	#include <chrono>
	#include <syslog.h>
	#include <errno.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <sys/stat.h>
	#include <arpa/inet.h>
	#include <netinet/tcp.h>

#endif

using namespace std;

///========================================
//		       Data Structure
///========================================

typedef int ERRO;
typedef int NUMB;
typedef unsigned int   uint;
typedef unsigned short UI16;
typedef unsigned char  BYTE;
typedef uint32_t   UI32;
typedef uint8_t    BYTE;

#ifdef _BLOCK_

// Gateway / Device Type
#define UIGW3_232CAN 0x0010		// Using this if using UIM2503
#define UIGW3_232UIM 0x0030		// Using this if using UIM341
#define UIGW3_URTCAN 0x0040		// Using this if using MMC904
#define UIGW3_UFQCAN 0x0050		// Using this if using UFQ750
#define UIGW3_GW2513 0x0060		// Using this if using UIM2513
#define UIGW3_ETHCAN 0x0070		// Using this if using UIM2523
#define UIGW3_USBCAN 0x0080		// Using this if using UIM2533
#define UIRGW_MASK30 0x00F0

enum PPSCommand
{
	PPS_SysUpdate, // System Update / 系统更新	
	PPS_BDR, // Baud Rate / 波特率
	PPS_Parity = 3, // Parity / 奇偶校验
	PPS_BTR = 5, // Bit Rate / 比特率
	PPS_TargetID, // Target ID / 目标 ID
	PPS_NodeID, // Node ID / 节点 ID
	PPS_GroupID, // Group ID / 分组 ID
	PPS_GatewayACK = 11 // Gateway ACK / 网关 ACK
};
enum MPSCommand
{
	MPS_ClearPVT = 0, // Clear PVT / 清空 PVT
	MPS_StartPosition = 1, // Start Position / 起始点 Index
	MPS_EndPosition = 2, // End Position / 终止点 Index
	MPS_PvtMode = 3, // PVT Mode / PVT 模式（FIFO/单循环/重复循环）
	MPS_WarningLevel = 5, // Warning Level / PVT 水位警告
	MPS_NextPosition = 6, // Next Position / 写入点 Index
	MPS_IOC_IDX = 7, // IOC Index / 如使用远程 I/O 设备，设置远程设备地址，=0:本地输出，1...125:远程输出设备的地址（CAN-ID）
};
enum ICCommand
{
	ExeEnableMotor,	// 0 = DIS  1 = ENA		Automatic enable after power-up delay / 上电延时后自动使能
	PositiveDirect,	// 0 = cw   1 = ccw		Positive Direct / 电机正转向
	ExeUserProgram,	// 0 = DIS  1 = ENA		Execute User Program / 上电自动执行用户程序
	EmerLockSystem,	// 0 = DIS  1 = ENA		Emergency Lock System / 传感器/堵转触发急停或脱机后，锁定系统
	AccDecUnitsDef,	// 0 = ppss 1 = ms		Acceleration/Deceleration Units / 加减速单位
	EncoderTypeDef,	// 0 = INC  1 = ABS		Encoder Type / 编码器类型
	ClosedLoopCtrl,	// 0 - open loop 1 - closed loop		Closed Loop Control / 使用正交编码器作为自闭环控制的位移反馈
	AllSoftLimitOn,	// 0 = DIS  1 = ENA		All Soft Limit On / 运动参数软件限位
	BrakeCircuitOn, // 0 = DIS  1 = ENA		Brake Circuit On / 使用内置刹车：1: 使用内置刹车电路 MT[5]
	BrakeControlOn,	// 0 = DIS  1 = ENA		Brake Control On / 刹车逻辑使能：1: 使能
	BrakeMonitorOn,	// 0 = DIS  1 = ENA		Brake Monitor On / 内置刹车监控：1: 制动解除时，禁止脱机
	ForceSilenceOn, // 0 = Normal 1 = Power-up	Force Silence On / 强制静默
	ICCommandMAX
};

#pragma pack(1)

//[UIR MSG] UIROBOT's Universal Interface Message
typedef struct _UI_MSG_OBJ
{
	BYTE  id;		// Node-ID;			[ 控制器（包括网关）站点/地址 ]
	BYTE  cw;		// Control Word;	[ 报文指令主码 ]
	BYTE  dl;	    // Data Length, i.e. Quantity of Valid Bytes in data[];
					//                  [ data[] 中有效的字节数量 ]
	BYTE  data[9];	// Data Array;		[ 报文数据 ]

	uint  ix;       // Sub-Index, Resreved; [ 报文指令副码, 保留 ]

} UI_MSG_OBJ;
//[ Model Info  ]
typedef struct _MODEL_INFO_OBJ 
{
	BYTE Main;			// Model Main				[ 器件主型号	]
	BYTE SubModel;		// Model Sub				[ 器件副型号	]
	BYTE FuncSpec;		// Function Specification	[ 器件功能描述	]
	BYTE MaxAmp;		// Maximum Drive Current	[ 最大驱动电流	]
	uint FirmVer;		// Firmware Version			[ 固件版本		]
	uint PCBVer;		// PCB Version				[ PCB 版本		]

	char  ModelStr[32]; // Model String				[ 型号字符串	]

} MODEL_INFO_OBJ;

//[ Gateway Info]
typedef union  _GATEWAY_INFO_OBJ
{
	struct
	{
		uint GtwyHandle; 		// Gateway Handle			[ 网关句柄       ]

		uint COMidx;			// Serial Port number (if Linux OS, = 0, don't care)  [ 串口号 (Linux OS 时 = 0,不必关心)]
		uint COMbaud;			// Serial Baudrate			[ 串口波特率	 ]

		uint BTRidx;			// CAN Bitrate [ CAN 比特率]

		uint SerialNo;			// Gateway Serial Number	[ 网关序列号     ]
		uint FirmVer;			// Gateway Firmware Version [ 网关固件版本   ]
		char ModelStr[64];		// Gateway Model String		[ 网关型号字符串 ]
								// [!] When using Linux OS, input: Serial Port Info (e.g. "/dev/ttyS0") [ Linux系统时，输入串口信息 ]

		uint MemberQty;			// Member Quantity			[ 成员数量 ]
		BYTE MemberCid[100];	// Member Node-ID List		[ 成员CAN ID列表 ]
	};
	
	struct
	{
		uint GtwyHandle;

		uint IPaddr;		// Gateway IP address, e.g. 0xFE01A8C0 (= 192.168.1.254)
		uint IPport;        // Gateway IP port#, e.g. 8888 

		uint BTRidx;

		uint SerialNo;
		uint FirmVer;
		char ModelStr[64];

		uint MemberQty;
		BYTE MemberCid[100];

	}tcp;

	struct
	{
		BYTE CANnid : 8;	// Gateway Node-ID 
		BYTE zone   : 8;    // CAN Segment#, = 0, don't care
		UI16 type   : 16;	// Gateway Type, e.g. UIGW3_ETHCAN

		BYTE IP[4];			// IP address: IP[3]=192, IP[2]=168, IP[1]=1, IP[0]=254 -> IP = 192.168.1.254    
		uint IPport;

		uint BTRidx;

		uint SerialNo;
		uint FirmVer;
		char ModelStr[64];

		uint MemberQty;
		BYTE MemberCid[100];

	}detail;

}GATEWAY_INFO_OBJ;
//[ Members Info]
typedef struct _MEMBER_INFO_OBJ
{
	uint GtwyHandle; 	// Gateway Handle	[ 所属网关		]

	BYTE CANnid;		// Node ID			[ 节点ID/站点	]
	BYTE CANgid;		// Group ID			[ 分组 ID		] 

	UI16 COMbaud;		// COM Baud Rate	[ 波特率（针对串口类设备）]

	uint SerialNo;		// Serial Number	[ 器件序列号	]

	MODEL_INFO_OBJ Model; // Device Model	[ 成员型号		]

} MEMBER_INFO_OBJ, DRV_INFO_OBJ;

//[ Error Report]
typedef struct _ERR_REPRT_OBJ
{
	BYTE  index;   // Error Item Number / 错误条目序号; 0...7 最新...最老
	BYTE  ecode;   // Error Code / 错误代码;
	BYTE  errcw;   // Error Related "Command Code/Control Code" / 错误相关的"指令码/控制码";
	BYTE  errix;   // Error Related "Command Auxiliary Code/Control Auxiliary Code" / 错误相关的"指令辅码/控制辅码";
	BYTE  res0;    // Reserved Byte 0 / 保留字节 0;
	BYTE  res1;    // Reserved Byte 1 / 保留字节 1;

} ERR_REPORT_OBJ;

//[ Power-up Config ]
typedef union  _INI_CONFG_OBJ
{
	struct
	{
		unsigned bExeEnableMotor : 1; // After power-up, perform motor  enable 0:forbidden, 1:allowed /上电后, 执行电机使能  0:禁止，  1:允许; 
									  // Automatic enable after power-up delay / 上电后延时一定时间（可由 MT 指令预设），自动使能电机	
		unsigned bPositiveDirect : 1; // After power-up, motor rotate positive direction 0:clockwise, 1:counterclockwise /上电后, 电机旋转正向  0:顺时针，1:逆时针;
		unsigned bExeUserProgram : 1; // After power-up, execute user program 0:forbidden, 1:enabled /上电后, 执行用户程序  0:禁止，  1:使能;
		unsigned bEmerLockSystem : 1; // After power-up, accident lock system 0:unlock, 1:lock /上电后, 事故锁定系统  0:不锁定，1:锁定; 发生错误时，锁定系统，必须重启才能继续

		unsigned b4 : 1; // Reserved / 保留
		unsigned b5 : 1; // Reserved / 保留
		unsigned b6 : 1; // Reserved / 保留
		unsigned bForceSilenceOn : 1;  // After power-up, system silence 0:forbidden, 1:immediately enter silent state / 上电后, 系统静默,     0:禁止,	 1:立即进入静默状态

		unsigned b8 : 1; // Reserved / 保留
		unsigned bAccDecUnitsDef : 1; // After power-up, acceleration/deceleration units 0:time unit (ms), 1:numerical unit (pulse/square second) / 上电后, 加减速单位    0:时间单位（ms），1:数值单位（脉冲/平方秒）;
		unsigned bEncoderTypeDef : 1; // After power-up, encoder type 0:incremental, 1:absolute (requires battery) / 上电后, 编码器类型    0:增量式，  1:绝对式(需要电池);
		unsigned bClosedLoopCtrl : 1; // After power-up, closed loop control enable 0:open loop control, 1:closed loop control / 上电后, 闭环控制使能  0:开环控制，1:闭环控制;

		unsigned bAllSoftLimitOn : 1; // After power-up, software limit enable 0:forbidden, 1:enabled, enable/disable all software limits / 上电后, 软件限位使能  0:禁止, 1:使能，使能/禁止所有的软件限位;

		unsigned bBrakeSaftyLogic : 1; // After power-up, brake control enable 0:forbidden, 1:enabled, only when bBrakeCircuitOn = 1 / 上电后, 刹车控制使能  0:禁止, 1:使能，Only when bBrakeCircuitOn = 1
		unsigned bBrakeFeedback : 1; // After power-up, brake monitoring enable 0:forbidden, 1:enabled, will occupy IN01, used as brake module (other) signal acquisition; / 上电后, 刹车监测使能  0:禁止, 1:使能，将占用 IN01, 用作刹车模块（另配）信号采集;
		unsigned bBrakeCircuitOn : 1; // Enable built-in brake circuit 0:forbidden, 1:enabled, use MT[5] to control brake / 使能内置刹车电路      0:禁止, 1:使能，使用MT[5]来控制刹车

		UI16 na0;   // N/A
	};

	uint  uiValue;

} INITIAL_CONFIG_OBJ;
//[ Inform Enable Config ]
typedef union  _IEC_CONFG_OBJ
{
	struct
	{
		unsigned bP1IE : 1; // S1IE /  S1通知开关
		unsigned bP2IE : 1; // S2IE /  S2通知开关
		unsigned bP3IE : 1; // S3IE /  S3通知开关
		unsigned bP4IE : 1; // S4IE /  S4通知开关

		unsigned bP5IE : 1; //	Reserved / 保留
		unsigned bP6IE : 1; //	Reserved / 保留
		unsigned bP7IE : 1; //	Reserved / 保留
		unsigned bP8IE : 1; //	Reserved / 保留

		unsigned bSTPIE : 1; //Position RTCN / 位置通知开关
		unsigned bSTIE : 1; //Stall Detection RTCN / 堵转通知开关
		unsigned bPVT_FINISH_IE : 1; //PVT FINISH IE / PVT 结束通知开关
		unsigned bPVT_FIFO_IE : 1; //PVT FIFO IE / PVT FIFO 通知开关

		unsigned : 4; // Reserved / 保留

		UI16 na0; // N/A
	};

	uint uiValue;

} INFENA_CONFIG_OBJ;

//[ Motor Driver ]
typedef struct _MOT_PARAM_OBJ
{
	uint iMicrostep;		// Micro Stepping Resolution					[ 微步细分 ]
	uint iCurrent;			// Working Current, 7= 0.7A,10=1A,22=2.2A...	[ 电流 x10	]
	uint iPowerSaveRate;	// Idle Current Rate = Idle Current/Working Current * 100	[ 待机电流/工作电流 *100]
	uint iAutoMotorOntime;  // Auto Driver-On Delay Time after Power-up		[ 上电后自动使能延时 ]

} MOTOR_PARAM_OBJ;
//[ Servo Driver ]
typedef struct _SVO_PARAM_OBJ
{
	int iCatalog;	// Catalog
	int iParamId;	// Parameter ID
	int iData;		// Data

} SERVO_PARAM_OBJ;
//[ Motor Status ]
typedef union  _MOT_STATU_OBJ
{
	struct
	{
		// 持续状态		
		unsigned MODE : 2;	// [Md1:Md0] 0=JOG, 1=PTP, 2=PVT 
		unsigned SVON : 1;	// 1=使能[1], 0=脱机 / 1=使能[1], 0=脱机

		unsigned IN0LV : 1; // IN0 Level / IN0 电平
		unsigned IN1LV : 1; // IN1 Level / IN1 电平
		unsigned IN2LV : 1; // IN2 Level / IN2 电平
		unsigned QP0LV : 1; // OP0 Level / OP0 电平
		unsigned QP1LV : 1; // OP1 Level / OP1 电平

		//...............

		// interrupt flag /	中断标志;
		unsigned STOP : 1; // Motor Speed Zero / 电机速度为零
		unsigned PAIF : 1; // PTP Positioning (PTP arrives = 1, maintained until the next PTP movement starts automatically) / PTP 定位（PTP到位后 = 1，维持 1 直到下次 PTP 运动开始时自动清零）
		unsigned PSIF : 1; // PVT End (PVT arrives = 1, maintained until the next PVT movement starts automatically) / PVT 结束（PVT到位后 = 1，维持 1 直到下次 PVT 运动开始时自动清零）
		unsigned TCIF : 1; // Torque Limit / 力矩超限

		// continuous alarm / 持续报警
		unsigned HEAT : 1; // Temperature Limit / 温度超限
		unsigned LOCK : 1; // System Lock / 系统锁定
		unsigned GALM : 1; // 1 = Crew alert sent (note: passive recipient = 0) / 1=发送了机组警报(注：被动接收者=0)
		unsigned SERR : 1; // 1 = System Error / 1=发生了系统错误

		BYTE res0;
		BYTE res1;

	}bits; //

	struct
	{
		BYTE b0;
		BYTE b1;
		BYTE b2;
		BYTE b3;

	}byte; //

	uint w;

} MOTION_STATUS_OBJ;
//[ Soft-Limits  ]
typedef struct _MXN_LIMIT_OBJ
{
	int MaxVelocity = 0; // Maximum operating speed pps /最大工作速度 pps
	int LowWorkRange = 0; // Lowest operating limit pls (overruns will stop and decelerate) / 最低工作限位 pls (超过会急停减速)
	int UppWorkRange = 0; // Highest operating limit pls (overruns will stop and decelerate) / 最高工作限位 pls (超过会急停减速)
	int LowBumpLimit = 0; // Lowest collision limit pls (overruns will stop instantly) / 最低碰撞极限 pls (超过会瞬间停止)
	int UppBumpLimit = 0; // Highest collision limit pls (overruns will stop instantly) / 最高碰撞极限 pls (超过会瞬间停止)
	int MaxVeloError = 0; // Maximum speed error pps (overruns indicate reverse dragging) / 最大速度误差 pps (超过表示发生了倒拖)
	int MaxPosiError = 0; // Maximum position error pls (overruns indicate stall) / 最大位移误差 pls (超过表示发生了堵转)
	int MaxAccValue = 0; // Maximum acceleration (numerical mode pps/ms, 107 = 107pps per ms, in PVT mode, limit the calculated acceleration too large) / 最大加速度 (数值模式 pps/ms, 107 = 每 ms 增速 107pps, PVT 模式下，限制计算出来的加速度过大)

} MOTION_LIMIT_OBJ;

//[ PVT Config  ] 
typedef struct _PVT_PARAM_OBJ 
{
	uint QueueLevel; //.0 Cache water level /缓存水位;
	uint Idx1;		 //.1 Trajectory Start Point Number (can be 0); / 轨迹起点标号(可以不为 0)
	uint Idx2;		 //.2 Trajectory End Point Number; / 轨迹终点标号
	uint Mode;		 //.3 Execution Mode; 0:FIFO; 1:Single-pass; 3:Loop;	/ 执行模式; 0:FIFO; 1:单程; 3:循环;	
	uint PTtime;	 //.4 P-T Movement Time; [!] PTtime=0: set to PVT Movement / P-T 运动的 Time; [!] PTtime=0: set to PVT 运动
	uint AlertLevel; //.5 Remaining Points Less Than or Equal to AlertLevel, Warning; / FIFO 中剩余待执行点小于等于 AlertLevel 时，发出警告;
	uint WriteIdx;	 //.6 Cache Writable Point Number (Read Only) / 缓存可写点序号（Read Only）;

} PVT_CONFIG_OBJ; 
//[ PVT Point   ]
typedef struct _PVT_POINT_OBJ 
{
	int	 QP; // Position / 位置
	int	 QV; // Speed / 速度
	uint QT; // Time Interval with the Previous Point / 与上一点的间隔时间
} PVT_QPVT_OBJ; 
//[ PVT K-Code  ]
typedef struct _PVT_KCODE_OBJ 
{
	uint dlen;		// 数据长度：0..8 
	BYTE data[8];	// 数据
} PVT_KCODE_OBJ; 

//[ Input       ] 16 In and 16 Out
typedef union  _DIO_IOPORT_OBJ
{
	struct
	{
		unsigned bIN00 : 1;
		unsigned bIN01 : 1;
		unsigned bIN02 : 1;
		unsigned bIN03 : 1;
		unsigned bIN04 : 1;
		unsigned bIN05 : 1;
		unsigned bIN06 : 1;
		unsigned bIN07 : 1;

		unsigned bQP00 : 1;
		unsigned bQP01 : 1;
		unsigned bQP02 : 1;
		unsigned bQP03 : 1;
		unsigned bQP04 : 1;
		unsigned bQP05 : 1;
		unsigned bQP06 : 1;
		unsigned bQP07 : 1;

		unsigned bIN08 : 1;
		unsigned bIN09 : 1;
		unsigned bIN10 : 1;
		unsigned bIN11 : 1;
		unsigned bIN12 : 1;
		unsigned bIN13 : 1;
		unsigned bIN14 : 1;
		unsigned bIN15 : 1;

		unsigned bQP08 : 1;
		unsigned bQP09 : 1;
		unsigned bQP10 : 1;
		unsigned bQP11 : 1;
		unsigned bQP12 : 1;
		unsigned bQP13 : 1;
		unsigned bQP14 : 1;
		unsigned bQP15 : 1;
	}bits;

	struct
	{
		BYTE LocalInPut : 8;
		BYTE LocalOutPut : 8;

		BYTE RemoteInPut : 8;
		BYTE RemoteOutPut : 8;
	}byte;

	struct
	{
		BYTE I_0;
		BYTE Q_0;
		BYTE I_1;
		BYTE Q_1;

	}Byte;

	uint uiValue;

} UIM_DIO_OBJ;
//[ Output      ] 16 Out = 8out|8mask | 8out|8mask
typedef union  _DIO_OUTPUT_OBJ
{
	struct
	{
		unsigned bQP00 : 1; // Local Output Level（1-8） / 本端口输出电平（1-8）
		unsigned bQP01 : 1;
		unsigned bQP02 : 1;
		unsigned bQP03 : 1;
		unsigned bQP04 : 1;
		unsigned bQP05 : 1;
		unsigned bQP06 : 1;
		unsigned bQP07 : 1;

		unsigned bMSK00 : 1; // Mask (0: don't operate this port, 1: operate this port) / 掩膜（0:不要操作本端口，1:需要操作本端口）
		unsigned bMSK01 : 1;
		unsigned bMSK02 : 1;
		unsigned bMSK03 : 1;
		unsigned bMSK04 : 1;
		unsigned bMSK05 : 1;
		unsigned bMSK06 : 1;
		unsigned bMSK07 : 1;
		
		unsigned bQP08 : 1; // Local Output Level（9-16） / 本端口输出电平（9-16）
		unsigned bQP09 : 1;
		unsigned bQP10 : 1;
		unsigned bQP11 : 1;
		unsigned bQP12 : 1;
		unsigned bQP13 : 1;
		unsigned bQP14 : 1;
		unsigned bQP15 : 1;
		
		unsigned bMSK08 : 1; // Mask (0: don't operate this port, 1: operate this port) / 掩膜（0:不要操作本端口，1:需要操作本端口）
		unsigned bMSK09 : 1;
		unsigned bMSK10 : 1;
		unsigned bMSK11 : 1;
		unsigned bMSK12 : 1;
		unsigned bMSK13 : 1;
		unsigned bMSK14 : 1;
		unsigned bMSK15 : 1;
	}bits; // Q0, Q1

	struct
	{
		uint8_t LocalOutPut      : 8;
		uint8_t LocalOutPutMask  : 8;
		
		uint8_t RemoteOutPut     : 8;
		uint8_t RemoteOutPutMask : 8;

	}byte; //
	
	uint32_t uiOutPut;

} DIGITAL_OUTPUT_OBJ;
//[ Input Logic ]
typedef union  _INP_LOGIC_OBJ
{
	struct
	{
		// Settings for falling edge / 下降沿设置
		unsigned Action0 : 6; // Action Code / 动作码
		unsigned bOTS0 : 1; // One-Time Shoot, 0:Disable, Contiunous Sampling; 1:Enable, once triggered, action code will be reset to 0 / 一次触发, 0:禁用, 连续采样; 1:启用, 一次触发后, 动作码将重置为0
		unsigned bPOT0 : 1; // Power-ON Trig,  0:Force to trig on power-up; 1:Disable / 上电触发,  0:强制上电触发; 1:禁用

		// Settings for rising edge / 上升沿设置
		unsigned Action1 : 6; // Action Code / 动作码
		unsigned bOTS1 : 1; // One Time Shoot, 0:Disable, Contiunous Sampling; 1:Enable, once triggered, action code will be reset to 0 / 一次触发, 0:禁用, 连续采样; 1:启用, 一次触发后, 动作码将重置为0
		unsigned bPOT1 : 1; // Power-ON Trig,  0:Force to trig on power-up; 1:Disable / 上电触发,  0:强制上电触发; 1:禁用

		BYTE res[2];
	};

	uint w;

} INPUT_LOGIC_OBJ;
//[ Output      ] PWM
typedef struct _DIO_PWMCFG_OBJ
{
	uint Period;
	uint Duty;

} DIGITAL_PWMCFG_OBJ;

//[  DMA  ] Direct Memory Operation
typedef struct _DMA_DATA_OBJ
{
	BYTE ad;   // DMA Data Start Address / DMA 数据起始地址
	BYTE d0;   // data0
	BYTE d1;
	BYTE d2;
	BYTE d3;
	BYTE d4;
	BYTE d5;   // data5

	BYTE len;  // Valid Data Length / 有效数据长度，包括 ad 在内的有效字节数

} DMA_DATA_OBJ;
//[  OPR  ] Special Operation
typedef struct _OPR_DATA_OBJ
{
	BYTE cat;	// OPR Category			[ OPR 类别 00,01...	]
	BYTE sub;	// OPR Operation Code	[ OPR 操作代码		]
	BYTE d0;	// Data[]
	BYTE d1;
	BYTE d2;
	BYTE d3;
	BYTE d4;
	BYTE d5;

	uint len;	// Valid Data Length	[ 有效数据长度		]

} OPR_DATA_OBJ;

// === For UIC322 USB-CAN products ====

#define __CANMSG 0x00
typedef union _UIC322_MSG_OBJ
{
	struct {
		//  CAN Message, Send -> UIC322
		//  00   01   02   03   04   05   06    07   08   09   10   11   12   13   14   15   16   17
		// [AA][00] [--- canid.32 ----] [dlc] [b0] [b1] [b2] [b3] [b4] [b5] [b6] [b7] [C0] [C1]【CC】 
		//                               dlc=[.0.SRR.IDE.RTR | .dl3 .dl2 .dl1 .dl0]
		//  Recieve
		//  00   01   02   03   04   05   06    07   08   09   10   11   12   13   14   15   16   17
		// [AA][00] [--- canid.32 ----] [dlc] [b0] [b1] [b2] [b3] [b4] [b5] [b6] [b7] [C0] [C1]【CC】 
		//                               dlc=[.0.SRR.IDE.RTR | .dl3 .dl2 .dl1 .dl0]

		BYTE cw;		// Control Word == 0x00;
		UI32 ID;	    // CAN ID 32bits          [ data[] 中有效的字节数量 ]
		BYTE lc;		// Data Length & Control; [ 报文指令主码 ]
		BYTE data[8];	// Data Array;		      [ 报文数据 ]
		BYTE crc[2];
	};

	BYTE bytes[18];

} UIC322_MSG_OBJ;
#define __CANCFG 0xC0
typedef union _UIC322_CFG_OBJ
{
	//  Configure, Send -> UIC322 
	//  00   01   02   03   04   05   06   07   08   09   10   11   12   13   14   15   16   17
	// [AA] [C0] [cf] [00] [00] [BitRate] [----- Filter ----] [------ Mask -----] [C0] [C1]  CC
	//            cf = [ .0.0.0.0 | .0 .mode1 .mode0 .ide]
	//                           BitRate = 50, 125, 250, 500, 800, 1000 (K) 
	//  Recieve
	//  00   01   02   03   04   05   06   07   08   09   10   11   12   13   14   15   16   17
	// [AA] [C0] [cf] [00] [00] [BitRate] [----- Filter ----] [------ Mask -----] [C0] [C1]  CC
	//            cf = [ .0.0.0.0 | .0 .mode1 .mode0 .ide]
	//                           BitRate = 50, 125, 250, 500, 800, 1000 (K) 

	struct {

		BYTE cw;		// Control Word == 0xC0;
		BYTE cf;	    // Config
		BYTE r0;	    // Reserved
		BYTE r1;	    // Reserved
		UI16 rate;		// BitRate
		UI32 filter;	// Filter
		UI32 mask;	    // Mask
		BYTE crc[2];
	};

	BYTE bytes[18];

} UIC322_CFG_OBJ;
#define __CANSTA 0xC1
typedef struct _UIC322_STA_OBJ
{
	// Status Check, Send -> UIC322
	//  00   01   02   03   04   05   06   07   08   09   10   11   12   13   14   15   16   17
	// [AA] [C1] [00] [00] [00] [00] [00] [00] [00] [00] [00] [00] [00] [00] [00] [C0] [C1]【CC】
	// Recieve
	//  00   01   02   03   04   05   06   07   08   09   10   11   12   13   14   15   16   17
	// [AA] [C1] [cf] [RX Error][TX Error][ Error ] [00] [00] [00] [00] [00] [00] [C0] [C1]【CC】
	//            cf = [ .0.0.0.0 | .0 .mode1 .mode0 .ide]

	BYTE cw;		// Control Word == 0xC1;
	BYTE cf;	    // Config
	UI16 txerr;		// Tx Error Summation
	UI16 rxerr;		// Rx Error Summation
	UI16 error;		// Other Error Summation
	BYTE res[7];    // reserved

} UIC322_STA_OBJ;

// ==== For UFQ products ====

// UFQ Message
typedef struct _UIR_UFQMSG_OBJ
{
	BYTE	domain;
	BYTE	id; 	 // 驱动器站点地址
	char			cw;		 // 消息指令码
	char			dl;		 // 数据长度，长度不能超过8;
	uint	ix;		 // 消息子指令码
	BYTE	data[8]; // 数据
}UI_FLOATSOCKETMSG_OBJ, * P_UI_FLOATSOCKETMSG_OBJ;
// RGB Lights
typedef struct _RGB_VALUE_OBJ
{
	BYTE chRValue; // 红色值 
	BYTE chGValue; // 绿色值 
	BYTE chBValue; // 黄色值 
	BYTE chWValue; // 白色值

}RGBW_VALUE_OBJ, * P_RGBW_VALUE_OBJ;
// RGB Config	
typedef union  _RGB_CONFIG_OBJ
{
	struct
	{
		unsigned bEnaColor : 1; // 
		unsigned bEnaPositon : 1; // 
		unsigned : 1;
		unsigned : 1;
		unsigned : 1;
		unsigned : 1;
		unsigned : 1;
		unsigned bEnaBegin : 1; //  
	};
	BYTE chRGBWCfgVal;

} RGBW_CONFIG_OBJ, * P_RGBW_CONFIG_OBJ;
// RGB Control
typedef union  _OLDRGB_CONFIG_OBJ
{
	// 第一代颜色值
	struct
	{

		unsigned red : 8; // 
		unsigned green : 8;
		unsigned blue : 8; // 
		unsigned white : 8;
	};

	uint w;
} OLDRGB_CONFIG_OBJ, * P_OLDRGB_CONFIG_OBJ;
// RGB Quick Feed
typedef struct _QUICKRGB_PARAMETER_OBJ
{
	int				iPositonValue;	//位置值
	RGBW_VALUE_OBJ	stColorValue;	//颜色值
	RGBW_CONFIG_OBJ uConfig;		//配置
}QUICKRGB_PARAMETER_OBJ, * P_QUICKRGB_PARAMETER_OBJ;

#pragma pack()

#endif

///========================================
//		SimpleCAN 3 Instruction Code
///========================================

#ifdef _BLOCK_

// ===== System Info =====
#define CWSTR_SY "SY - System Operation"
#define CWSTR_PP "PP - Protocol Parameters"
#define CWSTR_SN "SN - Serial Number"
#define CWSTR_ML "ML - Model Name"
#define CWSTR_IC "IC - Initial Config"
#define CWSTR_IE "IE - Information Enable"
#define CWSTR_ER "ER - Error Report"

// ===== Motor Config =====
#define CWSTR_MT "MT - Motor Config"
#define CWSTR_QE "QE - Encoder Config"

// ===== Motion Status =====
#define CWSTR_MS "MS - Motion Status"

// ===== Motion (Static Parameters) =====
#define CWSTR_MF "MF - Motion Param. Frame"
#define CWSTR_AC "AC - Acceleration"
#define CWSTR_DC "DC - Deceleration"
#define CWSTR_SS "SS - Cut-In Speed"
#define CWSTR_SD "SD - Stop Deceleration"
#define CWSTR_BL "BL - Backlash Compensation"
#define CWSTR_LM "LM - Software Motion Limits"

// ===== Motion (Dynamic Operation) =====
#define CWSTR_MO "MO - Motor On/Off"
#define CWSTR_OG "OG - Set Origin"
#define CWSTR_JV "JV - Jog Velocity"
#define CWSTR_SP "SP - PTP Motion Speed"
#define CWSTR_PR "PR - PTP Relative Position"
#define CWSTR_PA "PA - PTP Absolute Position"
#define CWSTR_BG "BG - Begin Motion"
#define CWSTR_ST "ST - Emergency Stop"
#define CWSTR_DV "DV - Desired Value"

// ===== Motion (Interpreted / Profiled) =====
#define CWSTR_MP "MP - PVT Motion Params"
#define CWSTR_PV "PV - Set PVT Motion"
#define CWSTR_PT "PT - Set P-T Motion"
#define CWSTR_QP "QP - Queued Position"
#define CWSTR_QV "QV - Queued Velocity"
#define CWSTR_QT "QT - Queued Time Interval"
#define CWSTR_QF "QF - Quick Feed PVT Data"

// ===== I/O & Logic =====
#define CWSTR_IO "IO - I/O Port Config"
#define CWSTR_IL "IL - Input Logic"
#define CWSTR_TG "TG - Trigger Config"
#define CWSTR_AI "AI - Analog Input"
#define CWSTR_DI "DI - Digital I/O"
#define CWSTR_PW "PW - Digital PWM"

// ===== RTCN =====
#define CWSTR_RT "RT - Real-Time Messages"

#define GROUP_CANMSG_CMD	  126  // Need to receive global messages / 需要接收全局消息
#define GROUP_CANMSG_CMD_SN0  127  // Need to receive global messages / 需要接收全局消息
#define OPTION_NEED_AK        0x80 // Need to receive gateway confirmation (i.e. _AK confirmation) / 需要网关确认收到报文（即 _AK 确认）

// [ Communication Protocol ] / [ 通讯协议 ]

#define __PP	1	// Communication Protocol / 通讯协议 

	#define PPS_SYS_IDX	0	// Sys Update: [GET] _PP 01  00 ... [SET] _PP 02  00 s ... [ACK] _PP 02  00 s ...; s=[0:RS232; 1:CAN;] / 系统更新: [GET] _PP 01  00 ... [SET] _PP 02  00 s ... [ACK] _PP 02  00 s ...; s=[0:RS232; 1:CAN;]
	#define PPS_BDR_IDX	1	// Baud RS232: [GET] _PP 01  01 ... [SET] _PP 02  01 s ... [ACK] _PP 02  01 s ...; s=[0:4.8K; 1:9.6K; 2:19.2K; 3:38.4K; 4:57.6K(默认); 5:115.2K;] / 波特率 RS232: [GET] _PP 01  01 ... [SET] _PP 02  01 s ... [ACK] _PP 02  01 s ...; s=[0:4.8K; 1:9.6K; 2:19.2K; 3:38.4K; 4:57.6K(默认); 5:115.2K;]
	#define PPS_BTR_IDX	5	// Baud CAN  : [GET] _PP 01  05 ... [SET] _PP 02  05 s ... [ACK] _PP 02  05 s ...; s=[0:1000Kbps; 1:800K; 2:500K(默认); 3:250K; 4:125K;]
	#define PPS_TID_IDX	6	// ConsumerID: [GET] _PP 01  06 ... [SET] _PP 02  06 s ... [ACK] _PP 02  06 s ...; Slave s=[1...4]
	#define PPS_NID_IDX	7	// ProducerID: [GET] _PP 01  07 ... [SET] _PP 02  07 s ... [ACK] _PP 02  07 s ...; Gtwy  s=[1...4]; Slave s=[5...126];
	#define PPS_GID_IDX	8	// GroupID   : [GET] _PP 01  08 ... [SET] _PP 02  08 s ... [ACK] _PP 02  08 s ...; Slave s=[5...126]; Note 禁止与已有 ID 同号

// [ System Operation ] / [ 系统操作 ]

#define __SY    126	// System Operation / 系统操作

	#define _SY_ULK_SYS 0	// Unlock System: [SET] _SY 02  00 01... [ACK] _SY 02  00 01...; System Unlock; UIM620/720/630/730 Only / 系统解锁: [SET] _SY 02  00 01... [ACK] _SY 02  00 01...; 系统解锁; UIM620/720/630/730 Only
	#define _SY_RST_SYS 1	// Reboot System: [SET] _SY 02  01 01... [ACK] _SY 02  01 01...; Reboot System / 重启系统: [SET] _SY 02  01 01... [ACK] _SY 02  01 01...; 重启系统
	#define _SY_RST_DFL 2	// Reset Default: [SET] _SY 02  02 01... [ACK] _SY 02  02 01...; Reset Default / 恢复出厂: [SET] _SY 02  02 01... [ACK] _SY 02  02 01...; 恢复出厂
	#define _SY_SYN_ENA 4	// Sync. Signal : [SET] _SY 02  04  s... [ACK] _SY 02  04  s...; s=[0:Disable; 1:Enable]; Gateways/MMC901 Only / 同步信号 : [SET] _SY 02  04  s... [ACK] _SY 02  04  s...; s=[0:Disable; 1:Enable]; Gateways/MMC901 Only 

#define __IC	6	// Power-Up Configuration / 上电配置 

	#define ICFG_AMO_IDX 0   //PowerUp DRV-ON  : [GET] _IC 01  00...; [SET]: _IC 03  01  s 00... [ACK] _IC 03  00   s 00...; s=[0:Disable; 1:Enable] 上电使能  : [GET] _IC 01  00...; [SET]: _IC 03  01  s 00... [ACK] _IC 03  00   s 00...; s=[0:Disable; 1:Enable] 上电使能     
	#define ICFG_CCW_IDX 1   //Positive Direct : [GET] _IC 01  01...; [SET]: _IC 03  02  s 00... [ACK] _IC 03  01   s 00...; s=[0:CW;      1:CCW   ] 电机转向: [GET] _IC 01  01...; [SET]: _IC 03  02  s 00... [ACK] _IC 03  01   s 00...; s=[0:CW;      1:CCW   ] 转向设定  (UIM720 只可 ccw)
	#define ICFG_UPG_IDX 2   //Exec, UPG       : [GET] _IC 01  02...; [SET]: _IC 03  03  s 00... [ACK] _IC 03  02   s 00...; s=[0:Disable; 1:Enable] UPG 使能     
	#define ICFG_LCK_IDX 3   //Input Lock Sys  : [GET] _IC 01  03...; [SET]: _IC 03  04  s 00... [ACK] _IC 03  03   s 00...; s=[0:Disable; 1:Enable] 输入锁定系统 
	#define ICFG_ACM_IDX 4   //Acc./Dec. Unit  : [GET] _IC 01  04...; [SET]: _IC 03  05  s 00... [ACK] _IC 03  04   s 00...; s=[0:pps/ms;  1:ms    ] 加减速单位   
	#define ICFG_ABS_IDX 5   //Encoder Type    : [GET] _IC 01  05...; [SET]: _IC 03  06  s 00... [ACK] _IC 03  05   s 00...; s=[0:Inc.;    1:abs   ] 编码器类型 (720 由伺服驱动器决定）
	#define ICFG_QEM_IDX 6   //Closed-Loop Mode: [GET] _IC 01  06...; [SET]: _IC 03  07  s 00... [ACK] _IC 03  06   s 00...; s=[0:Disable; 1:Enable] 闭环控制模式
	#define ICFG_SLM_IDX 7   //Software Limits : [GET] _IC 01  07...; [SET]: _IC 03  08  s 00... [ACK] _IC 03  07   s 00...; s=[0:Disable; 1:Enable] 软件限位使能 
	#define ICFG_BK1_IDX 8   //Brake Control   : [GET] _IC 01  08...; [SET]: _IC 03  09  s 00... [ACK] _IC 03  08   s 00...; s=[0:Normal;  1:MT[5] ] 刹车端口控制（刹车信号输出 OP01）
	#define ICFG_BK2_IDX 9   //Brake Monitor   : [GET] _IC 01  09...; [SET]: _IC 03  0A  s 00... [ACK] _IC 03  09   s 00...; s=[0:Disable; 1:Enable] 使能刹车监测（刹车信号输入 IN01） 
	#define ICFG_SLP_IDX 10  //Forced Sleep    : [GET] _IC 01  0A...; [SET]: _IC 03  0B  s 00... [ACK] _IC 03  0A   s 00...; s=[0:Normal;  1:Sleep ] 上电后即进入系统休眠（无运控及CAN发送）
	#define ICFG_IBK_IDX 15
	#define ICFG_ALL_IDX 255 //Operate as Word : [GET] _IC 01  FF...; [SET]: _IC 03  FF c0 c1... [ACK] _IC 03  FF  c0 c1...; 整体查询/设置

	#define ICFG_ENC_ABS 1  // Absolute Encoder
	#define ICFG_ENC_INC 0  // Incremental Encoder, ABZ

#define __IE	7	// 通知使能 

	#define  __IE_SUBCOMMAND_NEW (MAX_IE_CONFIG_COUNT+1)

	#define IECF_P1E_IDX 0	 //P1IE: [GET] _IE 01  00... [SET] _IE 03  00 00 ss... [ACK] _IE 02  00 ss...; ss=[0:Disable; 1:Enable]
	#define IECF_P2E_IDX 1	 //P2IE: [GET] _IE 01  01... [SET] _IE 03  01 00 ss... [ACK] _IE 02  01 ss...; ss=[0:Disable; 1:Enable]
	#define IECF_P3E_IDX 2	 //
	#define IECF_P4E_IDX 3	 //
	#define IECF_P5E_IDX 4	 //
	#define IECF_P6E_IDX 5	 //
	#define IECF_P7E_IDX 6	 //
	#define IECF_P8E_IDX 7	 //
	#define IECF_P9E_IDX 8
	#define IECF_P10E_IDX 9
	#define IECF_P11E_IDX 10
	#define IECF_P12E_IDX 11
	#define IECF_P13E_IDX 12
	#define IECF_P14E_IDX 13
	#define IECF_P15E_IDX 14
	#define IECF_P16E_IDX 15

	#define IECF_PAE_IDX 8   //In Position IE  : [GET] _IE 01  08 ... [SET] _IE 03  08 00 ss ... [ACK] _IE 02  08 00 ss ...; ss=[0:Disable; 1:Enable] 到位通知
	#define IECF_STE_IDX 9   //On Stall IE     : [GET] _IE 01  09 ... [SET] _IE 03  09 00 ss ... [ACK] _IE 02  09 00 ss ...; ss=[0:Disable; 1:Enable] 堵转侦测通知
	#define IECF_PSE_IDX 10  //PVT FIFO Dry IE : [GET] _IE 01  0A ... [SET] _IE 03  0A 00 ss ... [ACK] _IE 02  0A 00 ss ...; ss=[0:Disable; 1:Enable] PVT 到位通知 ( FIFO 水干 )
	#define IECF_PWE_IDX 11  //PVT FIFO Low IE : [GET] _IE 01  0B ... [SET] _IE 03  0B 00 ss ... [ACK] _IE 02  0B 00 ss ...; ss=[0:Disable; 1:Enable] PVT 水位告警 ( FIFO 水少 )

	#define IECF_ALL_IDX 255 //Operate as Word : [GET] _IC 01  FF ... [SET] _IC 03  FF c0 c1 ... [ACK] _IE 03  FF c0 c1 ...; 整体查询/设置

#define __SV	10	// Servo Drive / 伺服驱动 

	// 1) Cannot be set in motion / 运动中无法设置; 
	// 2) Communication time > 10ms / 通讯时间 > 10ms
	// Set: SVx[ca][ix][D0][D1][D2][D3]; ca-main code 8 bits; ix-subcode 8 bits; [D3...D0]-data 32 bits / 设置: SVx[ca][ix][D0][D1][D2][D3]; ca-主码8位; ix-副码8位; [D3..D0]-数据32位
	// Parameter Query: SVx[ca][ix]; |ACK: _SV[ca][ix][D0][D1][D2][D3] / 参数查询: SVx[ca][ix]; |ACK: _SV[ca][ix][D0][D1][D2][D3]
	// Quick Function Setting, / 快捷功能设置,
	#define _SV_SET_TLC 0xF0 // Set Touching Torque / 设定触碰力矩
	#define _SV_RST_SVO 0xF1 // Reboot Servo Drive / 重启伺服驱动器

	#define _SV_QKF_STF 0xF6 // Set Stiffness / 刚度设定,	 SVx[F6][b1]; 反馈 _SV[F6][b1][00][00][00][00];(注：1)b1=1...30; 2)将启动自适应功能)
	#define _SV_QKF_ADP 0xF7 // Adaptive Function / 自适应功能,	 SVx[F7][b1]; 反馈 _SV[F7][b1][00][00][00][00];(注：b1=0 禁止, b1=1 使能)
	#define _SV_QKF_FLT 0xF8 // Instruction Filter / 指令滤波,	 SVx[F8][b1]; 反馈 _SV[F8][b1][00][00][00][00];(注：b1=1...100ms; 将禁止自适应功能)
	#define _SV_CLR_ABS 0xF9 // Encoder Clear / 编码器清零,	 SVx[F9][00]; 反馈 _SV[F9][01][00][00][00][00];
	#define _SV_CLR_ALM 0xFA // Clear Current Alarm / 清除当前报警,SVx[FA][00]; 反馈 _SV
	#define _SV_GET_ALM 0xFC // Read Current Alarm / 读取当前报警,SVx[FC][00]; 反馈 _SV[FC][01][报警主码][报警副码][00][00];
	#define _SV_SET_EEP 0xFD // Save to EEP / 保存到 EEP,	 SVx[FD][00]; 反馈 _SV[FD][01][00][00][00][00];
	#define _SV_RST_AUT 0xFE // Release Modify Permission / 释放修改权,	 SVx[FE][00]; 反馈 _SV[FE][01][b2][00][00][00]; b2=0/1
	#define _SV_GET_AUT 0xFF // Get Modify Permission / 取得修改权,	 SVx[FF][00]; 反馈 _SV[FF][01][b2][00][00][00]; b2=0/1

#define __ML	11	//Get Model: [GET] _ML 00 ... [ACK] _ML 08  m0 m1 m2 m3 f0 f1 r0 r1; 查询型号
					//           m0     m1     m2    m3    f1:f0   r1:r0  (x:Factory Use)
					//342XS     0x20   0x0A   0x11   xx   FirmVer  xxxx
					//342S      0x21   0x0A   0x11   xx   FirmVer  xxxx
					//342C02    0x22   0x14   0x13   xx   FirmVer  xxxx
					//342C04    0x22   0x28   0x13   xx   FirmVer  xxxx
					//342C08    0x22   0x50   0x13   xx   FirmVer  xxxx
					//342H08    0x22   0x50   0x19   xx   FirmVer  xxxx
					//341C04    0x22   0xA8   0x13   xx   FirmVer  xxxx
					//341H04    0x22   0xA8   0x19   xx   FirmVer  xxxx
					//UIM1616   0x10   0x10    00    xx   FirmVer  xxxx
					//UIM0808   0x08   0x08    00    xx   FirmVer  xxxx
					//UIM2513   0x19   0x0D    00    xx   FirmVer  xxxx
					//UIM2523   0x19   0x17    00    xx   FirmVer  xxxx
					//UIM2533   0x19   0x21    00    xx   FirmVer  xxxx
					//MMC901S   0x5A   0x01    00    xx   FirmVer  xxxx

#define __SN	12	//Get S/N  ; [GET] _SN 00 ... [ACK] _SN 08  s0 s1 s2 s3 p0 p1 v0 v1...; s3:s0 = S/N; p1:p0 = Product ID; v1:v0 = Vender ID; 查询序列号

#define __TM	14	//Sys.Time : [GET] _TM 00 ... [SET] _TM 04  t0 t1 t2 t3 ... [ACK] _TM 04  t0 t1 t2 t3 ...; 设置/查询 系统时间(us); 运动时无法设置

#define __ER	15	//Err.Report:/ 错误报告

	#define ERRP_LST_IDX 0	//Last Error: [GET] _ER 01 00 ... [ACK] _ER 05 ix e0 e1 e2 e3 ...; Query Latest Errors /查询最新错误
							//	ix - Feedback Attribute = 0 (ie. ERRP_LST_IDX) / 反馈属性 = 0 (ie. ERRP_LST_IDX)
							//	e1 - Error Code (Refer to Error.h) / 错误码 (Refer to Error.h)
							//	e2 - Main Code of the Command that caused the error / 发生该错误的指令的主码
							//	e3 - Subcode of the Command that caused the error / 发生该错误的指令的副码
							//	e4 - Reserved (= 0) / 保留 (= 0)

	#define ERRP_RRB_IDX 1	// ER[1]; Factory Use / 工厂使用
	#define ERRP_RSB_IDX 2	// ER[2]; Factory Use / 工厂使用
	#define ERRP_CRB_IDX 3	// ER[3]; 工厂使用 
	#define ERRP_CSB_IDX 4	// ER[4]; 工厂使用

	#define ERRP_SVO_IDX 5	// Query servo error: [GET] _ER 01 05 ... [ACK] _ER 06 05 Master code, slave code ... ; /查询伺服错误:[GET] _ER 01  05 ... [ACK] _ER 06  05 主码 辅码 ...;

	#define ERRP_INI_IDX 6	// Query power-up error: [GET] _ER 01 06 ... [ACK] _ER 05 06 b1 b2 b3 b4 ... [ACK] _ER 05 06 b1 b2 b3 b4 ... ; /查询上电错误: [GET] _ER 01  06 ... [ACK] _ER 05  06 b1 b2 b3 b4 ...;
							//	b1.0 - MCUB internal error, only repair / MCUB 内部出错，只能返修
							//	b1.1...b1.3 - Reserved, read as 0 / 保留，读为 0
							//	b1.4 - Servo needs to be power-off restart, reason: appears after servo EEPROM is written / 伺服需断电重启，  原因：出现在写过伺服EEP之后
							//	b1.5 - Servo IO fault, reason: SCSI50 servo cable not plugged in / 伺服IO故障，      原因：SCSI50伺服排线未插好
							//	b1.6 - Servo has alarm, specific reason needs to be queried after executing ER[5] / 伺服有报警，具体原因需执行ER[5]查询
							//	b1.7 - Servo serial communication fault, reason: RS232 communication cable not plugged in / 伺服串口通讯故障，原因：RS232通讯线未插好
							//	Note: To clear the power-up initialization error, the fault must be eliminated and the power must be turned off and restarted

	#define ERRP_ER0_IDX 10	//Last Error  : [GET] _ER 01  0A ... [ACK] _ER 06  0A e0 e1 e2 e3 ...; Query Latest Errors / 查询最新错误
	#define ERRP_ER1_IDX 11 //Older Error : [GET] _ER 01  0B ... [ACK] _ER 06  0B e0 e1 e2 e3 ...; Historical Errors / 历史错误
	#define ERRP_ER2_IDX 12 //
	#define ERRP_ER3_IDX 13 //
	#define ERRP_ER4_IDX 14 //
	#define ERRP_ER5_IDX 15 //
	#define ERRP_ER6_IDX 16 //
	#define ERRP_ER7_IDX 17 //
	#define ERRP_ER8_IDX 18 //Oldest Error: [GET] _ER 01  12 ... [ACK] _ER 06  12 e0 e1 e2 e3 ...; Oldest Error / 最老错误
							//Clear All   Errors: [SET] _ER 02 00 00 ... [ACK] _ER 06 00 00 00 00 00 ...; Clear All Errors / 清除所有错误:
							//Clear Servo Errors: [SET] _ER 02 05 00 ... [ACK] _ER 06 05 00 00 00 00 ...; UIM720 Only; Clear Servo Errors / 清除伺服报警: 

// [ 电机驱动 ]

#define __MT 16	// 电机驱动

	#define MTS_MCS_IDX 0 //Micro-Step  : [GET] _MT 01  00 ... [SET] _MT 03  00 mm 00 ... [ACK] _MT 03  00 mm 00 ...; mm=[1,2,4,8,16,32,64,128]; 微步细分;
	#define MTS_CUR_IDX 1 //Current Run : [GET] _MT 01  01 ... [SET] _MT 03  01 ii 00 ... [ACK] _MT 03  01 ii 00 ...; ii=[5...80] x0.1 Amp;      工作电流;  
	#define MTS_PSV_IDX 2 //Current Idle: [GET] _MT 01  02 ... [SET] _MT 03  02 pp 00 ... [ACK] _MT 03  02 pp 00 ...; pp=[0...100]%;             待机电流百分比;
	#define MTS_ENA_IDX 3 //DRV-ON Delay: [GET] _MT 01  03 ... [SET] _MT 03  03 t0 t1 ... [ACK] _MT 03  03 t0 t1 ...; t1:t0 =[0...60000]ms;      上电使能延时 
	#define MTS_BRK_IDX 5 //Brake Lock  : [GET] _MT 01  05 ... [SET] _MT 03  05 ss 00 ... [ACK] _MT 03  05 ss 00 ...; ss=[0:Release; 1:Lock];    使能/解除刹车

// [ 运动控制 ]

#define __MS 17	// Motion Status & Disp. : [GET] _MS 01  00 ... [RST] _MS 02  00 00 ... [ACK] _MS 08  00 ms0 ms1  00 PR0 PR1 PR2 PR3; ms1:ms0 = MOTION_STATUS_OBJ; PR3:PR0 = Displacement; 查询运动状态标志 + 位移
				    // Get Speed & Position  : [GET] _MS 01  01 ...                         [ACK] _MS 08  01 sp0 sp1 sp2 PA0 PA1 PA2 PA3; SP2:SP0 = Speed;            PA3:PA0 = Position;     查询速度 + 位置
                    //                               CW DL  ix     Reset  CW DL  ix

#define __RG 20	// Reserved

#define __MO 21	//Drv ON/OFF  : [GET] _MO 00 ... [SET] _MO 01  ss ... [ACK] _MO 01  ss ...; Drive ON/OFF / 驱动 开启/关闭:
					//ss=[0:OFF; 1:ON] / ss=[0:OFF; 1:ON] 

#define __BG 22	//Begin Exec  : [SET] _BG 00 ... [ACK] _BG 01  00 ...; Start Executing Instructions / 开始执行指令

#define __ST 23	//Stop Motion : [SET] _ST 00 ... [ACK] _ST 01  00 ...; Emergency Stop (Using _SD Parameter) / 紧急停止（使用 _SD 参数）

#define __MF 24	//Setting window for selecting motion parameters /选择运动参数的设置窗口 

	#define MFS_INS	0	// Select Motion-Frame of Instructions : [SET] _MF 01  00 ... [ACK] _MF 01  00 ...; 
	#define MFS_STL 1	// Select Motion-Frame of Stall1	   : [SET] _MF 01  01 ... [ACK] _MF 01  01 ...;
	#define MFS_S1R 2	// Select Motion-Frame of S1R Action   : [SET] _MF 01  02 ... [ACK] _MF 01  02 ...;
	#define MFS_S1F 3	// Select Motion-Frame of S1F Action   : [SET] _MF 01  03 ... [ACK] _MF 01  03 ...;
	#define MFS_S2R 4	// Select Motion-Frame of S2R Action   : [SET] _MF 01  04 ... [ACK] _MF 01  04 ...;
	#define MFS_S2F 5	// Select Motion-Frame of S2F Action   : [SET] _MF 01  05 ... [ACK] _MF 01  05 ...;
	#define MFS_S3R 6	// Select Motion-Frame of S3R Action   : [SET] _MF 01  06 ... [ACK] _MF 01  06 ...;
	#define MFS_S3F 7	// Select Motion-Frame of S3F Action   : [SET] _MF 01  07 ... [ACK] _MF 01  07 ...;
	#define MFS_S4R 8	// Select Motion-Frame of S4R Action   : [SET] _MF 01  08 ... [ACK] _MF 01  08 ...;
	#define MFS_S4F 9	// Select Motion-Frame of S4F Action   : [SET] _MF 01  09 ... [ACK] _MF 01  09 ...;

#define __AC	25	//Acceleration: [GET] _AC 00 ... [SET] _AC 04  a0 a1 a2 a3 ... [ACK] _AC 04  a0 a1 a2 a3 ...; 加速度
					//In Time Method: a3:a0 =[10...60,000]ms; In Value Method: a3:a0 =[10...1,000,000,000]pps/s

#define __DC    26	//Deceleration: [GET] _DC 00 ... [SET] _DC 04  d0 d1 d2 d3 ... [ACK] _DC 04  d0 d1 d2 d3 ...; 减速度
					//In Time Method: d3:d0 =[10...60,000]ms; In Value Method: d3:d0 =[10...1,000,000,000]pps/s

#define __SS	27	//Start Speed : [GET] _SS 00 ... [SET] _SS 04  v0 v1 v2 v3 ... [ACK] _SS 04  v0 v1 v2 v3 ...; 切入速度
					//v3:v0 =[0...65535]pps

#define __SD	28	//Stop Deceleration: [GET] _SD 00 ... [SET] _SD 04  d0 d1 d2 d3 ... [ACK] _SD 04  d0 d1 d2 d3 ...; 急停减速度 
					//Only Value Method: d3:d0 =[400...1,000,000,000]pps/s

#define __JV	29	//Jog Velocity: [GET] _JV 00 ... [SET] _JV 04  v0 v1 v2 v3 ... [ACK] _JV 04  v0 v1 v2 v3 ...; JOG 速度, 并切换JOG模式
					//v3:v0 =[-2,147,483,648...2,147,483,648]pps

#define __SP	30	//PTP Speed   : [GET] _SP 00 ... [SET] _SP 04  v0 v1 v2 v3 ... [ACK] _SP 04  v0 v1 v2 v3 ...; PTP 速度
					//v3:v0 =[-2,147,483,648...2,147,483,648]pps

#define __PR	31	//Position Relative: [GET] _PR 00 ... [SET] _PR 04  p0 p1 p2 p3 ... [ACK] _PR 04  p0 p1 p2 p3 ...; PTP 位移, 并切换PTP模式
					//p3:p0 =[-2,147,483,648...2,147,483,648]pulse

#define __PA	32	//Position Absolute: [GET] _PA 00 ... [SET] _PA 04  p0 p1 p2 p3 ... [ACK] _PA 04  p0 p1 p2 p3 ...; PTP 位置, 并切换PTP模式
					//p3:p0 =[-2,147,483,648...2,147,483,648]pulse

#define __OG	33	//Set Origin  : [SET] _OG 00  ... [ACK] _OG 00  ...; 设置原点, 注：必须在停机（V=0，不是脱机）状态下设置，执行时间可能 > 100ms

// [ Curvilinear Motion ] / [ 曲线运动 ] Only UIM620/720

#define __MP	34	// PT/PVT Motion Setting / PT/PVT 运动设置

	 // MP[0],  1 - 清空PVT缓存区, 复位水位/执行点等
	//         2 - 保存PVT缓存区至 NVM
	// MP[1], PVT 序列的起点位置标号, 0...254
	// MP[2], PVT 序列的终点位置标号, 1...255
	// MP[3], PVT 序列执行模式 
	//         0 - 无限进给 FIFO
	//         1 - 单次进给，仅单次执行 MP[1]...MP[2]的 PVT
	//         2 - 保留
	//         3 - 无限循环，MP[1]...MP[2]->MP[1]...
	// MP[5]  PVT 缓存区水位通知
	//         0 - 无通知
	//         1...255 : 剩余 1…255 条待执行点位时，发送RTCN通知
	// MP[6]  下一个点位序列的写入点
	// MP[7]  QA 输出配置，
	//         0 - QA 输出端口位于 720/620 自身
	//         1 - QA 输出端口映射到 MMC900A 的 OP01...OP04
	//                如果对 720/620 指令操作（设置或读取）则：
	//                DI = [ x  x  x  x   x  x  x  x      x x x x  x x x x ];
	//                       16 15 14 13  12 11 10 09                    
	//                      映射输出端口  自身输出端口     自身输入端口
	//                       4  3  2  1   4  3  2  1      8 7 6 5  4 3 2 1
	//                注意：QA 的设置方式不变
	// MP[9] PVT 位置补偿死区，默认 2 pps

	#define PVT_FIFO_MODE 0   // FIFO播放方式
	#define PVT_SNGL_MODE 1   // 单程播放方式
	#define PVT_LOOP_MODE 3   // 循环播放方式

	#define MPS_CLR_IDX 0   // =1:清空PVT缓存区, 复位水位/执行点等 2 - 保存PVT缓存区至 NVM
	#define MPS_STA_IDX 1	// 设置 PVT 序列起始点[0...254]（仅对循环模式有效，结束点后执行起始点）
	#define MPS_END_IDX 2	// 设置 PVT 序列结束点[1...255]（仅对单程/循环模式有效）
	#define MPS_MOD_IDX 3	// 设置 PVT 模式，=0:FIFO / 1:单程 / 3:循环
	#define MPS_PTT_IDX 4 	// 保留
	#define MPS_WLN_IDX 5 	// 设置警告水位高度[0...100]，即距离枯竭前n行报警
	#define MPS_WRT_IDX 6 	// 查询可写入点，写入无效
	#define MPS_ROA_IDX 7 	// 设置 I/O 输出设备地址(CAN-ID), =0/nid:本地输出，=1...125 远程输出的站点
	#define MPS_DGP_IDX 9 	// 设置 PVT 死区，默认 2 pps

#define __PV	35	// PVT模式指令
					// 功能 1）切换到PVT模式 2）设置运动起始点 Y.16
					// 查询时返回当前实际执行点
					// 设置后发送bg;才被执行

#define __PT	36	// PT Motion's Position
#define __QP	37	// 位置; FIFO 模式下，必须按 QP[i]->QV[i]->QT[i] 顺序输入, PVT 运动中无法查询
#define __QV	38	// 速度, 语法: 设置"QV[X.16]= Y.32;" 查询"QV[X.16];"
#define __QT	39	// 时间, 语法: 设置"QT[X.16]= Y.32;" 查询"QT[X.16];"
#define __QF	41	// Quick Feed

#define __QA	40	// U-code I/O , 语法: 设置"QA[X.16]= Y.32;" 查询"QA[X.16];"
#define __PK	42	// K-code 轨迹指令
#define __PS	43	// U-code 轨迹指令 [ix][p0][p1][p2] [v0][v1][v2][t]

// [ 运动控制 ]

#define __LM	44	// 软件限位 

	#define LMT_SPH_IDX 0   //Maximum Speed    : [GET] _LM 01  00 ... [SET] _LM 05  00 v0 v1 v2 v3 ... [ACK] _LM 05  00 v0 v1 v2 v3 ...; 最大工作速度
	#define LMT_PAL_IDX 1   //Lower Work Range : [GET] _LM 01  01 ... [SET] _LM 05  01 v0 v1 v2 v3 ... [ACK] _LM 05  01 v0 v1 v2 v3 ...; 反向工作限位（下限位，超过会急停）
	#define LMT_PAH_IDX 2   //Upper Work Range : [GET] _LM 01  02 ... [SET] _LM 05  02 v0 v1 v2 v3 ... [ACK] _LM 05  02 v0 v1 v2 v3 ...; 正向工作限位（上限位，超过会急停）
	#define LMT_LML_IDX 3   //Lower Bump Pos.  : [GET] _LM 01  03 ... [SET] _LM 05  03 v0 v1 v2 v3 ... [ACK] _LM 05  03 v0 v1 v2 v3 ...; 反向碰撞极限（下限位，超过会瞬间停止）
	#define LMT_LMH_IDX 4   //Upper Bump Pos.  : [GET] _LM 01  04 ... [SET] _LM 05  04 v0 v1 v2 v3 ... [ACK] _LM 05  04 v0 v1 v2 v3 ...; 正向碰撞极限（上限位，超过会瞬间停止）
	#define LMT_SPE_IDX 5   //Max Speed Error  : [GET] _LM 01  05 ... [SET] _LM 05  05 v0 v1 v2 v3 ... [ACK] _LM 05  05 v0 v1 v2 v3 ...; 最大速度误差（倒拖），未用
	#define LMT_PQE_IDX 6   //Pulse/Encoder Err: [GET] _LM 01  06 ... [SET] _LM 05  06 v0 v1 v2 v3 ... [ACK] _LM 05  06 v0 v1 v2 v3 ...; 最大位移误差（堵转），堵转
	#define LMT_ACC_IDX 7   //Max Acceleration : [GET] _LM 01  07 ... [SET] _LM 05  07 v0 v1 v2 v3 ... [ACK] _LM 05  07 v0 v1 v2 v3 ...; 最大加速度 pps/ms, 100 = 100ms 加速到 10,000 pps
	#define LMT_TLC_IDX 8   //Max Torque       : [GET] _LM 01  08 ... [SET] _LM 05  08 v0 v1 v2 v3 ... [ACK] _LM 05  08 v0 v1 v2 v3 ...; 最大扭矩（仅720） 1-500(%)额定扭矩, 若设定值 > 500，则遵从伺服驱动器设定
	#define LMT_SLM_IDX 255 //Enable Soft Limit: [GET] _LM 01  FF ... [SET] _LM 05  FF ss 00 00 00 ... [ACK] _LM 05  FF ss 00 00 00 ...; 使能限位检测，0:禁止限位检测; 注：不保存, 每次上电时，将遵从 IC 值。
							//ss=[0:Disable; 1:Enable];

#define __BL	45	// 反向行程间隙补偿
#define __DV	46	// 期望值查询

	#define DVR_MOD_IDX 0	//DV[0] 查询 Control Mode:
		#define MXN_JOG_MODE 0
		#define MXN_PTP_MODE 1
		#define MXN_PVT_MODE 2
		#define MXN_PA_MODE  10
		#define MXN_PR_MODE  11
		#define MXN_PT_MODE  20
	#define DVR_CUR_IDX 1	//DV[1] 查询 CUR 期望值
	#define DVR_SPD_IDX 2	//DV[2] 查询 SP 期望值
	#define DVR_PRM_IDX 3	//DV[3] 查询 PR 期望值
	#define DVR_PAM_IDX 4	//DV[4] 查询 PA 期望值
	#define DVR_TIS_IDX 5	//DV[5] 查询 TI 期望值

// [ I/O 端口 ]

#define __IO	51	// 端口功能 

	// Following Only for UIM342/342/720
	#define IOC_P01C_IDX 0   //Port# 1 Function : [GET] _IO 01  00 ... [SET] _IO 02  00 ss ... [ACK] _IO 02  00 ss ...; ss=[0:Digital Out; 1:Digital In; 3:Encoder]  (Encoder Only for UIM342/341) 
	#define IOC_P02C_IDX 1   //Port# 2 Function : [GET] _IO 01  01 ... [SET] _IO 02  01 ss ... [ACK] _IO 02  01 ss ...; ss=[0:Digital Out; 1:Digital In; 3:Encoder]  (Encoder Only for UIM342/341) 
	#define IOC_P03C_IDX 2   //Port# 3 Function : [GET] _IO 01  02 ... [SET] _IO 02  02 ss ... [ACK] _IO 02  02 ss ...; ss=[0:Digital Out; 1:Digital In; 2:Analog In](Analog  Only for UIM342/341)
	#define IOC_P04C_IDX 3   //Port# 4 Function : [GET] _IO 01  03 ... [SET] _IO 02  03 ss ... [ACK] _IO 02  03 ss ...; ss=[0:Digital Out; 1:Digital In;]
	#define IOC_P05C_IDX 4   //Port# 5 Function : [GET] _IO 01  04 ... [SET] _IO 02  04 ss ... [ACK] _IO 02  04 ss ...; ss=[0:Digital Out; 1:Digital In;]
	#define IOC_P06C_IDX 5   //Port# 6 Function : [GET] _IO 01  05 ... [SET] _IO 02  05 ss ... [ACK] _IO 02  05 ss ...; ss=[0:Digital Out; 1:Digital In;]
	#define IOC_P07C_IDX 6   //Port# 7 Function : [GET] _IO 01  06 ... [SET] _IO 02  06 ss ... [ACK] _IO 02  06 ss ...; ss=[0:Digital Out; 1:Digital In;]
	#define IOC_P08C_IDX 7   //Port# 8 Function : [GET] _IO 01  07 ... [SET] _IO 02  07 ss ... [ACK] _IO 02  07 ss ...; ss=[0:Digital Out; 1:Digital In; 3:PWM] (PWM Only for UIM342/341)
	#define IOC_P09C_IDX 8   //Port# 9 Function : [GET] _IO 01  08 ... [SET] _IO 02  08 ss ... [ACK] _IO 02  08 ss ...; ss=[0:Digital Out; 1:Digital In; 3:PWM]
	#define IOC_P10C_IDX 9   //Port#10 Function : [GET] _IO 01  09 ... [SET] _IO 02  09 ss ... [ACK] _IO 02  09 ss ...; ss=[0:Digital Out; 1:Digital In;]
	#define IOC_P11C_IDX 10  //Port#11 Function : [GET] _IO 01  0A ... [SET] _IO 02  0A ss ... [ACK] _IO 02  0A ss ...; ss=[0:Digital Out; 1:Digital In;]
	#define IOC_P12C_IDX 11  //Port#12 Function : [GET] _IO 01  0B ... [SET] _IO 02  0B ss ... [ACK] _IO 02  0B ss ...; ss=[0:Digital Out; 1:Digital In;]
	#define IOC_P13C_IDX 12  //Port#13 Function : [GET] _IO 01  0C ... [SET] _IO 02  0C ss ... [ACK] _IO 02  0C ss ...; ss=[0:Digital Out; 1:Digital In;]
	#define IOC_P14C_IDX 13  //Port#14 Function : [GET] _IO 01  0D ... [SET] _IO 02  0D ss ... [ACK] _IO 02  0D ss ...; ss=[0:Digital Out; 1:Digital In;]
	#define IOC_P15C_IDX 14  //Port#15 Function : [GET] _IO 01  0E ... [SET] _IO 02  0E ss ... [ACK] _IO 02  0E ss ...; ss=[0:Digital Out; 1:Digital In;]
	#define IOC_P16C_IDX 15	 //Port#16 Function : [GET] _IO 01  0F ... [SET] _IO 02  0F ss ... [ACK] _IO 02  0F ss ...; ss=[0:Digital Out; 1:Digital In;]
	#define IOC_ANL_IDX 16	 //Analog Trig Level Lower: : [GET] _IO 01  10 ... [SET] _IO 03  10 s0 s1 ... [ACK] _IO 03  10 s0 s1 ...; s1:s0 =[1...4000]
	#define IOC_ANH_IDX 17	 //Analog Trig Level Lower: : [GET] _IO 01  11 ... [SET] _IO 03  11 s0 s1 ... [ACK] _IO 03  11 s0 s1 ...; s1:s0 =[1...4000]

	#define IOC_RIO_IDX 200

		// 端口功能 X 取值
		#define IOC_DOU_IDX 0	// Digital Output /数字输出
		#define IOC_DIN_IDX 1	// Digital Input  /数字输入			
		#define IOC_AIN_IDX 2	// Analog  Input /模拟输入(仅P3)				
		#define IOC_QEI_IDX 3	// Aux. Encoder Input 外部编码器输入 (Only UIM342，UIM341)				
		#define IOC_PWM_IDX 3	// PWM Output PWM输出 (Only UIM6828's P7/P8)

#define __IL	52	// Input Triggered Action Logic / 输入触发动作逻辑 

	// Following Only for UIM342/342/720
	#define SCF_S1C_IDX 0   // Input# 1 : [GET] _IL 01  00 ... [SET] _IL 03  00 Af Ar ... [ACK] _IL 03  00 Af Ar ...; Ar=[Action Code on Rising Edge]; Af=[Action Code on Falling Edge];
	#define SCF_S2C_IDX 1   // Input# 2 : [GET] _IL 01  01 ... [SET] _IL 03  01 Af Ar ... [ACK] _IL 03  01 Af Ar ...; Ar=[Action Code on Rising Edge]; Af=[Action Code on Falling Edge];
	#define SCF_S3C_IDX 2
	#define SCF_S4C_IDX 3
	#define SCF_S5C_IDX 4
	#define SCF_S6C_IDX 5
	#define SCF_S7C_IDX 6
	#define SCF_S8C_IDX 7   
	#define SCF_P1C_IDX 8
	#define SCF_P2C_IDX 9
	#define SCF_P3C_IDX 10
	#define SCF_P4C_IDX 11
	#define SCF_P5C_IDX 12
	#define SCF_P6C_IDX 13
	#define SCF_P7C_IDX 14
	#define SCF_P8C_IDX 15  // Input#16 : [GET] _IL 01  0F ... [SET] _IL 03  0F Af Ar ... [ACK] _IL 03  0F Af Ar ...; Ar=[Action Code on Rising Edge]; Af=[Action Code on Falling Edge];
	#define SCF_STL_IDX 16	// On Stall      : [GET] _IL 01  10 ... [SET] _IL 03  10 As 00 ... [ACK] _IL 03  10 As 00 ...; As=[Action Code on Stall] 堵转时触发动作 (Only for UIM342/341)
	#define SCF_TLC_IDX 17	// On TorqueLimit: [GET] _IL 01  11 ... [SET] _IL 03  11 Ac Tq ... [ACK] _IL 03  11 Ac Tq ...; Ac=[Action Code on Torque Limit] 力矩超限触发动作;  Tq=[10...300]% (Torque Limit) (Only for UIM720)

		// On Input Trigered Action                         /输入触发的动作
		#define ILC_NOP_IDX 0x00 // 00 Disable              /无动作
		#define ILC_OFF_IDX 0x01 // 01 Driver OFF            /脱机
		#define ILC_EST_IDX 0x02 // 02 Emergent Stop         /急停
		#define ILC_DST_IDX 0x03 // 03 Decelerating Stop     /减停
		#define ILC_OPR_IDX 0x04 // 04 Origin + reverse PR;  /设原点+换向位移
		#define ILC_OES_IDX 0x05 // 05 Origin + EStop;      /设原点+急停
		#define ILC_ODS_IDX 0x06 // 06 Origin + DStop;      /设原点+减速停止
		#define ILC_RJV_IDX 0x07 // 07 Reverse JV;   ~JV     /换向匀速
		#define ILC_SJV_IDX 0x08 // 08 Signed  JV; +/-JV     /定向速度
		#define ILC_RPR_IDX 0x09 // 09 Reverse PR;   ~PR     /换向位移
		#define ILC_SPR_IDX 0x0A // 10 Signed  PR; +/-PR     /定向位移
		#define ILC_SPA_IDX 0x0B // 11 Signed  PA; +/-PA     /定向位置
		#define ILC_PVT_IDX 0x0F // 15 Execute PVT /执行PVT

		// On Stall/TorqueLimit Trigered Action            /转矩超限触发的动作
		#define MXN_TLC_SVOF 1	// Servo OFF			   /脱机(反馈触点位置)   
		#define MXN_TLC_EMST 2	// Emergent Stop		   /急停(反馈触点位置)   
		#define MXN_TLC_REPR 3	// Reverse PR			   /换向位移(含触点位置) 
		#define MXN_TLC_HOME 4	// Reverse PR..Set Origin  /换向位移后清零
		#define MXN_TLC_REJV 5	// Reverse JV		       /换向持续速度(含触点位置) 

#define __TG	53	// Input Trigger Mode / 输入触发方式 

	#define STG_S1T_IDX 0   //TG[0]=X : IN01TrigCfg = X / TG[0]=X : IN01TrigCfg = X
	#define STG_S2T_IDX 1   //TG[1]=X : IN01TrigCfg = X / TG[1]=X : IN01TrigCfg = X
	#define STG_S3T_IDX 2   //TG[2]=X : IN01TrigCfg = X / TG[2]=X : IN01TrigCfg = X
	#define STG_S4T_IDX 3   //TG[3]=X : IN01TrigCfg = X / TG[3]=X : IN01TrigCfg = X
	#define STG_S5T_IDX 4   //TG[4]=X : IN01TrigCfg = X / TG[4]=X : IN01TrigCfg = X
	#define STG_S6T_IDX 5   //TG[5]=X : IN01TrigCfg = X / TG[5]=X : IN01TrigCfg = X
	#define STG_S7T_IDX 6   //TG[6]=X : IN01TrigCfg = X / TG[6]=X : IN01TrigCfg = X
	#define STG_S8T_IDX 7   //TG[7]=X : IN08TrigCfg = X / TG[7]=X : IN08TrigCfg = X

#define __AI	54	// Analogue signal query AI[4] Query servo motor torque (720 only) / 模拟信号查询 AI[4] 查询伺服电机力矩(仅720)

#define __DI	55	// Digital Signal Inputs and Outputs / 数字信号输入输出; [GET] _DI 00... [SET] _DI 04 OP0 MK0 OP1 MK1...; ACK|Tx: _DI 04 IN0 OP0 IN1 OP1 ...;
					// IN0.0 - Input #1
					// IN0.1 - Input #2
					// ...
					// IN0.7 - Input #8
					// 
					// OP0.0 - Output #1
					// OP0.1 - Output #2
					// ...
					// OP0.7 - Output #8
					// 
					// MK0.0 - Mask for Output #1; 0= Don't Change; 1= Apply Change
					// MK0.1 - Mask for Output #2; 0= Don't Change; 1= Apply Change
					// ...
					// MK0.7 - Mask for Output #8; 0= Don't Change; 1= Apply Change

#define __PW	57  // PW[X.8] = Y.16  占空比 0-200
	// PW[0] - Port 1 Duty Cycle, [0,200]  = [0.5, 100]%
	// PW[1] - Port 1 Base Cycle Shift, (1,2or3) = 1us,8us,32us
	// PW[2] - Port 1 Base Cycle, [200,50000] = [200, 50,000]us,[1600, 400,000]us,[6400, 1,600,000]us 
	// PW[3] - Port 2 Duty Cycle, [0,200]  = [0.5, 100]%
	// PW[4] - Port 2 Base Cycle Shift, (1,2or3) = 1us,8us,32us
	// PW[5] - Port 2 Base Cycle, [200,50000] = [200, 50,000]us,[1600, 400,000]us,[6400, 1,600,000]us

#define __TI	58	// Temperature display, set alarm value /温度显示，设置报警值

#define __DA    59  // Query/Set Microcontroller Date / 查询/设置 微控机日期

// [ Closed-loop feedback ] 61-70 [ 闭环反馈 ] 61-70

#define __QE	61	// Encoder parameters, Note: cannot be queried/set in motion // 编码器参数, Note: 运动中无法查询/设置

	#define QES_LPR_IDX 0 // QE[0] Number of wires per turn for closed loop control (not multiplied by 4) /闭环控制每圈线数（不乘以 4）
	#define QES_PQE_IDX 1 // QE[1] Stall tolerance 0...65535;          / 堵转容差 0...65535
	#define QES_AER_IDX 2 // QE[2] ABS Encoder single turn bit number; / ABS 编码器单圈位数
	#define QES_ABW_IDX 3 // QE[3] ABS Encoder battery voltage;        / ABS 编码器电池电压;
	#define QES_CPR_IDX 4 // QE[4] Counts for one motor revolution     / 电机每转脉冲数;
	#define QES_SYN_IDX 5 // QE[5] Reset position error (only when Vc=0) / 清除残留位置误差，在Vc = 0 时设置

#define __PX	62	// Query Auxiliary Position / 查询辅助位置
	// PX[0]; - When closed-loop, return open-loop position (pulse) / 闭环时，返回开环位置（脉冲）
	// PX[0]; - When open-loop, return closed-loop position (encoder count) / 开环时，返回闭环位置（编码器计数）
	// PX[1]; - When using touch force control, return touch point position / 使用碰触力控时，返回碰触点位置
	// PX[1]; - When not using touch force control, return infinity 0x7FFFFFFF / 不使用碰触力控时，返回无穷大 0x7FFFFFFF

// [ Auxiliary Operation ] 81-90 [ 辅助操作 ] 81-90

#define __OP	81  // OPR Operation / OPR 操作
#define __DM	82	// Direct Data Transfer, IDX.8 / 直接数据传递, IDX.8
#define __UI	83	// User-defined Integer Array, IDX.16 / 用户编程整型数组, IDX.16
#define __UF	84	// User-defined Floating-point Array, IDX.16 / 用户编程浮点型数组, IDX.16
#define __NT	85	// Node Data Transmission, 900 / 节点数据发送，900
#define __NR	86	// Node Single-line Data Reception, 900 / 节点单行数据接受，900

// [ 信息标识 ]

#define __RT	90	// 自动反馈，UPG 返回, EMR由此通知 

	#define RTCN_SYS_EMR 0

	#define RTCN_DIO_P1L 1  // P1 Low  Level
	#define RTCN_DIO_P1H 2  // P1 High Level
	#define RTCN_DIO_P2L 3  // P2 Low  Level
	#define RTCN_DIO_P2H 4  // P2 High Level
	#define RTCN_DIO_P3L 5  // P3 Low  Level
	#define RTCN_DIO_P3H 6  // P3 High Level
	#define RTCN_DIO_P4L 7  // P4 Low  Level
	#define RTCN_DIO_P4H 8  // P4 High Level
	#define RTCN_DIO_P5L 9
	#define RTCN_DIO_P5H 10
	#define RTCN_DIO_P6L 11
	#define RTCN_DIO_P6H 12
	#define RTCN_DIO_P7L 13
	#define RTCN_DIO_P7H 14
	#define RTCN_DIO_P8L 15
	#define RTCN_DIO_P8H 16

	#define RTCN_DIO_P9L  17
	#define RTCN_DIO_P9H  18
	#define RTCN_DIO_P10L 19
	#define RTCN_DIO_P10H 20
	#define RTCN_DIO_P11L 21
	#define RTCN_DIO_P11H 22
	#define RTCN_DIO_P12L 23
	#define RTCN_DIO_P12H 24
	#define RTCN_DIO_P13L 25
	#define RTCN_DIO_P13H 26
	#define RTCN_DIO_P14L 27
	#define RTCN_DIO_P14H 28
	#define RTCN_DIO_P15L 29
	#define RTCN_DIO_P15H 30
	#define RTCN_DIO_P16L 31
	#define RTCN_DIO_P16H 32

	#define RTCN_MXN_GMR  40 // Group Emergency
	#define RTCN_MXN_INP  41 // PTP Motion, In Position

	#define RTCN_MXN_STL  43 // Stall Detected
	#define RTCN_MXN_PVW  44 // Warnning for PVT Fifo iData Quantity Low
	#define RTCN_MXN_PVS  45 // Stopped due to PVT Fifo Empty

	#define RTCN_MXN_TLC  50 // TLC Hit

	#define RTCN_MXN_ST0  51 //
	#define RTCN_MXN_ST1  52 //

	#define RTCN_UPG_PRT  60 //
	#define RTCN_CAM_TG1  71 //
	#define RTCN_CAM_TG2  72 //

	#define RTCN_ABS_TRP0 101 //
	#define RTCN_ABS_TRP1 102 //
	#define RTCN_ABS_TRP2 103 //

#define __GW	91  // Automatic Feedback, Group Emergency Warning; After the device emergency stop, notify the equipment in the same group; it is recommended that the group number is set to small, in order to obtain the priority of the message 
					// 自动反馈，分组紧急警告; 本设备急停后, 通知同组设备; 建议组号设小, 以便取得报文优先级

#define __RO	92  // Remote Output Projection, Extension PVTA Triggered Output via 900A/901S OP01-08 / 远程输出投射, 分机 PVTA 触发后，通过900A/901S OP01-08输出

/// OPR Instruction


///----------------------------------------------------------------
// OPR Code (Classe 02, for UFQ) / OPR 02 类, UFQ
///----------------------------------------------------------------	

#define __OP	81		// OPR Operation / OPR 操作

	#define OPR_CATALOG_SYSTEM     (0)
	#define OPR_CATALOG_RFID       (1)	//  RFID
	#define OPR_CATALOG_UFQ        (2)  //  UFQ
	#define OPR_CATALOG_SERVO 	   (3)	//	SERVO

//  00 Classes, Systems. / 00 类;  OPR [00] [ _DMA_SET] [ NN ];

// Programming operations /编程操作
#define OPR_DMA_SET 1 	// All DmaData[] = NN, fill DMA: OPR[00][_DMA_SET][NN]; / 所有DmaData[] = NN, 填充DMA: OPR[00][_DMA_SET][NN];
#define OPR_UPG_CLR 2	// Clear program latch area; / 清空程序锁存区;
#define OPR_UPG_WRT 3	// Write/modify one line of program; / 编写/修改一行程序;
#define OPR_UPG_RED 4	// Read one line of program; / 读取一行程序;
#define OPR_UPG_EPT 5	// Set program execution position; / 设置程序执行位置;

// DUMP
#define OPR_PVT_DMP 8 //

// Tracking management / 追踪管理
#define OPR_SUB_VER	20	//
#define OPR_SNR_NUM	21	//
#define OPR_PID_NUM	22	//
#define OPR_VID_NUM	23	//
#define OPR_MOT_CUR	28	// Set motor current peak; / 设置电机电流峰值
#define OPR_MOT_DIR	29	// Set motor open-loop direction; / 设置电机开环方向

// Self-check /自检
#define OPR_SLF_EDU	30	// 0x1E Automatic aging test; / 0x1E 自动老化测试
#define OPR_SLF_CHK	31	// 0x1F Port self-check, encoder self-check; / 0x1F 端口自检, 编码器自检
#define OPR_CAN_SHD	32	// 0x20 CAN off; / 0x20 CAN 关闭		

#endif

///========================================
//		    Macro Function Define
///========================================

// PauseMS(uint)/PauseUS(uint)
#ifdef _WINDOWS
	#define PauseMS(ms) Sleep(ms)
	#define PauseUS(us) Sleep(us)
#elif defined(_LINUX)
	#define PauseMS(ms) usleep(ms * 1000)
	#define PauseUS(us) usleep(us)
#endif
