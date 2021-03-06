/******************************************************************************
*
* Copyright (C) 2015 - 17 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xfsbl_main.c
*
* This is the main file which contains code for the FSBL.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date        Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00  kc   10/21/13 Initial release
* 1.00  ba   02/22/16 Added performance measurement feature.
* 2.0   bv   12/02/16 Made compliance to MISRAC 2012 guidelines
*                     Added warm restart support
*
* </pre>
*
* @note
*
******************************************************************************/

/***************************** Include Files *********************************/
#include "xfsbl_hw.h"
#include "xfsbl_main.h"
#include "bspconfig.h"
//#include "emmc_test.h"
//#include "xdppsu_common_example.h"
//#include "xdppsu_poll_example.h"
//#include "xdppsu_intr_example.h"
//#include "xdpdma_video_example.h"
#include "avrboot.h"
//#include "fatbit.h"
#include "xemacps.h"

//#pragma GCC optimize ("O0")

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/
static void XFsbl_UpdateMultiBoot(u32 MultiBootValue);
static void XFsbl_FallBack(void);
u32 EthernetPhyEnable(u32 nGEMAddr);

/************************** Variable Definitions *****************************/
XFsblPs FsblInstance={0x3U, XFSBL_SUCCESS, 0U, 0U, 0U};
/*****************************************************************************/
/** This is the FSBL main function and is implemented stage wise.
 *
 * @param	None
 *
 * @return	None
 *
 *****************************************************************************/
