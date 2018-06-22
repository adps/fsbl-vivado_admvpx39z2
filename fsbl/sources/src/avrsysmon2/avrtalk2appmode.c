#include "avrsysmon2.h"
#include "xil_printf.h"

bool RequestVPD( AVRSysMon_Context* pContext, void( *fnPump )( AVRSysMon_Context* ), unsigned char* pVPD )
{
	static unsigned char pSendBuffer[1024];
	static unsigned char pRespBuffer[1024];

	pSendBuffer[0] = 0x05;
	pSendBuffer[1] = 0x8C;

	AVRSysMon_CommandReq sReq;
	sReq.m_pTXData = pSendBuffer;
	sReq.m_nTXSize = 2;
	sReq.m_nRXBuffSize = 1024;
	sReq.m_pRXData = pRespBuffer;
	AVRSysMon_Result eStatus = AVRSysMon_Request( pContext, &sReq );
	switch( eStatus )
	{
	case AVRSysMon_ResultOK:
		break;
	default:
		xil_printf("\r\nUnexpected result from AVRSysMon_Request.");
		return false;
	}
	AVRSysMon_Result eRes = AVRSysMon_ResultQueued;
	while( ( eRes == AVRSysMon_ResultActive ) || ( eRes == AVRSysMon_ResultQueued ) )
	{
		fnPump( pContext );
		eRes = AVRSysMon_CompleteReq( pContext, &sReq );
	}
	if( ( pRespBuffer[0] != 0x05 ) )
	{
		xil_printf("\r\nUnexpected response from uCPU.");
		return false;
	}
	if( ( pRespBuffer[1] != 0x0C ) )
	{
		xil_printf("\r\nUnexpected response from uCPU.");
		return false;
	}
	if( ( pRespBuffer[2] != 0x00 ) )
	{
		xil_printf("\r\nUnexpected status from uCPU.");
		return false;
	}

	memcpy( pVPD, pRespBuffer + 3, 256 );


	return true;
}

bool ReportSensors( AVRSysMon_Context* pContext, void( *fnPump )(AVRSysMon_Context*), bool bServiceMode )
{
	static unsigned char pSendBuffer[1024];
	static unsigned char pRespBuffer[1024];

	pSendBuffer[0] = 0x05;
	pSendBuffer[1] = 0x89;

	AVRSysMon_CommandReq sReq;
	sReq.m_pTXData = pSendBuffer;
	sReq.m_nTXSize = 2;
	sReq.m_nRXBuffSize = 1024;
	sReq.m_pRXData = pRespBuffer;
	AVRSysMon_Result eStatus = AVRSysMon_Request( pContext, &sReq );
	switch( eStatus )
	{
	case AVRSysMon_ResultOK:
		break;
	default:
		xil_printf("\r\nUnexpected result from AVRSysMon_Request.");
		return false;
	}
	AVRSysMon_Result eRes = AVRSysMon_ResultQueued;
	while( (eRes == AVRSysMon_ResultActive) || (eRes == AVRSysMon_ResultQueued) )
	{
		fnPump( pContext );
		eRes = AVRSysMon_CompleteReq( pContext, &sReq );
	}
	if( (pRespBuffer[0] != 0x05) )
	{
		xil_printf("\r\nUnexpected response from uCPU.");
		return false;
	}
	if( (pRespBuffer[1] != 0x09) )
	{
		xil_printf("\r\nUnexpected response from uCPU.");
		return false;
	}
	if( (pRespBuffer[2] != 0x00) )
	{
		xil_printf("\r\nUnexpected status from uCPU.");
		return false;
	}
	return true;
}
