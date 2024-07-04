/**
 ******************************************************************************
 * @file    ProcessdataWpk.h
 * @author  MKE
 * @brief   Process data definitions for water cooler unit (WPK - "Wasserpumpenkontrolleinheit")
 ******************************************************************************
 **/
#pragma once

/**
 * @brief  Indices WPK errors
 */
enum WpkErrNums {
    E_WPK_Leak,      //!< Leakage error
    E_WPK_Pump,      //!< Pump error
    E_WPK_MVOLT,     //!< multioltage relay error
    E_WPK_Temp,      //!< Temperature sensor error
    E_WPK_Fan1,      //!< Fan 1 error
    E_WPK_Fan2,      //!< Fan 2 error
    E_WPK_Valve1,    //!< Valve 1 error
    E_WPK_Valve2,    //!< Valve 2 error
    E_WPK_FillSens,  //!< Level/Tank-sensor error
    E_WPK_FlowSens,  //!< Flow-sensor error
    E_WPK_Sup24V,    //!< 24V supply error (over- or undervoltage)
    E_WPK_Sup24VExt, //!< 24V_Ext supply error (over- or undervoltage)
    E_WPK_Sup5V,     //!< 5V error (over- or undervoltage)
    E_WPK_OptSwOn,   //!< Optional Switch Error
    E_WPK_DDSL_ON,   //!< Flow_Check_Error
    E_WPK_M15Rsv,    //!< Reserved
    E_WPK_N          //!< number of enum elements

};

/** @defgroup sWPK_Flags Status-Bits WPK
 * @{
 */
#define sWPK_FillS1     0x0001 // Level sensor switch 1 closed
#define sWPK_FillS2     0x0002 // Level sensor switch 2 closed
#define sWPK_FillS3     0x0004 // Level sensor switch 3 closed
#define sWPK_DDS_ON     0x0008 // Coolant flow sensor active
#define sWPK_FILL_ON    0x0010 // Coolant level sensor active
#define sWPK_Rsv5       0x0020 // Reserved
#define sWPK_Rsv6       0x0040 // Reserved
#define sWPK_Rsv7       0x0080 // Reserved
#define sWPK_Fan1On     0x0100 // Fan 1 on
#define sWPK_Fan2On     0x0200 // Fan 2 on
#define sWPK_Valve1On   0x0400 // Valve 1 on
#define sWPK_Valve2On   0x0800 // Valve 2 on
#define sWPK_PumpOn     0x1000 // Water pump on
#define sWPK_MultiVltOn 0x2000 // Multi-voltage relay on
#define sWPK_OptSwOn    0x4000 // Optional switch on
#define sWPK_Rsv15      0x8000 // Reserved
/**
 * @}
 */

/**
 * @brief  Indices WPKs actual values (Msg. 0x312 Bytes 4..6)
 */
enum WPKStatValNums {
    WPKSVN_CoolantTemp, //!< Coolant temperature [°C]
    WPKSVN_CoolantFlow, //!< Coolant flow [l/min]
    WPKSVN_CoolantFill, //!< Coolant fill level [%]
    WPKSVN_Rsv3,        //!< Reserved
    WPKSVN_Rsv4,        //!< Reserved
    WPKSVN_Rsv5,        //!< Reserved
    WPKSVN_Rsv6,        //!< Reserved
    WPKSVN_Rsv7,        //!< Reserved
    WPKSVN_N            //!< Number of elements
};
