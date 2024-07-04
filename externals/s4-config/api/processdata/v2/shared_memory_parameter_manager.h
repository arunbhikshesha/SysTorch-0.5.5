/**
 ******************************************************************************
 * @file    shared_memory_parameter_manager.h
 * @author  AS
 * @brief   Gemeinsame Definitionen für den Parameteraustausch über Shared-Memory
 ******************************************************************************
 */
#pragma once

#include <stdint.h>
#include <Parameter/ParaEnums.h>
#include <Parameter/ParaObject.h>

/**
    @brief  Version of this shared-memory-definition

    Everytime, when the structure of the shared-memory-definition changes, this
    number must be incremented.
 */
#define SHM_VERSION 3

/**
    Indicates that a this parameters is in use. When set to 0 the writer of
    the shared memory will not set or even initialize the value and a reader
    must not use it in any way.
*/
#define FLAG_PARAMETER_USED 0x00000001

/**
    When set to 1, indicates that this parameters value has changed. Writers of the
    shared-memory should set this flag to 1 before posting the semaphore. Readers
    of the shared-memory should check this flag after waiting on the semaphore.
*/
#define FLAG_PARAMETER_CHANGED 0x00000002

/**
    When set to 1, indicates that this parameter-extended value has changed. Writers of the
    shared-memory should set this flag to 1 before posting the semaphore. Readers
    of the shared-memory should check this flag after waiting on the semaphore.
*/
#define FLAG_PARAMETER_EXTENDED_CHANGED 0x00000004

/**
    @brief  Layout of Parameter Values Job and DokuParameter aka ParameterManager relevant Data
    This Struct is used in the systems Parameter manager and Shared Memory and also used im HMI Requests
 */
struct __attribute__((__packed__, __aligned__(4))) SHMParameterValues {
    /**
        Version of the shared-memory-definition. This field must be ckecked by every
        user and when it does not match the own version it is not allowed to access
        the shared-memory in any way.
     */
    uint32_t Version;

    /**
        This array holds the values for each job-settings-parameter.
     */
    float JParaSettings[JS_PAR_N];
    /**
        This array holds the values for each job-control-parameter.
     */
    uint32_t JParaControl[JC_PAR_N];
    /**
        This array holds the values for each doku-parameter.
     */
    float DokuPara[vout_N];

    /**
        The owner (creator) of the shared-memory sets this version to tha same
        value as `Version`. When `Version` matches a user should also check this
        field to make sure compiler-alignment and padding is correct.
     */
    uint32_t Version2;
};

/**
    @brief  Layout of Parameter extended Format Data aka
    This Struct is used in the systems Parameter manager and Shared Memory and also used im HMI Requests

 */
struct __attribute__((__packed__, __aligned__(4))) SHMParameterExtended {
    /**
        Version of the shared-memory-definition. This field must be ckecked by every
        user and when it does not match the own version it is not allowed to access
        the shared-memory in any way.
     */
    uint32_t Version;

    /**
        This array holds the extended parameters for each job-settings-parameter
     */
    JD_ParSetting_extended_struct_t      JParaSettings_ex[JS_PAR_N];
    /**
        This array holds the extended parameters for each job-control-parameter
     */
    JD_ParControl_extended_struct_t      JParaControl_ex[JC_PAR_N];
    /**
        This array holds the extended parameters for each doku-parameter
     */
    JD_ParSetting_extended_struct_t      DParaSettings_ex[vout_N];

    /**
        The owner (creator) of the shared-memory sets this version to tha same
        value as `Version`. When `Version` matches a user should also check this
        field to make sure compiler-alignment and padding is correct.
     */
    uint32_t Version2;
};

/**
    @brief  Layout of a Shared-Memory for exchanging parameter-values and the extended Parameter Format Data

    After opening and mmapping the shared memory, the returned pointer must be casted
    to this structure to access the shared memory. Users of the shared-memory must
    check the version before using it.
 */
struct __attribute__((__packed__, __aligned__(4))) SHMParameter {
    uint32_t Version;
    /**
        This structure holds the memory layout for the parameter-values.
     */
    struct SHMParameterValues      ParameterValues;
    /**
        This structure holds the memory layout for the extended parameters and flags.
     */
    struct SHMParameterExtended    ParameterExtended;

    /**
        This array holds the flags for each job-settings-parameter (only used to synchorize shm)
     */
    uint32_t JParaSettingsFlags[JS_PAR_N];

    /**
        This array holds the flags for each job-control-parameter (only used to synchorize shm)
     */
    uint32_t JParaControlFlags[JC_PAR_N];

    /**
        This array holds the values for each doku-parameter. (only used to synchorize shm)
     */
    uint32_t DParaFlags[vout_N];

    uint32_t Version2;
};
