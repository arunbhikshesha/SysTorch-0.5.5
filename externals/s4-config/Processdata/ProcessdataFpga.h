/*** SPI interface to FPGA: register definition (Do not edit, autom. generated from Excel!) ***/

#pragma once

#define FPGA_BUF_SIZE  100 // number of 16 bit words in SPI data stream µC <-> FPGA

enum I_REG32_FPGA_ENUM {
    i_reg32_Ialt_set_1,
    i_reg32_Ialt_set_2,
    i_reg32_i_adca_cum_gain,
    i_reg32_i_adca_minmaxlast_gain,
    i_reg32_u_adcb_a_gain,
    i_reg32_u_adcb_b_gain,
    i_reg32_u_adcb_cum_gain,
    i_reg32_ccsafety_umax,
    i_reg32_ccsafety_umin,
    i_reg32_Ialt_max,
    i_reg32_Ialt_min,
    i_reg32_mag_faktor,
    i_reg32_Rsv_12,
    i_reg32_N
};
enum I_REG16_FPGA_ENUM {
    i_reg16_I_soll_max,
    i_reg16_reserv_2,
    i_reg16_Luefgr1_duty_cycle,
    i_reg16_Luefgr2_duty_cycle,
    i_reg16_arect_ctrl,
    i_reg16_arect_en_delay,
    i_reg16_arect_ep_delay,
    i_reg16_i_adca_cum_offset,
    i_reg16_i_adca_minmaxlast_offset,
    i_reg16_u_adcb_a_offset,
    i_reg16_u_adcb_b_offset,
    i_reg16_u_adcb_cum_offset,
    i_reg16_ki_1,
    i_reg16_ki_2,
    i_reg16_ki_3,
    i_reg16_kp_1,
    i_reg16_kp_2,
    i_reg16_kp_3,
    i_reg16_ki_ceil,
    i_reg16_ki_floor,
    i_reg16_kp_ceil,
    i_reg16_kp_floor,
    i_reg16_i_ceil,
    i_reg16_i_floor,
    i_reg16_di_max,
    i_reg16_driver_reset,
    i_reg16_driver_delay,
    i_reg16_stromsensor_ctrl,
    i_reg16_Pd_Number,
    i_reg16_diag_phase_shift,
    i_reg16_ctrlbit_inverter_on,
    i_reg16_Modulator_Typ,
    i_reg16_psa_reset,
    i_reg16_ltse1_delay,
    i_reg16_ltse2_delay,
    i_reg16_np_sync_enable,
    i_reg16_rlcomp_R,
    i_reg16_rlcomp_L,
    i_reg16_rxtx_tx_ltse,
    i_reg16_Th_model_K1,
    i_reg16_Th_model_K2,
    i_reg16_Th_model_K3,
    i_reg16_Th_model_K4,
    i_reg16_Th_model_K5,
    i_reg16_Th_model_K6,
    i_reg16_Th_model_K71,
    i_reg16_Th_model_K72,
    i_reg16_Th_model_K73,
    i_reg16_Th_model_K74,
    i_reg16_Th_model_K75,
    i_reg16_Th_model_R0,
    i_reg16_Th_model_Tkk,
    i_reg16_u_sense_ctrl,
    i_reg16_vrd_v1,
    i_reg16_crd_c1,
    i_reg16_vrd_v2,
    i_reg16_crd_c2,
    i_reg16_steuer_bits,
    i_reg16_MagHT_Limit,
    i_reg16_i_ist_rv_ltse,
    i_reg16_i_ist_ep_en_ltse,
    i_reg16_N
};

