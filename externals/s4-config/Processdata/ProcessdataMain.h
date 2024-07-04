/**
 ******************************************************************************
 * @file    ProcessdataMain.h
 * @author  UM
 * @brief   Process data definitions for S4_Main App
 ******************************************************************************
 **/
#pragma once

/**
 * @brief  Indices S4_Main errors
 */
enum ErrInternNums {
    E_M4_Comm,            //!< Communication with M4 core missing or disrupted
    E_Prim1_TimeOut,      //!< CAN messages from primary 1 timed out
    E_Prim2_TimeOut,      //!< CAN messages from primary 2 timed out
    E_Prim1_OverTemp,     //!< Overtemperature primary module 1 (4 meas. points)
    E_Prim2_OverTemp,     //!< Overtemperature primary module 2 (4 meas. points)
    E_WldProg_File,       //!< Error reading welding program files
    E_WldProg_Copy,       //!< Error copying welding program files to M4 core
    E_SharedMemGen,       //!< Problem with Shared Memory between S4_Main and Backend
    E_SharedMemJob,       //!< Shared Memory Problem in Job module
    E_M4_VersionMismatch, //!< Firmware version mismatch between S4_Main and S4_M4 (unrecoverable error)
    E_M4_Sum,             //!< error in S4_M4/ Process module (Group error, see module error bits)
    E_Prim1_Sum,          //!< error in Primary #1 module (Group error, see module error bits)
    E_Prim2_Sum,          //!< error in Primary #2 module (Group error, see module error bits)
    E_DMR_TimeOut,        //!< CAN messages from Acitve Wire feeder timed out
    E_DmrCanBusInit,      //!< DMR CAN bus init failure (unrecoverable error)
    E_PrimaryCanBusInit,  //!< Primary CAN bus init failure (unrecoverable error)
    E_OverTemp_AC01M1,    //!< Overtemperature AC01, Sensor 1 (from M4 core)
    E_OverTemp_AC01M2,    //!< Overtemperature AC01, Sensor 2 (from M4 core)
    E_WaterCooling,       //!< Torch water cooling, flow rate to low
    E_CalibAdj,           //!< error during Calibration/ Adjustment procedure
    E_BackendGrpc,        //!< Could not reach Backend gRPC service (unrecoverable error)
    E_ReadInitFile,       //!< error reading necessary Init file (unrecoverable error)
    E_Prim_TempAsymm,     //!< Primary module: Temperatures M1/M2 differ too much
    E_ConfigMismatch,     //!< Data from ConfigStore not plausible (unrecoverable error)
    E_N                   //!< number of enum elements
};

/**
 * @brief  Indices S4_Main warnings
 */
enum WarnInternNums {
    WARN_Remote_InitFail,    //!< Remote control module - Initialization failed! (unrecoverable warning)
    WARN_VCC_ExtCanNotOk,    //!< Supply voltage External CAN bus missing (24V X124)
    WARN_PoE_InitFail,       //!< Init I2C Communication to Poe chip failed, check X125 (unrecoverable warning)
    WARN_CalibrationSuccess, //!< Calibration completed successfully
    WARN_AdjustmentSuccess,  //!< Adjustment completed successfully
    WARN_ModbusTcp,          //!< Modbus-TCP Initialization problem
    WARN_FanSpeed,           //!< Speed of one of the fans to low
    WARN_TempSensor,         //!< Temperature sensor AC01/M4 not connected
    WARN_N
};

// Indices in S4_Main error counter array
enum ErrCounterNums {
    ErrC_M4RpcCall,   //!< Remote Procedure Call to M4: error in response
    ErrC_gRPC_Server, //!< gRPC server message errors
    ErrC_gRPC_Client, //!< gRPC client message errors
    ErrC_ShM_Gen,     //!< Access error in Shared Memory
    ErrC_ShM_Job,     //!< Access error in Job Shared Memory
    ErrC_Can0,        //!< Error Internal CAN bus (Bus-Off)
    ErrC_Can1,        //!< Error External CAN bus (Bus-Off)
    ErrC_ConfigStore, //!< Error accessing Config-Store gRPC server
    ErrC_ModbusTcp,   //!< Modbus-TCP errors
    ErrC_Resv9,
    ErrC_Resv10,
    ErrC_Resv11,
    ErrC_Resv12,
    ErrC_Resv13,
    ErrC_Resv14,
    ErrC_Resv15,
    ErrC_N //!< number of enum elements
};

/** @defgroup PI_Flags Process In Flags
 * @{
 */
#define PI_DDS       0x0001     // Kühlwasser Druck/Durchfluss Sensor
#define PI_DDSOff    0x0002     // Kühlwasser Sensor deaktiviert
#define PI_GasTest   0x0008     // Gastest-Taste gedrückt
#define PI_K_2T      0x0010     // Kennung Zwangs-2Takt von Fernreglerbuchse
#define PI_K_LSP     0x0040     // Kennung Leitspannung " "
#define PI_BT1       0x1000     // Brennertaste Koffer 1
#define PI_BT2       0x2000     // Brennertaste Koffer 2
#define PI_K_WIG     0x4000     // WIG-Kennung in ser. Brenner?
#define PI_BA_RC     0x8000     // Kennung Fernsteuer-Betrieb
#define PI_BT        0x00010000 // resultierende Brennertaste ->Start!
#define PI_DrEinlauf 0x00020000 // resultierende Drahteinlauftaste
#define PI_DrRueck   0x00040000 // resultierende Draht-Rück-Taste
/**
 * @}
 */

/** @defgroup PO_Flags Process Out Flags, also for Legacy CAN ID 0x190
 * @{
 */
#define PO_Gas         0x0001     // Gas Ein
#define PO_LUE         0x0002     // Lüfter Ein
#define PO_WP          0x0004     // Wasserpumpe Ein
#define PO_StickCheck  0x0008     // Befehl WireStick-Check durchführen
#define PO_Slope       0x0040     // Slope Ein ->Interpolation
#define PO_IZweit      0x0080     // Zweitstrom verwenden
#define PO_LTOn        0x0100     // Primär-Leistungsteil Ein
#define PO_WIG         0x0400     // Neu: Wig-Modus
#define PO_IHaupt      0x1000     // gesetzt während Takt 2 (Hauptstromzeit)
#define PO_Elektr      0x2000     // gesetzt bei BA Elektrode
#define PO_TestMode    0x8000     //->Testbetrieb am Widerstand ermöglichen
#define PO_Hold        0x00010000 // Actual Values display state "Hold"
#define PO_PE_Ok       0x00020000 // Protective earth monitoring ok
#define PO_GasOk1      0x00040000 // Gas pressure switch 1 ok
#define PO_GasOk2      0x00080000 // Gas pressure switch 2 ok
#define PO_WF2Active   0x00100000 // Wire feeder #2 active
#define PO_GasFlowOk1  0x00200000 // Gas flow metering 1 ok
#define PO_GasFlowOk2  0x00400000 // Gas flow metering 1 ok
#define PO_ProcessAct  0x00800000 // Process Active (including Gas Pre and Post flow)
#define PO_ProcessAct2 0x01000000 // Process Active 2 (from Arc ignition to Gas Post flow)
/**
 * @}
 */
