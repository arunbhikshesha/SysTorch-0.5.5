#!/usr/bin/env bash
# Generate info file and assemble the firmware package

# arg1 = build directory path
# arg2 = application file name (e.g. FW_WUK, without extension, file has to be in build directory!)
# arg3 = Firmware file ending (e.g. f003)
# arg4 = FW version (Semantic)
# arg5 = FW Date (a.k.a. K_VERSION), now Build time: YYMMDDHHMM

BUILD_DIR=$1
APP_NAME=$2
FW_FILE="$APP_NAME.$3"
FW_VER_SEMANTIC=$4
FW_DATE=$5  # former 'K_VERSION'
SFB_FILE="$APP_NAME.sfb"
INFO_FILE="$APP_NAME.info"

# check arguments:
# Test if SFB file present
[ -f $BUILD_DIR/$SFB_FILE ] || { 
    echo "`basename $0`: Elf file $BUILD_DIR/$SFB_FILE not found!" >&2; 
    exit 2;
}
# validate semantic version number
reg_ex='^(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)'
[[ $FW_VER_SEMANTIC =~ $reg_ex ]] || {
    echo "`basename $0`: Check arg4 - FW version (Semantic), '${FW_VER_SEMANTIC}' not SemVer compliant!" >&2;
    echo "Expected format: '1.2.3'";
    exit 1;
}
# Test FW_Date >= year 2020 and < MAX_ULONG
[ "$FW_DATE" -gt 2001011159 ] && [ "$FW_DATE" -lt 4212312359 ] || { 
    echo "`basename $0`: Check arg5 - 'FW Date/Build time', format should be 'YYMMDDHHMM'." >&2; 
    exit 2;
}

cd $BUILD_DIR
mkdir -p $APP_NAME
cp $SFB_FILE $APP_NAME/$FW_FILE

MD5SUM=($(md5sum -b $APP_NAME/$FW_FILE))

# Output can be useful for Reproducible Build Testing etc. (with SOURCE_DATE_EPOCH set in CMakeLists.txt!):
MD5_SFU=($(md5sum -b $APP_NAME.sfu))
echo "MD5SUM of $APP_NAME.sfu is: $MD5_SFU"

cat <<EOF > $APP_NAME/$INFO_FILE
{
"Obj":[{
"Name":"$FW_FILE",
"SEMVER":"$FW_VER_SEMANTIC",
"KVERSION":"$FW_DATE",
"end":"sfb",
"md5":"$MD5SUM"
}]
}
EOF
