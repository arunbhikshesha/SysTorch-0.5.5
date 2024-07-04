#ifndef _GDATA_H
#define _GDATA_H

/*** Include *****************************************************************/
#include "types.h"

/*** Preprocessor definitions ************************************************/
/* NO DEFINITIONS */

/*** Definition of variables *************************************************/

//=========================================================
//  name ulMainStat
//  description: Main System Status bits
//=========================================================
typedef enum {
    MS_POWERUPDONE = 0x00000001,    // Power up done flag   //WRN: @RMa: check this
    MS_STARTUP_DONE = 0x00000002,   // StartUp routine is done --> process routine is unlocked
    MS_PFC_STARTED = 0x00000004,    // PFC on Primary is started
    MS_LOCKED = 0x00000008,         // System locked
    MS_SHUTDOWN = 0x00000010,       // Shutdown
    MS_VRD = 0x00000020,            // Device with VRD
    MS_SELFTEST_DONE = 0x00000040,  // Selftest finished
    MS_SELFTEST_ERROR = 0x00000080, // Error while selftest
    MS_PROCRUN = 0x00000100,        // welding process is running
    MS_PROCESSLOCKED = 0x00000200,  // lock welding process until SUP finalized
    MS_DOWN = 0x00000400,           // Shutdown EEPROM save completed
    MS_SENDNOW = 0x00000800,
    MS_RESTART = 0x00001000,     // Maschine reset
    MS_MASTERRESET = 0x00002000, // Machine reset due to master reset command.
    // MS_EMPTY          = 0x00004000,
    // MS_EMPTY          = 0x00008000,
    // MS_EMPTY          = 0x00010000,
    MS_FANFIRSTSET = 0x00020000, // Placed after the fan startup according to the current temp
    // MS_EMPTY          = 0x00040000,
    MS_PSTEST = 0x00080000,       // Power source test
    MS_CAN_UNLOCKED = 0x00100000, // CAN special instructions unlocked by special key
    MS_CONFIGED = 0x00200000,     // Startup configuration done
    MS_CANDIAG_ON = 0x00400000,   // Diagnostic messages enabled by CAN, also activates special commands (CAN)
    // MS_EMPTY          = 0x00800000,
    // MS_EMPTY          = 0x01000000,
    // MS_EMPTY          = 0x02000000,
    // MS_EMPTY          = 0x04000000,
    MS_STANDBY = 0x08000000 // Standby mode active
                            // MS_EMPTY          = 0x10000000,
                            // MS_EMPTY          = 0x20000000,
                            // MS_EMPTY          = 0x40000000,
                            // MS_EMPTY          = 0x80000000,
} gData_MS_Status_t;

//=========================================================
//  name ulWeldingConf
//  description:  hose flags are stored in flash memory during power down.
//=========================================================
typedef enum {
    WC_BASIC = 0x00000001,
    WC_RUTIL = 0x00000002,
    WC_ELEC_VRD = 0x00000002,
    WC_CEL = 0x00000004,
    WC_WIG_LIARC = 0x00000004,
    WC_WIG_HF = 0x00000008,
    WC_NOSTBY_AFTER_RESET = 0x00000010, // Flag is set when after reset the device shouldn't enter the standby mode.
    WC_AUTOSTBY_OFF = 0x00000020,       // Flag is set when Autostandby feature is turned off
    WC_MASTERRESET = 0x00000040,        // Flag is set when Master Reset is requested
} gData_WC_Flags_t;

#define WC_WELD_MODE_MASK     0x0000000F
#define WC_CURRENT_TIG_MASK   0x0000FF00 // Mask of tig current value [A] when display panel is used
#define WC_CURRENT_CEL_MASK   0x00FF0000 // Mask of cel current value [A] when display panel is used
#define WC_CURRENT_BASIC_MASK 0xFF000000 // Mask of basic current value [A] when display panel is used
#define WC_WIGMASK            (WC_WIG_LIARC | WC_WIG_HF)
#define WC_ELECMASK           (WC_BASIC | WC_ELEC_VRD)

