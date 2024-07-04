# Changelog

## [0.26.19] - 2023-10-06
### Added
- Adjustment factors in ipc_process.h for optimizes U/I-LLGen PWM calculation & improved Gain/Offset adjustment.

## [0.26.18] - 2023-10-05
### Added
* Warning entries for the LicenseManager (Warnings on changes in Database, weldprogs and LicenseFile, and the selection of an unlicesed weldprog)
### Changed
* Mapro_R.db to 1.0.9 (add mass_per_length for every material in the database)

## [0.26.17] - 2023-10-04
### Added
* gRPC: debug info collector
### Changed
* gRPC: fix case warning message in .proto

## [0.26.16] - 2023-09-25
### Added
* DMR H80-10 (wDMR_TorchCom): CAN torch communication problem

## [0.26.15] - 2023-09-07
### Added
* MLimits Concept & MLimits config Data (deploy)

## [0.26.14] - 2023-09-05
### Changed
* Weldprogs to 0.2.5 ProgV-0-2-5_2023-09-05_15-36-27

## [0.26.13] - 2023-09-05
### Changed
* Weldprogs to 0.2.5 ProgV-0-2-5_2023-09-04_13-37-10
## [0.26.12] - 2023-09-01
### Changed
* Weldprogs to 0.2.5 01.09.2023
* Torchconfig updated

## [0.26.11] - 2023-08-28
### Changed
* DMRs H80 from category 3 "Meldung" to 1 "LogEntry"
* update mapro_R.db to 1.0.8 (26.08.2023)
* update Weldprogs to  V.0.2.5 (2023-08-28)
* update torchconfig.json


## [0.26.10] - 2023-08-24
### Added
* update Weldprogs to  V.0.2.5 (2023-08-24)

## [0.26.9] - 2023-08-24
### Changed
* update mapro_R.db to 1.0.8 (24.08.2023 15:42) 

## [0.26.8] - 2023-08-24
### Changed
* update mapro_R.db to 1.0.8 (24.08.2023 15:00) 
* torchconfig 2Stroke removed for SuS (not functional)

## [0.26.8] - 2023-08-22
### Fixed
- Database Version missing in the "tb_infos" in the mapro_R.db

## [0.26.7] - 2023-08-17
### Changed
- Return all limits grouped by their groups in parameter manager
- Return all active limits in parameter manager

## [0.26.6] - 2023-08-16
### Added
- SubNumber for FPGA Errors

## [0.26.5] - 2023-08-16
### Added
- mapro_R.db (Reduced size Database)

## [0.26.4] - 2023-08-14
### Added
- WeldProgs -> 65522, 'Testprogramm HMI'
- WeldProgs -> s4db_verfdat_id_bemerkung.txt (2023_8_9)
- Error Id 108 for Database corruption
### Changed
- WeldProgs -> Optimization in many programs

## [0.26.2] - 2023-08-03
### Added
- WeldProgs -> Const Current 65532
### Changed
- WeldProgs -> Optimization in many programs

## [0.26.1] - 2023-07-28
### Added
- Wire feeder TimeOut error (former E_DMR_Sum)
### Changed
- make ConfigStore keys under /DevFunction/ persistent (Sub keys for now: ShowAllWeldprogs, SuppressMissingDeviceErr)

## [0.26.0] - 2023-07-25
### Added
- FPGA Process data to shared_memory_processdata.h
- WsTrackerService proto file definition for Backend gRPC
### Changed
- relocate ipc_fpga_io.h to Processdata/ProcessdataFpga.h

## [0.25.4] - 2023-07-25
### Added
- for Web-tools: Main_Defs.json + E_WPK_DDSL_ON bit in WPK_Defs.json
- Error 105 -> JParLimits_N overflow
- target präfix to errormessage text
### Changed
- Error 10 11 Text changed
- Limits for dmat vout Parameter increased
- WeldProgs -> Twin-Progs/Balance optimized

## [0.25.3] - 2023-07-18
### Changed
- Err92 reduced
- peripherie\tools\version.h.in U and UL for Versions added
### Added
- Mapro.db to /deploy/wld/ (on MaPro = /usr/share/s4-config/wld)

