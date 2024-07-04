#pragma once

#include <stdint.h>

struct __attribute__((__packed__, __aligned__(8))) EEPRom {
    uint32_t Version;      //                              Version of Struct
    uint32_t SerialNo[4];  // /System/Serial               single uint32_t may be to less for SerialNo with Datecode etc.
    uint32_t CalFactor;    // /System/CalFactor            Mapro Independent Calibration Factor
    uint32_t CalOffset;    // /System/CalOffset            Mapro Independent Calibration Offset
    uint32_t VRDType;      // /System/VRDType              enum VRDList ID
    uint32_t SystemConfID; // /System/SystemConfID         enum defines Konfiguration of System
    uint32_t WorkingTime;  // /System/Timer/WorkingTime    WorkingTime in Seconds
    uint32_t WeldingTime;  // /System/Timer/WeldingTime    WeldingTime in Seconds  
    uint32_t Version2;     //                              Version2 of Struct for lenght checking
};
