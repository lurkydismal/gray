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

export declare BUILD_DEFINES=(
)
export declare BUILD_INCLUDES=(
    "glad/include"
)

export LINK_FLAGS="-flto -fPIC -fuse-ld=mold -Wl,-O1 -Wl,--gc-sections -Wl,--no-eh-frame-hdr"
export LINK_FLAGS_DEBUG="-g"
export LINK_FLAGS_RELEASE="-s"
export LINK_FLAGS_PROFILE="$LINK_FLAGS_DEBUG"

export declare LIBRARIES_TO_LINK=(
    "glfw"
    "dl"
    "mimalloc"
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
    if [ ${#BUILD_DEFINES[@]} -ne 0 ]; then
        printf -v definesAsString -- "-D %s " "${BUILD_DEFINES[@]}"
        echo $definesAsString
    fi

    if [ ${#BUILD_INCLUDES[@]} -ne 0 ]; then
        printf -v includesAsString -- "-I $SCRIPT_DIRECTORY/%s " "${BUILD_INCLUDES[@]}"
        echo $includesAsString
    fi

    source "$partToBuild/config.sh" && './build_general.sh' "$partToBuild" "$BUILD_C_FLAGS" "$definesAsString" "$includesAsString"

    BUILD_STATUS=$?

    if [ $BUILD_STATUS -ne 0 ]; then
        break
    fi
done

if [ $BUILD_STATUS -eq 0 ]; then
    if [ ${#partsToBuild[@]} -ne 0 ]; then
        printf -v partsToBuildAsString -- "$BUILD_DIRECTORY/lib%s.a " "${partsToBuild[@]}"
        echo $partsToBuildAsString
    fi

    if [ ${#LIBRARIES_TO_LINK[@]} -ne 0 ]; then
        printf -v librariesToLinkAgainst -- "-l%s " "${LIBRARIES_TO_LINK[@]}"
        echo $librariesToLinkAgainst
    fi

    $LINKER $LINK_FLAGS $partsToBuildAsString $librariesToLinkAgainst -o "$BUILD_DIRECTORY/$EXECUTABLE_NAME"

    if [ ! -z "${NEED_STRIP_EXECUTABLE+x}" ]; then
        if [ ${#EXECUTABLE_SECTIONS_TO_STRIP[@]} -ne 0 ]; then
            printf -v sectionsToStripAsString -- "--remove-section %s " "${EXECUTABLE_SECTIONS_TO_STRIP[@]}"
            echo $sectionsToStripAsString
        fi

        objcopy "$BUILD_DIRECTORY/$EXECUTABLE_NAME" $sectionsToStripAsString

        strip --strip-section-headers "$BUILD_DIRECTORY/$EXECUTABLE_NAME"
    fi
fi

}