## [0.25.2] - 2023-07-17
### Changed
- raise J_PAR_LIMITS_MAX on SHM interface M4 - S4_Main from 20 to 40

## [0.25.1] - 2023-07-10
### Added 
- MaskWPK created and error category and reactions defined in SBK_Fehlerliste.xlsm
- JSON Files for S4_DiagApp Text Updates (Value Names & ToolTips/Text)
- Updated ProcessData.WPK updated with new ErrorBits that can occur
- ErrorDef.ewu & ErrorDef.Jason updated
- Extra Permanent Datachannels for DigiWeld (SP1-3_GLV) & Reserve DokuPars

## [0.25.0] - 2023-07-03
### Added 
- Permanent Datachannels for DigiWeld

## [0.24.19] - 2023-06-19
### Added 
- FPGA SWE-Bits & FastSPI-Bits to the FPGA_Register_Definition.xlsm
- JSON Files for S4_DiagApp Text Updates (Value Names & ToolTips/Text)
### Changed
- Macro in FPGA_Register_Definition.xlsm to create JSON/C output for S4_DiagApp/S4_main & S4_M4 Projects

## [0.24.18] - 2023-06-12
### Changed
- WeldProgs -> Twin-Progs/Parameters optimized

## [0.24.17] - 2023-06-06
### Added
- Wlan interface for grpc

## [0.24.16] - 2023-05-17
### Added 
- Error ID92 MaPro Tempsensor not Ok
### Changed
- SBK_Fehlerliste.xlsm CAT and REACT Info read by Index 


## [0.24.15] - 2023-05-17
### Changed
- WeldProgs -> to V4 for Twin-Progs/Parameters

## [0.24.14] - 2023-05-09
### Added
- Mask Attribute in Parameters, values added
- Default Values added as Value for .ewu, only takes affect in Simulation of Embedded Wizard
- Helper Script for deploy a temporary ParaDef.json to Mapro

## [0.24.13] - 2023-04-25
### Added
- Errors added (StartUp and Shutdown logs)
- Fan Speed Error level increased
- EEPROM Error Level (temporary) reduced 

## [0.24.12] - 2023-04-19
### Added
- Errors for Backend: KBC and general Error

## [0.24.11] - 2023-04-15
### Added
- Fan speed monitoring constants to ConfigStore

## [0.24.10] - 2023-04-13
### Added
- S4_Main Warnings/ Errors E_ConfigMismatch, WARN_FanSpeed, WARN_TempSensor

## [0.24.9] - 2023-04-13
### Added
- Primary ErrorBit for DESAT Detektion on DRV1 or DRV2
### Changed
- WeldProgs -> For Feldtest B1
- Primary Hardfaults on Startup from "act" Errors to "Remove from Mains" Errors
- TempLimits/AC01/T_MaxM1 & T_MaxM2 from 80°C to 97°C (need factory reset)
- FPGA Out32Bit "Rsv_11" used for demagnetize factor so changed value from 0 to 8 (needs for FPGA v0.5.10)
### Fixed
- Primary ErrorBit "Overcurrent down" -> "Overcurrent period"

## [0.24.8] - 2023-03-20
### Added
- WeldProgs -> SpeedRoot & SpeedCold (2704-2725, 2767, 2770, 2773)

## [0.24.7] - 2023-03-17
### Added
- ErrorDef.ewu, generated from SBK_Fehlerliste, Text and Textid for HMI (EmbeddedWizardUnit)
### Changed
- peripherie FPGA Id reserved (400), HMI default files for TouchConfiguration (Touchconfig = touchfirmaware)

## [0.24.6] - 2023-03-15
### Added
- EEPROM data layout and declaration
- Add temperature to the backend status shared memory
### Changed
- Return error notice if a backgroundjob does not yet exist

## [0.24.5] - 2023-03-15
### Added
- project independent tools and documentation for S4 microcontroller projects

