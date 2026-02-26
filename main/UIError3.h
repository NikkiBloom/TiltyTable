
////////////////////////////////////////////////////////////////////////////////
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,//
// EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED      //
// WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.     //
//                                                                            //
//                          Copyright (c) UIROBOT.                            //
//                           All rights reserved.                             //
////////////////////////////////////////////////////////////////////////////////

#define _BLOCK_
#pragma once

/// SDK returns success (No Error)
#define SDK_RETURN_SUCCESS 0  

/// Error From uirSDK
#ifdef _BLOCK_

#define SDK_ERROR_GTWYDOWN           0x90000001
#define STR_ERROR_GTWYDOWN           "Gateway is not running."

#define SDK_ERROR_TXRXBUSY           0x90000003
#define STR_ERROR_TXRXBUSY           "Gateway in busy in TX/RX; use mutex in multithread."

#define SDK_ERROR_TIMEOUT            0x90000004
#define STR_ERROR_TIMEOUT            "Gateway timeout during recieving process."

#define SDK_ERROR_ANAYLSEPACKAGE     0x91000005
#define STR_ERROR_ANAYLSEPACKAGE     "Failed to parse CAN message."

#define SDK_ERROR_ANAYLSENOTPACKAGE  0x91000006
#define STR_ERROR_ANAYLSENOTPACKAGE  "No message input for parsing."

#define SDK_ERROR_NOT_OUTBUFFER      0x91000007
#define STR_ERROR_NOT_OUTBUFFER      "No output buffer given for parsing."

#define SDK_ERROR_RECEIVE_ERROR      0x91000008
#define STR_ERROR_RECEIVE_ERROR      "Received error message."

#define SDK_ERROR_ANAYLSE_ERROR_LEN  0x91000009
#define STR_ERROR_ANAYLSE_ERROR_LEN  "Length of the Message Received is wrong."

#define SDK_ERROR_FUNCINPUT_NULL	 0x9100000A
#define STR_ERROR_FUNCINPUT_NULL     "Input parameter of SDK function is illegal (NULL pointer)."

#define SDK_ERROR_FUNCINPUT_PARA	 0x9100000B
#define STR_ERROR_FUNCINPUT_PARA     "Invalid / missing parameter(s) for SDK function."

#define SDK_ERROR_PARAMETER          0x91000010
#define STR_ERROR_PARAMETER          "Invalid input parameter."

#define SDK_ERROR_WRITE_ERROR        0x91000011
#define STR_ERROR_WRITE_ERROR        "Write to failed."

#define SDK_ERROR_TCW_ERROR          0x91000012
#define STR_ERROR_TCW_ERROR          "Invalid instruction control word."

#define SDK_ERROR_PORT_ERROR        0x91000013
#define STR_ERROR_PORT_ERROR        "Unkown Communication Port Error."

#define SDK_ERROR_COM_FILE			0x91000213
#define STR_ERROR_COM_FILE			"Serial COM port does not exist or is unavailable."

#define SDK_ERROR_COM_PATH			0x91000313
#define STR_ERROR_COM_PATH			"Serial COM port does not exist or is unavailable."

#define SDK_ERROR_COM_ACCESS		0x91000513
#define STR_ERROR_COM_ACCESS		"Serial COM port is already in use or access denied."

#define SDK_ERROR_COM_NAME			0x91007B13 
#define STR_ERROR_COM_NAME			"Invalid Serial COM port name."

// Opening a USB–serial COM port (CH340, CP2102, FTDI, etc.) can fail if unplugged 
// or accessed too soon after insertion. Some drivers may show the port in the 
// registry even when it is non‑functional.
#define SDK_ERROR_COM_UNIT			0x91001413
#define STR_ERROR_COM_UNIT			"Serial Device not ready or not exist."

// Some USB‑to‑serial chips may not support certain baud rates, parity modes, or
// advanced flow control, and will return an error if used. Likewise, some APIs
// in older systems are unimplemented or disabled but still have entry points,
// and will return ERROR_NOT_SUPPORTED when called.
#define SDK_ERROR_COM_SUPPORT		0x91003213
#define STR_ERROR_COM_SUPPORT		"Serial Device does not support this operation."

// Serial port access may fail if hardware fails, the USB-to-serial device
// is unplugged, or the driver encounters a serious error.
#define SDK_ERROR_COM_GENFAIL		0x91001F13
#define STR_ERROR_COM_GENFAIL		"General Serial device failure."

#define SDK_ERROR_INITIAL_ERROR     0x91000014
#define STR_ERROR_INITIAL_ERROR     "SDK Initialization failed."

#endif

/// Error From Motor Instruction
#ifdef _BLOCK_

#define ERR_INS_SYNT		0x32		// Error From Motor
#define STR_ERR_INS_SYNT	"Instruction's Syntax is wrong."
#define SDK_ERROR_INS_SYNT	0x93000032  // Error From uirSDK Function Calling
#define STR_ERROR_INS_SYNT	"Instruction's Syntax is wrong."

