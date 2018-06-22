#ifndef _RESPS
#define _RESPS

#include "cpp_types.h"

struct SCmdResponces
{
    unsigned char m_pHeader[2];
    bool (*m_fpt)( unsigned short*, unsigned char, bool, bool* );
};

#define RESP_LIST_START(x) struct SCmdResponces x[] = {
#define RESP_LIST_END { { 0x00, 0x00 } , (bool(*)( unsigned short*, unsigned char, bool, bool* )) 0x0 } };

#define RESP_FUNC(x) bool x ( unsigned short* nProgress, unsigned char nByte, bool bEoT, bool* bFinished )

#include "resps_auto.h"
#include "resps_auto_ext.h"
#include "resps_user.h"
#include "resps_user_ext.h"
#include "resps_flash.h"

static RESP_LIST_START( g_pCommandResponce )
    // Auto
	{ { 0x00, 0x00 } , Resp_ServiceModeDetect },
	{ { 0x00, 0x02 } , Resp_ApplicationModeDetect },
	{ { 0x00, 0x03 } , Resp_ApplicationModeDetect },
	// Auto Extended
    { { 0x02, 0x00 } , Resp_AlarmTemperatureW },
    { { 0x02, 0x02 } , Resp_AlarmTemperatureC },
    { { 0x02, 0x03 } , Resp_AlarmVoltageW },
    { { 0x02, 0x05 } , Resp_AlarmVoltageC },
    { { 0x02, 0x06 } , Resp_SensorUpdate },
    { { 0x02, 0x07 } , Resp_VPDUpdate },
    { { 0x02, 0x08 } , Resp_Mez0ModuleUpdate },
    { { 0x02, 0x09 } , Resp_Mez1ModuleUpdate },
    { { 0x02, 0x0A } , Resp_Mez2ModuleUpdate },
    { { 0x02, 0x0B } , Resp_Mez3ModuleUpdate },
    // User
    { { 0x03, 0x00 } , RespServiceMode },
    { { 0x03, 0x02 } , RespApplicationMode },
    { { 0x03, 0x03 } , RespVersion },
    { { 0x03, 0x04 } , RespLoopback32x8 },
    { { 0x03, 0x06 } , RespGetProductID },
    // User Extended
    { { 0x05, 0x00 } , Resp_I2CRead },
    { { 0x05, 0x02 } , Resp_I2CWrite },
    { { 0x05, 0x05 } , Resp_AbortProtection },
    { { 0x05, 0x06 } , Resp_InvokeProtection },
    { { 0x05, 0x07 } , Resp_RequestVPDUpdate },
    { { 0x05, 0x08 } , Resp_RequestMezUpdate },
    { { 0x05, 0x09 } , Resp_RequestSensors },
    { { 0x05, 0x0A } , Resp_RequestBlockBox },
    { { 0x05, 0x0B } , Resp_ForceMezSupply },
    { { 0x05, 0x0C } , Resp_RequestVPDData },
    { { 0x05, 0x0D } , Resp_RequestMezData },
    { { 0x05, 0x0E } , Resp_SetFrequency },
    { { 0x05, 0x0F } , Resp_GetFrequency },
    { { 0x05, 0x01 } , Resp_OverrideSensor },
	{ { 0x05, 0x03 } , Resp_SetFrequencyEx },
	{ { 0x05, 0x04 } , Resp_GetFrequencyEx },
	// Auto
	{ { 0x06, 0x00 } , Resp_FlashApplicationRead },
	{ { 0x06, 0x02 } , Resp_FlashApplicationWrite },
RESP_LIST_END

#endif