int main(void )
{
	/**
	 * Local variables
	 */
	u32 FsblStatus = XFSBL_SUCCESS;
	u32 FsblStage = XFSBL_STAGE1;
	u32 PartitionNum = 0U;
	u32 EarlyHandoff = FALSE;
#ifdef XFSBL_PERF
	XTime tCur = 0;
#endif

#if defined(EL3) && (EL3 != 1)
#error "FSBL should be generated using only EL3 BSP"
#endif

	/**
	 * Initialize globals.
	 */
	while (FsblStage<=XFSBL_STAGE_DEFAULT) {

		switch (FsblStage)
		{

		case XFSBL_STAGE1:
			{
				/**
				 * Initialize the system
				 */

				FsblStatus = XFsbl_Initialize(&FsblInstance);
				if (XFSBL_SUCCESS != FsblStatus)
				{
					FsblStatus += XFSBL_ERROR_STAGE_1;
					FsblStage = XFSBL_STAGE_ERR;
				} else {

					EthernetPhyEnable(0xFF0C0000);
					EthernetPhyEnable(0xFF0E0000);
					/**
					 *
					 * Include the code for FSBL time measurements
					 * Initialize the global timer and get the value
					 */

					FsblStage = XFSBL_STAGE2;
				}
			} break;

		case XFSBL_STAGE2:
			{
				/* Reading Timer value for Performance measurement.*/
#ifdef XFSBL_PERF
				/* Get Start time for Boot Device init. */
				XTime_GetTime(&tCur);
#endif

				XFsbl_Printf(DEBUG_INFO,
						"================= In Stage 2 ============ \n\r");

				/**
				 * 	Primary Device
				 *  Secondary boot device
				 *  DeviceOps
				 *  image header
				 *  partition header
				 */

				//Small delay before trying to interact with the AVR to allow it to power up properly.
				usleep(200000);
				AVRBootCheck( BootCheckVPD );
				AVRBootCheck( BootCheckNormal );
				FsblStatus = XFsbl_BootDeviceInitAndValidate(&FsblInstance);
				//AVRBootCheck( BootCheckVPD );
				if ( (XFSBL_SUCCESS != FsblStatus) &&
						(XFSBL_STATUS_JTAG != FsblStatus) )
				{
					XFsbl_Printf(DEBUG_GENERAL,"Boot Device "
							"Initialization failed 0x%0lx\n\r", FsblStatus);
					FsblStatus += XFSBL_ERROR_STAGE_2;
					FsblStage = XFSBL_STAGE_ERR;
				} else if (XFSBL_STATUS_JTAG == FsblStatus) {
					/**
					 * This is JTAG boot mode, go to the handoff stage
					 */

				    //emmcTest();
					//XFsbl_Printf(DEBUG_PRINT_ALWAYS,"Running DP poll example\r\n");
					//DpPsu_PollExample(&DpPsuInstance, DPPSU_DEVICE_ID);

					FsblStage = XFSBL_STAGE4;
				} else {
					XFsbl_Printf(DEBUG_INFO,"Initialization Success \n\r");

					/**
					 * Start the partition loading from 1
					 * 0th partition will be FSBL
					 */
					PartitionNum = 0x1U;

					FsblStage = XFSBL_STAGE3;
				}
#ifdef XFSBL_PERF
				XFsbl_MeasurePerfTime(tCur);
				XFsbl_Printf(DEBUG_PRINT_ALWAYS, " : Boot Dev. Init. Time\n\r");
#endif
			} break;

		case XFSBL_STAGE3:
			{

				XFsbl_Printf(DEBUG_INFO,
					"======= In Stage 3, Partition No:%d ======= \n\r",
					PartitionNum);
				/**
				 * Load the partitions
				 *  image header
				 *  partition header
				 *  partition parameters
				 */
				FsblStatus = XFsbl_PartitionLoad(&FsblInstance,
								  PartitionNum);
				if (XFSBL_SUCCESS != FsblStatus)
				{
					/**
					 * Error
					 */
					XFsbl_Printf(DEBUG_GENERAL,"Partition %d Load Failed, 0x%0lx\n\r",
							PartitionNum, FsblStatus);
					FsblStatus += XFSBL_ERROR_STAGE_3;
					FsblStage = XFSBL_STAGE_ERR;
				} else {
					XFsbl_Printf(DEBUG_INFO,"Partition %d Load Success \n\r",
									PartitionNum);
					/**
					 * Check loading all partitions is completed
					 */

					FsblStatus = XFsbl_CheckEarlyHandoff(&FsblInstance, PartitionNum);

					if (PartitionNum <
						(FsblInstance.ImageHeader.ImageHeaderTable.NoOfPartitions-1U))
					{
						if (TRUE == FsblStatus) {
							EarlyHandoff = TRUE;
							FsblStage = XFSBL_STAGE4;
						}
						else {
							/**
							 * No need to change the Fsbl Stage
							 * Load the next partition
							 */
							PartitionNum++;
						}
					} else {
						/**
						 * No more partitions present, go to handoff stage
						 */
						XFsbl_Printf(DEBUG_INFO,"All Partitions Loaded \n\r");

						//XFsbl_Printf(DEBUG_PRINT_ALWAYS,"Running DP example\r\n");
						//DpPsu_PollExample(&DpPsuInstance, DPPSU_DEVICE_ID);
						//DpPsu_IntrExample();
						//DpdmaVideoExample();

#ifdef XFSBL_PERF
						XFsbl_MeasurePerfTime(FsblInstance.PerfTime.tFsblStart);
						XFsbl_Printf(DEBUG_PRINT_ALWAYS, ": Total Time \n\r");
						XFsbl_Printf(DEBUG_PRINT_ALWAYS, "Note: Total execution time includes print times \n\r");
#endif
						FsblStage = XFSBL_STAGE4;
						EarlyHandoff = FsblStatus;

					}
				} /* End of else loop for Load Success */

			} break;

		case XFSBL_STAGE4:
			{

				XFsbl_Printf(DEBUG_INFO,
						"================= In Stage 4 ============ \n\r");

				/**
				 * Handoff to the applications
				 * Handoff address
				 * xip
				 * ps7 post config
				 */
				FsblStatus = XFsbl_Handoff(&FsblInstance, PartitionNum, EarlyHandoff);

				if (XFSBL_STATUS_CONTINUE_PARTITION_LOAD == FsblStatus) {
					XFsbl_Printf(DEBUG_INFO,"Early handoff to a application complete \n\r");
					XFsbl_Printf(DEBUG_INFO,"Continuing to load remaining partitions \n\r");

					PartitionNum++;
					FsblStage = XFSBL_STAGE3;
				}
				else if (XFSBL_STATUS_CONTINUE_OTHER_HANDOFF == FsblStatus) {
					XFsbl_Printf(DEBUG_INFO,"Early handoff to a application complete \n\r");
					XFsbl_Printf(DEBUG_INFO,"Continuing handoff to other applications, if present \n\r");
					EarlyHandoff = FALSE;
				}
				else if (XFSBL_SUCCESS != FsblStatus) {
					/**
					 * Error
					 */
					XFsbl_Printf(DEBUG_GENERAL,"Handoff Failed 0x%0lx\n\r", FsblStatus);
					FsblStatus += XFSBL_ERROR_STAGE_4;
					FsblStage = XFSBL_STAGE_ERR;
				} else {
					/**
					 * we should never be here
					 */
					FsblStage = XFSBL_STAGE_DEFAULT;
				}

			} break;

		case XFSBL_STAGE_ERR:
			{
				XFsbl_Printf(DEBUG_INFO,
						"================= In Stage Err ============ \n\r");

				XFsbl_ErrorLockDown(FsblStatus);
				/**
				 * we should never be here
				 */
				FsblStage = XFSBL_STAGE_DEFAULT;
			}break;

		case XFSBL_STAGE_DEFAULT:
		default:
			{
				/**
				 * we should never be here
				 */
				XFsbl_Printf(DEBUG_GENERAL,"In default stage: "
						"We should never be here \n\r");

				/**
				 * Exit FSBL
				 */
				XFsbl_HandoffExit(0U, XFSBL_NO_HANDOFFEXIT);


			}break;

		} /* End of switch(FsblStage) */
		if(FsblStage==XFSBL_STAGE_DEFAULT) {
			break;
		}
	} /* End of while(1)  */

	/**
	 * We should never be here
	 */
	XFsbl_Printf(DEBUG_GENERAL,"In default stage: "
				"We should never be here \n\r");
	/**
	 * Exit FSBL
	 */
	XFsbl_HandoffExit(0U, XFSBL_NO_HANDOFFEXIT);

	return 0;
}

