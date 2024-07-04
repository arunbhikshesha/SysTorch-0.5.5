/*
******************************************************************************
* @file errors.c
* @author
* @brief
******************************************************************************
*
******************************************************************************
*/

/*** Include *****************************************************************/
#include "errors.h"
#include "gData.h"
/* NO MORE DEFINITIONS */

/*** Preprocessor definitions ************************************************/
/* NO DEFINITIONS */

/*** Definition of variables *************************************************/
typedef struct {
    uint8_t ucMainID;   // The main ID with is shown as "E__"
    uint8_t ucSubID;    // The sub ID with is shown as "-__"
    uint8_t ucPriority; // Showing priority
} ErrDispParam;

const ErrorTableStruct ErrorTable[ERROR_TABLE_CNT] = {
    {1, 1, 1},   // 0x0000000000000001  //E01-01 Standard Übertemperatur -Sek Diode
    {2, 2, 2},   // 0x0000000000000002  //E01-02 Standard Übertemperatur- Primär Modul
    {2, 0, 3},   // 0x0000000000000004  //E02-00 Standard Netz-Überspannung
    {2, 1, 4},   // 0x0000000000000008  //E02-01 Standard Überspannung -UZK
    {2, 2, 5},   // 0x0000000000000010  //E02-02 Standard Überspannung -UZK Startup
    {3, 0, 6},   // 0x0000000000000020  //E03-00 Standard Überstrom
    {6, 0, 7},   // 0x0000000000000040  //E06-00 Standard Überspannung
    {7, 0, 8},   // 0x0000000000000080  //E07-00 Standard EEProm Prüfsummenfehler
    {7, 1, 9},   // 0x0000000000000100  //E07-01 Standard EEProm Initialisierungsfehler
    {7, 2, 10},  // 0x0000000000000200  //E07-02 Standard EEProm Zugriffsfehler
    {9, 0, 11},  // 0x0000000000000400  //E09-00 Standard Spannungs-Erfassung
    {10, 0, 12}, // 0x0000000000000800  //E10-00 Standard Brenner-Buchse/Leitung
    {10, 1, 13}, // 0x0000000000001000  //E10-01 Standard Brenner Übertemperatur
    {11, 0, 14}, // 0x0000000000002000  //E11-00 Standard Fernregler-Buchse
    {12, 0, 15}, // 0x0000000000004000  //E12-00 Standard Leistungsteil
    {13, 1, 16}, // 0x0000000000008000  //E13-01 Standard Temperatur-Sensor -sek Diode
    {13, 2, 17}, // 0x0000000000010000  //E13-02 Standard Temperatur-Sensor - primär Modul
    {14, 0, 18}, // 0x0000000000020000  //E14-00 Standard Versorgungsspannung
    {14, 1, 19}, // 0x0000000000040000  //E14-01 Standard Versorgungsspannung Startup
    {15, 0, 20}, // 0x0000000000080000  //E15-00 Standard Strom Erfassung
    {15, 1, 21}, // 0x0000000000100000  //E15-01 Standard Strom Erfassung FPGA
    {16, 0, 22}, // 0x0000000000200000  //E16-00 Standard Überstromabschaltung
    {17, 0, 23}, // 0x0000000000400000  //E17-00 Standard Peripherie Überlast/Kurzschluss
    {18, 0, 24}, // 0x0000000000800000  //E18-00 Standard Überlastabschaltung
    {18, 1, 25}, // 0x0000000001000000  //E18-01 Standard Überlastabschaltung -  Schweißen
    {18, 2, 26}, // 0x0000000002000000  //E18-02 Standard Überlastabschaltung - Leerlauf
    {19, 0, 27}, // 0x0000000004000000  //E19-00 Standard Störung Zündgerät
    {21, 0, 28}, // 0x0000000008000000  //E21-00 Standard Ausg.-Spannung/Strom
    {22, 0, 29}, // 0x0000000010000000  //E22-00 Standard Netzunterspannung
    {22, 3, 30}, // 0x0000000020000000  //E22-03 Standard Netzunterspannung Startup
    {22, 4, 31}, // 0x0000000040000000  //E22-04 Standard Unterspannung Zwischenkreis
    {23, 0, 32}, // 0x0000000080000000  //E23-00 Standard Netzüberspannung
    {24, 0, 33}, // 0x0000000100000000  //E24-00 Standard Brennerüberwachung
    {25, 0, 34}, // 0x0000000200000000  //E25-00 Standard Voltage Reduction Device
    {30, 1, 35}, // 0x0000000400000000  //E30-01 Standard Fehler Konfiguration
    {30, 3, 36}, // 0x0000000800000000  //E30-03 Standard Bedienfeld Erkennung
    {30, 5, 37}, // 0x0000001000000000  //E30-05 Standard Kabelbaumkonfiguration
    {30, 6, 38}, // 0x0000002000000000  //E30-06 Standard Software konfig
    {30, 7, 39}, // 0x0000004000000000  //E30-07 Standard VRD Konfig geändert
    {30, 8, 40}, // 0x0000008000000000  //E30-08 Standard Bedienfeld Kennung falsch
    {31, 0, 41}, // 0x0000010000000000  //E31-00 Standard Kommunikation  Fehler
    {31, 1, 42}, // 0x0000020000000000  //E31-01 Standard Kommunikation  Master
    {31, 3, 43}, // 0x0000040000000000  //E31-03 Standard Kommunikation  Bedienfeld
    {31, 4, 44}, // 0x0000080000000000  //E31-04 Standard Kommunikation  FPGA
    {31, 5, 45}, // 0x0000100000000000  //E31-05 Standard Kommunikation primaer
    {31, 6, 46}, // 0x0000200000000000  //E31-06 Standard Kommunikation primaer Startup
    {32, 0, 47}, // 0x0000400000000000  //E32-00 Standard FPGA
    {32, 1, 48}, // 0x0000800000000000  //E32-01 Standard FPGA Ueberstrom
    {32, 2, 49}, // 0x0001000000000000  //E32-02 Standard FPGA Timeout
    {32, 3, 50}, // 0x0002000000000000  //E32-03 Standard FPGA Leistungsteiltest
    {32, 4, 51}, // 0x0004000000000000  //E32-04 Standard FPGA IP Max Error
    {34, 0, 52}, // 0x0008000000000000  //E34-00 Standard Lüfter-/Treiberstrom zu hoch
    {34, 1, 53}, // 0x0010000000000000  //E34-01 Standard Lüfterstrom zu niedrig
    {49, 0, 54}, // 0x0020000000000000  //E49-00 Standard Batteriefehler allgemein
    {49, 1, 55}, // 0x0040000000000000  //E49-01 Standard Ladezustand Batterie niedrig
    {49, 2, 56}, // 0x0080000000000000  //E49-02 Standard Kommunikation Batterie
    {99, 2, 57}, // 0x0100000000000000  //E00-00 Empty
    {99, 3, 58}, // 0x0200000000000000  //E00-00 Empty
    {99, 4, 59}, // 0x0400000000000000  //E00-00 Empty
    {99, 5, 60}, // 0x0800000000000000  //E00-00 Empty
    {99, 6, 61}, // 0x1000000000000000  //E00-00 Empty
    {99, 7, 62}, // 0x2000000000000000  //E00-00 Empty
    {99, 8, 63}, // 0x4000000000000000  //E00-00 Empty
    {99, 9, 64}  // 0x8000000000000000  //E00-00 Empty
};

