
// !!!! This file was generated automatically on 17.03.2023 17:09:04 Do not edit manually !!!!

/*
 * SoftwareEnums.h
 */

#ifndef SOFTWARE_ENUMS
#define SOFTWARE_ENUMS

// Defines of the target IDs for CAN devices

// CAN Software ID enum
enum CANSoftwareID
{
	CAN_SW_ID_NONE = 0,		// undefined Software
	CAN_SW_ID_DRV08_BL = 1,		// Bootloader of DRV08
	CAN_SW_ID_Primary_FW = 2,		// Primary Firmware
	CAN_SW_ID_DMR06_BL = 3,		// Bootloader of DMR06
	CAN_SW_ID_DMR_FW = 4,		// Digital Motor Regulator Firmware
	CAN_SW_ID_WPK01_BL = 5,		// Bootloader of WPK01
	CAN_SW_ID_WUK_FW = 6,		// Water Cooler Firmware
	CAN_SW_ID_TC22_BL = 7,		// Bootloader of TC22 
	CAN_SW_ID_SystemTorch_FW = 8,		// SystemTorch Firmware
	CAN_SW_ID_WPK02_BL = 9,		// Bootloader of WPK02
	CAN_SW_ID_WUK02_FW = 10		// WPKFujitsuSTM Firmware
};

// The number of CAN Software IDs in CAN enum CANSoftwareID
#define CAN_SW_ID_N	11

// Defines of the target IDs for ALL devices

// Software ID enum
enum SoftwareID
{
	SW_ID_NONE = 0,			// undefined Software
	SW_ID_DRV08_BL = 1,			// Bootloader of DRV08
	SW_ID_Primary_FW = 2,			// Primary Firmware
	SW_ID_DMR06_BL = 3,			// Bootloader of DMR06
	SW_ID_DMR_FW = 4,			// Digital Motor Regulator Firmware
	SW_ID_WPK01_BL = 5,			// Bootloader of WPK01
	SW_ID_WUK_FW = 6,			// Water Cooler Firmware
	SW_ID_TC22_BL = 7,			// Bootloader of TC22 
	SW_ID_SystemTorch_FW = 8,			// SystemTorch Firmware
	SW_ID_WPK02_BL = 9,			// Bootloader of WPK02
	SW_ID_WUK02_FW = 10,			// WPKFujitsuSTM Firmware
	SW_ID_BF36_BL = 300,			// Bootloader of BF36
	SW_ID_HMI_Modul_FW = 301,			// HMI Firmware
	SW_ID_HMI_Modul_BKUP_FW = 302,			// HMI Backup Firmware Ressource independent
	SW_ID_TouchFirmwareHMI1_FW = 305,			// TouchFirmwareHMI1
	SW_ID_TouchFirmwareHMI2_FW = 306,			// TouchFirmwareHMI2
	SW_ID_Control_FW = 320,			// M4 Firmware
	SW_ID_FPGA_iQs_Mapro_FW = 400,			// FPGA iQs Mapro
	SW_ID_HMI_Modul_Resources_DA = 1000,			// HMI General Ressources
	SW_ID_HMI_Modul_Bitmaps_DA = 1001,			// HMI Bitmaps no used Reserved SGE 11.04.2022
	SW_ID_HMI_Modul_Strings_DA = 1002,			// HMI Strings no used Reserved SGE 11.04.2022
	SW_ID__KY = 2000,			// 
	SW_ID_DRV08_KY = 2001,			// Key of DRV08
	SW_ID_DMR06_KY = 2002,			// Key of DMR06
	SW_ID_WPK01_KY = 2003,			// Key of WPK01
	SW_ID_TC22_KY = 2004,			// Key of TC22
	SW_ID_WPK02_KY = 2005,			// Key of WPK02
	SW_ID_BF36_KY = 2300			// Key of BF36
};

// The number of  Software IDs in enum SoftwareID
#define SW_ID_N	28

#endif

