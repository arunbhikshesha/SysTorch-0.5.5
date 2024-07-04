# Changelog

## [0.5.5] - 2024-05-23
### Added
- Old/New PCB detection by reading PA10. HIGH: OLD PCB (PA10 is not connected), LOW: New PCB (PA10 is pulled to GND).
- CTR_LED_EXT, CTR_LED_RH1, CTR_LED_RH2, I2C_SDA, I2C_SCL are initialized based on PBC Type. 
- Either I2C2 or I2C3 is initialized based on the PCB type. I2C2: OlD PCB, I2C3: New PCB.

## [0.5.4] - 2024-04-15
### Added
- Scroll bar derived from parameter index.
- 2 stroke/4 stroke parameter.
- Slope parameter.
- Total number of parameters to Embedded Wizard.

## [0.5.3] - 2024-03-28
### Changed
- Added functionality to identify, ds2431 chip is connected or not. 
- Modified the EEPROM driver functions to address for chip connection status. 
### Added
- Scroll bar derived from parameter index.
- 2 stroke/4 stroke parameter.
- Slope parameter.
- Total number of parameters to Embedded Wizard.

## [0.5.2] - 2024-03-05
### Changed
- SE to 2.1.2 .info fix and Release Subfolder renamed *_Release

## [0.5.1] - 2024-02-29
### Added
- BKC test, and function to store control paramters in eeprom
### Changed
- Added support function for eeprom page read/write in DS2431.h, andDS2431.c
- DS2431 EEprom ROM id read function added partly. Failing to read the complete address. (Need to be addressed)
- Added control paramerters, which will be stored in four pages of eeprom .
- Added function to read/write Torch type control parameter, function to read ROM id, function to initiate BKC test.
### Removed 
- eepromconfig.c and eepromconfig.h are unused, so removed from the project


## [0.5.0] - 2024-02-21
### Changed
- Bootloadertoolchain with SE Standalone Repo
- Yarn updates
- CO_GET_LOCKSTATE added

## [0.4.5] - 2024-02-14
### Fixed
- Merge issues for Embedded Wizard
- Fixed issues related to testcode on running in github build
### Added
- Scroll bar based on parameters.
- A file, Icon_Parameter_16x16_V3.png for parameter image.
### Changed
- Path for Embedded Wizard generated code.
- A function name from GetUntitStr to GetUnitStr.
### Removed 
- Unused folder SystemTorch.


## [0.4.4] - 2023-10-27
### Added
- A piece of code to test DS2431, 1 wire EEPROM.
- Merged Status LED blinking branch.
- A class TestDialog inside Embedded Wizard application to display the test results.
- Functions to return the test mode and results.
- Background Colors for different test result states.
- Macros for Test TC22 FW case.
- Added TestBoard.c and TestBoard.h files, test functions are included.
- Removed sim.c,sim.h, state.c,state.h file, and unused functions from mgs.c,msg.h
### Changed
- Function to update results to display.
- Clock configuration for I2C. The clock input for I2C peripheral was very high, hence changed the input clock to HSI to make it work.
- The path to generated code from Embedded Wizard.
- Left Key press failure condition
- Function to update results to display.
- Byte ordering of received test CAN messages.
- Removed unused code.

## [0.4.3] - 2023-10-19
### Changed
- update dev. container to v1.2.1

### Removed
- 'Setup SSH Keys' section in GitHub Actions build workflow

## [0.4.2] - 2023-10-17
### Fixed
- save all source files in Core/ dir in UTF-8 format (instead of UTF-8 with BOM)
- re-format all source files in Core/ dir (except Embedded Wizard generated files)
- new Compiler warnings in Release mode
### Changed
- set optimize level ``-O1`` in Release mode, -> shrinks firmware size from 226kByte to 170kByte

## [0.4.1] - 2023-10-16
### Fixed
- all Compiler warnings (except 1 in Embedded Wizard PlatformPackage/RTE)
- potentially serious bugs:
  - in eeprom_config.c: Update_EEPROM_Page() + Update_EEPROM_Pages() + Write_RAM(): '&' instead of '&&'
  - in gui.c: get_unit_string(): missing return value/pointer
- linker warnings with newer arm-gnu-toolchain v12.2
### Changed
- update STM32G4xx_HAL_Driver to v1.5.1, CMSIS to v1.2.2
- replace STM32G474xx with STM32G473xx definition

