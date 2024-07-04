/**
 ******************************************************************************
 * @file    shared_memory_processdata.h
 * @author  UM
 * @brief   Process data shared-memory interface definition
 ******************************************************************************
 */
#pragma once

#include <stdint.h>
#include <Processdata/ProcessdataMain.h>
#include <Processdata/ProcessdataM4.h>
#include <Processdata/ProcessdataPrimary.h>
#include <Processdata/ProcessdataDmr.h>
#include <Processdata/ProcessdataWpk.h>
#include <Processdata/ProcessdataBackend.h>
#include <Processdata/ProcessdataFpga.h>

#define SHM_VERSION_PROCESSDATA 3 // increment on each incompatible change!

// ProcessStatus S4_Main, for definitions see Processdata/ProcessdataMain.h
struct __attribute__((__packed__, __aligned__(8))) ProcessStatusMainStruct {
    uint64_t err_bits;                   // \@ref ErrInternNums
    uint64_t warn_bits;                  // \@ref WarnInternNums
    uint32_t stat_bits;                  // not (yet) used here
    uint32_t pi_bits;                    // \@ref PI_Flags
    uint32_t po_bits;                    // \@ref PO_Flags
    uint32_t rob_bits;                   // \@ref rob_Flags (see Processdata/ProcessdataRemote.h)
    uint32_t cntrl_phase;                // \@ref StPh_IDs (see Processdata/ProcessdataM4.h)
    uint32_t err_counts_per_sec[ErrC_N]; // \@ref ErrCounterNums
    uint16_t Rsv[16];
    float RsvFloat[4];
    uint32_t seq_number; // Seqlock counter, has to be even and equal before and after reading this struct
};

// ProcessStatus M4, for definitions see Processdata/ProcessdataM4.h
struct __attribute__((__packed__, __aligned__(4))) ProcessStatusM4Struct {
    uint32_t stat_bits;            // \@defgroup ps_Flags
    uint32_t err_bits;             // \@defgroup perr_Flags
    uint32_t warn_bits;            // \@defgroup pwarn_Flags
    uint16_t proz_err_no;          // Error number Process ('Irror' numbers)
    uint16_t proz_warn_no;         // Warning number Process (TBD)
    uint32_t pnp_pnv;              // High word = Place number Process automat, Low word = PN 'Verfahrensautomat'
    uint32_t dig_in;               // \@defgroup DI_Flags
    uint32_t dig_out;              // \@defgroup DO_Flags
    float mc_fpga_i_set;           // SPI TX data to FPGA I-Soll
    uint32_t mc_fpga_pd_no;        // SPI TX data to FPGA PD No.
    uint32_t mc_fpga_pd_val;       // SPI TX data to FPGA PD Value
    float fpga_mc_i_act;           // SPI RX data from FPGA I-Ist
    float fpga_mc_u_act;           // SPI RX data from FPGA U-Ist
    float adin_val_10ms[ADC_N];    // index = \@ref ADC_InputChannels
    float fpga_mean_values[FMV_N]; // index = \@ref ProzStat_FpgaMeanValues_IDs
    uint32_t run_time_mean[RT_N];  // index = \@ref ProzStat_RunTime_IDs
    uint32_t run_time_max[RT_N];   // index = \@ref ProzStat_RunTime_IDs
    uint32_t err_counter[PIEC_N];  // index = \@ref ProzInnerErrCounter
    uint16_t Rsv[16];
    float RsvFloat[4];
    uint32_t seq_number; // Seqlock counter, has to be even and equal before and after reading this struct
};

// ProcessStatus Primary, for definitions see Processdata/ProcessdataPrimary.h
struct __attribute__((__packed__, __aligned__(4))) ProcessStatusPrimaryStruct {
    uint32_t stat_bits_1;               // Status bits module 1, can be a value of \@ref prS_Flags
    uint32_t err_bits_1;                // Error Bits module 1, can be a value of \@ref prE_Flags
    uint32_t detected_1;                // module 1 registered
    uint32_t stat_bits_2;               // Status bits module 2, can be a value of \@ref prS_Flags
    uint32_t err_bits_2;                // Error Bits module 2, can be a value of \@ref prE_Flags
    uint32_t detected_2;                // module 2 registered
    uint32_t stat_bits;                 // General status bits, not (yet) used
    uint32_t warn_bits;                 // General warning bits, not (yet) used
    uint32_t Rsv;                       // in case we need warn_bits_1 and _2
    float primstat_values_1[PSVN_N];    // module 1 \@ref PrimStatValNums
    float primstat_values_2[PSVN_N];    // module 2 \@ref PrimStatValNums
    uint32_t primcontrol_values[PCN_N]; // \@ref PrimControlNums
    uint32_t seq_number; // Seqlock counter, has to be even and equal before and after reading this struct
};

