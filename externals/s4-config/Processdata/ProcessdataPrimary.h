/**
 ******************************************************************************
 * @file    ProcessdataPrimary.h
 * @author  UM
 * @brief   Process data definitions for Primary power modules
 ******************************************************************************
 **/
#pragma once

/** @defgroup prS_Flags Statusbits Primär-Controller
 * @{
 */
#define prS_ID_Bits_Mask    0x000F //!< Maske CAN-Message-Unter-ID
#define prS_PowerUpDone     0x0010 //!< PowerUp done
#define prS_StartUpDone     0x0020 //!< StartUp done
#define prS_PFC_ON          0x0040 //!< PFC Started
#define prS_SysLock         0x0080 //!< System locked
#define prS_MS_down         0x0100 //!< MainState Shutdown
#define prS_VoltRed         0x0200 //!< Voltage reduction
#define prS_ProcRun         0x0400 //!< Process run
#define prS_ProcLock        0x0800 //!< Process locked
#define prS_HLRRelayOn      0x1000 //!< HLR Relay = On
#define prS_MV_RelayOn      0x2000 //!< MV Relay = On
#define prS_PTC_RelayOn     0x4000 //!< PTC Relay = On
#define prS_VIN_OK          0x8000 //!< Netzinput OK
/**
 * @}
 */

/** @defgroup prC_Flags Controlbits Primär-Controller
 * @{
 */
#define prC_ID_Bits_Mask    0x000F //!< Maske CAN-Message-Unter-ID
#define prC_AutoMode_Enab   0x0010
#define prC_HLR_Relay_Enab  0x0020
#define prC_Softstart_Disab 0x0040
#define prC_MV_Relay_Enab   0x0080
#define prC_PTC_Enab        0x0100
#define prC_Booster_Enab    0x0200
#define prC_Control_Enab    0x0400
#define prC_Debug_Enab      0x0800
#define prC_Error_Reset     0x1000
#define prC_Enumerate       0x2000
#define prC_Primary2        0x4000
#define prC_Primary1        0x8000
/**
 * @}
 */

/**
 * @brief  Indizes Primary Fehlerbits Primär-Controller (prE_Flags)
 */
enum PrimErrNums {
    prE_Internal,           //!< uC Interner Error
    prE_UNetzUnter_StartUp, //!< E22-03 Netzunterspannung Startup
    prE_UNetzUeber_StartUp, //!< E02-00 Netz-Überspannung Startup
    prE_UNetzUnter,         //!< E22-00 Netzunterspannung, Phasenfehler
    prE_UNetzUeber,         //!< E02-00 Netz-Überspannung
    prE_UZK_StartUp,        //!< E02-02 Überspannung - ZK Startup
    prE_UZK_Unter,          //!< E22-04 Unterspannung - Zwischenkreis
    prE_UZK_Ueber,          //!< E02-01 Überspannung - ZK
    prE_UZK_Ueber_INT1,     //!< E0X-0X Überspannung - DC1 Modul
    prE_UZK_Ueber_INT2,     //!< E0X-0X Überspannung - DC2 Modul
    prE_UZK_Ueber_INT3,     //!< E0X-0X Überspannung - Beide Module
    prE_TempSensor,         //!< E13-02 Temperatur-Sensor - primär Modul
    prE_UZK_Ueber_EXT1,     //!< E0X-0X OV DC1 Comparator
    prE_UZK_Ueber_EXT2,     //!< E0X-0X OV DC2 Comparator
    prE_Comm,               //!< E31-00 Kommunikation  Fehler
    prE_CanTimeOutMaPro,    //!< E31-01 Kommunikation  Master Timeout
    prE_Temp_Ueber_ZK1,     //!< E01-02 Übertemperatur - DC1 Modul
    prE_Temp_Ueber_ZK2,     //!< E01-02 Übertemperatur - DC2 Modul
    prE_Temp_Ueber,         //!< E01-02 Übertemperatur - Booster Modul
    prE_USupply,            //!< E14-00 Versorgungsspannung
    prE_USupply_StartUp,    //!< E14-01 Versorgungsspannung Startup
    prE_CurrentSensor,      //!< E15-00 Strom Erfassung
    prE_IL_Dauer,           //!< E16-00 Überstromabschaltung
    prE_IL_Ueber,           //!< E03-00 Überstrom
    prE_Driver1,            //!< Exx-01 Treiber1 Fehler (UVLO)
    prE_Driver2,            //!< Exx-01 Treiber2 Fehler (UVLO)
    prE_HW_Config,          //!< E30-01 Fehler Konfiguration
    prE_SW_Config,          //!< E30-06 Software konfig, Watchdog
    prE_DesatDrv1,          //!< E03-0X Treiber1 Fehler (DESAT)
    prE_DesatDrv2,          //!< E03-0X Treiber2 Fehler (DESAT)
    prE_Debug3,             //!< Error Bit Debug/Reserve
    prE_Debug4,             //!< Error Bit Debug/Reserve
    prE_N                   //!< Anzahl Elemente
};

