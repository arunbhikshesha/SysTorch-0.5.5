@echo off
rem Copy the generated binary to the final binary with the correct name
rem and generate a software info file for it.

rem Command line Parameters:
rem %1 Binary filename
rem %2 Firmware filename
rem %3 semantic version
rem %4 KVERSION

rem copy binary file
copy %1 %2

set infofilename=%~dpn2.info
set filename=%~nx2
set extension=%~x1

echo %infofilename%
echo %filename%

set semversion=%3
set kversion=%4

rem calculate MD5 hash code of the binary file
setlocal enabledelayedexpansion

set /a count=1 
for /f "skip=1 delims=:" %%a in ('CertUtil -hashfile %1 MD5') do (
  if !count! equ 1 set "md5=%%a"
  set/a count+=1
)
set "md5=%md5: =%

> %infofilename% (
	echo {
	echo "Obj":[{
	echo "Name":"%filename%",
	echo "SEMVER":"%semversion%",
	echo "KVERSION":"%kversion%",
	echo "end":"%extension:~1%",
	echo "md5":"%md5%"
	echo }]
	echo }
) 

endlocal