enum O_REG32_FPGA_ENUM {
    o_reg32_psa_i_soll,
    o_reg32_psa_i_ist,
    o_reg32_errorstatus,
    o_reg32_i_corr_sum,
    o_reg32_i_corr_acc,
    o_reg32_i_soll_sum,
    o_reg32_u_corr_sum,
    o_reg32_u_corr_a_sum,
    o_reg32_u_corr_b_sum,
    o_reg32_u_corr_acc,
    o_reg32_u_comp_sum,
    o_reg32_u_comp_acc,
    o_reg32_d_i_acc,
    o_reg32_i_square_acc,
    o_reg32_i_square_sum,
    o_reg32_u_square_acc,
    o_reg32_u_square_comp_acc,
    o_reg32_u_square_comp_sum,
    o_reg32_p_acc,
    o_reg32_p_comp_acc,
    o_reg32_p_comp_sum,
    o_reg32_XY1_i_acc,
    o_reg32_XY2_i_acc,
    o_reg32_i_var_sum,
    o_reg32_n_acc,
    o_reg32_soll_phase_shift_sum,
    o_reg32_psa_MB,
    o_reg32_Rsv_27,
    o_reg32_Rsv_28,
    o_reg32_N
};
enum O_REG16_FPGA_ENUM {
    o_reg16_tacho_luefter1,
    o_reg16_tacho_luefter2,
    o_reg16_psa_pwm_soll,
    o_reg16_psa_np,
    o_reg16_version_major,
    o_reg16_version_minor,
    o_reg16_psa_LT_OUT,
    o_reg16_rxtx_diag,
    o_reg16_psa_np2,
    o_reg16_drv_lue_diag,
    o_reg16_gd_driver_overcurrent,
    o_reg16_psa_zm_status,
    o_reg16_n_sum,
    o_reg16_n_u_ima,
    o_reg16_spi3_crc_error,
    o_reg16_spi2_crc_error,
    o_reg16_tacho_luefter3,
    o_reg16_tacho_luefter4,
    o_reg16_status_bits,
    o_reg16_unused_pins,
    o_reg16_N
};

enum PD_NUMBER_FPGA_ENUM {
    pd_number_none = 0,
    pd_number_Trap = 1,
    pd_number_VRD_off = 2,
    pd_number_VRD_on1 = 3,
    pd_number_VRD_on2 = 4,
    pd_number_AC_RV = 5,
    pd_number_AC_EP = 6,
    pd_number_AC_EN = 7,
    pd_number_TX_LT1_direct = 8,
    pd_number_TX_LT1_nega = 9,
    pd_number_TX_LT2_direct = 10,
    pd_number_TX_LT2_nega = 11,
    pd_number_TX_LT_direct = 12,
    pd_number_TX_LT_nega = 13,
    pd_number_TX_SE1_direct = 14,
    pd_number_TX_SE1_nega = 15,
    pd_number_TX_SE2_direct = 16,
    pd_number_TX_SE2_nega = 17,
    pd_number_TX_SE_direct = 18,
    pd_number_TX_SE_nega = 19,
    pd_number_Uavr_Activ = 20,
    pd_number_Uim_Activ = 21,
    pd_number_Trig_d2u = 22,
    pd_number_Pd_23_unused = 23,
    pd_number_Pd_24_unused = 24,
    pd_number_Pd_25_unused = 25,
    pd_number_Meld_Daks = 26,
    pd_number_KeinMeld_Daks = 27,
    pd_number_Clear_Start_Preproc = 28,
    pd_number_Restart_Preproc = 29,
    pd_number_PI_control = 30,
    pd_number_Min_PI_Reg = 31,
    pd_number_Max_PI_Reg = 32,
    pd_number_MinMax_PI_Reg = 33,
    pd_number_Reg_Para_1 = 34,
    pd_number_Reg_Para_2 = 35,
    pd_number_Reg_Para_3 = 36,
    pd_number_PI_Set_Ialt_1 = 37,
    pd_number_PI_Set_Ialt_2 = 38,
    pd_number_CBCC_on_min = 39,
    pd_number_CBCC_on_max = 40,
    pd_number_Inverter_AUS = 41,
    pd_number_Inverter_AN = 42,
    pd_number_Set_Phase_shift_min = 43,
    pd_number_Set_Phase_shift_max = 44,
    pd_number_Reset_Phase_Shift = 45,
    pd_number_Ig_EP = 46,
    pd_number_Ig_EN = 47,
    pd_number_vD_D1 = 48,
    pd_number_vD_R1 = 49,
    pd_number_vD_D2 = 50,
    pd_number_vD_R2 = 51,
    pd_number_vD_B = 52,
    pd_number_sync_1 = 53,
    pd_number_sync_2 = 54,
    pd_number_Break = 55,
    pd_number_Vm_init = 56,
    pd_number_Vm_mean = 57,
    pd_number_R_measure = 58,
    pd_number_L_measure = 59,
    pd_number_Measures_On = 60,
    pd_number_Measures_Off = 61,
    pd_number_Measures_Hold = 62,
    pd_number_Set_Wvm = 63,
    pd_number_Weld_End_message = 64,
    pd_number_vD_Norm = 65,
    pd_number_Rst_Timeout_FastSPI = 254,
    pd_number_Init = 255
};

