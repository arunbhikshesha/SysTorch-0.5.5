/**
 ******************************************************************************
 * @file    ProcessdataM4.h
 * @author  UM / KLA
 * @brief   Process data definitions for M4 core (Weld process control)
 ******************************************************************************
 **/
#pragma once
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  JobParLimits_t Struktur: Nummer und Min-/Max-Werte verwendete J_-Parameter
 */
typedef struct __attribute__((__packed__, __aligned__(4))) {
    float ParMin;    //!< Untere Parameter-Grenze
    float ParNorm;   //!< Parameter Normwert
    float ParMax;    //!< Obere Parameter-Grenze
    uint16_t JParNr; //!< Nummer Job-Parameter, gemäß \@ref JobParIDs
    uint16_t Rsv;    //!< wg. Data alignment u. später Attribute-Bits o.ä?..
} JobParLimits_t;
#define J_PAR_LIMITS_MAX 40 // Größe Array Job-Par-Limits

/**
 * @brief  Control phase numbers for Process-'Verfahrensautomat'
 */
enum StPh_IDs {
    STPH_NotDef,    // Not defined phase
    STPH_Init,      // Initial phase
    STPH_Hold,      // Hold phase
    STPH_StandBy,   // Standby phase
    STPH_GasVor,    // Gas preflow phase
    STPH_Zuendung,  // Ignition phase
    STPH_StartStr,  // start current phase
    STPH_UpSlope,   // Up slope phase
    STPH_HauptStr,  // Main current phase
    STPH_SlopeHZ,   // Slope main to second current phase
    STPH_ZweitStr,  // Second current phase
    STPH_SlopeZH,   // Slope second to main current phase
    STPH_DownSlope, // Down Slope phase
    STPH_EndStr,    // End current phase
    STPH_SchwEnd,   // weld end phase
    STPH_N          // -> Number of elements
};

/**
 * @brief  Indizes FGPA gemittelte Messwerte
 */
enum ProzStat_FpgaMeanValues_IDs {
    FMV_IO_m,   // Mittelwert Istwert Strom
    FMV_Uim,    // Mittelwert Istwert Spannung (unkompensiert)
    FMV_Uim_a,  // Istwert Integrierte Spannung, Kanal A
    FMV_Uim_b,  // Istwert Integrierte Spannung, Kanal B
    FMV_Ucomp,  // Mittelwert Istwert Spannung (kompensiert)
    FMV_P_comp, // Mittelwert Istwert Leistung (kompensiert)
    FMV_Rsv6,   // Placeholder
    FMV_Rsv7,   // Placeholder
    FMV_Rsv8,   // Placeholder
    FMV_Rsv9,   // Placeholder
    FMV_N       // -> Number of elements
};

/**
 * @brief  Indizes runtimes ProzStatus-RunTime-Array
 */
enum ProzStat_RunTime_IDs {
    RT_ProzCyclic_1us,          // Runtime of outer process
    RT_VerfCyclic_1us,          // Runtime of s4verf
    RT_MainCyclic_1us,          // Runtime of main loop
    RT_SysTickCyclic_1us,       // Runtime of systick-interrupt
    RT_ProzInnerPeriodic_100ns, // Periodic time of inner process
    RT_ProzInnerCyclic_100ns,   // Runtime of inner process
    RT_Rsv6,                    // Placeholder
    RT_Rsv7,                    // Placeholder
    RT_Rsv8,                    // Placeholder
    RT_Rsv9,                    // Placeholder
    RT_N                        // -> Number of elements
};

/**
 * @brief  Indizes ProzessInner Error Counter
 */
enum ProzInnerErrCounter {
    PIEC_PER_S_SPI3_SPI_CRC,    // Error per second: SPI3-Error
    PIEC_PER_S_SPI3_DMA,        // Error per second: DMA2 Stream0 & Stream1-Error
    PIEC_PER_S_SPI2_SPI_CRC,    // Error per second: SPI2-Error
    PIEC_PER_S_SPI2_DMA,        // Error per second: DMA2-Stream4 & Stream5-Error
    PIEC_PER_S_SPI3_MSG_CNT,    // Error per second: SPI3 Message-counter-evaluation
    PIEC_SINCE_ON_SPI3_ALL,     // Error since power on: SPI3, DMA2 Stream0 & Stream1-Error
    PIEC_SINCE_ON_SPI2_ALL,     // Error since power on: SPI5, DMA2 Stream4 & Stream5-Error
    PIEC_SINCE_ON_SPI3_MSG_CNT, // Error since power on: SPI3 Message-counter-evaluation
    PIEC_Rsv8,                  // Placeholder
    PIEC_Rsv9,                  // Placeholder
    PIEC_Rsv10,                 // Placeholder
    PIEC_Rsv11,                 // Placeholder
    PIEC_Rsv12,                 // Placeholder
    PIEC_Rsv13,                 // Placeholder
    PIEC_Rsv14,                 // Placeholder
    PIEC_Rsv15,                 // Placeholder
    PIEC_N                      // -> Number of elements
};