## [0.24.4] - 2023-03-14
### Added
- DMR warning H80 (derating of motor current occurred)
### Changed
- Default Werte fuer Ki1 u. Kp1 auf 500 bzw. 5500 gesetzt 

## [0.24.3] - 2023-03-10
### Added
- SBK_Fehlerliste.xlsx in folder "ErrorDef"
- Known DMR errors (E8-1 to E8-7 & E8-10, E8-11, E8-14)
- Known Primary errors (E1-2 & E2-0 to E2-2 & E13-2, E15 E16, E22-3, E22-4, E30-1, E30-6, E31, E31-1, E32)
- Known M4 errors (E3, E3-1, E6, E6-1, E8-14, E14, E25-1, E30, E32)

## [0.24.2] - 2023-03-07
### Changed
- set I_soll_max FPGA Init-Wert geändert

## [0.24.1] - 2023-03-02
### Added
- WeldProgs -> Puls-II & SpeedPulse-III for RCS (65480-65489)
### Fixed
- WeldProgs -> BugFix „NaN“ for ArcLen-Korr

## [0.24.0] - 2023-03-01
### Fixed
- Calibration/ Adjustment 'Bus Error' problem: SicherUI_Struct_t Alignment in ipc_process.h fixed
### Added
- additional Version field in Shared Memory structs S4_Main - M4
- Seamtracking Signal as DokuPar #0 (unused before)

## [0.23.5] - 2023-02-24
### Added
- Added WUK02 configuration

## [0.23.4] - 2023-02-13
### Added
- E_Prim_TempAsymm error and config key
- missing S4_Main warning/error definitions

## [0.23.3] - 2023-02-09
### Added
- /DevFunction/ShowAllWeldprogs key (1) added

## [0.23.2] - 2023-02-09
### Changed
- Errordef.json renamed for DMR
- EEPROM and Config errors from general error removed
- WARN_ZALock connection to geid 32 removed (workaround sonst steht beim schweißen immer ein error/warning an )

## [0.23.1] - 2023-02-06
### Added
- GRPC function to ack all errors

## [0.23.0] - 2023-02-04
### Changed
- Interface S4_Main-M4: new fields for I and P machine limits + some reserve fields

## [0.22.16] - 2023-02-02
### Added
- Refresh available version grpc function for update-manager

## [0.22.15] - 2023-02-01
### Changed
- KoAct-Pd_number renamed

## [0.22.14] - 2023-01-30
### Fixed
- Primary ControlBits (Debug_Enab -> AutoMode_Enab, Control_Slow -> Debug_Enab, Ramp_Disab -> Error_Reset)

## [0.22.13] - 2023-01-25
### Added
- Primary Flex parameters .proto and S4_Diag definitions

## [0.22.12] - 2023-01-24
### Added
- Primary CAN Messages 0x180 ID 0x01 Byte 2+3 (AC Input Current) & ID 0x04 Byte 5 (Debug1)
- Primary CAN Messages 0x181 ID 0x0A FlexMsgID (0...100) for FlexParameter 1 & 2 (VoltOffsetZK, UZK_Faktor, DauerStromMax, DebugValues, CBC_Limit)
- Primary CAN ControlBits 4 (DebugEnable), 6 (SoftstartDisable), 12 (RampeDisable)

### Changed
- Primary CAN ControlBit 11 (ControlSlow -> ControlForce)
- Primary CAN ControlValue ID 0x01 Byte7 (ScaleShift -> ParamNum)

## [0.22.11] - 2023-01-20
### Added
- GetStatus function for lockmanager to proto file
- GetProcessStatusBackend function to proto file

## [0.22.10] - 2023-01-23
### Changed
- ErrodDef.json GID3 to REACT1

## [0.22.9] - 2023-01-18
### Added
- Missing sWPK-bits

