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

FOLDERS=("$APP_NAME"_Release "${APP_NAME}_Dummy")
SFB_FILES=("$APP_NAME.sfb" "$APP_NAME.sfbd")
SFB_ENDS=("sfb" "sfbd")
INFO_FILES=("$APP_NAME.info" "$APP_NAME.info")

echo postbuildbundle starting
i=0
max=2
while [ $i -lt $max ]
do
        # check arguments:
    # Test if SFB file present
    [ -f "$BUILD_DIR/${SFB_FILES[i]}"  ] || { 
        echo "`basename $0`: Elf file "$BUILD_DIR/${SFB_FILES[i]}"  not found!" >&2; 
        true $(( i++ ))
        continue;
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

    #copy files to folders for assets
    echo bundle for "${FOLDERS[i]}"

    cd $BUILD_DIR
    rm -rf "${FOLDERS[i]}"
    mkdir -p "${FOLDERS[i]}"
    cp ${SFB_FILES[i]} ${FOLDERS[i]}/$FW_FILE

    #MD5 the firmware
    MD5SUM=($(md5sum -b ${FOLDERS[i]}/$FW_FILE))

    # Output can be useful for Reproducible Build Testing etc. (with SOURCE_DATE_EPOCH set in CMakeLists.txt!):
    MD5_SFU=($(md5sum -b ${FOLDERS[i]}/$FW_FILE))
    echo "MD5SUM of ${FOLDERS[i]}/$FW_FILE is: $MD5_SFU"

    #cat << END_INFO > ${FOLDERS[i]}/$INFO_FILE
    {
    echo {
    echo "\"Obj\"":[{
    echo "\"Name\"":"\"$FW_FILE\"",
    echo "\"SEMVER\"":"\"$FW_VER_SEMANTIC\"",
    echo "\"KVERSION\"":"\"$FW_DATE\"",
    echo "\"end\"":"\"${SFB_ENDS[i]}\"",
    echo "\"md5\"":"\"$MD5SUM\""
    echo }]
    echo }
    } >> ${FOLDERS[i]}/$INFO_FILE
    true $(( i++ ))
done

echo postbuildbundle sucess