## [0.4.0] - 2023-10-13
###	Changed
- dev. environment from VisualGDB to VSCode/ CMake
### Removed
- drop SBSFU-Embedded-Bootloader external
### Added
- Bootloader BL_SBSFU.elf (BL_TC22 v1.17.1)
- CMake target ``build_and_encrypt`` generates package with FW_SystemTorch.f004 and .info files
- CMake target ``build_and_run`` also copies these files to MaPro /lib/firmware/ directory
- VSCode tasks ``Build Project``, ``Build and Encrypt project``, ``CubeProg: Build and Flash project unencoded (for <<F5>>)``, ``CubeProg: Flash Bootloader (SWD)`` and ``CubeProg: Build and Flash All (encoded)``


============================================================
# Changelog Previous Project ***SystemTorch***, 
archived under: https://github.com/LorchAW/SystemTorch
------------------------------------------------------------
##[0.3.4] 31.08.2023

### Changed 
	- Workaround for Param Display.
	- Display of -0 changed to 0.
	
### Added
	- Function for TorchBT capture and LED ON.
	
## TODO / LIMITATIONS

##[0.3.3] 30.08.2023

### Changed 
	- ParameterUpdate in GUI Fixed values for min max -> Workaround for Schweißen und Schneiden Exibition
	- Minor changes for value correction and display
	- Parameter values received from MaPro are adjusted to display values.
	- CAN implememtation.
	- Moved UART function to separate file.
	- Removed commented out keys update functions
	- Moved UART function to separate file.
	- Configured system to use external clock(Modified 2 files SystemClock_Config() in main.c and MX_FDCAN2_Init() in fdcan2.c based on inputs from Sheela) 
	
### Added
	- UART interface.
	- I2C interface.
	- DS2431 one wire EEPROM interface.
	- CAN errors.
	- Embedded Wizard new unit for CAN information display.
	- CAN messages updating GUI.
	- CAN messages updating GUI.
	- Moved SPI, I2C, Timers, DMA init and other functions to different files.
	- Created data_handler_parameter.c and h files to include all middleware and GUI data exchange.

	
## TODO / LIMITATIONS
	- Not getting expected results out of DS2431 one wire EEPROM. Need to do more testing.
	- Update via Mapro/S4-Tools not possible (device id = 1)
	
-----------------------------------------------------------
##[0.3.2] 11.05.2023

### Changed 
	- CAN Interface Torch state in filter and msg state machine added
	- output improved, reset of LEDs
	
	
------------------------------------------------------------
##[0.3.1] 31.10.2022

### Added 
	- Postbuildscript pre Version added

### Changed 
	- Added the following batch files under tools directory
	- ExtractChangeLogVersion.bat
	- ExtractSysVersion.bat
	- Versionmessage.bat
	- Above batch files are used to extract version from changelog.md file
	- And also, to set the KVERSION in Version.h file
	- The project post build action added to call Versionmessage.bat.
	- The Arguments for the Versionmessage.bat are 
	- 1) project build directory - $(BuildDir)\Build  
	- 2) elf file path+name - $(TargetDir)/SystemTorch.elf 
	- 3) bin file path+name - $(TargetDir)/SystemTorch.bin 
	- 4) Fw Type id - 0 
	- 5) project name - TC22 
	- 6) version.h file path - $(BuildDir)\Inc
	- MessageBox, with Yes,No,Cancel - button shows. Press yes to update kversion, 
	- Press no dont update kversion,and Press cancel to update kversion with 9.
		
### TODO / LIMITATIONS
	- GUI is not connected to Middleware
	- Pullups on Buttons needs to be changed
	

------------------------------------------------------------
------------------------------------------------------------
##[0.3.0] 05.10.2022

### Added 
	- Postbuildscript pre Version added

### Changed 
	- GUI Only in Demo Mode
	- Button Logic Changed and inout module reworked (Hardware Update necessary)
	
### TODO / LIMITATIONS
	- GUI is not connected to Middleware
	- Pullups on Buttons needs to be changed
	

------------------------------------------------------------
##[0.2.0] 23.05.2022
Bootloader hinzugefügt

### Added 
	- S4-Config über externals eingebunden
	- Bootloader über externals eingebunden
	- Software IDs aus S4-Config verwenden
	- Detach und Get Version Kommandos implementiert
### Changed 
	
------------------------------------------------------------
##[0.1.0] 06.08.2021
Umsetzung MDG nach Syleguide

### Added 
	- Projekt aufgesetzt
### Changed 
	
### TODO / LIMITATIONS
	- Eventlog Logids noch nicht final
	- MDG/Funktions Screen noch  Maße und Bitmaps ausstehend.

------------------------------------------------------------