## [0.22.8] - 2023-01-16
### Added
- WeldProgs 65490-65500 -> RnD MSG SpeedRCS
- WeldProgs 2505-2568 -> S3PrNr4-68 PulsII
- WeldProgs 2572-2645 -> S3PrNr100-178 SpeedPulsII
### Changed
- WeldProgs - update to wldProgs_2023-01-11 (optimized for impedance compensation -> 65515)
- WeldProgs - 65508/65509/65510 -> ParentS3_SPulsII/SpeedPulseII & RnD Master S3 SpeedRootCold & RnD-Master S3 SpeedPulse IUI XT -> optimized

## [0.22.7] - 2023-01-11
### Added
- Some DMR outC-Bits
### Changed
- Renamed some DMR Bits

## [0.22.6] - 2023-01-11
### Changed
- Primary StatusBits for B1-Feldtest

## [0.22.5] - 2023-01-09
### Changed
- Primary ErrorBits for B1-Feldtest
- ZA-Lock for MultiDMR

## [0.22.4] - 2023-01-09
### Changed
- replace Primary Processdata 'Control_Sollwert' with PWM_Max + ScaleShift
- change Primary diag gRPC proto def. to new structured format
- change Wire Feeder inC_-Bits to 32-Bit, add WFSVN_60V_Supply

## [0.22.3] - 2023-01-02
### Fixed
- use __aligned__(4/8) attribute in SHM structs definition
- tidy up ProcessdataMain rob_ and PI_ Constants
### Added
- PO_ProcessAct2 status bit
### Removed
- Latest/Pre-Release from GH-Actions workflow

## [0.22.2] - 2022-12-22
### Added
- WPK process status in SHM and gRPC-Proto definitions
- former inW_DDS_ON and inW_FILL_ON status bits to sWPK_Flags
- WPK data into GetDmrValues gRPC service (only for S4_Diag tool)
### Changed
- extend DMR processdata in SHM and gRPC-Proto definitions
- comments for WPK act_values indices (WPKStatValNums): SI units at interface to backend

## [0.22.1] - 2022-12-22
### Added
- GitHub Release workflow with Changelog enforcer on PR

## [0.22.0] - 2022-12-22
### Changed
- Update go package names
- Updated Used-Ports.md
### Added
- Add process status backend
- Add lockmanager proto definition
- Add ProcessdataBackend

## [0.21.25] - 2022-12-19
### Added
- WeldProgs for Wire calibration
- S4_Main Err/Warn definitions
### Changed
- Persistent keys definition for Adjustment
- in .proto replace ProcessStatusDmr in_bits_rc with wrn_bits_pp like in SHM ProcessStatusDmrStruct


## [0.21.24] - 2022-12-19
### Changed
- Unit ID for Dynamics

## [0.21.23] - 2022-12-19
### Added
- shared memory-processdata warn_bits_pp for DMR  prepared but commented, activate is todo
- Warning bits for DMR in ProcessdataDMR.h
- File ProcessdataWPK.h for water cooler unit
### Changed
- Errordef.json DMR Warnbits added and Global Error for ZALock
- Parameter min values changed input JAE
- Parameter Default activation of JC Parameterbits Start_I Start_Slope
### Removed
- Obsolete fr-Bits (Fernregler) in ProcessdataDMR.h

## [0.21.22] - 2022-12-02
### Fixed
- WeldProg 65518 -> startphase
- WeldProg 65506 -> more stable (but not perfekt restart in Ctrl 2 Standby/Hold)

## [0.21.21] - 2022-11-30
### Changed
- expand max. parameter setting range I to 1 - 1000A, WF to 0.1 - 40m/min, U to 1 - 100V
### Added
- missing Job setting parameters 37 = JS_Start_vWire, 38 = JS_Start_U

## [0.21.20] - 2022-11-28
### Added
- attachment and error definition to error_manager proto file

## [0.21.19] - 2022-11-23
### Added
- Include platform, error_manager, eventlog and update_manager proto files

## [0.21.18] - 2022-11-23
### Added
- WeldProg 65506 -> Adjustment LT U/I
### Changed
- WeldProgs - update to wldProgs_2022-11-23 (optimized for Adjustment)

## [0.21.17] - 2022-11-23
### Added
- inC_ZaVerr to DMR status bits