/**
 * @brief  Indizes für Vorgabe der gewählten Leerlaufspannung/VRD
 */
enum Leerlauf_IDs {
    LLS_None,     // Initial 0 als sicherer Zustand
    LLS_113V,     // Maximale zul. Leerlaufspannung (Versuchszwecke)
    LLS_85V_MMA,  // Normale 85V Leerlaufspannung für MMA in D mit S-Zeichen
    LLS_60V_SELV, // Reduzierte 60V Leerlaufspannung SELV (Kalibrierung)
    LLS_35V_AUST, // VRD für Australien
    LLS_24V_VRD,  // VRD für 24V
    LLS_12V_RUSS, // VRD für Russland
    LLS_AdjOP1,   // JustAP1 / Arbeitspunkt 1 (viel Spannung, wenig Strom) für Justage des Leerlaufgenerators
    LLS_AdjOP2,   // JustAP2 / Arbeitspunkt 2 (wenig Spannung, viel Strom) für Justage des Leerlaufgenerators
    LLS_Rsv1,     // Placeholder
    LLS_Rsv2,     // Placeholder
    LLS_N         // -> Number of elements
};

/**
 * @brief  Indizes für Sicherungsmodel bzw. Abschaltgrenzen
 */
enum SicherUI_IDs {
    SUI_UoSoll, // Sollwert der Leerlaufspannung
    SUI_IoSoll, // Sollwert des Kurzschlussstromes des Leerlaufgenerators
    SUI_Zeit1,  // Ab dieser Zeit wird eine Schutzabschaltung der Treiber generiert
    SUI_Udiff,  // Zul. Differenzspannung bis Schutzabschaltung
    SUI_KGL,    // Koeffizient für Glättung der Messwerte
    SUI_KSY,    // Koeffizienten zur Einstellung der Symmetrie
    SUI_Zeit2,  // Ab dieser Zeit wird die 24V Versorgungsspannung abgeschaltet
    SUI_Io0,    // sicherheitstechnische Stromschwelle für Ausgangsstrom 0
    SUI_N       // -> Number of elements
};

/**
 * @brief  Indizes für Justage der ADC Messwerte & Leerlaufgenerator
 */
enum Adjust_IDs {
    ADJ_Iext,     // Externe Strommessung bei GSQ (AC01)
    ADJ_U130,     // Spannungsmessung an Schweißbuchsen
    ADJ_U130Diff, // Externe Differenzspannungsmessung
    ADJ_U_VRD,    // Interne VRD Messung Leerlaufgenerator
    ADJ_Igen,     // StromBegrenzer (Offset_Igen, Gain_Igen)
    ADJ_Ugen,     // Spannungsgenerator (Offset_Ugen, Gain_Ugen)
    ADJ_GASDF1,   // Gasdurchfluss Sensor 1
    ADJ_GASDF2,   // Gasdurchfluss Sensor 2
    ADJ_Rsv1,     //
    ADJ_Rsv2,     //
    ADJ_N         // -> Number of elements
};

/** @defgroup ps_Flags Statusbits äußere Prozess-Routine
 * @{
 */
// untere 16 Bit Kompatiblität mit ps_-Bits (-> CAN ID 0x192) Bestandsmaschinen:
#define ps_SlopeEnd  0x0010 //!< gesetzt nach Beendigung Up-/Downslope
#define ps_Igr0      0x0020 //!< "Strom fließt"-Signal
#define ps_UgrKS     0x0040 //!< Signal "Lichtbogen steht"
#define ps_WeldEnd   0x0080 //!< gesetzt nach Ende Schweißen (Endpuls)
#define ps_LT_Bereit 0x0100 //!< Status "Leistungsteil bereit"
// #define ps_NetzUntSpWarn	    0x0400	//!< "Warnung Unterspannung"
// #define ps_ProgValid		    0x0800	//!< Status "gewähltes Programm gültig"
#define ps_WireStick 0x1000 //!< Rückmeldung WireStick-Check
// #define ps_WireStickValid 0x2000 //!< Gültigkeit WireStick-Check-Bit
// #define ps_NetzEinphasig	    0x4000	//!< "Netzanschluss einphasig"
#define ps_CalibAdjActive 0x8000     //!< Calibration or Adjustment ongoing
#define ps_ProzessBereit  0x00010000 //!< Prozess-Modul hochgelaufen und ohne Fehler
#define ps_LiBoGest       0x00020000 //!< Signal "Lichtbogen gestört"
#define ps_ImpkompAktiv   0x00040000 //!< Signal "Impedanzkompensation aktiv"
/**
 * @}
 */