void XFsbl_PrintFsblBanner(void )
{
	s32 PlatInfo;
	/**
	 * Print the FSBL Banner
	 */
#if !defined(XFSBL_PERF) || defined(FSBL_DEBUG) || defined(FSBL_DEBUG_INFO) \
			|| defined(FSBL_DEBUG_DETAILED)
	XFsbl_Printf(DEBUG_INFO,
                 "Xilinx Zynq MP First Stage Boot Loader \n\r");
	XFsbl_Printf(DEBUG_INFO,
                 "Release %d.%d   %s  -  %s\r\n",
                 SDK_RELEASE_YEAR, SDK_RELEASE_QUARTER,__DATE__,__TIME__);

	if(FsblInstance.ResetReason == XFSBL_PS_ONLY_RESET) {
		XFsbl_Printf(DEBUG_GENERAL,"Reset Mode	:	PS Only Reset\r\n");
	}
	else if(FsblInstance.ResetReason == XFSBL_APU_ONLY_RESET)
	{
		XFsbl_Printf(DEBUG_GENERAL,"Reset Mode	:	APU Only Reset\r\n");
	}
	else if(FsblInstance.ResetReason == XFSBL_SYSTEM_RESET)
	{
		XFsbl_Printf(DEBUG_GENERAL,"Reset Mode	:	System Reset\r\n");
	}
	else
	{
		/*MISRAC compliance*/
	}
#endif

	/**
	 * Print the platform
	 */

	PlatInfo = (s32)XGet_Zynq_UltraMp_Platform_info();
	if (PlatInfo == XPLAT_ZYNQ_ULTRA_MPQEMU)
	{
		XFsbl_Printf(DEBUG_GENERAL, "Platform: QEMU, ");
	} else  if (PlatInfo == XPLAT_ZYNQ_ULTRA_MP)
	{
		XFsbl_Printf(DEBUG_GENERAL, "Platform: REMUS, ");
	} else  if (PlatInfo == XPLAT_ZYNQ_ULTRA_MP_SILICON)
	{
		XFsbl_Printf(DEBUG_GENERAL, "Platform: Silicon (%d.0), ",
				XGetPSVersion_Info()+1U);
	} else {
		XFsbl_Printf(DEBUG_GENERAL, "Platform Not identified \r\n");
	}

	return ;
}


