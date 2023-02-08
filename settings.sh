# Author: Carlos RIVERA
# Defining target board's IP
export IP=192.168.137.8

#Source environment-setup of Processor SDK
export PSDK_LINUX=/home/cmrivera/ti-processor-sdk-linux-rt-am57xx-evm-06.03.00.106
source ${PSDK_LINUX}/linux-devkit/environment-setup

#Variables creation for OpenCL compilation
export HOST_ROOTDIR=${PSDK_LINUX}/linux-devkit/sysroots/x86_64-arago-linux
export TARGET_ROOTDIR=${PSDK_LINUX}/linux-devkit/sysroots/armv7at2hf-neon-linux-gnueabi

export CLOCL=${HOST_ROOTDIR}/usr/bin/clocl

export TI_OCL_CGT_INSTALL=${HOST_ROOTDIR}/usr/share/ti/cgt-c6x
export TI_OCL_INSTALL=${TARGET_ROOTDIR}
export CL6X=${TI_OCL_CGT_INSTALL}/bin/cl6x