/** @defgroup perr_Flags Fehlerflags M4/Prozess
 * @{
 */
#define perr_MASKE_PROZ      0x0000FFFF //!< ProzessAutomaten Mask
#define perr_PROZ_Irror      0x00000001 //!< Pending process automat error "Irror" (errorlevel)
#define perr_PROZ_Rsv_1      0x00000002 //!< Placeholder
#define perr_PROZ_Rsv_2      0x00000004 //!< Placeholder
#define perr_PROZ_Rsv_3      0x00000008 //!< Placeholder
#define perr_PROZ_Rsv_4      0x00000010 //!< Placeholder
#define perr_FPGA_link       0x00000020 //!< E31-4 communication failed FPGA-M4
#define perr_FPGA_general    0x00000040 //!< E32-0 general FPGA error
#define perr_Overtemperatur  0x00000080 //!< E08-14 Overtemperatur (>100°C)
#define perr_U_limit_uC      0x00000100 //!< E06-0 Overvoltage M4
#define perr_I_limit_uC      0x00000200 //!< E03-0 Overcurrent M4
#define perr_U_limit_FPGA    0x00000400 //!< E06-1 Overvoltage FPGA
#define perr_I_limit_FPGA    0x00000800 //!< E03-1 Overcurrent FPGA
#define perr_VRD_activation  0x00001000 //!< E25-X VRD activation failed -> communication A7-M4-FPGA
#define perr_VRD_defekt      0x00002000 //!< E25-0 VRD defekt
#define perr_VRD_overcurrent 0x00004000 //!< E25-1 VRD short circuit / llgen defekt
#define perr_PE_Monitoring   0x00008000 //!< E04-1 Protective conductor monitoring has struck (welding)
#define perr_MASKE_M4        0xFFFF0000 //!< M4 Errors Mask
#define perr_InternalM4      0x00010000 //!< An M4 internal error has occurred
#define perr_Incorrect24V    0x00020000 //!< E14-0 The 24V-Inputvoltage is to high/low
#define perr_ProgCopy        0x00040000 //!< E27-X Error by copy programm from A7->M4
#define perr_ProgLoading     0x00080000 //!< E27-X Error by unpack/loading programm
#define perr_ProgKorrupt     0x00100000 //!< E27-X Incorrect program loaded
#define perr_M4_NMI          0x00200000 //!< M4 is in ErrorLoop from ISR-NMI
#define perr_M4_HardFault    0x00400000 //!< M4 is in ErrorLoop from ISR-HardFault
#define perr_M4_MemManage    0x00800000 //!< M4 is in ErrorLoop from ISR-MemManage
#define perr_M4_BusFault     0x01000000 //!< M4 is in ErrorLoop from ISR-BusFault
#define perr_M4_UsageFault   0x02000000 //!< M4 is in ErrorLoop from ISR-UsageFault
#define perr_HB_ProzInner    0x04000000 //!< Heart Beat: Call out of ProzessInner timeout
#define perr_HB_Prozess      0x08000000 //!< Heart Beat: Call out of Prozess timeout
#define perr_M4_Watchdog     0x10000000 //!< M4 watchdog became active & a restart took place
#define perr_SW_incorrect    0x20000000 //!< E30-0 incorrect Software
#define perr_Reserve_15      0x40000000 //!< Placeholder
#define perr_Reserve_16      0x80000000 //!< Placeholder
/**
 * @}
 */

/** @defgroup pwarn_Flags Warnungs-Flags M4/Prozess
 * @{
 */