/*****************************************************************************/
/**
 * This function is called in FSBL error cases. Error status
 * register is updated and fallback is applied
 *
 * @param ErrorStatus is the error code which is written to the
 * 		  error status register
 *
 * @return none
 *
 * @note Fallback is applied only for fallback supported bootmodes
 *****************************************************************************/
void XFsbl_ErrorLockDown(u32 ErrorStatus)
{
	u32 BootMode;

	/**
	 * Print the FSBL error
	 */
	XFsbl_Printf(DEBUG_GENERAL,"Fsbl Error Status: 0x%08lx\r\n",
		ErrorStatus);

	/**
	 * Update the error status register
	 * and Fsbl instance structure
	 */
	XFsbl_Out32(XFSBL_ERROR_STATUS_REGISTER_OFFSET, ErrorStatus);
	FsblInstance.ErrorCode = ErrorStatus;

	/**
	 * Read Boot Mode register
	 */
	//Allow software override of the 9z2 boot mode to avoid having to
	//solder the boot mode select resistor
#ifdef XFSBL_OVERRIDE_BOOT_MODE
	BootMode = XFSBL_OVERRIDE_BOOT_MODE;
#else
	BootMode = XFsbl_In32(CRL_APB_BOOT_MODE_USER) &
			CRL_APB_BOOT_MODE_USER_BOOT_MODE_MASK;
#endif

	/**
	 * Fallback if bootmode supports
	 */
	if ( (BootMode == XFSBL_QSPI24_BOOT_MODE) ||
			(BootMode == XFSBL_QSPI32_BOOT_MODE) ||
			(BootMode == XFSBL_NAND_BOOT_MODE) ||
			(BootMode == XFSBL_SD0_BOOT_MODE) ||
			(BootMode == XFSBL_EMMC_BOOT_MODE) ||
			(BootMode == XFSBL_SD1_BOOT_MODE) ||
			(BootMode == XFSBL_SD1_LS_BOOT_MODE) )
	{
		XFsbl_FallBack();
	} else {
		/**
		 * Be in while loop if fallback is not supported
		 */
		XFsbl_Printf(DEBUG_GENERAL,"Fallback not supported \n\r");

		/**
		 * Exit FSBL
		 */
		XFsbl_HandoffExit(0U, XFSBL_NO_HANDOFFEXIT);
	}

	/**
	 * Should never be here
	 */
	return ;
}

/*****************************************************************************/
/**
 * In Fallback,  soft reset is applied to the system after incrementing
 * the multiboot register. A hook is provided to before the fallback so
 * that users can write their own code before soft reset
 *
 * @param none
 *
 * @return none
 *
 * @note We will not return from this function as it does soft reset
 *****************************************************************************/
static void XFsbl_FallBack(void)
{
	u32 RegValue;

#ifdef XFSBL_WDT_PRESENT
	/* Stop WDT as we are restarting */
	XFsbl_StopWdt();
#endif

	/* Hook before FSBL Fallback */
	(void)XFsbl_HookBeforeFallback();

	/* Read the Multiboot register */
	RegValue = XFsbl_In32(CSU_CSU_MULTI_BOOT);

	XFsbl_Printf(DEBUG_GENERAL,"Performing FSBL FallBack\n\r");

	XFsbl_UpdateMultiBoot(RegValue+1U);

	return;
}


/*****************************************************************************/
/**
 * This is the function which actually updates the multiboot register and
 * does the soft reset. This function is called in fallback case and
 * in the cases where user would like to jump to a different image,
 * corresponding to the multiboot value being passed to this function.
 * The latter case is a generic one and need arise because of error scenario.
 *
 * @param MultiBootValue is the new value for the multiboot register
 *
 * @return none
 *
 * @note We will not return from this function as it does soft reset
 *****************************************************************************/