## [0.21.16] - 2022-11-22
### Added
- Default value Ki2, Kp2 added

## [0.21.15] - 2022-11-21
### Added
- WeldProg 65507 -> Adjustment LLGen

## [0.21.14] - 2022-11-16
### Added
- In FPGA Excel die Seiten: Statusbits, unused_pins, ErrorStatus
- Generierung dieser in C Datei IPC_A7_M4/ipc_fpga_io.h nun durch Makro
### Changed
- FPGA Register i_ist_rv_ltse, i_ist_ep_en_ltse, status_bits, unused_pins added bzw. adjusted

## [0.21.13] - 2022-11-15
### Changed
- WFS_dV_Soll_0m01 (dynamic setpoint) to WFeedStatusNums
- Unit "m/min/s" to Doku Parameter vout_dv_DMR_Soll
- Unit "m³/h" to Doku Parameter vout_RauchAbs_Empf
### Removed
- WFS_inC_BitsZwiTriInAkt (state bits of inactive Zwischentrieb) from WFeedStatusNums

## [0.21.12] - 2022-11-14
### Added
- JF_AdjustOcGen Bit "Justage LLGen aktiv"
### Changed
- AnlgIn channel name 'V-65V' to U_VRD according to schematic

## [0.21.11] - 2022-11-11
### Added
- FPGA Error stuff & pd_number_Rst_Timeout_FastSPI (254)

## [0.21.10] - 2022-11-05
### Added
- Calibration/Adjustment stuff

## [0.21.9] - 2022-09-28
### Added
- Added Error for deleting Errors (Storage full)

## [0.21.8] - 2022-09-26
### Added
- Added factory-reset to configstore proto-definition.

## [0.21.7] - 2022-09-26
### Added
- Global Error definitions and mapping of module/device errors to Glboal Errors
- Torch water cooling error bit and Config-Store defaults

## [0.21.6] - 2022-09-22
### Added
- Definitions of default-values und persistent-keys for configstore.

## [0.21.5] - 2022-09-21
### Changed
- FPGA - wegen PSA Abschaltung, KP Wert von 7500 auf 3750 angepasst

## [0.21.4] - 2022-09-15
### Changed
- WeldProgs - update to wldProgs_2022-09-15 (optimized)

## [0.21.3] - 2022-09-14
### Changed
- ETH Doku to V18

## [0.21.2] - 2022-09-13
### Added
- WeldProgs - Test-Programms 65509 & 65520
### Changed
- WeldProgs - update to wldProgs_2022-08-26 (optimized)
- Startprogramm set from 65533 to 2003 (1,0mm Steel)

## [0.21.1] - 2022-09-08
### Fixed
- increment SHM_VERSION

## [0.21.0] - 2022-09-07
### Changed
- Text in docu-Parameter gekürzt
- Job-Parameter tSpotOn/Off für Tig entfernt und durch "Res" ersetzt
- diag_service.proto: move S4_Main/_M4 Logs to Global Status
### Added
- neue Spalte "StepSize" in JSPara und DPara Sheet (Object_V4.xlsm) eingefügt
- entsprechende Änderungen in den Makros wegen der ParaDef.json, ParaObjectInit.h und MW_ParameterModelObjects.ewu Generierung hinzugefügt
- step-parameter to parameter_manager_service.proto definition

## [0.20.3] - 2022-09-05
###  Changed
- M4 Error & Warnbits für VRD & Watchdog & Sonstige E00-X nach Standard Fehlerliste ergänzt

## [0.20.2] - 2022-09-02
###  Changed
- Default Wert Modulator_Typ auf 4 gesetzt

## [0.20.1] - 2022-07-28
###  Fixed
- FPGA: Default Wert für Kp geändert.
- torchconfig für systemtorch angepasst.

## [0.20.0] - 2022-07-19
###  Changed
- Zeitliche Auflösung zu den Elementnamen von ProzStat_RunTime_IDs hinzugefügt.

