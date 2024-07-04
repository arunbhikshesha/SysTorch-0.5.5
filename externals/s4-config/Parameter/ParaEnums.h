/**
 ******************************************************************************
 * @file    ParaEnums.h
 * @author  Objects_V4.xls Generated
 * @brief   Enums for Job and Doku Parameter, Parameter Data from Code Generator
 ******************************************************************************
//Parameter and Units are defned in Objects_V4.xls located in S4-Config https://github.com/LorchAW/S4_Config
 **/
#pragma once


/**
 * @brief  Indizes Doku-Parameter
 */
enum vout_DokuPars {
    vout_SeamTracking,               //!< Seam Tracking Arc Length Signal[ ]
    vout_I_Proj,                     //!< Strom Prognosewert[A]
    vout_U_Proj,                     //!< Spannung Prognosewert[V]
    vout_P_Proj,                     //!< Leistung Prognosewert[W]
    vout_DV_Proj,                    //!< Drahtvorschub Prognosewert[m/min]
    vout_I_Ist,                      //!< Strom Istwert[A]
    vout_I_Hold,                     //!< Strom Holdwert[A]
    vout_U_Ist,                      //!< Spannung Istwert[V]
    vout_U_Hold,                     //!< Spannung Holdwert[V]
    vout_P_Ist,                      //!< Leistung Istwert[W]
    vout_P_Hold,                     //!< Leistung Holdwert[W]
    vout_KS_Prozessinfo,             //!< Info Prozesstyp KLB[Faktor]
    vout_Puls_Prozessinfo,           //!< Info Prozesstyp Puls[Faktor]
    vout_dMat_Empf,                  //!< Blechdicke Empfehlung[mm]
    vout_d2Mat_Empf,                 //!< Blechdicke Blech 2 Empfehlung[mm]
    vout_a_Mass_Empf,                //!< a-Maß Empfehlung[mm]
    vout_vWeld_Empf,                 //!< Schweißgeschwindigkeit Empfehlung[cm/min]
    vout_tWeld_Ist,                  //!< Schweißzeit Istwert[s]
    vout_GasDF_Empf,                 //!< Gasdurchflussmenge Empfehlung oder Sollwert wenn Regelung[l/min]
    vout_DV_Einschl_Proj,            //!< Prognosewert korrigierte Drahteinschleichgeschwindigkeit[m/min]
    vout_RauchAbs_Empf,              //!< Schweißrauchabsaugleistung Empfehlung[m³/h]
    vout_P_Emission_Proj,            //!< Emissionsleistung(Prognose)[W]
    vout_dv_DMR_Soll,                //!< Steilheit Drahtvorschub-Änderung Soll[m/min/s]
    vout_DMR_Soll,                   //!< Sollwert Drahtvorschub Motorregler[m/min]
    vout_BrennKuehl_Empf,            //!< Brennerkühlleistung Empfehlung[W]
    vout_UniversalPara1,             //!< Universeller Parameter 1 (R)[ ]
    vout_UniversalPara2,             //!< Universeller Parameter 2 (L)[ ]
    vout_UniversalPara3,             //!< Universeller Parameter 3[ ]
    vout_UniversalPara4,             //!< Universeller Parameter 4[ ]
    vout_UniversalPara5,             //!< Universeller Parameter 5[ ]
    vout_fIist_GLV,                  //!< geglätteter Stromistwert[A]
    vout_fUist_GLV,                  //!< geglätteter Spannungsistwert[V]
    vout_fPist_GLV,                  //!< geglätteter Leistungswert[W]
    vout_fIIist_GLV,                 //!< geglätteter quadratischer Stromistwert[A]
    vout_fUUist_GLV,                 //!< geglätteter quadratischer Spannungsistwert[V]
    vout_fIsollDIist_GLV,            //!< geglättet Stromsollwert - Stromistwert[A]
    vout_Scr_GLV,                    //!< Short Circuit Ratio[ ]
    vout_ScAtP_GLV,                  //!< Short Circuit Arc Time Period[ ]
    vout_rVdv_GLV,                   //!< Reference Voltage Deviation Value[V]
    vout_rVdv_rGLV,                  //!< Reference Voltage Deviation Value Reversion[V]
    vout_oVdv_GLV,                   //!< Over Voltage Deviation Value[V]
    vout_oVdv_rGLV,                  //!< Over Voltage Deviation Value Reversion[V]
    vout_uVdv_GLV,                   //!< Under Voltage Deviation Value[V]
    vout_uVdv_rGLV,                  //!< Under Voltage Deviation Value Reversion[V]
    vout_dU_wvm_GLV,                 //!< Weighted Voltage[V]
    vout_SP1_GLV,                    //!< Sendekanal 1 Prozess[ ]
    vout_SP2_GLV,                    //!< Sendekanal 2 Prozess[ ]
    vout_SP3_GLV,                    //!< Sendekanal 3 Prozess[ ]
    vout_rsvPara48,                  //!< Reserve Parameter 1[ ]
    vout_rsvPara49,                  //!< Reserve Parameter 2[ ]
    vout_rsvPara50,                  //!< Reserve Parameter 3[ ]
    vout_N
};




