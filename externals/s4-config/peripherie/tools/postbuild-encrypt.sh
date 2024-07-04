#!/usr/bin/env bash
# Post build for SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256

# arg1 = build directory path
# arg2 = application file name (without extension, file has to be in build directory!)
# arg3 = FW Type
# arg4 = FW version (Semantic)
# arg5 = FW Date (a.k.a. K_VERSION), now Build time: YYMMDDHHMM

BUILD_DIR=$1
APP_NAME=$2
ELF_FILE="${BUILD_DIR}/$APP_NAME.elf"
BIN_FILE="${BUILD_DIR}/$APP_NAME.elf.bin"
SFU_FILE="${BUILD_DIR}/$APP_NAME.sfu"
SFB_FILE="${BUILD_DIR}/$APP_NAME.sfb"
SIGN_FILE="${BUILD_DIR}/$APP_NAME.sign"
FW_TYPE=$3
FW_VER_SEMANTIC=$4
FW_DATE=$5  # former 'K_VERSION'

KEY_ID=1
SBSFU_DIR=.github/tools/sbsfu
IV="${SBSFU_DIR}/iv.bin"
OEM_KEY="${SBSFU_DIR}/OEM_KEY_COMPANY${KEY_ID}_key_AES_CBC.bin"
ECC_KEY="${SBSFU_DIR}/ECCKEY${KEY_ID}.txt"
SBSFUELF="${SBSFU_DIR}/BL_SBSFU.elf"
HEADER_BIN=${BUILD_DIR}/${APP_NAME}sfuh.bin
BIG_BINARY=${BUILD_DIR}/SBSFU_${APP_NAME}.bin
MAGIC="LBL${KEY_ID}"
VERSION_MAJOR=$(cut -d '.' -f 1 <<< $FW_VER_SEMANTIC)
VERSION_MINOR=$(cut -d '.' -f 2 <<< $FW_VER_SEMANTIC)
VERSION_PATCH=$(cut -d '.' -f 3 <<< $FW_VER_SEMANTIC)
PREPARE_IMAGE_PATH=externals/s4-config/peripherie/tools/prepareimage
[ "$OS" == "Windows_NT" ] && {
    PREPARE_IMAGE_PATH=${PREPARE_IMAGE_PATH}.exe
}

# check arguments:
# Test if Elf file present
[ -f $ELF_FILE ] || { 
    echo "`basename $0`: Elf file $ELF_FILE not found!" >&2; 
    exit 2;
}
# Test if prepareimage tool present
[ -x $PREPARE_IMAGE_PATH ] || { 
    echo "`basename $0`: prepareimage tool $PREPARE_IMAGE_PATH not found or not executable!" >&2; 
    exit 2;
}
# validate semantic version number
reg_ex='^(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)'
[[ $FW_VER_SEMANTIC =~ $reg_ex ]] || {
    echo "`basename $0`: Check arg4 - FW version (Semantic), '${FW_VER_SEMANTIC}' not SemVer compliant!" >&2;
    echo "Expected format: '1.2.3'";
    exit 1;
}
# Test again semantic version parts
( [ -z $VERSION_MAJOR ] || [ -z $VERSION_MINOR ] || [ -z $VERSION_PATCH ] ) && { 
    echo "`basename $0`: Check arg4 - 'FW version (Semantic)', format should be '1.2.3'." >&2; 
    exit 2;
}
# Test FW_Date >= year 2020
[ "$FW_DATE" -gt 2001011159 ] || { 
    echo "`basename $0`: Check arg5 - 'FW Date/Build time', format should be 'YYMMDDHHMM'." >&2; 
    exit 2;
}

$PREPARE_IMAGE_PATH enc --key $OEM_KEY --iv $IV $BIN_FILE $SFU_FILE
$PREPARE_IMAGE_PATH sha256 $BIN_FILE $SIGN_FILE
$PREPARE_IMAGE_PATH pack --magic $MAGIC --key $ECC_KEY  --reserved 16 -fwtype $FW_TYPE -vmaj $VERSION_MAJOR -vmin $VERSION_MINOR -vpat $VERSION_PATCH -vdat $FW_DATE --iv $IV --firmware $SFU_FILE --tag $SIGN_FILE $SFB_FILE --offset 512

# $PREPARE_IMAGE_PATH header --magic $MAGIC --key $ECC_KEY --reserved 16 -fwtype $FW_TYPE -vmaj $VERSION_MAJOR -vmin $VERSION_MINOR -vpat $VERSION_PATCH -vdat $FW_DATE --iv $IV --firmware $SFU_FILE --tag $SIGN_FILE --offset 512 $HEADER_BIN
# $PREPARE_IMAGE_PATH merge --install $HEADER_BIN --sbsfu $SBSFUELF --userapp $ELF_FILE $BIG_BINARY
# programmertool -ms $ELF_FILE $HEADER_BIN $SBSFUELF