/*** Prototypes of functions *************************************************/
void ErrorShutdown(void);
// uint16_t GetErrorToShow(void);

/*** Definitions of functions ************************************************/
/* NO DEFINITIONS */

/**********************************************************
 ** Name            : errorHandler
 **
 ** Created from/on : WRN / 11.04.2017
 **
 ** Description     : This function is called periodically and check hardware
 **                   status
 **
 ** Calling         : while(1)
 **
 ** InputValues     : none
 ** OutputValues    : none
 **********************************************************/
void errorHandler(void) {

    gData.SystemStat.ulMainStat |= MS_LOCKED; // Lock the machine and disable process
}

/**********************************************************
 ** Name            : errors_Handler
 **
 ** Created from/on : RM / 19.01.2016
 **
 ** Description     : This method is called periodically and check suberror and
 **                   analyze suberror and next set error if needed
 **
 ** Calling         : In main loop
 **
 ** InputValues     : none
 ** OutputValues    : none
 **********************************************************/
void errors_Handler(void) {}

/**********************************************************
 ** Name            : ErrorShutdown
 **
 ** Created from/on : RM / 19.01.2016
 **
 ** Description     : Funciton takes care of process stop
 **                   in case of critical errors
 **
 ** Calling         : In main loop
 **
 ** InputValues     : none
 ** OutputValues    : none
 **********************************************************/
void ErrorShutdown(void) {
    gData.SystemStat.ulMainStat |= MS_LOCKED; // Lock the machine and disable process
                                              //  // Power section need to be stopped immediately.
    //  // Important to stop power source before writing error code to flash memory
    //  // otherwise on SPI bus is a time lag and FPGA watchdog error occurs.
    //  StopPwrSourceImmediately();
    //
    //  // Disconnect the power relay when overvoltage is detected on the input and
    //  // the unit is powered from the mains.
    //  if ((Errors & ERR_UIN_HIGH) && (gData.SystemStat.ulSupplyStat & SUP_MAINS_DETECT))
    //  {
    //    DigIOSetOut(DIGO_PWR_REL, DIGIO_DISABLE);
    //  }
}