## [0.19.2] - 2022-07-14
###  Changed
- soll_phase_shift_sum in Rsv_25 geschrieben
- 16Bit Register psa_MB gelöscht und als 32Bit in Rsv_26 geschrieben
- zwei reservierte Register Rsv_27, Rsv_28 hinzugefügt

## [0.19.1] - 2022-07-12
### Added
- for ConfigStore: Proto and Init file

## [0.19.0] - 2022-07-06
### Added
- Strom/Spannungs Kalibration Schnittstelle zu M4
- SicherUI Schnittstelle zu M4 für Grenzwert Überwachung bei Leerlauf & Libo

## [0.18.9] - 2022-07-05
### Added
- WeldProgs - S3XT-SpeedPulse/Arc-KL Port. hinzugefügt (P2621-P2623, P2656-P2658, P2660-P2662, P2664-P2669, P2671-P2671, P2674-P2678)
### Changed
- WeldProgs - Optimierung von P65510

## [0.18.8] - 2022-06-21
### Added
- WeldProgs - MiMi-KL Port. hinzugefügt (P2001-P2070, P2101-P2118, P2127, P2132-P2137, P2142-P2150, P2191-P2206, P2239-P2249, P2248-P2251)
### Changed
- WeldProgs - Optimierung von P65515 bis P65519, P65527, P65531 bis P65533

## [0.18.7] - 2022-06-03
### Changed
- WeldProgs - ProgV2 entfernt & ProgV3 hinzugefügt (nun kompatibel mit ParaV2)
- Excel Sheet angepasst & neu generiert damit Startprogram 65533 anstelle 0

## [0.18.6] - 2022-05-28
### Changed
- JobControl- and JobSettingsParameterValues are handled together

## [0.18.5] - 2022-05-17
### Changed
- Excel Sheet zur Dateigenerierung von CANBL_Devices.json und SoftwareEnums.h hinzugefügt.
- Batch File zur Generierung der Versions Info Datei zu einer Anwendung hinzugefügt.

## [0.18.4] - 2022-05-17
### Added
- UnitDef.json from generation

## [0.18.3] - 2022-05-16
### Changed
- Attributes in DPara values changed
## [0.18.2] - 2022-05-13
### Changed
- ParaObject.h Attribute added
- Objects v4 generation of Initialvalues for HMI (MW_ParameterModelObjects)

# Changelog
## [0.18.1] - 2022-04-19
### Changed
- Changed processdata-api definitions to implement JobSettings- and JobControl-Parameter and extended parameter.

## [0.18.0] - 2022-03-30
### Changed
- Parameter definitions for Job/Doku Para in .h and .json files

## [0.17.13] - 2022-05-02
### Added
- Config file for S4_SystemTorch app

## [0.17.12] - 2022-04-28
### Changed
- Default-Werte ccsafety_umax, ccsafety_umin und Ialt_max geändert

## [0.17.11] - 2022-04-22
### Fixed
- Ergänzungen in Kommentar Feld von Birger für FPGA Register Definition
- VRD / CRD Reihenfolge geändert im FPGA Write Register

## [0.17.10] - 2022-03-31
### Changed
- Default-Werte der Gain- und Offset-Parameter der Spannung (a/b/cum) sowie von stromsensor_ctrl-Register angepasst

## [0.17.9] - 2022-03-31
### Added
- S4_Main Warning and Error enums

## [0.17.8] - 2022-03-30
### Changed
- Bootloader Präfix auf alten stand angepasst um Flash tool ohne Update nutzen zu können

## [0.17.7] - 2022-03-23
### Added
- missing DO/DI_Flags (Digital Out/ Digital In) bit definitions

## [0.17.6] - 2022-03-21
### Added
- gRPC-Difinitions of jobmanager to allow paged loading of joblist.

## [0.17.5] - 2022-03-18
### Added
- gRPC-Definitions of jobmanager to load a background-job
## [0.17.4] - 2022-03-15
### Added
- Excel Sheet Funktion für automatisches ersetzen von " " mit "_" bei den Namen für Enum
### Fixed
- Excel Sheet zur Generierung von PD_NUMBER_FPGA_ENUM korrigiert.
## [0.17.3] - 2022-03-15
### Added
- Excel Sheet zur Generierung von PD_NUMBER_FPGA_ENUM in ipc_fpga_io.h und FPGA_PD_NumberList.json hinzugefügt