// ulSupplyStat - Supply status bits
#define SUPLY_MAINS_DETECT 0x0001 // Mains electricity is detected (not battery supply).

//=========================================================
//  ulSysStat - system states
//  description:  -
//=========================================================
typedef enum {
    SYS_FANIDLE = 0x00000001,
    SYS_FAN1 = 0x00000002,
    SYS_FAN2 = 0x00000004,
    SYS_FAN3 = 0x00000008,
    SYS_FAN4 = 0x00000010,
    SYS_FAN5 = 0x00000020,
    SYS_ALLFAN = 0x0000003F,
    SYS_GRUNDLAST_FAN = 0x00000040,
    SYS_FAN_MANUAL = 0x00000080,
    SYS_FAN_MANUAL_ENBL = 0x00000100,
    SYS_LL_GEN_ON = 0x00000200,
    SYS_HF_GEN_ON = 0x00000400,
    SYS_HF_PCB_DETECTED = 0x00000800,
    SYS_FR_PCB_DETECTED = 0x00001000,
    SYS_LLGEN_ON_EXT = 0x00002000,
    SYS_PAR_CHANGED = 0x00004000
} gData_SYS_Status_t;

//=========================================================
//  name Status Struct
//  description: -
//=========================================================
typedef struct {
    uint64_t ulMainStat;    // System status
    uint64_t ulWeldingConf; // WeldingConf
    uint64_t ulWorkingTime; // WorkingTime
    uint64_t ulWeldingTime; // Welding Time
    uint64_t ulSupplyStat;
    uint64_t ulSysStat; // System status eg. temp diode treshold, boards HF,RC detection
} gData_statstruct_t;

//=========================================================
//  Sub structur  "PrimaryData"
//  description:  data for/of Primary processor
//=========================================================

// ucPrimaryControl - Primary control bits (Master -> Primary)
// WRN: @RMa --> uint8_t is only 8bit!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// #define PRIM_ENABLE_PFC             0x0010
// #define PRIM_ENABLE_RELAY           0x0020
// #define PRIM_MASTER_IN_BOOTLOADER   0x0040

// uiPrimaryStatus - Primary status bits (Primary -> Master)
// #define PRIM_STAT_AC                0x0010
// #define PRIM_STAT_BATT_RDY          0x0020
// #define PRIM_STAT_MAINS_RELAY       0x0040
// #define PRIM_STAT_JUMP_BOOTLOADER   0x0080
// #define PRIM_STAT_PFC_MODE          0x0700

// fuiHandlingFlags - Primary handling flags
// #define PRIM_COMM_ESTABLISHED       0x0001  // Flag is set when first time all frames from the primary are
// received #define PRIM_MANUAL_CONTROL         0x0002  // Flag is set when manual control is required for
// primary #define PRIM_FPGA_ON_REQUEST        0x0004  // Flag is set when Primary wants to enable FPGA

typedef struct {
    uint8_t ucPrimaryControl;    // PrimaryControl
    uint16_t uiPrimaryStatus;    // PrimaryStatus
    uint16_t uiRecivedErrors;    // RecivedErrors
    uint8_t scTemperature;       // Temperature IGBT [1C]
    uint8_t ucVoltage16v;        // Voltage16v [0,1V]
    uint16_t uiCurrentReduce;    // CurrentReduce [0-65535]
    uint16_t uiPFCOutVoltage0V1; // PFC generated OutVoltageMeasure [0,1V]
    uint16_t uiInVoltageMeasure; // InVoltageMeasure [0,1V]
    uint16_t uiInPfcCurrent;     // InPfcCurrent [0,1A]
    uint16_t fuiHandlingFlags;   // HandlingFlags;
} gData_primarystruct_t;

//=========================================================
//  Sub structur  "ProzessDaten"
//  description:  data for/of welding process structur
//=========================================================

// ulIntStat - internal Status bits
#define ISB_LTSTART  0x0001 //  Power section was started
#define ISB_LTLOCKED 0x0002 // power section locked

