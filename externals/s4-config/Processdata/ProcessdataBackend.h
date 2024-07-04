/**
 ******************************************************************************
 * @file    ProcessdataBackend.h
 * @author  SGE
 * @brief   Process data definitions for Backend Module
 ******************************************************************************
 **/
#pragma once

/** @defgroup bstat_Flags Backend Status Bits
 * @{
 */
#define bstat_WeldEnable     	0x0001 // a new welding process only starts if weld_enable true
#define bstat_WeldStop       	0x0002 // causes the active welding process to stop
#define bstat_UpdateActive   	0x0010 // indicates a running Update
#define bstat_UpdatePending  	0x0020 // indicates a Update is requested
#define bstat_UpdateAvailable	0x0040 // indicates a Update is Available
#define bstat_PowerFail 	 	0x0100 // indicates a Powerfail is Triggerd
/**
 * @}
 */
