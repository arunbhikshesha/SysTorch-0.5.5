#!/usr/bin/env bash
# To be called from a peripheral build to encrypt the bin file 
# Post build for SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256


# arg1 = build directory path
# arg2 = application file name (without extension, file has to be in build directory!)
# arg3 = Key Configuration Name (name of Configuration in SBSFU-SE)
# arg4 = FW Type
# arg5 = FW version (Semantic)
# arg6 = FW Date (a.k.a. K_VERSION), now Build time: YYMMDDHHMM

BUILD_DIR=$1
APP_NAME=$2
KEY_NAME=$3
#ELF_FILE="${BUILD_DIR}/$APP_NAME.elf"
#Bin File Input from Build
BIN_FILE="${BUILD_DIR}/$APP_NAME.elf.bin"
#SFU File Output
SFU_FILE="${BUILD_DIR}/$APP_NAME.sfu"
SFU_FILE_Dummy="${BUILD_DIR}/$APP_NAME.sfud"
#SFB File Output
SFB_FILE="${BUILD_DIR}/$APP_NAME.sfb"
SFB_FILE_Dummy="${BUILD_DIR}/$APP_NAME.sfbd"
#SIGN File Output
SIGN_FILE="${BUILD_DIR}/$APP_NAME.sign"
SIGN_FILE_Dummy="${BUILD_DIR}/$APP_NAME.signd"

FW_TYPE=$4
FW_VER_SEMANTIC=$5
FW_DATE=$6  # former 'K_VERSION'

# For Keyupdates KEY ID can be specified in MAGIC "LBL1" for KEY 1.
#Key id cant be identified at runtime but wrong Magic will fail
KEY_ID=1
MAGIC="LBL${KEY_ID}"

#Read Version from Argument
VERSION_MAJOR=$(cut -d '.' -f 1 <<< $FW_VER_SEMANTIC)
VERSION_MINOR=$(cut -d '.' -f 2 <<< $FW_VER_SEMANTIC)
VERSION_PATCH=$(cut -d '.' -f 3 <<< $FW_VER_SEMANTIC)

#Location of Encrypt tool
PREPARE_IMAGE_PATH=externals/sbsfu-se/tools/prepareimage
[ "$OS" == "Windows_NT" ] && {
    PREPARE_IMAGE_PATH=${PREPARE_IMAGE_PATH}.exe
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



echo "Postbuild encryp startet with Key Name: ${KEY_NAME}"

#On Github actions ENV_VAR is set
#DMR06_G474QCT6_ZIP=""

declare -n ZIPKEY="${KEY_NAME}_ZIP"
echo "Direct $ZIPKEY"
echo "Reference ${!ZIPKEY}"
if [[ -n ${ZIPKEY} ]]; then
    echo "ZIPKEY Found updating Keys for Release"
    echo $ZIPKEY
    unzip -o -P ${ZIPKEY} ./externals/sbsfu-se/Keys/$KEY_NAME/Releasekey/Release.zip -d externals/sbsfu-se/Keys/$KEY_NAME/Releasekey 
    SBSFU_DIR_Release=externals/sbsfu-se/Keys/$KEY_NAME/Releasekey  
    DO_Release=true

else 
    echo "ZIPKEY Not found only Dummy Keys used"
fi

SBSFU_DIR=externals/sbsfu-se/Keys/$KEY_NAME

#Key Material Dummy
echo "Keydir for Dummy encryption $SBSFU_DIR"
IV="${SBSFU_DIR}/iv.bin"
OEM_KEY="${SBSFU_DIR}/OEM_KEY_COMPANY${KEY_ID}_key_AES_CBC.bin"
ECC_KEY="${SBSFU_DIR}/ECCKEY${KEY_ID}.txt"

echo "\n starting encryption Dummy"
$PREPARE_IMAGE_PATH enc --key $OEM_KEY --iv $IV $BIN_FILE $SFU_FILE_Dummy
$PREPARE_IMAGE_PATH sha256 $BIN_FILE $SIGN_FILE_Dummy
$PREPARE_IMAGE_PATH pack --magic $MAGIC --key $ECC_KEY --reserved 16 -fwtype $FW_TYPE -vmaj $VERSION_MAJOR -vmin $VERSION_MINOR -vpat $VERSION_PATCH -vdat $FW_DATE --iv $IV --firmware $SFU_FILE_Dummy --tag $SIGN_FILE_Dummy $SFB_FILE_Dummy --offset 512

if [[ $DO_Release = true ]]; then
    echo "Keydir for Release encryption $SBSFU_DIR_Release"
    IV="${SBSFU_DIR_Release}/iv.bin"
    OEM_KEY="${SBSFU_DIR_Release}/OEM_KEY_COMPANY${KEY_ID}_key_AES_CBC.bin"
    ECC_KEY="${SBSFU_DIR_Release}/ECCKEY${KEY_ID}.txt"

    echo "\n starting encryption Release"
    $PREPARE_IMAGE_PATH enc --key $OEM_KEY --iv $IV $BIN_FILE $SFU_FILE
    $PREPARE_IMAGE_PATH sha256 $BIN_FILE $SIGN_FILE
    $PREPARE_IMAGE_PATH pack --magic $MAGIC --key $ECC_KEY --reserved 16 -fwtype $FW_TYPE -vmaj $VERSION_MAJOR -vmin $VERSION_MINOR -vpat $VERSION_PATCH -vdat $FW_DATE --iv $IV --firmware $SFU_FILE --tag $SIGN_FILE $SFB_FILE --offset 512
fi 



echo "\n postbuild encrypt done"
#Keeps for BigBinary (BL with APPHeader and APP )

#SBSFUELF="${SBSFU_DIR}/BL_SBSFU.elf"
#HEADER_BIN=${BUILD_DIR}/${APP_NAME}sfuh.bin
#BIG_BINARY=${BUILD_DIR}/SBSFU_${APP_NAME}.bin

# check arguments:
# Test if Elf file present
#[ -f $ELF_FILE ] || { 
#    echo "`basename $0`: Elf file $ELF_FILE not found!" >&2; 
#    exit 2;
#}

# $PREPARE_IMAGE_PATH header --magic $MAGIC --key $ECC_KEY --reserved 16 -fwtype $FW_TYPE -vmaj $VERSION_MAJOR -vmin $VERSION_MINOR -vpat $VERSION_PATCH -vdat $FW_DATE --iv $IV --firmware $SFU_FILE --tag $SIGN_FILE --offset 512 $HEADER_BIN
# $PREPARE_IMAGE_PATH merge --install $HEADER_BIN --sbsfu $SBSFUELF --userapp $ELF_FILE $BIG_BINARY
# programmertool -ms $ELF_FILE $HEADER_BIN $SBSFUELF