## [0.17.2] - 2022-03-15
### Added
- Excel Sheet zur Generierung von Bootloader_def.h und CANBL_Devices.json hinzugefügt

## [0.17.1] - 2022-03-03
### Changed
- FPGA: Adress in Excel-Datei korrigiert
- FPGA: Kommentar für R/L-Register in Excel-Datei geändert

## [0.17.0] - 2022-03-02
### Added
- deploy/config/CANBL_Devices.json
### Changed
- FPGA: Default Wert für u_adcb_a_offset
- FPGA: Gain und Offset für kumulierte Istspannung hinzugefügt **Breaking change!**

## [0.16.11] - 2022-02-28
### Changed
- Fixed datatypes in definitions of shared-memory-processdata.

## [0.16.10] - 2022-02-27
### Added
- Protodefinitions to lock/unlock jobs and load next/previous job
  in `job_manager_service`

## [0.16.9] - 2022-02-25
### Added
- Added proto-definition for job-manager.

## [0.16.8] - 2022-02-24
### Changed
- Schweißprogramme bzgl. Initialisierung Spannungsmessung Uavr_Activ / Uim_Activ sind aktualisiert

## [0.16.7] - 2022-02-23
### Changed
- JPara Order changed J_WeldCycle->4; 0 undef; J_WldMthd removed

## [0.16.6] - 2022-02-23
### Added
- StatusBit "Impedanzkompensations aktiv"
- Fehler/Warnungs Flags des ProzAutom. bzw. "Irrors" nun in ProzErrNo/ProzWarnNo deshalb Sammelbit
### Changed
- Define "DO_FPGA_SYNC" ist nun "DO_MC_FPGA_SYNC"

## [0.16.5] - 2022-02-21
### Added
- WeldProg 65511

## [0.16.4] - 2022-02-17
### Added
- Dig. input bit definitions for AnybusCC-module mode pins

## [0.16.3] - 2022-02-11
### Fixed
- WeldProg 65517
### Added
- WeldProgs 65515 to 65512

## [0.16.2] - 2022-02-08
### Changed
- Objects V4 Current Values Max from 600 to 800
### Added
- docs Ethernet_Schnittstellendefinition

## [0.16.1] - 2022-02-08
### Fixed
- Defaults for Gain and Offset
### Added
- new 16-bit register 'MagHT_Limit'

## [0.16.0] - 2022-02-08
### Changed
- separate parameter-manager- and process-data services for gRPC and SHM interface
- Primary and DMR process-data reworked, now analog values in float/ SI units
### Added
- new process-data Shared-memory struct with S4_Main, S4_M4, Primary and DMR sections

## [0.15.0] - 2022-02-06
### Changed
- concentrate all process data definitions in 'Processdata' directory
- header file includes now relative to S4_Config directory
- some preventive reserved ErrCounter, RunTime fields -> PROZ_IPC_STRUCT_VERSION to 6
- preventive fields in ProzStatus struct: ProzErrNo (= 'Irror'?) + ProzWarnNo

## [0.14.0] - 2022-01-27
### Changed
- Updated Wld Progs to ProgV2_2021-01-27_15-04-00 for ProcessAutomat-V2

## [0.13.2] - 2022-01-27
# Added
- header file defining shared-memory layout
- gRPC methods: set/get limits, request/release shared-memory

## [0.13.1] - 2022-01-26
### Changed
- Updated element-names of ProzStat_RunTime_IDs.
# Added
- Added enum ProzStat_FpgaMeanValues_IDs for calculation of FPGA mean values.

## [0.13.0] - 2022-01-25
### Changed
- DokuPars (vout) um vout_UniversalParameter1 - 5 erweitert
- Namen mit Id_Outputindex_Verfahrensautomat synchronisiert
- Änderung bei id 13 vout_d2Mat_Empf und id 21 vout_DMR_Soll
### Removed
- redundant IPC_A7_M4/ipc_enums.h, replace references with Parameter/Para_enums.h!
### Added
- new WeldCycleMode WCM_TestMode (2-stroke, no start/end cycle, no 'Zwangsabschaltzeit' ..)