#define ERR_INS_NUMB		0x33	
#define STR_ERR_INS_NUMB	"Instruction's Data are wrong."
#define SDK_ERROR_INS_NUMB	0x93000033
#define STR_ERROR_INS_NUMB	"Instruction's Data are wrong."

#define ERR_INS_IDXR		0x34	
#define STR_ERR_INS_IDXR	"Instruction's Sub-Index is wrong."
#define SDK_ERROR_INS_IDXR	0x93000034
#define STR_ERROR_INS_IDXR	"Instruction's Sub-Index is wrong."

#define ERR_SYS_STTM		0x35	
#define STR_ERR_SYS_STTM	"[TIME] Cannot change system time, while the motor is running."
#define SDK_ERROR_SYS_STTM	0x93000035
#define STR_ERROR_SYS_STTM	"[TIME] Cannot change system time, while the motor is running."

#define ERR_MXN_DCSD		0x3C	
#define STR_ERR_MXN_DCSD	"[MXN] Stop Decelleration (SD) is slower than the Decelleration(DC)."
#define SDK_ERROR_MXN_DCSD	0x9300003C
#define STR_ERROR_MXN_DCSD	"[MXN] Stop Decelleration (SD) is slower than the Decelleration(DC)."

#define ERR_MXN_MRUN		0x3D
#define STR_ERR_MXN_MRUN	"[MXN] Cannot change or query, while the motor is running."
#define SDK_ERROR_MXN_MRUN	0x9300003D
#define STR_ERROR_MXN_MRUN	"[MXN] Cannot change or query, while the motor is running."

#define ERR_MXN_MOFF		0x3E
#define STR_ERR_MXN_MOFF	"[MXN] Cannot BG, when the motor driver is OFF."
#define SDK_ERROR_MXN_MOFF	0x9300003E
#define STR_ERROR_MXN_MOFF	"[MXN] Cannot BG, when the motor driver is OFF."

#define ERR_MXN_MTSD		0x3F
#define STR_ERR_MXN_MTSD	"[MXN] Cannot BG, when the motor is performing Emergency Stop."
#define SDK_ERROR_MXN_MTSD	0x9300003F
#define STR_ERROR_MXN_MTSD	"[MXN] Cannot BG, when the motor is performing Emergency Stop."

#define ERR_MXN_BENA		0x40
#define STR_ERR_MXN_BENA	"[MXN] Cannot BG, when the motor Brake is Locked."
#define SDK_ERROR_MXN_BENA	0x93000040
#define STR_ERROR_MXN_BENA	"[MXN] Cannot BG, when the motor Brake is Locked."

#define ERR_MXN_BDIS		0x41	
#define STR_ERR_MXN_BDIS	"[MXN] Cannot turn off the motor driver, when the motor Brake is unlocked."
#define SDK_ERROR_MXN_BDIS	0x93000041	
#define STR_ERROR_MXN_BDIS	"[MXN] Cannot turn off the motor driver, when the motor Brake is unlocked."

#define ERR_MXN_SPOG		0x42	
#define STR_ERR_MXN_SPOG	"[MXN] Cannot set origin (for ABS encoder only), when the motor is running."
#define SDK_ERROR_MXN_SPOG	0x93000042	
#define STR_ERROR_MXN_SPOG	"[MXN] Cannot set origin (for ABS encoder only), when the motor is running."

#define ERR_PVT_RUNG		0x46	
#define STR_ERR_PVT_RUNG	"[PVT] Cannot set PV or MP[0], when the motor is running."
#define SDK_ERROR_PVT_RUNG	0x93000046	
#define STR_ERROR_PVT_RUNG	"[PVT] Cannot set PV or MP[0], when the motor is running."

#define ERR_PVT_WPOV		0x47	
#define STR_ERR_PVT_WPOV	"[PVT] Index of QP/QV/QT exceeds MP[6]"
#define SDK_ERROR_PVT_WPOV	0x93000047	
#define STR_ERROR_PVT_WPOV	"[PVT] Index of QP/QV/QT exceeds MP[6]"

#define ERR_PVT_IOFN		0x48	
#define STR_ERR_PVT_IOFN	"[PVT] QA Mask not meeting I/O function requirements"
#define SDK_ERROR_PVT_IOFN	0x93000048	
#define STR_ERROR_PVT_IOFN	"[PVT] QA Mask not meeting I/O function requirements"

#define ERR_PVB_OVFL		0x49	
#define STR_ERR_PVB_OVFL	"[PVT] PVT buffer overflow"
#define SDK_ERROR_PVB_OVFL  0x93000049
#define STR_ERROR_PVB_OVFL	"[PVT] PVT buffer overflow"

#define ERR_SXP_BUSY		0x4A	
#define STR_ERR_SXP_BUSY	"[PVT] Sx processing not complete, new parameters not accepted"
#define SDK_ERROR_SXP_BUSY  0x9300004A
#define STR_ERROR_SXP_BUSY	"[PVT] Sx processing not complete, new parameters not accepted"

