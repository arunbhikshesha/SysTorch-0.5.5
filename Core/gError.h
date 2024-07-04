#ifndef _GERROR_H
#define _GERROR_H

/*** Include *****************************************************************/
#include "types.h"

/*** Preprocessor definitions ************************************************/
/* NO DEFINITIONS */

/*** Definition of variables *************************************************/

//=========================================================
//  Sub structur  "gError_t"
//  description:  Global Error List - Standard Error List
//=========================================================

#define ERROR_NOATALL        0x0000000000000000u
#define ERROR_TEMPDIODESEK   0x0000000000000001u // E01-01  Standardübertemperatur -Sek Diode
#define ERROR_TEMPMODULPRIM  0x0000000000000002u // E01-02  Standard Übertemperatur- Primär Modul
#define ERROR_UHIGHMAIN      0x0000000000000004u // E02-00  Standard Netz-Überspannung
#define ERROR_UHIGHUZK       0x0000000000000008u // E02-01  Standard Überspannung -UZK
#define ERROR_UHIGHUZKSUP    0x0000000000000010u // E02-02  Standard Überspannung -UZK Startup
#define ERROR_IHIGHOUT       0x0000000000000020u // E03-00  Standard Überstrom
#define ERROR_UHIGHOUT       0x0000000000000040u // E06-00  Standard Überspannung
#define ERROR_EEPROMCRC      0x0000000000000080u // E07-00  Standard EEProm Prüfsummenfehler
#define ERROR_EEPROMINIT     0x0000000000000100u // E07-01  Standard EEProm Initialisierungsfehler
#define ERROR_EEPROMREAD     0x0000000000000200u // E07-02  Standard EEProm Zugriffsfehler
#define ERROR_UOUTDETECT     0x0000000000000400u // E09-00  Standard Spannungs-Erfassung
#define ERROR_TORCH          0x0000000000000800u // E10-00  Standard Brenner-Buchse/Leitung
#define ERROR_TORCHTEMPOVER  0x0000000000001000u // E10-01  Standard Brenner Übertemperatur
#define ERROR_RC             0x0000000000002000u // E11-00  Standard Fernregler-Buchse
#define ERROR_LTDRIVING      0x0000000000004000u // E12-00  Standard Leistungsteil
#define ERROR_TSENSDIODESEK  0x0000000000008000u // E13-01  Standard Temperatur-Sensor -sek Diode
#define ERROR_TSENSMODULPRIM 0x0000000000010000u // E13-02  Standard Temperatur-Sensor - primär Modul
#define ERROR_ULOWINTERN     0x0000000000020000u // E14-00  Standard Versorgungsspannung
#define ERROR_ULOWINTERNSUP  0x0000000000040000u // E14-01  Standard Versorgungsspannung Startup
#define ERROR_IDEETCT        0x0000000000080000u // E15-00  Standard Strom Erfassung
#define ERROR_IDEETCTFPGA    0x0000000000100000u // E15-01  Standard Strom Erfassung FPGA
#define ERROR_IHIGHMAIN      0x0000000000200000u // E16-00  Standard Überstromabschaltung
#define ERROR_IHIGHPERIPH    0x0000000000400000u // E17-00  Standard Peripherie Überlast/Kurzschluss
#define ERROR_PHIGHOUT       0x0000000000800000u // E18-00  Standard Überlastabschaltung
#define ERROR_PHIGHWELD      0x0000000001000000u // E18-01  Standard Überlastabschaltung - Schweißen
#define ERROR_PHIGHOPLOOP    0x0000000002000000u // E18-02  Standard Überlastabschaltung - Leerlauf
#define ERROR_HF             0x0000000004000000u // E19-00  Standard Störung Zündgerät
#define ERROR_UIOUT          0x0000000008000000u // E21-00  Standard Ausg.-Spannung/Strom
#define ERROR_ULOWMAIN       0x0000000010000000u // E22-00  Standard Netzunterspannung
#define ERROR_ULOWMAINSUP    0x0000000020000000u // E22-03  Standard Netzunterspannung Startup
#define ERROR_ULOWUZK        0x0000000040000000u // E22-04  Standard Unterspannung Zwischenkreis
#define EMPTY32              0x0000000080000000u // E23-00  Standard Netzüberspannung
#define ERROR_TORCHWRONG     0x0000000100000000u // E24-00  Standard Brennerüberwachung
#define ERROR_VRD            0x0000000200000000u // E25-00  Standard Voltage Reduction Device
#define ERROR_KONFIG         0x0000000400000000u // E30-01  Standard Fehler Konfiguration
#define ERROR_UIWRONG        0x0000000800000000u // E30-03  Standard Bedienfeld Erkennung
#define ERROR_KBCCONFIG      0x0000001000000000u // E30-05  Standard Kabelbaumkonfiguration
#define ERROR_SWWRONG        0x0000002000000000u // E30-06  Standard Software konfig
#define ERROR_VRDCHANGE      0x0000004000000000u // E30-07  Standard VRD Konfig geändert
#define ERROR_UIIDWRONG      0x0000008000000000u // E30-08  Standard Bedienfeld Kennung falsch
#define ERROR_KOMM           0x0000010000000000u // E31-00  Standard Kommunikation Fehler
#define ERROR_KOMMMASTER     0x0000020000000000u // E31-01  Standard Kommunikation Master
#define ERROR_KOMMUI         0x0000040000000000u // E31-03  Standard Kommunikation Bedienfeld
#define ERROR_KOMMFPGA       0x0000080000000000u // E31-04  Standard Kommunikation FPGA
#define ERROR_KOMMPRIM       0x0000100000000000u // E31-05  Standard Kommunikation primaer
#define ERROR_KOMMPRIMSUP    0x0000200000000000u // E31-06  Standard Kommunikation primaer Startup
#define ERROR_FPGA           0x0000400000000000u // E32-00  Standard FPGA
#define ERROR_FPGAIHIGH      0x0000800000000000u // E32-01  Standard FPGA Ueberstrom
#define ERROR_FPGATIMEOUT    0x0001000000000000u // E32-02  Standard FPGA Timeout
#define ERROR_PGALTTEST      0x0002000000000000u // E32-03  Standard FPGA Leistungsteiltest
#define ERROR_FPGAIPMAX      0x0004000000000000u // E32-04  Standard FPGA IP Max Error
#define ERROR_IHIGHFAN       0x0008000000000000u // E34-00  Standard Lüfter-/Treiberstrom zu hoch
#define ERROR_ILOWFAN        0x0010000000000000u // E34-01  Standard Lüfterstrom zu niedrig
#define ERROR_BATT           0x0020000000000000u // E49-00  Standard Batteriefehler allgemein
#define ERROR_LOWBATT        0x0040000000000000u // E49-01  Standard Ladezustand Batterie niedrig
#define ERROR_KOMMBATT       0x0080000000000000u // E49-02  Standard Kommunikation Batterie
#define ERROR_EMPTY57        0x0100000000000000u // E
#define ERROR_EMPTY58        0x0200000000000000u // E
#define ERROR_EMPTY59        0x0400000000000000u // E
#define ERROR_ULOW_16V       0x0800000000000000u // E
#define ERROR_UHIGH_16V      0x1000000000000000u // E
#define ERROR_ULOW_24V       0x2000000000000000u // E
#define ERROR_UHIGH_24V      0x4000000000000000u // E
#define ERROR_TESTERROR      0x8000000000000000u // ERROR_EMPTY64    Reserved for "No Error"

