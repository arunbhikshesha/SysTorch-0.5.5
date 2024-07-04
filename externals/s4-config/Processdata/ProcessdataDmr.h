/**
 ******************************************************************************
 * @file    ProcessdataDmr.h
 * @author  UM
 * @brief   Process data definitions for wire feeder unit
 ******************************************************************************
 **/
#pragma once

/** @defgroup outC_Flags Control Bits DMR/Feed
 * @{
 */
#define outC_MAVGas          0x0001 // Schutzgasventil aktiver Koffer Ein
#define outC_Rsv1            0x0002 // Reserved
#define outC_DrEinlauf       0x0004 //->auf DMR Funktion wie Drahteinlauftaster
#define outC_Rsv3            0x0008 // Reserved
#define outC_MotRetract      0x0010 // nur zusammen mit outC_DrEinlauf aktiv
#define outC_MsgValid        0x0020 // f. Kompatibilität m. alten Baugruppen
#define outC_MAVGasKoInaktiv 0x0040 // Gasventil inaktiver Koffer
#define outC_DisplaysOff     0x0080 // Displays dunkelschalten
#define outC_MAVWP           0x0100 // Wasserpumpe Ein (CAN-Erweiterungsplatine)
#define outC_MAVAusbl        0x0200 // Ausblasventil
#define outC_Luefter         0x0400 // cooling fans
#define outC_DrRoBremse      0x0800 // wire reel brake
#define outC_Rsv12           0x1000 // Reserved
#define outC_DrRaHeiz        0x2000 // wire feeder interior heating
#define outC_DrRaLicht       0x4000 // wire feeder interior LED-lights
#define outC_Rsv15           0x8000 // Reserved

/**
 * @}
 */

/** @defgroup inC_Flags Status-Bits DMR/Feed
 * @{
 */
// Byte 1
#define inC_CwContact 0x00000001 // Coldwire has contact with workpiece
#define inC_Torch1Err 0x00000002 // BR1 torch error
#define inC_Torch2Err 0x00000004 // BR2 torch error
// Byte 2
#define inC_TorchDown  0x00000100 // Torch rocker down
#define inC_TorchUp    0x00000200 // Torch rocker up
#define inC_TorchRight 0x00000400 // Torch rocker right
#define inC_TorchLeft  0x00000800 // Torch rocker button left
#define inC_TorchBT    0x00001000 // Torch button pressed
#define inC_TorchBT2   0x00002000 // Torch button 2 ('Zweitstromtaste') pressed
// Byte 3 unused
// Byte 4
#define inC_ZaVerr    0x01000000 // 'Zentralanschluss' locked
#define inC_DrESens   0x02000000 // Wire end sensor active
#define inC_DrEinlauf 0x04000000 // Drahteinlauf-Taste gedrückt
#define inC_TachoOn   0x08000000 // Motor-Tacho erkannt
#define inC_Rsv28     0x10000000 // Reserved
#define inC_TaGasTest 0x20000000 // Gastest-Taste gedrückt
#define inC_Kollision 0x40000000 // Eingang Kollisionsschutz aktiv
#define inC_DrRueck   0x80000000 // Drahtrücklauf-Taste gedrückt
/**
 * @}
 */

/** @defgroup sm_Flags Status-Bits DMR-PP
 * @{
 */
#define sm_PP_kalibriert       0x0001 // Aktueller Brenner bereits kalibriert
#define sm_Abgleich_DDia_OK    0x0002 // Fertigmeldung Abgleich Draht-Durchmesser
#define sm_Abgleich_draht_OK   0x0004 // Fertigmeldung Abgleichwerte bestimmen
#define sm_Abgleich_hitri_OK   0x0008 // Fertigmeldung Abgleich Hilfstrieb
#define sm_Kalibrierung_laeuft 0x0010 // Kalibrierung läeuft (n. Ta. Drahteinlauf)
#define sm_Abgleich_DDia_M     0x0020 // Kennung Abgl. Drahtdurchmesser möglich
#define sm_Abgleich_draht_M    0x0040 // Kennung Abgleich mit Draht möglich
#define sm_PPOptions           0x0100 // zusätzliche PP Optionen aktiv
#define sm_PPTiming            0x0200 // PP TP Timing aktiv
#define sm_AnaPPSim            0x0400 // Indikator Brenner Analog-PP-Simulation
/**
 * @}
 */

/** @defgroup wDMR_Flags Warnung-Bits DMR
 * @{
 */
#define wDMR_ZaNotLocked 0x0001 // ZA (Zentralanschluss) not properly locked
#define wDMR_Calibration 0x0002 // Calibration problem
#define wDMR_MaschID     0x0004 // Machine ID unknown
#define wDMR_Slip        0x0008 // Wire speed slippage too high
#define wDMR_Derating    0x0010 // Derating of motor power occurred
#define wDMR_TorchCom    0x0020 // CAN torch communication problem
/**
 * @}
 */

/** @defgroup eDMR_Flags Fehler-Bits DMR
 * @{
 */
#define eDMR_Standby     0x0001 // Wire feed not ready/ collective error, stop welding!
#define eDMR_UeberspMot  0x0002 // Motor overvoltage
#define eDMR_Unterspg    0x0004 // Motor op. voltages
#define eDMR_MotstromDV  0x0008 // Motor 1 overcurrent
#define eDMR_MotstromPP  0x0010 // Motor 2 overcurrent
#define eDMR_TachoDV     0x0020 // Tachometer motor 1
#define eDMR_TachoPP     0x0040 // Tachometer motor 2
#define eDMR_EEPROM      0x0080 // EEProm access/ checksum error
#define eDMR_PeriOutDrv  0x0100 // Any peripheral driver in Overcurrent
#define eDMR_HwDmrConfig 0x0200 // Evaluated DMR-HW-Configuration unknown
#define eDMR_HwErwConfig 0x0400 // Evaluated ERW-HW-Configuration unknown
#define eDMR_TorchConn   0x0800 // Torch connection
#define eDMR_VAError     0x1000 // V/A measuring error (plausibility..)
#define eDMR_Uebertemp   0x2000 // Thermal overload
#define eDMR_Rsv_14      0x4000 // Reserve
#define eDMR_Rsv_15      0x8000 // Reserve
/**
 * @}
 */

/**
 * @brief  Indices Wire feeder Actual values
 */
enum WFeedStatValNums {
    WFSVN_V_Ist,          //!< Drahtgeschwindigkeit in m/min
    WFSVN_I_Ist_Main,     //!< Motorstrom Hauptmotor in A
    WFSVN_I_Ist_PuPu,     //!< Motorstrom Push-Pull-Motor in A
    WFSVN_I_Ist_ZwiTri,   //!< Motorstrom Zwischentrieb-Motor in A
    WFSVN_Ambient_Temp_C, //!< Ambient temperature [°C]
    WFSVN_DV_Mot_Temp_C,  //!< DV motor temperature [°C]
    WFSVN_PP_Mot_Temp_C,  //!< PP motor temperature [°C]
    WFSVN_MCU_Temp_C,     //!< MCU temperature [°C]
    WFSVN_24V_Supply_V,   //!< 24V supply voltage [V]
    WFSVN_10V_Supply_V,   //!< 10V supply voltage [V]
    WFSVN_5V_Supply,      //!< 5V supply voltage [V]
    WFSVN_60V_Supply,     //!< 60V supply voltage [V]
    WFSVN_Rsv_12,
    WFSVN_Rsv_13,
    WFSVN_Rsv_14,
    WFSVN_Rsv_15,
    WFSVN_N //!< Anzahl Elemente
};
