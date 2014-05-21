#!/bin/bash
# @file:
#   make_links.sh
# @author:
#   Dr. Roger G. Doss, PhD
# @description:
#   Construct links for the target platform.
#   Note that for future portability, the
#   code in platform/i386 links to the right hand side
#   and the C source reference from the right hand side.
#   So ./include/platform gives us #include <platform/call.h>
#   So ./include/platform/drivers gives us #include <drivers/chara/console.h>
#
HOMEVAR=$(pwd)
ln -s ${HOMEVAR}/platform/i386/include ./include/platform
ln -s ${HOMEVAR}/platform/i386/drivers ./include/platform/drivers
ln -s ${HOMEVAR}/platform/i386/drivers/include/chara ./include/platform/drivers
ln -s ${HOMEVAR}/platform/i386/drivers/include/block ./include/platform/drivers
ln -s ${HOMEVAR}/platform/i386/drivers/chara ./platform/i386/drivers/include/chara
ln -s ${HOMEVAR}/platform/i386/drivers/block ./platform/i386/drivers/include/block
ln -s ${HOMEVAR}/platform/i386/arch ./kernel/platform
ln -s ${HOMEVAR}/platform/i386/drivers/src ./drivers
ln -s ${HOMEVAR}/platform/i386/boot ./boot