static void XFsbl_UpdateMultiBoot(u32 MultiBootValue)
{
	u32 RegValue;

	XFsbl_Out32(CSU_CSU_MULTI_BOOT, MultiBootValue);

	/**
	 * Due to a bug in 1.0 Silicon, PS hangs after System Reset if RPLL is used.
	 * Hence, just for 1.0 Silicon, bypass the RPLL clock before giving
	 * System Reset.
	 */
	if (XGetPSVersion_Info() == (u32)XPS_VERSION_1)
	{
		RegValue = XFsbl_In32(CRL_APB_RPLL_CTRL) |
				CRL_APB_RPLL_CTRL_BYPASS_MASK;
		XFsbl_Out32(CRL_APB_RPLL_CTRL, RegValue);
	}

	/* make sure every thing completes */
	dsb();
	isb();

	if(FsblInstance.ResetReason != XFSBL_APU_ONLY_RESET) {

	/* Soft reset the system */
	XFsbl_Printf(DEBUG_GENERAL,"Performing System Soft Reset\n\r");
	RegValue = XFsbl_In32(CRL_APB_RESET_CTRL);
	XFsbl_Out32(CRL_APB_RESET_CTRL,
			RegValue|CRL_APB_RESET_CTRL_SOFT_RESET_MASK);

	/* wait here until reset happens */
	while(1) {
	;
	}
	}
	else
	{
		for(;;){
			/*We should not be here*/
		}
	}

	return;

}

/*****************************************************************************/
/**
 * This function measures the total time taken between two points for FSBL
 * performance measurement.
 *
 * @param Current/start time
 *
 * @return none
 *
 * @note none
 *****************************************************************************/
#ifdef XFSBL_PERF

void XFsbl_MeasurePerfTime(XTime tCur)
{
	XTime tEnd = 0;
	XTime tDiff = 0;
	u64 tPerfNs;
	u64 tPerfMs = 0;
	u64 tPerfMsFrac = 0;

	XTime_GetTime(&tEnd);
	tDiff = tEnd - tCur;

	/* Convert tPerf into nanoseconds */
	tPerfNs = ((double)tDiff / (double)COUNTS_PER_SECOND) * 1e9;

	tPerfMs = tPerfNs / 1e6;
	tPerfMsFrac = tPerfNs % (u64)1e6;

	/* Print the whole (in ms.) and fractional part */
	XFsbl_Printf(DEBUG_PRINT_ALWAYS, "%d.%06d ms.",
			(u32)tPerfMs, (u32)tPerfMsFrac);
}

#endif



/******************************************************************************/
/**
 *
 * This function enables the SGMII to Copper mode in the RTM 88E1512 phys
 *
 * @param
 *      - nGEMAddr: Address of the GEM module
 *
 * @return
 *		- XST_SUCCESS if the initialization is successful
 *		- XST_FAILURE if the  initialization is NOT successful
 *
 * @note		None.
 */
