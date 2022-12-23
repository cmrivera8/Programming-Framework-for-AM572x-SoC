# Defining board's name
# Options: BeagleBoard_X15
export BOARD_NAME="BeagleBoard_X15"

# Defining Buildroot location (Absolute path)
export BR_DIR=/home/cmrivera/Project_M2/buildroot_BeagleBoard_X15

# Defining target board's IP
export IP=192.168.137.8

# Defining NFS location in host computer (to install executables and modules)
export NFS=/home/cmrivera/Project_M2/nfs

# Create necessary variables to compile projects (app directory)
export APP=export APP=$PWD/app