// FpgaStat_Flags from o_reg16_status_bits
#define FPGA_STATS_DRV_Reset    0x0001 // driver_reset Register = 1
#define FPGA_STATS_VRD_OFF    0x0002 // VRD1 und VRD2 inaktiv (PD_Number = 255 or 2 ) or Timeout_fastspi = 1
#define FPGA_STATS_VRD1_ON    0x0004 // PD_Number = 3
#define FPGA_STATS_VRD2_ON    0x0008 // PD_Number = 4
#define FPGA_STATS_Modulator_off    0x0010 // psa_reset  = 65535
#define FPGA_STATS_AR_Short    0x0020 // arect_ctrl = 666 (Überstromschutz)
#define FPGA_STATS_AR_Single    0x0040 // arect_ctrl = 1 (Isense_1 Modus)
#define FPGA_STATS_AR_Both    0x0080 // arect_ctrl  = 2 (Beide aktiv)
#define FPGA_STATS_LT_ON    0x0100 // Inverter_on PD_Number = 42 (inverter on)

// FPGA unused_pins from o_reg16_unused_pins
#define FPGA_NUSE_PTP_Eth    0x0001 // 0
#define FPGA_NUSE_PTP_Eth2    0x0002 // 1
#define FPGA_NUSE_CBCC_TRG    0x0004 // 0
#define FPGA_NUSE_ADS1205_SPI_MISODATA_1    0x0008 // 1
#define FPGA_NUSE_ADS1205_SPI_MISODATA_2    0x0010 // 1
#define FPGA_NUSE_ADS1205_SPI_CLK_OUT    0x0020 // 1
#define FPGA_NUSE_RX_SE2    0x0040 // 1
#define FPGA_NUSE_MC_FPGA_SYNC    0x0080 // 0

// FpgaErr_Flags from o_reg32_errorstatus (lower 16 bits)
#define FPGA_ERR_RCS_Timeout    0x00000001 // Timeout Rapid Current Shutdown wenn innerhalb 1ms nach dem setzen des Modus LTSE_RV kein Wechsel zu den Modis  LTSE_EP oder LTSE_EN geschieht
#define FPGA_ERR_RCS_OcTurnON    0x00000002 // Overcurrent turnon Rapid Current beim Wechsel auf pd_Nr 5, i_ist > i_reg_i_ist_rv_ltse
#define FPGA_ERR_RCS_OcTurnOFF    0x00000004 // Overcurrent turnoff Rapid Current Shutdown beim Wechsel von 5 auf pd_Nr 6 bzw. 7, i_ist > i_reg_i_ist_ep_en_ltse
#define FPGA_ERR_FSPI_Timeout    0x00000008 // Timeout fastspi wenn 10 hintereinander ungültige Nachrichten kommen (CRC-behaftete und/oder übersprungenen Nachrichten)
#define FPGA_ERR_PrimCurrOFF    0x00000010 // Primärstromabschaltung psa_reset = 65535 oder einer der 4 Primärstromauslösungen geschieht (psa_zm_status(3:0) /= 0)
#define FPGA_ERR_Lt_sphs_gr_limit    0x00000020 // wird gesetzt, wenn der Wert von Soll_phase_shift 5 Minuten lang größer als 700
#define FPGA_ERR_I_soll_gr_limit    0x00000040 // wird gesetzt, wenn der Soll-Strom größer als der Wert, der im Register reg_i_soll_max als maximaler Soll-Strom definiert wurde
#define FPGA_ERR_rsv_7    0x00000080 // Reserve Errorbit 7
#define FPGA_ERR_rsv_8    0x00000100 // Reserve Errorbit 8
#define FPGA_ERR_rsv_9    0x00000200 // Reserve Errorbit 9
#define FPGA_ERR_rsv_10    0x00000400 // Reserve Errorbit 10
#define FPGA_ERR_rsv_11    0x00000800 // Reserve Errorbit 11
#define FPGA_ERR_rsv_12    0x00001000 // Reserve Errorbit 12
#define FPGA_ERR_rsv_13    0x00002000 // Reserve Errorbit 13
#define FPGA_ERR_rsv_14    0x00004000 // Reserve Errorbit 14
#define FPGA_ERR_rsv_15    0x00008000 // Reserve Errorbit 15