/**
 * @brief  Indices Primary power modules Actual values
 */
enum PrimStatValNums {
    PSVN_Max_I_ArcSetpoint, //!< Maximum Arc Current Setpoint in A
    PSVN_WeldEfficiency,    //!< Welding Efficiency in %
    PSVN_U_Bus_16V,         //!< Versorgungsspannung 16V in V
    PSVN_Temp_GR,           //!< Temperatur Gleichrichter in °C
    PSVN_I_ZK1,             //!< Zwischenkreis-Strom in A
    PSVN_I_ZK2,             //!< Zwischenkreis-Strom in A
    PSVN_U_ZK1,             //!< Zwischenkreisspannung in V
    PSVN_U_ZK2,             //!< Zwischenkreisspannung in V
    PSVN_Temp_M1,           //!< Modul-Temperaturen in °C
    PSVN_Temp_M2,           //!< Modul-Temperaturen in °C
    PSVN_Temp_M3,           //!< Modul-Temperaturen in °C
    PSVN_U_Mains,           //!< Netzeingangsspannung in V
    PSVN_I_Mains,           //!< Netzeingangs Strom in A
    PSVN_Debug1,            //!< Debug Wert1 0.0 .. 25.5
    PSVN_Rsv14,
    PSVN_Rsv15,
    PSVN_N //!< Anzahl Elemente
};

/**
 * @brief  Indices Primary Setpoints (like sent via CAN-Bus)
 */
enum PrimControlNums {
    PCN_BoosterPWM1_01Proz, //!< Booster PWM 1 0..100 [0,1%]
    PCN_BoosterPWM2_01Proz, //!< Booster PWM 2 0..100 [0,1%]
    PCN_Control_PWM_Max,    //!< Regler Max. PWM 0..100 [1%]
    PCN_Control_ParamNum,   //!< Regler ParamHandler Nummer 0..4 [1]
    PCN_Control_U_KP,       //!< Regler KP 0..10 [0,01]
    PCN_Control_U_KI,       //!< Regler KI 0..10 [0,01]
    PCN_Control_U_OutMax,   //!< Regler I Output Max 0..100 [0,1A]
    PCN_Control_I_KP,       //!< Regler KP 0..10 [0,01]
    PCN_Control_I_KI,       //!< Regler KI 0..10 [0,01]
    PCN_Control_I_OutMax,   //!< Regler I Output Max 0..100 [0,1%]
    PCN_ControlBits,        //!< Controlbits, can be a value of \@ref prC_Flags
    PCN_Rsv_11,
    PCN_N //!< Anzahl Elemente
};

/**
 * @brief  Indices Primary FlexParameter
 */
enum PrimFlexParaNums {
    PFPN_Inaktiv,    //!< FlexParameter Inaktiv/Keine Funktion
    PFPN_VoltOff_ZK, //!< Voltage Offset ZK1 & ZK2 0..1000,0 [0,1V]
    PFPN_UZK_Faktor, //!< UZK Faktor min & max 0..2 [0,01]
    PFPN_CBC_Limit,  //!< CBC Limit HW & SW 0..120 [1A]
    PFPN_Imax_Dbg1,  //!< Dauer Strom max & Debug Value1  0..100 [1A]
    PFPN_Dbg2_Dbg3,  //!< Debug Value2 & Debug Value3 0..65535 [1]
    PFPN_N //!< Anzahl Elemente
};