u32 EthernetPhyEnable(u32 nGEMAddr)
{
	XEmacPs_Config* cConfig = XEmacPs_LookupConfig(0);
	XEmacPs InstancePtr;

	XFsbl_Printf(DEBUG_GENERAL, "Setting ethernet SGMII Amplitude...\r\n");
	if( XEmacPs_CfgInitialize(&InstancePtr, cConfig, nGEMAddr) != XST_SUCCESS)
	{
		xil_printf("Error initialising the GEM driver.\r\n");
		return XST_FAILURE;
	}

	XEmacPs_SetMdioDivisor(&InstancePtr, 7);

	//Phy is at MDIO address 1
	u32 nPhyAddress = 0x1;
	const u8 nPageAddr = 0x16;
	const u8 nFiberControlAddr = 0x1a;


    /*************************************************************************
     * Increase SGMII ampitude of both PHYs                                  *
     *************************************************************************/
	if( XEmacPs_PhyWrite(&InstancePtr, 0, 22, 1) != XST_SUCCESS)
	{
		xil_printf("Error setting Ethernet phy mode.\r\n");
		return XST_FAILURE;
	}
	//Set phy page register to 1.
	if( XEmacPs_PhyWrite(&InstancePtr, 0, 26, 16455) != XST_SUCCESS)
	{
		xil_printf("Error setting Ethernet phy mode.\r\n");
		return XST_FAILURE;
	}
	if( XEmacPs_PhyWrite(&InstancePtr, 1, 22, 1) != XST_SUCCESS)
	{
		xil_printf("Error setting Ethernet phy mode.\r\n");
		return XST_FAILURE;
	}
	//Set phy page register to 1.
	if( XEmacPs_PhyWrite(&InstancePtr, 1, 26, 16455) != XST_SUCCESS)
	{
		xil_printf("Error setting Ethernet phy mode.\r\n");
		return XST_FAILURE;
	}

    /*************************************************************************
     * Set PHY at address 1 to RGMII to SGMII mode                           *
     *************************************************************************/
	//Set phy page register to 18.
	if( XEmacPs_PhyWrite(&InstancePtr, 1, 22, 18) != XST_SUCCESS)
	{
		xil_printf("Error setting Ethernet phy mode.\r\n");
		return XST_FAILURE;
	}
	//Force RGMII to SGMII mode and reset the PHY state machines.
	if( XEmacPs_PhyWrite(&InstancePtr, 1, 20, 33284) != XST_SUCCESS)
	{
		xil_printf("Error setting Ethernet phy mode.\r\n");
		return XST_FAILURE;
	}
	//Set phy page register to 1.
	if( XEmacPs_PhyWrite(&InstancePtr, 1, 22, 1) != XST_SUCCESS)
	{
		xil_printf("Error setting Ethernet phy mode.\r\n");
		return XST_FAILURE;
	}
	//Disable auto-negotiation and reset state machine.
	if( XEmacPs_PhyWrite(&InstancePtr, 1, 0, 37184) != XST_SUCCESS)
	{
		xil_printf("Error setting Ethernet phy mode.\r\n");
		return XST_FAILURE;
	}


    /*************************************************************************
     * Set PHY at address 0 to SGMII to Copper mode                          *
     *************************************************************************/
	//Set phy page register to 18.
	if( XEmacPs_PhyWrite(&InstancePtr, 0, 22, 18) != XST_SUCCESS)
	{
		xil_printf("Error setting Ethernet phy mode.\r\n");
		return XST_FAILURE;
	}
	//Force RGMII to SGMII mode and reset the PHY state machines.
	if( XEmacPs_PhyWrite(&InstancePtr, 0, 20, 33281) != XST_SUCCESS)
	{
		xil_printf("Error setting Ethernet phy mode.\r\n");
		return XST_FAILURE;
	}
	//Set phy page register to 1.
	if( XEmacPs_PhyWrite(&InstancePtr, 0, 22, 1) != XST_SUCCESS)
	{
		xil_printf("Error setting Ethernet phy mode.\r\n");
		return XST_FAILURE;
	}
	//Disable auto-negotiation and reset state machine.
	if( XEmacPs_PhyWrite(&InstancePtr, 0, 0, 37184) != XST_SUCCESS)
	{
		xil_printf("Error setting Ethernet phy mode.\r\n");
		return XST_FAILURE;
	}

    /*************************************************************************
     * Return page register to their original state                          *
     *************************************************************************/
	if( XEmacPs_PhyWrite(&InstancePtr, 1, 22, 0) != XST_SUCCESS)
	{
		xil_printf("Error setting Ethernet phy mode.\r\n");
		return XST_FAILURE;
	}
	if( XEmacPs_PhyWrite(&InstancePtr, 1, 22, 1) != XST_SUCCESS)
	{
		xil_printf("Error setting Ethernet phy mode.\r\n");
		return XST_FAILURE;
	}

	XFsbl_Printf(DEBUG_GENERAL, "Done setting Ethernet phy mode.\r\n");
	return XST_SUCCESS;
}