// ulISR_PCReg - ISR Process status bits
#define IPCR_SEE        0x0001 // SEE ausgelöst
#define IPCR_IGR0       0x0004 // Output current is detected
#define IPCR_SCHWEISSEN 0x0040 // Machine is used for welding (analysis of KS)
#define IPCR_NO_LOAD    0x0100 //(ger. IPCR_LEERLAUF) Power secion at IDLE state - no load.

// ulProcessBits/G.BArt
#define BA_TWOSTROKE      0x00000001
#define BA_FOURSTROKE     0x00000002
#define BA_FOURSTROKEINDI 0x00000004
#define BA_PULSE          0x00000400

// uiProcDig   //WRN: @SN: Reihenfolge sauber machen
#define PROZ_BT1    0x8000
#define PROZ_BT2    0x4000
#define PROZ_BTUP   0x2000
#define PROZ_BTDOWN 0x1000
#define PROZ_KNGGAS 0x0800
#define PROZ_KLSP   0x0400
#define PROZ_HF     0x0200
#define PROZ_LLGEN  0x0100
#define PROZ_K2T    0x0080
// #define PROZ_EMPTY        0x0040
// #define PROZ_EMPTY        0x0020
// #define PROZ_EMPTY        0x0010
// #define PROZ_KIMAX        0x0008
// #define PROZ_EMPTY        0x0004
#define PROZ_DEBUG_ON  0x0002
#define PROZ_ELLAST_ON 0x0001

// ulProcStat
#define PROZSTAT_Igr0             0x00000001u // Strom fliesst
#define PROZSTAT_SHORT            0x00000002u // Kurzschluss erkannt
#define PROZSTAT_LIBO             0x00000004u // Lichtbogen steht
#define PROZSTAT_KSENDE           0x00000008u // Kein Prozessende nach Kurzschluss, warten bis kein Start mehr anliegt
#define PROZSTAT_SEEDETECT        0x00000010u
#define PROZSTAT_GASON            0x00000020u // MV Einschalten
#define PROZSTAT_GASPOST          0x00000040u // Gasnachströmen
#define PROZSTAT_SHORTRAMP        0x00000080u // Short Current Ramp activa
#define PROZSTAT_FIRSTJUMPDONE    0x00000100u
#define EMPTY                     0x00000200u
#define PROZSTAT_LLGENOK          0x00000400u
#define PROZSTAT_LLGENREADY       0x00000800u
#define PROZSTAT_PM_ON            0x00001000u
#define PROZSTAT_PM_DET           0x00002000u
#define PROZSTAT_PM_DETBT         0x00004000u
#define PROZSTAT_MMASTART         0x00008000u
#define PROZSTAT_LLGENON          0x00010000u
#define PROZSTAT_LLGENKALIB       0x00020000u
#define PROZSTAT_LIBOABRISS       0x00040000u
#define PROZSTAT_CANCELPROC       0x00080000u
#define PROZSTAT_BT1FLHIGH        0x01000000u
#define PROZSTAT_BT1FLLOW         0x02000000u
#define PROZSTAT_BT2FLHIGH        0x04000000u
#define PROZSTAT_BT2FLLOW         0x08000000u
#define PROZSTAT_MAINSTART        0x01000000u
#define PROZSTAT_SECSTART         0x02000000u
#define PROZSTAT_FOURSTROKEACTIVE 0x04000000u
#define PROZSTAT_FOURSTROKEBEGIN  0x10000000u
#define PROZSTAT_FOURSTROKEEND    0x40000000u
#define PROZSTAT_FOURSTROKESTOP   0x80000000u

