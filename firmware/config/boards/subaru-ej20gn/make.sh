#!/bin/sh

export PROJECT_BOARD=subaru-ej20gn
export PROJECT_CPU=ARCH_STM32F7
export PROJECT_CORE=cortex-m7
export EXTRA_PARAMS="-DDUMMY -DEFI_ENABLE_ASSERTS=FALSE -DCH_DBG_ENABLE_TRACE=FALSE -DCH_DBG_ENABLE_ASSERTS=FALSE -DCH_DBG_ENABLE_STACK_CHECK=FALSE -DCH_DBG_FILL_THREADS=FALSE -DCH_DBG_THREADS_PROFILING=FALSE"
#echo $EXTRA_PARAMS
#export DEBUG_LEVEL_OPT="-O0"
#export USE_BOOTLOADER=yes

CROSS_COMPILE=../../toolchain/gcc-arm-none-eabi-8-2018-q4-major/bin/arm-none-eabi- make $*