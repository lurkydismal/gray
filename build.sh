#!/bin/bash
export SCRIPT_DIRECTORY=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
export BUILD_DIRECTORY_NAME='out'
export BUILD_DIRECTORY="$SCRIPT_DIRECTORY/$BUILD_DIRECTORY_NAME"

# 0 - Debug
# 1 - Release
# 2 - Profile
export BUILD_TYPE=0

export BUILD_C_FLAGS="-flto -std=gnu99 -march=native -ffunction-sections -fdata-sections -fPIC -fopenmp-simd -fno-ident -fshort-enums -Wall -Wextra"
export BUILD_C_FLAGS_DEBUG="-Og -g"
export BUILD_C_FLAGS_RELEASE="-Ofast -funroll-loops -fno-asynchronous-unwind-tables"
export BUILD_C_FLAGS_PROFILE="$BUILD_C_FLAGS_RELEASE"

export BUILD_DEFINES=""

export LINK_FLAGS="-flto -fPIC -fuse-ld=mold -Wl,-O1 -Wl,--gc-sections -Wl,--no-eh-frame-hdr"
export LINK_FLAGS_DEBUG="-g"
export LINK_FLAGS_RELEASE="-s"
export LINK_FLAGS_PROFILE="$LINK_FLAGS_DEBUG"

export declare LIBRARIES_TO_LINK=(
    "glfw"
    "dl"
    "mimalloc"
    "GL"
)
export LINKER="ccache gcc"
export EXECUTABLE_NAME="main.out"
export declare EXECUTABLE_SECTIONS_TO_STRIP=(
    ".note.gnu.build-id"
    ".note.gnu.property"
    ".comment"
    ".eh_frame"
    ".eh_frame_hdr"
    ".relro_padding"
)

clear

source './config.sh' && {

mkdir -p "$BUILD_DIRECTORY"

if [ $BUILD_TYPE -eq 0 ]; then
    echo "Debug build"

    BUILD_C_FLAGS="$BUILD_C_FLAGS $BUILD_C_FLAGS_DEBUG"
    LINK_FLAGS="$LINK_FLAGS $LINK_FLAGS_DEBUG"

elif [ $BUILD_TYPE -eq 1 ]; then
    echo "Release build"

    BUILD_C_FLAGS="$BUILD_C_FLAGS $BUILD_C_FLAGS_RELEASE"
    LINK_FLAGS="$LINK_FLAGS $LINK_FLAGS_RELEASE"

elif [ $BUILD_TYPE -eq 2 ]; then
    echo "Profile build"

    BUILD_C_FLAGS="$BUILD_C_FLAGS $BUILD_C_FLAGS_PROFILE"
    LINK_FLAGS="$LINK_FLAGS $LINK_FLAGS_PROFILE"
fi

for partToBuild in "${partsToBuild[@]}"; do
    source "$partToBuild/config.sh" && './build_general.sh' "$partToBuild" "$BUILD_C_FLAGS" "$BUILD_DEFINES"

    BUILD_STATUS=$?

    if [ $BUILD_STATUS -ne 0 ]; then
        break
    fi
done

if [ $BUILD_STATUS -eq 0 ]; then
    printf -v partsToBuildAsString "$BUILD_DIRECTORY/lib%s.a " "${partsToBuild[@]}"

    echo $partsToBuildAsString

    printf -v librariesToLinkAgainst -- "-l%s " "${LIBRARIES_TO_LINK[@]}"

    echo $librariesToLinkAgainst

    $LINKER $LINK_FLAGS $partsToBuildAsString $librariesToLinkAgainst -o "$BUILD_DIRECTORY/$EXECUTABLE_NAME"

    if [ ! -z "${NEED_STRIP_EXECUTABLE+x}" ]; then
        printf -v sectionsToStripAsString -- "--remove-section %s " "${EXECUTABLE_SECTIONS_TO_STRIP[@]}"

        echo $sectionsToStripAsString

        objcopy "$BUILD_DIRECTORY/$EXECUTABLE_NAME" $sectionsToStripAsString

        strip --strip-section-headers "$BUILD_DIRECTORY/$EXECUTABLE_NAME"
    fi
fi

}