typedef struct // prozstruct
{
    uint64_t ulIntStat;
    uint64_t ulISR_PCReg; // ISR Process status bits
    // uint16_t uiFan1Lim;                 //Temperature at which the fan is at level 1, current-dependent
    // uint16_t uiFanIngegrFactor;         // Scaling factor for the fan current integration
    // uint16_t uiTempDiodeMax;            //Sollstromabhängige maximale Diodentemperatur               =EN=
    // Should Inverse maximum diode temperature uint16_t uiTempDiodeMin;            // Diode temperature at
    // which welder is again ready to use after overheat. uint16_t uiTempDiodeAvMax;          //maximum average
    // temperature of diode (heat sink) uint16_t uiTempDiodePause;          //Pause after shutdown due to diode
    // average overtemp
    //
    // uint16_t uiGasPreflowTimeT10;       //Gasvorstroemzeit
    // uint16_t uiIgnitionPeakTimeT1;      //Zündpeakzeit
    // uint16_t uiStartCurrentTimeT1;      //Startstromzeit
    // uint16_t uiUpslopeTimeT100;         //Upslopezeit
    // uint16_t uiDownslopeTimeT100;       //Downslopezeit
    // uint16_t uiEndCurrentTimeT10;       //Endstromzeit
    // uint16_t uiGasPostflowTimeT10;      //Gasnachströmzeit
    // uint16_t uiShortCircCurrent100mA;   //Liftarcstrom
    // uint16_t uiWIGShortCircCurrent100mA;  //Prozess Kurzschlusstrom WIG
    // uint16_t uiIgnitionPeak100mA;       //Zündpeakstrom
    // uint16_t uiStartCurrent100mA;       //Startstrom
    // uint16_t uiMainCurrentProc100mA;    //Hauptstrom
    // uint16_t uiSecondCurrent100mA;      //Zweitstrom
    // uint16_t uiPulsFrequency100mHz;     //Pulsfrequenz
    // uint16_t uiPulsRelation;            //Pulsverhaeltnis
    // uint16_t uiIntervallOnTimeT10;         //
    // uint16_t uiIntervallPauseTimeT10;      //
    // uint16_t uiEndCurrent100mA;         //Endstrom
    // uint16_t uiThirdCurrent100mA;       //Stromabsenkung
    // uint16_t uiArcForce;                //
    //
    // uint16_t uiActualWeldingCurrent0A1;    //gemessener Schweißstrom (gefiltert) 0A1
    // uint16_t uiActualWeldingVoltage0V1;    //gemessene Schweißspannung (gefiltert) 0V1
    // uint16_t uiWeldingVoltage0V1Filtered;
    // uint64_t ulActualWeldingPower0W1;      //berechnete Schweißleistung 0W1
    // uint64_t ulActualWeldingPower0W1Filtered;      //berechnete Schweißleistung 0W1 (gefiltert)
    // uint16_t uiSetWeldingCurrent;       //Sollwert 0A1
    // uint16_t uiWSMSetWeldingCurrent;    //Sollwert 0A1 aus State Machine
    // uint16_t uiISRSetWeldingCurrent;    //Sollwert 0A1 aus ISR
    // uint16_t uiVoltageThresHoldUp0V1;      //Obergrenze für Schweißspannung -> Lichtbogenerkennung
    // uint16_t uiVoltageThresHoldDwn;     //Untergrenze für Schweißspannung -> Kurzschluss
    //
    // uint16_t uiHotStartT10;  //HotStartZeit
    // uint16_t uiKSHotStartT10;  //KurzSchluss HotStartZeit
    // uint16_t uiIKSHS100mA; //Electrode Hot Start Short Circuit Current
    // uint16_t uiIKS100mA; //Electrode Short Circuit Current
    // uint16_t uiIHS100mA; //Electrode Hot Start Current
    //
    // uint16_t uiFlankeAN;    //Stromanstieg
    // uint16_t uiFlankeAB;    //Stromabfall
    // uint16_t uiEpsilonIRampe;     //Grenzwert fuer Rampenminimum
    // uint16_t uiUSEE;        //Schweissendespannung
    // uint16_t uiAED;         //Arc End Detection [%]

    uint64_t ulProcessBits; // Prozessbits
    uint16_t uiProcDig;
    uint64_t ulProcStat;
} gData_prozstruct_t;

//=========================================================
//  Sub structur  "KBCReg"
//  description:  data for/of OneWireChip (KBC)
//=========================================================

