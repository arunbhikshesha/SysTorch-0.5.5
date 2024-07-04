/**
 ******************************************************************************
 * @file    ParaObject.h
 * @author  Objects_V4.xls Generated
 * @brief   Parameter Data from Code Generator
 * @brief   Structs and initial Values for J&D Parameters
 ******************************************************************************
 **/

// S4_Config Version :0.18.1
#pragma once
#include "stdint.h"
#include "ParaEnums.h"

 //  JobParameter Struct Wertaustausch 
typedef struct {
    float JParaSettings[JS_PAR_N];
    uint32_t JParaControl[JC_PAR_N];
}J_ParaSC_struct_t ;

 //  Formaterweiterung für JobSetting und DokuParameter zur Anzeige und Verarbeitung im Backend
typedef struct {
    float Norm;   //default Value
    float Min;    //min limit
    float Max;    //max limit
    uint32_t Mask;  //MaskID
    uint32_t Unit;  //Unit ID
	float Step;    //Step Size
	uint32_t Attr;  //Attribute ATTR_Bits  
}JD_ParSetting_extended_struct_t ;

 //  Formaterweiterung für Job Control zur Anzeige und Verarbeitung im Backend, Struktur gilt auch für DokuParameter
typedef struct {
    uint32_t Norm; //default Value
    uint32_t Min;  //min limit
    uint32_t Max;  //max limit
    uint32_t Type; //Value = 0, Bitfield = 1
}JD_ParControl_extended_struct_t ;

 //  GesamtStruktur für Job, DokuParameter extended Values
typedef struct {
    JD_ParSetting_extended_struct_t JParaSettings_ex[JS_PAR_N];  //Formaterweiterung für JobSettings 
    JD_ParControl_extended_struct_t JParaControl_ex[JC_PAR_N];   //Formaterweiterung für JobControl
    JD_ParSetting_extended_struct_t DParaSettings_ex[vout_N];   //Formaterweiterung für Doku DPara
}JD_Para_extended ;

