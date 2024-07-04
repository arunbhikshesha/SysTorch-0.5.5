/*******************************************************************************
*
* E M B E D D E D   W I Z A R D   P R O J E C T
*
*                                                Copyright (c) TARA Systems GmbH
*                                    written by Paul Banach and Manfred Schweyer
*
********************************************************************************
*
* This software is delivered "as is" and shows the usage of other software
* components. It is provided as an example software which is intended to be
* modified and extended according to particular requirements.
*
* TARA Systems hereby disclaims all warranties and conditions with regard to the
* software, including all implied warranties and conditions of merchantability
* and non-infringement of any third party IPR or other rights which may result
* from the use or the inability to use the software.
*
********************************************************************************
*
* DESCRIPTION:
*   This file is part of the interface (glue layer) between an Embedded Wizard
*   generated UI application and the board support package (BSP) of a dedicated
*   target.
*
*******************************************************************************/

#ifndef EW_BSP_FLASH_READER_H
#define EW_BSP_FLASH_READER_H


#ifdef __cplusplus
  extern "C"
  {
#endif


/*******************************************************************************
* FUNCTION:
*   EwBspFlashReaderInit
*
* DESCRIPTION:
*   The function EwBspFlashReaderInit() is used to initialize a flash driver in
*   order to access an external flash that is not in the CPU address range.
*   Furthermore, a callback function is registered that is called by Graphics
*   Engine or Runtime Environment when a new block of data has to be read.
*
* ARGUMENTS:
*   aStartAddress - Address that is used by the linker as start address for the
*     external flash. The address cannot be accessed directly by CPU.
*   aSize         - Size of the flash memory area in bytes.
*
* RETURN VALUE:
*   Returns 1 if successful, 0 otherwise.
*
*******************************************************************************/
int EwBspFlashReaderInit
(
  void* aStartAddress,
  int  aSize
);


#ifdef __cplusplus
  }
#endif

#endif /* EW_BSP_FLASH_READER_H */


/* msy */