/**
 * @brief  Indizes JobSetting
 */
enum JobSettingIDs {
    JS_undef,                //!< Undef[ ]
    JS_tPreGas,              //!< Gasvorströmzeit (absolut)[s]
    JS_vWireCreep,           //!< Drahteinschleichgeschwindigkeit (absolut)[m/min]
    JS_Start_Erel,           //!< Startenergie in % von Hauptphase[Faktor]
    JS_Start_ArcLen,         //!< Start-Lichtbogenlänge[Faktor]
    JS_Start_t,              //!< Startzeit[s]
    JS_Main_tSlope,          //!< Start zu Hauptenergie Rampenzeit[s]
    JS_Main_vWire,           //!< Drahtvorschubgeschwindigkeit Sollwert Hauptphase[m/min]
    JS_Main_vWireRel,        //!< Drahtvorschub-Korrektur in [Faktor]
    JS_Main_U,               //!< Spannung Sollwert Hauptphase [V]
    JS_Main_I,               //!< Strom Sollwert Hauptphase [A]
    JS_Main_ArcLen,          //!< Lichtbogenlänge Hauptphase [Faktor]
    JS_Dynamics,             //!< Dynamik Sollwert [Faktor]
    JS_ArcForce,             //!< ArcForce[Faktor]
    JS_2d_tSlope,            //!< Slopezeit von Hauptenergie zu Zweitenergie und zurück[s]
    JS_2d_Erel,              //!< Zweitenergie von Hauptphase[Faktor]
    JS_Pulse_f,              //!< Puls-Frequenz[Hz]
    JS_Pulse_Bal,            //!< Puls-Tastverhältnis[Faktor]
    JS_TwinRange,            //!< Twin Energieänderung[Faktor]
    JS_Twin_f,               //!< Frequenz Twinpuls[Hz]
    JS_Twin_Bal,             //!< Balance Twinpuls[Faktor]
    JS_Stwin_Ht,             //!< SuperTwin High Zeit[s]
    JS_Stwin_Lt,             //!< SuperTwin Low Zeit[s]
    JS_2d_ArcLen,            //!< Lichtbogenlänge 2[Faktor]
    JS_2d_vWireRel,          //!< Drahtkorrektur 2[Faktor]
    JS_End_tSlope,           //!< End_Slopezeit (von Hauptenergie auf Endenergie)[s]
    JS_End_Erel,             //!< Endenergie von Hauptphase [Faktor]
    JS_End_ArcLen,           //!< End-Libolänge[Faktor]
    JS_End_vWire,            //!< EndDV (absolut) (manuell)[m/min]
    JS_End_U,                //!< Endspannung (absolut) (manuell)[V]
    JS_End_t,                //!< Endzeit[s]
    JS_WireBurnBack,         //!< Drahtrückbrandzeit (Korrektur)[Faktor]
    JS_tPostGas,             //!< Gasnachströmzeit absolut[s]
    JS_tPostGas_rel,         //!< Gasnachströmzeit relativ (Korrektur)[Faktor]
    JS_End_Detection,        //!< Schweißendeerkennung[Faktor]
    JS_tSpotOn,              //!< Schweißzeit Punkten / Intervall[s]
    JS_tSpotOff,             //!< Pausenzeit Intervall[s]
    JS_Start_vWire,          //!< StartDV (absolut) (manuell)[m/min]
    JS_Start_U,              //!< Startspannung (absolut) (manuell)[V]
    JS_Remote_Imin,          //!< Minimalstrombegrenzung (vlt nur verwenden wenn Fußfernregler)[A]
    JS_Remote_Imax,          //!< Max. Strombegrenzung[A]
    JS_Start_E,              //!< KorrekturLichtbogenstart (Beeinflussung von Zündpeakhöhe, Zündpeakdauer, Zündpulshöhe, Zündpulsdauer)[Faktor]
    JS_End_E,                //!< KorrekturLichtbogenende (Beeinflussung Endpulshöhe, Endpulsdauer, u.a.)[Faktor]
    JS_vWire_toHigh,         //!< Drahtumschaltung zu LOW (Korrektur)[ms]
    JS_vWire_toLow,          //!< Drahtumschaltung zu HIGH (Korrektur)[ms]
    JS_ArcRegSpeed,          //!< Lichtbogen-Reglegeschwindigkeit[Faktor]
    JS_WeldingSpeed,         //!< Schweißgeschwindigkeit[cm/min]
    JS_TwinSlope,            //!< TwinSlope[Faktor]
    JS_Res48,                //!< Res48[ ]
    JS_Res49,                //!< Res49[ ]
    JS_Res50,                //!< Res50[ ]
    JS_Res51,                //!< Res51[ ]
    JS_Res52,                //!< Res52[ ]
    JS_PAR_N
};