#define ERROR_MASKTEST 0x7FFFFFFFFFFFFFFF

//---------------------------------------------------------------------------

// ulError - Main error flags definitions
#define ERR_ERROR     0x00000001 // A system locking error has occured
#define ERR_OVERTEMP  0x00000002 // Overtemperature error group
#define ERR_UIN_HIGH  0x00000004 // Overvoltage error group
#define ERR_LOWU      0x00000008 // Low voltage error group
#define ERR_TSENS     0x00000010 // Temperature sensor error
#define ERR_FPGA      0x00000020 // FPGA error group
#define ERR_PRIMARY   0x00000040 // Primary processor error
#define ERR_UEBERLAST 0x00000080 // Sammelfehler unzulässiger Betrieb mit Überlast (kein reguläres Schweissen mehr)
#define ERR_EEPROM    0x00000100 // EEPROM error group #define ERR_SHUNT 0x00000200 //Shunt error group
#define ERR_ITREIBER  0x00000400 // Sammelfehler Über-/Unterstrom Ansteuertreiber/Lüfter
#define ERR_UOUT_HIGH 0x00000800 // Output voltage too high
#define ERR_UMESSUNG  0x00001000 // Fehler in der Spannungserfassung =EN= Errors in the voltage detection
#define ERR_VRD       0x00002000 // VRD Error
#define ERR_LT        0x00004000 // Fehler im LT vom DSP festgestellt
#define ERR_FPGACOM   0x00008000 // FPGA communication error
#define ERR_COMM_CAN  0x00010000 //
#define ERR_BATTCOM   0x00020000 // Battery communication error
#define ERR_USERINT   0x00040000 // User interface communication error
#define ERR_PANEL     0x00080000
#define ERR_Error21   0x00100000
#define ERR_Error22   0x00200000
#define ERR_Error23   0x00400000
#define ERR_Error24   0x00800000
#define ERR_Error25   0x01000000
#define ERR_Error26   0x02000000
#define ERR_Error27   0x04000000
#define ERR_Error28   0x08000000
#define ERR_Error29   0x10000000
#define ERR_Error30   0x20000000
#define ERR_Error31   0x40000000
#define ERR_Error32   0x80000000
/// Group faults
#define GROUP_ERR_MASK                                                                                       \
    (ERR_OVERTEMP | ERR_HIGHUZK | ERR_LOWU | ERR_TSENS | ERR_FPGA | ERR_UEBERLAST | ERR_EEPROM | ERR_SHUNT | \
     ERR_ITREIBER | ERR_OVERUOUT | ERR_UMESSUNG | ERR_VRD | ERR_LT)