## [0.12.0] - 2022-01-20
### Changed
- ui_preproc FPGA register optimized
- FPGA_BUF_SIZE (number of 16-Bit words for 'slow' SPI) from 128 to 100
### Removed
- unused sinc3 FPGA registers
- unused 'webserver' definitions

## [0.11.5] - 2021-12-23
### Added
- Parameter um HMI Parameter ergänzt

## [0.11.4] - 2021-12-15
### Changed
- Renamed PIEC ErrCounter element appropriate to their usage.

## [0.11.3] - 2021-12-14
### Changed
- Renamed reserved fast SPI struct elements to MsgCnt.
- Renamed reserved ErrCounter struct element to PIEC_SPI3_MSG_CNT.

## [0.11.2] - 2021-12-07
### Changed
- Defaultvalues of u_adcb_a_gain and u_adcb_a_offset changed

## [0.11.1] - 2021-11-10
### Changed
- Defaultvalues(Norm) of Objects changed

## [0.11.0] - 2021-11-08
### Added
- Status M4/ S4_Main and ParLimits rpc's to parameter manager service .proto definition v2

## [0.10.0] - 2021-11-05
### Added
 - Objects_V4.xls and generated Files
### Changed
 - IPC_A7_M4\ipc_enums.h is generated by Objects_V4.xls


## [0.9.1] - 2021-11-01
### Added
- Remote Procedure Call definitions and interface to IPC_A7_M4
- fan duty cycle init values
- FPGA mean values (now from M4) to proz_stat struct and diag api
### Changed
- move 'slow' SPI2 to FPGA from A7 to M4
- A7-M4 interface definition (!)

## [0.8.7] - 2021-10-21
### Added
- new primary module status and control bits

## [0.8.6] - 2021-10-20
### Changed
- new DokuPars Arc type and wire feed speed steepness

## [0.8.5] - 2021-10-19
### Added
- parameter manager service .proto definition (= v2 of processdata definition)

## [0.8.4] - 2021-10-15
### Changed
- FPGA fan related registers

## [0.8.3] - 2021-10-14
### Added
- Prozess Error & Warning Bits from M4
### Changed
- rename "PIEC_SPI3_CRC_FPGA_MC" to "PIEC_SPI3_FPGA_MC"

## [0.8.2] - 2021-10-14
### Changed
- weld program 65519 now compatible with weld cycle "electrode" (15)

## [0.8.1] - 2021-10-13
### Changed
- weld program organization, now single .csv file for every welding program

## [0.7.5] - 2021-10-11
### Added
- WeldCycleModes and WeldSlopeModes enum definitions

## [0.7.4] - 2021-10-05
### Fixed
- ADC enum names, some comments..
### Removed
- unused ProzMatrixIDs and ProzMatrixProgs enums
- 'typedef' with enums (better C++ compatibility)

## [0.7.3] - 2021-10-03
### Added
- S4_Main log messages to GetProcessStatus protobuf definition

## [0.7.2] - 2021-09-27
### Added
- new FPGA registers status_bits and steuer_bits

## [0.7.1] - 2021-09-24
### Fixed
- Program 65521, 65524, 65525 are now loadable

## [0.7.0] - 2021-09-20
### Changed
- included M4 Fast SPI data in diag_service protobuf definition GetProcessStatus

## [0.6.2] - 2021-09-17
### Added
- 'api' directory: Protocol Buffers definitions for gRPC interfaces
- CAN_Prim.json - Primary modules data definitions

## [0.6.1] - 2021-09-09
### Added
- set PD_Number, overwrites PD_Number from process SPI if !0
### Changed
- enum names FPGA register (!)
### Fixed
- names and description FPGA register
### Removed

## [0.5.0] - 2021-09-01
### Added
- make repository importable by yarn
