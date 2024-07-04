# Changelog  

## [2.1.2] - 2024-03-05
### Changed
- postbuild-bundle Release Version now in *_Release Path

### Fixed
- postbuild-bundle fixed info file generation " was missing

## [2.1.1] - 2024-02-28
- postbuild-bundle corrected -> only affect local build of tragets

## [2.1.0] - 2024-02-24
- Release Keys added and different Dummy keys for each Target

## [2.0.3] - 2024-02-13
### Changed
- All Keys are Dummy Keys

## [2.0.2] - 2024-01-04
### Changed
- SE_LockRestrictedServices initialized on SE_INIT_ID to workaround unitialized data problem- 
- Readme release added
- Linkerfiles updated for all Devices

## [2.0.1] - 2023-10-26
### Added
- Doku
### Changed
- tools to LF (CRLF)
- unzip with override

## [2.0.0] - 2023-10-24
### Changed 
- SE Standalone Repository started
- data_init.c changed datatype uint8_t to uint32_t https://community.st.com/t5/stm32-mcus-security/sbsfu-warray-bounds-error-when-compiling-data-init-c/td-p/597688
- scripts from externals moved to /tools 
### Added
- Keygeneration tool