/*
//ulErrorSub -
#define ALL_ERR             0xFFFFFFFE  ///< All errors except ERR_ERROR
#define NO_ERROR            0x00000000  //reset all error bits
#define SOLO_ERR            (0)          ///< Single error
// Priorities in sequence: X350 (V1.10)
//Sub errors flags definitions
#define ERRS_UIN_HIGH       0x00000001  //P0 1 Input voltage is too high
//#define ERRS_UZKUE_PIC    0x00000002  //P0 2 Fehler Uzk ueberU gemessen im PIC
//#define ERRS_UZKUE_FPGA   0x00000004  //P0 3 Fehler Uzk ueberU gemessen im FPGA
#define ERRS_UIN_HIGH_SUP   0x00000008  //P0 4 During Startup input voltage is too high
#define ERRS_UOUT_HIGH      0x00000010  //P2 5 Output (welding) voltage is too high
#define ERRS_FPGA_IPERR     0x00000020  //P3
#define ERRS_FPGA_TIMEOUT   0x00000040  //P3
#define ERRS_FPGA_IPMAX     0x00000080  //P3 8 FPGA IPMAX Error
#define ERRS_FPGA_STARTUP   0x00000100  //FPGA not READY on startup
//#define ERRS_UZKSYM       0x00000100  //P4 9 Fehler Uzk Symmetrie
//#define ERRS_SU_UZKSYM    0x00000200  //P4 10 Startup Fehler Uzk Symmetrie
#define ERRS_U16V_LOW       0x00000400  //P5 11 Voltage on 16V bus is too low.
#define ERRS_U16V_LOW_SUP   0x00000800  //P5 12 Voltage on 16V bus is too low during startup.
#define ERRS_UIN_LOW_SUP    0x00001000  //P6 13 During Startup input voltage is too low
#define ERRS_TEMPDIODE      0x00002000  //P7 14 Diode overtemperature
#define ERRS_TEMPIGBT       0x00004000  //P7 15 IGBT transistors overtemperature
#define ERRS_TEMPOVERLOAD   0x00008000  //P7 16 Overheat and overload shutdown
#define ERRS_TEMPDIODEAVER  0x00010000  //P7 17 Average diode overtemperature
#define ERRS_CSWDG          0x00020000  //P9 18 CS FPGA watchdog error
#define ERRS_SU_IOFFSET     0x00040000  //P13 19 zu grosser Stromoffsetwert, deutet auf Fehler in HW hin
#define ERRS_COMM_PRIMARY   0x00080000  // Communication with the primary uC is borken
#define ERRS_LT_SU          0x00100000  //P14 21 Fehler beim Startuptest des Leistungsteils
#define ERRS_INT_ERR_PRIMARY 0x00200000  //Internal error in primary processor
#define ERRS_TSENSDIO       0x00400000  //P16 23 Missing or faulty diode temperature sensor
#define ERRS_PANELCOM       0x00800000  // Communication with a display user interface is broken
#define ERRS_PANELINIT      0x01000000  // Display user interface wasn't initialized
#define ERRS_VRD            0x02000000  //P17 26 VRD defekt
#define ERRS_BATTCOMM       0x04000000  // Battery communication doesn't work
//#define ERRS_UNTERILUE    0x08000000  //P18 28 Lüfterstrom zu niedrig
#define ERRS_EEPROMINIT     0x10000000  //P20 29 EEPROM Init Error
#define ERRS_EEPROM         0x20000000  //P20 30 Fehler bei EEPROM Zugriff
#define ERRS_OVERLOAD       0x40000000  //P99 31 Power limitation - welding voltage too high when loop is
closed. #define ERRS_UOUT_MEASUR    0x80000000  //32 Voltage measurement circuit is broken

#define SUBERR_BITS_NO      32  ///< Number of bits of Suberror registry.
#define EMPTY_SUBERR        {0,0,99}
*/

/*** Prototypes of functions *************************************************/
/* NO DEFINITIONS */

#endif /*_GERROR_H */