// FpgaErr_Flags from o_reg32_errorstatus (higher 16 bits)
#define FPGA_WRN_rsv_0    0x00010000 // Reserve Warnbit 0
#define FPGA_WRN_rsv_1    0x00020000 // Reserve Warnbit 1
#define FPGA_WRN_rsv_2    0x00040000 // Reserve Warnbit 2
#define FPGA_WRN_rsv_3    0x00080000 // Reserve Warnbit 3
#define FPGA_WRN_rsv_4    0x00100000 // Reserve Warnbit 4
#define FPGA_WRN_rsv_5    0x00200000 // Reserve Warnbit 5
#define FPGA_WRN_rsv_6    0x00400000 // Reserve Warnbit 6
#define FPGA_WRN_rsv_7    0x00800000 // Reserve Warnbit 7
#define FPGA_WRN_rsv_8    0x01000000 // Reserve Warnbit 8
#define FPGA_WRN_rsv_9    0x02000000 // Reserve Warnbit 9
#define FPGA_WRN_rsv_10    0x04000000 // Reserve Warnbit 10
#define FPGA_WRN_rsv_11    0x08000000 // Reserve Warnbit 11
#define FPGA_WRN_rsv_12    0x10000000 // Reserve Warnbit 12
#define FPGA_WRN_rsv_13    0x20000000 // Reserve Warnbit 13
#define FPGA_WRN_rsv_14    0x40000000 // Reserve Warnbit 14
#define FPGA_WRN_rsv_15    0x80000000 // Reserve Warnbit 15

// FPGA RXTX-Bits from FastSPI
#define FPGA_RXTX_F_RX_LT1    0x0001 // RX Leistungsteil 1
#define FPGA_RXTX_F_RX_LT2    0x0002 // RX Leistungsteil 2
#define FPGA_RXTX_F_IN_RX_SE1    0x0004 // RX sekundär Erweiterung 1
#define FPGA_RXTX_F_IN_RX_SE2    0x0004 // RX sekundär Erweiterung 2
#define FPGA_RXTX_F_SE1    0x0010 // Drive sekundär Erweiterung 1
#define FPGA_RXTX_F_SE2    0x0020 // Drive sekundär Erweiterung 2
#define FPGA_RXTX_Rsv_6    0x0040 // Reserve Bit
#define FPGA_RXTX_Rsv_7    0x0080 // Reserve Bit

// FPGA Z_MLT-Bits from FastSPI
#define FPGA_ZMLT_Rsv_0    0x0001 // Reserve Bit
#define FPGA_ZMLT_ialt_limit_floor    0x0002 // ialt_limit_floor
#define FPGA_ZMLT_ialt_limit_ceil    0x0004 // ialt_limit_ceil
#define FPGA_ZMLT_voltage_limiting    0x0008 // voltage_limiting
#define FPGA_ZMLT_Z_M1_LT1    0x0010 // Z_M1_LT1
#define FPGA_ZMLT_Z_M2_LT1    0x0020 // Z_M2_LT1
#define FPGA_ZMLT_Z_M1_LT2    0x0040 // Z_M1_LT2
#define FPGA_ZMLT_Z_M2_LT2    0x0080 // Z_M2_LT2

// FPGA Enum Status Values
enum FpgaStatValNums {
    FPGA_SVN_N = 16
};
