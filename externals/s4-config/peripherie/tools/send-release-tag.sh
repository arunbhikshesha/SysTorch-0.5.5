#!/usr/bin/env bash
# sends tag (format v1.2.3) with first SemVer from CHANGELOG.md, only when on default branch!

# Test if in project dir
[[ -r cross.cmake ]] || { 
    echo "$0: cross.cmake not found, please call the script from the project root dir!" >&2; 
    exit 1;
}

# on default branch?
GH_DEFAULT_BRANCH=$(git remote show origin | grep "HEAD branch" | sed 's/.*: //')
ACTIVE_BRANCH=$(git branch --show-current)
[ $GH_DEFAULT_BRANCH == $ACTIVE_BRANCH ] || {
    echo "Not on default branch (= $GH_DEFAULT_BRANCH), if that's intentional you have to do the tagging manually!"
    exit 2;
}

# get SemVer from CHANGELOG.md
SEM_VER=$(externals/s4-config/peripherie/tools/changelog-get-version.sh)
[[ -n "${SEM_VER}" ]] || exit 3

# push tag to GitHub
git tag -am "\"Release $SEM_VER\"" v$SEM_VER
git push origin v$SEM_VER

[[ $? -eq 0 ]] || {
    echo "Sending Release tag v$SEM_VER failed!"
    exit 4
}
echo "Success sending Release tag v$SEM_VER!"