#endif

/// Realtime From Motor
#ifdef _BLOCK_

#define ERR_SYS_INIT		0X09
#define RTCN_ERR_SYS_INIT   0x93000009
#define STR_ERR_SYS_INIT    "Motor Self-test Error during Power-up"

#define ERR_SYS_LOCK		0X0A
#define RTCN_ERR_SYS_LOCK	0x9300000A
#define STR_ERR_SYS_LOCK	"Emergency Stopped & System Locked"

#define ERR_SYS_STOP		0X0B
#define RTCN_ERR_SYS_STOP	0x9300000B
#define STR_ERR_SYS_STOP	"Motor is in emergency stopping, NO instruction allowed !"

#define ERR_SYS_OVHT		0X0C
#define RTCN_ERR_SYS_OVHT	0x9300000C
#define STR_ERR_SYS_OVHT	"Motor Overheat"

#define ERR_SYS_POVL		0X0D
#define RTCN_ERR_SYS_POVL	0x9300000D
#define STR_ERR_SYS_POVL	"Motor Output Port Short Circuit"

#define ERR_SYS_S232		0X0E
#define STR_ERR_SYS_S232	"[UIM720] Communication Error (Only for UIM720)"
#define RTCN_ERR_SYS_SCME	0x9300000E
#define STR_ERR_SYS_SCME	"[UIM720] Communication Error (Only for UIM720)"

#define ERR_SYS_SRST		0X0F
#define RTCN_ERR_SYS_SRST	0x9300000F
#define STR_ERR_SYS_SRST	"[UIM720] Requires Restart (Only for UIM720)"

#define ERR_SYS_SALM		0X10
#define RTCN_ERR_SYS_SALM	0x93000010
#define STR_ERR_SYS_SALM	"[UIM720] Alarm Detected (Only for UIM720)"

#define ERR_SYS_SIOS		0X11
#define STR_ERR_SYS_SIOS	"[UIM720] I/O Fault Detected (Only for UIM720)"
#define RTCN_ERR_SYS_SIOE   0x93000011
#define STR_ERR_SYS_SIOE	"[UIM720] I/O Fault Detected (Only for UIM720)"

#define ERR_SYS_SLAV		0X12
#define RTCN_ERR_SYS_SLAV	0x93000012
#define STR_ERR_SYS_SLAV	"[UIM720] Sub-Chip Fault (Internal Error)"

#define ERR_SYS_STLC		0X13
#define RTCN_ERR_SYS_STLC   0x93000013 
#define STR_ERR_SYS_STLC	"[UIM720] Torque Sampling Error (Only for UIM720)"

#define ERR_CTX_TMOT 		0X14
#define RTCN_ERR_CTX_TMOT 	0x93000014
#define STR_ERR_CTX_TMOT	"[Gateway] CAN Message Sent, but get no response from any node !"

#define ERR_MXN_ACHL		0X16
#define RTCN_ERR_MXN_ACHL	0x93000016
#define STR_ERR_MXN_ACHL	"[Software Limit] Acceleration/Deceleration over Limit"

#define ERR_MXN_SPHL		0X17
#define RTCN_ERR_MXN_SPHL	0x93000017
#define STR_ERR_MXN_SPHL	"[Software Limit] Exceed Speed Limit"

#define ERR_MXN_BPLL		0X18
#define RTCN_ERR_MXN_BPLL	0x93000018
#define STR_ERR_MXN_BPLL	"[Software Limit] Exceed Lower Bump Position"

#define ERR_MXN_BPHL		0X19
#define RTCN_ERR_MXN_BPHL	0x93000019
#define STR_ERR_MXN_BPHL	"[Software Limit] Exceed Upper Bump Position"

#define ERR_MXN_PALL		0X1A
#define RTCN_ERR_MXN_PALL	0x9300001A
#define STR_ERR_MXN_PALL	"[Software Limit] Exceed Lower Working Position"

#define ERR_MXN_PAHL		0X1B
#define RTCN_ERR_MXN_PAHL	0x9300001B
#define STR_ERR_MXN_PAHL	"[Software Limit] Exceed Upper Working position"

#define ERR_MXN_SPER		0X1C
#define RTCN_ERR_MXN_SPER	0x9300001C
#define STR_ERR_MXN_SPER	"[Software Limit] Exceed Speed Error Limit (Drifting)"

#define ERR_MXN_PQER		0X1D 
#define RTCN_ERR_MXN_PQER	0x9300001D
#define STR_ERR_MXN_PQER	"[Software Limit] Exceed Position Error Limit (stall)"

#define ERR_MXN_ENCC		0X1E
#define RTCN_ERR_MXN_ENCC	0x9300001E
#define STR_ERR_MXN_ENCC	"Encoder Error"

#define ERR_MXN_ENCB		0X1F
#define RTCN_ERR_MXN_ENCB	0x9300001F
#define STR_ERR_MXN_ENCB	"Aboslute Encoder Battery Low"
#endif