/**
 * @brief  Indizes Job-Control
 */
enum JobControlIDs {
    JC_undef,                //!< 
    JC_ProgNo,               //!< Programmnummer war J_SynProgNo
    JC_ProgRev,              //!< ProgrammnummerVersion
    JC_ProcessId,            //!< Verfahrens-ID z.b. MIGMag; Elektorde; WIG Heißdraht PID_ProcessID
    JC_WeldMode,             //!< Bitfield mit WELD_Mode_Bits
    JC_Flags,                //!< Bitfield mit JF_BITS
    JC_PAR_N
};

/**
 * @brief WELD_Mode_Bits is a Bitfield for Stroke, Slope and other in a Job Saveable Bits true = active. Also Used in Weldprog Database true = activation possible
 */
#define WM_4stroke           0x00000001    //!< 4-Stroke active, (default is 2-Stroke)
#define WM_Spot              0x00000002    //!< Spot welding for time tSpotOn (in 2-Stroke mode)
#define WM_SpotIntv          0x00000004    //!< Interval (needs also WM_Spot)
#define WM_4stroke2          0x00000008    //!< Special 4-Stroke (needs also WM_4stroke)
#define WM_TestMode          0x00000010    //!< keine Zwangsabschaltung + nur Hauptstrom-Phase aktiv
#define WM_Autostart         0x00000020    //!< no 'Welding start' signal/torch switch required for Start (e.g. for Stick welding)
#define WM_Start_I           0x00010000    //!< Startphase Aktiv
#define WM_Start_Slope       0x00020000    //!< Start-Slope Aktiv
#define WM_2d_Start_Slope    0x00040000    //!< Slope nach Zweitstrom Aktiv
#define WM_2d_End_Slope      0x00080000    //!< Slope zurück von Zweitstrom Aktiv
#define WM_End_I             0x00100000    //!< Endphase Aktiv
#define WM_End_Slope         0x00200000    //!< End-Slope Aktiv
#define WM_No_Ign            0x00400000    //!< Nicht warten auf Zünden/I>0, gleich Ablauf fortsetzen
#define WM_Twin_ON           0x00800000    //!< TWIN Funktion (in Prozess zykl. Wechsel zwischen 2 Energieniveaus)
#define WM_HFIGN             0x01000000    //!< HF Zündung Ein
#define WM_ARCLEN_vWireRel   0x02000000    //!< Lichtbogenlänge über Draht (JS_Main_vWireRel) steuern

/**
 * @brief Job Flag Bits idikatoren from  external Sensor or Startrequests, not saveable
 */
#define JF_TouchSense            0x00000001    //!< Touchsense via LLGen aktivieren
#define JF_Calibration           0x00000002    //!< Kalibrierung läuft
#define JF_Adjustment            0x00000004    //!< Justage läuft
#define JF_AdjustOcGen           0x00000008    //!< Justage LLGen aktiv

/**
 * @brief Verfahrens-ID z.b. MIGMag; Elektorde; WIG Heißdraht PID_ProcessID
 */
enum PID_ProcessID {
    PID_none,                //!< undefined
    PID_TIG,                 //!< VerfahrenWIG
    PID_MMA,                 //!< Verfahren Elektrode 
    PID_MigMagMan,           //!< Mig-Mag Manuel
    PID_MigMagSyn,           //!< Mig-Mag Synergic
    PID_N
};
/**
 * @brief ATTR_Bits  Bitfield zur identifikation in welchem kontext ein Parameter genutzt wird
 */
#define ATTR_MAIN                0x00000001    //!< Main Anzeige
#define ATTR_ACT                 0x00000002    //!< Actual Anzeige Istwert
#define ATTR_HOLD                0x00000004    //!< HOLD Anzeige Holdwert
#define ATTR_START               0x00000008    //!< Parameter in der Startphase
#define ATTR_END                 0x00000010    //!< Parameter in der Endphase
#define ATTR_EXPERT              0x00000020    //!< Expertenparameter
#define ATTR_BASE                0x00000040    //!< 
#define ATTR_FORE                0x00000080    //!< Forecast Values
#define ATTR_RECOM               0x00000100    //!< Recommendation Values
#define ATTR_2T                  0x00010000    //!< Nur bei 2Stroke
#define ATTR_IND4T               0x00020000    //!< Nur bei Nur bei IND4T
#define ATTR_Slopes              0x00040000    //!< Nur wenn slopes aktiv
#define ATTR_CF                  0x00080000    //!< Nur wenn Craterfill aktiv
#define ATTR_TWIN                0x00100000    //!< Nur wenn Twin aktiv
#define ATTR_Pulsen              0x00200000    //!< Nur wenn Pulsen aktiv
#define ATTR_Punkten             0x00400000    //!< Nur wenn Punkten 2T
#define ATTR_Intervall           0x00800000    //!< Nur wenn Intervall
