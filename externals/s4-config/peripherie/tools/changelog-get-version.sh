#!/usr/bin/env bash

# Test if in project dir and file CHANGELOG.md present
[ -f CHANGELOG.md ] || { 
    echo "$0: File CHANGELOG.md not found, please call the script from the project root dir!" >&2; 
    exit 2;
}

# look for topmost version, assumed version number format: '## [1.2.3] - 2021-09-08'
NEWEST_VERSION=$(cat CHANGELOG.md | grep -s -E "^#+[ \t]+\[.*\." | cut -d" " -f2 | tr -d \[\] | head -1)
[[ -n "${NEWEST_VERSION}" ]] || { 
    echo "Failed to get version from CHANGELOG.md (expected format: '## [1.2.3] - 2021-09-08')." >&2;
    exit 1;
}

# validate version number
reg_ex='^(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)'
[[ $NEWEST_VERSION =~ $reg_ex ]] || {
    echo "First Version Tag in CHANGELOG.md: '${NEWEST_VERSION}' not SemVer compliant!" >&2;
    echo "Expected format: '## [1.2.3] - 2021-09-08'";
    exit 1;
}

echo -n $NEWEST_VERSION