#define pwarn_MASKE_PROZ      0x0000FFFF //!< ProzessAutomaten Mask
#define pwarn_PROZ_Irror      0x00000001 //!< Pending process automat error "Irror" (Warninglevel)
#define pwarn_PROZ_JParLimits 0x00000002 //!< JParLimits_N > J_PAR_LIMITS_MAX
#define pwarn_PROZ_Rsv_2      0x00000004 //!< Placeholder
#define pwarn_PROZ_Rsv_3      0x00000008 //!< Placeholder
#define pwarn_PROZ_Rsv_4      0x00000010 //!< Placeholder
#define pwarn_PROZ_Rsv_5      0x00000020 //!< Placeholder
#define pwarn_PROZ_Rsv_6      0x00000040 //!< Placeholder
#define pwarn_PROZ_Rsv_7      0x00000080 //!< Placeholder
#define pwarn_PROZ_Rsv_8      0x00000100 //!< Placeholder
#define pwarn_PROZ_Rsv_9      0x00000200 //!< Placeholder
#define pwarn_PROZ_Rsv_10     0x00000400 //!< Placeholder
#define pwarn_PROZ_Rsv_11     0x00000800 //!< Placeholder
#define pwarn_PROZ_Rsv_12     0x00001000 //!< Placeholder
#define pwarn_PROZ_Rsv_13     0x00002000 //!< Placeholder
#define pwarn_PROZ_Rsv_14     0x00004000 //!< Placeholder
#define pwarn_PE_Monitoring   0x00008000 //!< Protective conductor monitoring has struck (standby)
#define pwarn_MASKE_M4        0xFFFF0000 //!< M4 Warnuings Mask
#define pwarn_ProgLoading     0x00010000 //!< currently loading new Programm
#define pwarn_ProzInnerOFF    0x00020000 //!< Inner Processautomat is currently disabled
#define pwarn_Reserve_03      0x00040000 //!< Placeholder
#define pwarn_Reserve_04      0x00080000 //!< Placeholder
#define pwarn_Reserve_05      0x00100000 //!< Placeholder
#define pwarn_Reserve_06      0x00200000 //!< Placeholder
#define pwarn_Reserve_07      0x00400000 //!< Placeholder
#define pwarn_Reserve_08      0x00800000 //!< Placeholder
#define pwarn_Reserve_09      0x01000000 //!< Placeholder
#define pwarn_Reserve_10      0x02000000 //!< Placeholder
#define pwarn_HB_ProzInner    0x04000000 //!< Heart Beat: Call out of ProzessInner timeout
#define pwarn_HB_Prozess      0x08000000 //!< Heart Beat: Call out of Prozess timeout
#define pwarn_Reserve_13      0x10000000 //!< Placeholder
#define pwarn_SW_incorrect    0x20000000 //!< H24-0 incorrect Software
#define pwarn_Reserve_15      0x40000000 //!< Placeholder
#define pwarn_Reserve_16      0x80000000 //!< Placeholder
/**
 * @}
 */

/** @defgroup DI_Flags Digitalsignale µC DIN
 * @{
 */
#define DI_LP1_PB8       0x0001
#define DI_LP2_PB10      0x0002
#define DI_FASTSYNCIN    0x0004
#define DI_FPGA_MC_SYNC  0x0008
#define DI_KOAKT1_IN     0x0010
#define DI_KOAKT2_IN     0x0020
#define DI_ABCC_MI0_SYNC 0x0040
#define DI_ABCC_MI1      0x0080
#define DI_ABCC_NMD0     0x0100
#define DI_GASUE1_NOK    0x0200
#define DI_GASUE2_NOK    0x0400
#define DI_SLUE_NOK      0x0800
#define DI_VCC_EXT_NOK   0x1000
#define DI_RCS_TEMP      0x2000
#define DI_N_MAX         15 // für Entprellung, >= höchste Bit-Stelle der DI_Flags + 1!
/**
 * @}
 */

/** @defgroup DO_Flags Digitalsignale µC Digital Out
 * @{
 */
// DigitalOut-Bits, Ausgabe-Bits von Modul Prozess.c, können über DOmask/DOforce überschrieben werden
#define DO_KOAKT1A_OUT  0x0001
#define DO_KOAKT2A_OUT  0x0002
#define DO_LP3_PE7      0x0004
#define DO_LP4_PE8      0x0008
#define DO_FASTSYNCOUT  0x0010
#define DO_ETH_1588     0x0020
#define DO_OPTO_OUT1    0x0040
#define DO_MC_FPGA_SYNC 0x0080
#define DO_MC_DRV_ON    0x0100
#define DO_GASV1        0x0200
#define DO_GASV2        0x0400
#define DO_ON_24V       0x0800
/**
 * @}
 */

/**
 * @brief  Indizes ADC Input Channels
 */
enum ADC_InputChannels {
    // Aufteilung MP1 mit 2 ADC & 2 DMA-Streams
    ADC_I_Ext,     // ADC1_IN2  R1
    ADC_UIST_130V, // ADC1_IN15 R2
    ADC_UIST_VRD,  // ADC1_IN16 R3
    ADC_TempSens1, // ADC2_IN3  R1
    ADC_TempSens2, // ADC2_IN5  R2
    ADC_GASDF1,    // ADC2_IN9  R3
    ADC_GASDF2,    // ADC2_IN10 R4
    ADC_24V,       // ADC2_IN18 R5
    // Ab hier ADC2 INJ Channels ohne DMA
    ADC_TempSensMC, // ADC2_TEMP R1
    ADC_VBAT,       // ADC2_VBAT R2 -> nur erste 3s nach Einschalten gemessen!
    ADC_Rsv_10,
    ADC_Rsv_11,
    ADC_Rsv_12,
    ADC_Rsv_13,
    ADC_Rsv_14,
    ADC_Rsv_15,
    ADC_N // -> Number of elements
};

#ifdef __cplusplus
}
#endif
