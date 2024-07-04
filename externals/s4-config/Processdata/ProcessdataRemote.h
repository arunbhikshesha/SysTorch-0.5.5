/**
 ******************************************************************************
 * @file    ProcessdataRemote.h
 * @author  UM
 * @brief   Process data definitions for (CAN bus) remote control/ robot interface
 ******************************************************************************
 **/
#pragma once

/** @defgroup rob_Flags Control bits from Robot interface
 * @{
 */
#define rob_RobotReady     0x0001
#define rob_WeldStart      0x0002 // Welding Start
#define rob_WireFeed2      0x0004 // Wire feeder 2 active
#define rob_StickCheck     0x0008 // Stick-Check request signal
#define rob_WireRetract    0x0010 // Wire retract signal
#define rob_GasTest        0x0020 // Gas-test request signal
#define rob_WireInching    0x0040 // Wire inching request signal
#define rob_WeldSimulation 0x0080 // Setup mode
#define rob_TorchBlowOut   0x0100 // Blow-out valve request signal
#define rob_WaterPumpTest  0x0200
#define rob_CraterFill     0x0400
#define rob_SrcErrorReset  0x8000
#define rob_INT02_06       0x00010000 // CAN bus Robot interface unit INT02/INT06
#define rob_ModbusTcp      0x00020000
#define rob_Anybus         0x00040000
/**
 * @}
 */

/***** Konstanten Roboter-Interface *****/
#define roDI_ProgJobNr         0x00FF
#define roDI_BartSlope         0x1000
#define roDI_ProgJobNrHighBits 0x6000

/***** "DIP"-Schalter/Merker Umschaltung auf externe Steuerung *****/
#define roDIP_ProgWahlExt   0x0100
#define roDIP_BartExt       0x0200
#define roDIP_SetExtern     0x0400
#define roDIP_JobPlusAnalog 0x0800
#define roDIP_WireSpdPreset 0x1000