typedef struct // kbcstruct
{
    uint64_t ulTemp1; // temp1
} gData_kbcstruct_t;

//=========================================================
//  Sub structur  "DVReg"
//  description:  data for/of wire unit
//=========================================================

typedef struct // dvstruct
{
    uint64_t ulTemp1; // temp1
} gData_dvstruct_t;

//=========================================================
//  Sub structur  "EDReg"
//  description:  data for/of calculation for duty values
//=========================================================

typedef struct // edstruct
{
    uint64_t ulTemp1; // temp1
} gData_edstruct_t;

//=========================================================
//  Sub structur  "JobAdmin"
//  description:  data for/of job mode and admin handling
//=========================================================

typedef struct // jobadminstruct
{
    uint64_t ulTemp1; // temp1
} gData_jobadminstruct_t;

//=========================================================
//  Sub structur  "UIPanel"
//  description:  data for/of panel handling
//=========================================================

typedef struct // uistruct
{
    uint64_t ulTemp1; // temp1
} gData_uistruct_t;

//=========================================================
//  Sub structur  "PowerMaster"
//  description:  data for/of PowerMaster
//=========================================================

typedef struct // pmstruct
{
    uint64_t ulPMBits;
} gData_pmstruct_t;

//=========================================================
//  Sub structur  "LLgenerator"
//  description:  data for/of LLgenerator
//=========================================================

typedef struct // llgenstruct
{
    uint64_t ulTemp1; // temp1
} gData_llgenstruct_t;

//=========================================================
//  Sub structur  "Fan"
//  description:  data for/of fan
//=========================================================

typedef struct // fanstruct
{
    uint64_t ulTemp1; // temp1
} gData_fanstruct_t;

//=========================================================
//  Sub structur  "HF"
//  description:  data for/of hf
//=========================================================

typedef struct // hfstruct
{
    uint16_t uiHFmaxValue0V1; // maximale HF-Spannung (HF-Power)
    uint16_t uiHFminValue0V1; // minimale HF-Spannung (Einschaltschwelle)
} gData_hfstruct_t;

//=========================================================
//  Sub structur  "Error"
//  description:  Error Bits
//=========================================================
typedef struct // errstruct
{
    //  uint64_t ulErrorSub;    // SubError flags //WRN: <- not good nam? <-please explain
    //  uint64_t ulError;       // Error flags
    //  uint64_t ulAllErrors;
    //  uint64_t ulErrorSystem;    // critical Hardware and Software System errors -> only internal, not shown
    //  uint64_t ulErrorRuntime;   // runtime welding and hardware errors -> only internal, not shown
    uint64_t uexStandardErrors; // errors from standard error list -> must be shown
} gData_errstruct_t;

//=========================================================
// Main gData data structure
//=========================================================

typedef struct {
    gData_statstruct_t SystemStat;     // data for system status
    gData_uistruct_t UIReg;            // data for/of user interface communication
    gData_primarystruct_t PrimaryData; // data for/of Primary processor
    gData_prozstruct_t ProcessData;    // data for/of welding process structur including Electrode, WIG, MigMag,
                                       // ..., ArcForce, Slopes, Ramps, ...
    gData_kbcstruct_t KBCReg;          // data for/of OneWireChip (KBC)
    gData_dvstruct_t DVReg;            // data for/of wire unit
    gData_edstruct_t EDReg;            // data for/of calculation for duty values
    gData_jobadminstruct_t JobAdmin;   // data for/of job mode and admin handling
    gData_errstruct_t ErrorStat;       // system errors
    gData_pmstruct_t PMReg;            // data for/of PowerMaster
    gData_llgenstruct_t LLGenReg;      // data for/of LLgenerator
    gData_fanstruct_t FanReg;          // data for/of Fan
    gData_hfstruct_t HFReg;            // data for/of HF
} gData_coredata_t;

extern gData_coredata_t gData;

/*** Prototypes of functions *************************************************/
/* NO DEFINITIONS */

/* Private defines -----------------------------------------------------------*/

#endif /*_GDATA_H */
