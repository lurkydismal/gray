#!/bin/bash
export SCRIPT_DIRECTORY=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
export BUILD_DIRECTORY_NAME='out'
export BUILD_DIRECTORY="$SCRIPT_DIRECTORY/$BUILD_DIRECTORY_NAME"
export BUILD_C_FLAGS="-flto -std=c11 -Ofast -march=native -funroll-loops -ffunction-sections -fdata-sections -fPIC -fopenmp-simd -fno-ident -fno-asynchronous-unwind-tables -fshort-enums -Wall -Wextra"
export BUILD_DEFINES=""
export LINK_FLAGS="-flto -nostartfiles -fPIC -O1 -s -fuse-ld=mold -Wl,--gc-sections -Wl,--no-eh-frame-hdr"
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
    ".got"
    ".got.plt"
)

clear

source './config.sh' && {

mkdir -p "$BUILD_DIRECTORY"

for partToBuild in "${partsToBuild[@]}"; do

    source "$partToBuild/config.sh" && './build_general.sh' "$partToBuild" "$BUILD_C_FLAGS" "$BUILD_DEFINES"

    if [ $? -ne 0 ]; then
        break
    fi

done

if [ $? -eq 0 ]; then
    printf -v partsToBuildAsString "$BUILD_DIRECTORY/lib%s.a " "${partsToBuild[@]}"

    echo $partsToBuildAsString

    printf -v librariesToLinkAgainst -- "-l%s " "${LIBRARIES_TO_LINK[@]}"

    echo $librariesToLinkAgainst

    $LINKER $LINK_FLAGS $partsToBuildAsString $librariesToLinkAgainst -o "$BUILD_DIRECTORY/$EXECUTABLE_NAME"

    if [ -z "${NEED_STRIP_EXECUTABLE}" ]; then
        printf -v sectionsToStripAsString -- "--remove-section %s " "${EXECUTABLE_SECTIONS_TO_STRIP[@]}"

        echo $sectionsToStripAsString

        objcopy "$BUILD_DIRECTORY/$EXECUTABLE_NAME" $sectionsToStripAsString

        strip --strip-section-headers "$BUILD_DIRECTORY/$EXECUTABLE_NAME"
    fi
fi

}