// ProcessStatus Dmr, for definitions see Processdata/ProcessdataDMR.h
struct __attribute__((__packed__, __aligned__(4))) ProcessStatusDmrStruct {
    uint32_t in_bits_active;          // Status-Bits Aktiver Drahtvorschub, can be a value of \@ref inC_Flags
    uint32_t in_bits_inactive;        // Status-Bits Inaktiver Drahtvorschub, can be a value of \@ref inC_Flags
    uint32_t in_bits_zwitri_active;   // Status-Bits Aktiver Zwischentrieb, can be a value of \@ref inC_Flags
    uint32_t in_bits_zwitri_inactive; // Status-Bits Inaktiver Zwischentrieb, can be a value of \@ref inC_Flags
    uint32_t stat_bits;               // Status-Bits DMR-PP, can be a value of \@ref sm_Flags
    uint32_t err_bits;                // Error-Bits DMR-PP, can be a value of \@ref eDMR_Flags
    uint32_t warn_bits;               // Warning-Bits DMR-PP, can be a value of \@ref wDMR_Flags
    uint32_t out_bits;                // Control-Bits, can be a value of \@ref outC_Flags
    uint32_t Rsv[8];
    uint16_t Rsv16Bit[8];

    float dmr_act_values[WFSVN_N]; // index = \@ref WFeedStatValNums

    uint32_t seq_number; // Seqlock counter, has to be even and equal before and after reading this struct
};

// ProcessStatus Backend, for definitions see Processdata/ProcessdataBackend.h
struct __attribute__((__packed__, __aligned__(4))) ProcessStatusBackendStruct {
    uint32_t Backend_Status; // Status-Bits from Backend can be a value of \@ref bstat_Flags
    float Temperature_MaPro; // Indicates the maPro-board temperature
    uint32_t reserve3;
    uint32_t seq_number;
};

// ProcessStatus WPK (WUK), for definitions see Processdata/ProcessdataWpk.h
struct __attribute__((__packed__, __aligned__(4))) ProcessStatusWpkStruct {
    uint32_t err_bits;  // \@ref WpkErrNums
    uint32_t warn_bits; // not (yet) used here
    uint32_t stat_bits; // Status bits, can be a value of \@ref sWPK_Flags
    uint16_t Rsv16Bit[2];

    float act_values[WPKSVN_N]; // index = \@ref WPKStatValNums

    uint32_t seq_number; // Seqlock counter, has to be even and equal before and after reading this struct
};

// ProcessStatus FPGA, for definitions see Processdata/ProcessdataFpga.h
struct __attribute__((__packed__, __aligned__(4))) ProcessStatusFpgaStruct {
    uint32_t err_bits;  // \@ref FpgaErr_Flags
    uint32_t warn_bits; // not (yet) used here
    uint32_t stat_bits; // Status bits, \@ref FpgaStat_Flags
    uint16_t Rsv16Bit[2];

    float act_values[FPGA_SVN_N]; // index = \@ref FpgaStatValNums

    uint32_t seq_number; // Seqlock counter, has to be even and equal before and after reading this struct
};

struct __attribute__((__packed__, __aligned__(8))) SharedMemoryProcessData {

    uint32_t Version; // readers check: must have the same value as SHM_VERSION_PROCESSDATA !
    uint32_t Rsv;     // padding, PsMain starts with uint64..

    struct ProcessStatusMainStruct PsMain;

    struct ProcessStatusM4Struct PsM4;

    struct ProcessStatusPrimaryStruct PsPrimary;

    struct ProcessStatusDmrStruct PsDMR;

    struct ProcessStatusBackendStruct PsBackend;

    struct ProcessStatusWpkStruct PsWpk;

    struct ProcessStatusFpgaStruct PsFpga;

    uint32_t Version2; // readers check: must have the same value as field 'Version' !
};
