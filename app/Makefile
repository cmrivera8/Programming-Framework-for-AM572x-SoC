# Author: Carlos RIVERA
# This Makefile assumes the "../settings.sh" script was sourced. If not, an error message is displayed.

# Path macros
BIN_PATH := bin
INC_PATH := include
LIB_PATH := lib
OBJ_PATH := obj
SRC_PATH := src



# Tool macros
CC = ${TOOLCHAIN_PREFIX}gcc
CXX = ${TOOLCHAIN_PREFIX}g++

CFLAGS := 
COBJFLAGS := $(CFLAGS) -c

CXXFLAGS := -lOpenCL -locl_util -fopenmp
CXXOBJFLAGS := -std=c++11 -O3 -c
#mathlib OpenMP parallelization:
CXXOBJFLAGS +=-fopenmp

# -std=c++11 specifies the C++ standard to be used (in this case, C++11).
# -O3 is a compiler optimization flag in C++ that tells the compiler to perform additional optimization passes, beyond those performed by -O2. 
# -c is the flag that tells the compiler to generate object files instead of a binary.

# If creating DSP project:
ATLAS_LIB = $(LIB_PATH)/libcblas.a $(LIB_PATH)/libatlas.a

LIBS  = -lOpenCL -locl_util
LIBS += $(ATLAS_LIB)

LDFLAGS  += -L$(TI_OCL_INSTALL)/usr/lib
LDFLAGS += "--sysroot=$(TARGET_ROOTDIR)"

LDFLAGS += -L$(TI_OCL_INSTALL)/usr/lib -L$(TARGET_ROOTDIR)/lib -L$(TARGET_ROOTDIR)/usr/lib -Wl,-rpath-link,$(TARGET_ROOTDIR)/lib -Wl,-rpath-link,$(TARGET_ROOTDIR)/usr/lib

CXXFLAGS += -O3

CXXOBJFLAGS += "--sysroot=$(TARGET_ROOTDIR)"
CXXOBJFLAGS += -I$(INC_PATH)
CXXOBJFLAGS += -I$(TI_OCL_INSTALL)/usr/include -Wall

CLOCLFLAGS = -I$(INC_PATH)

CL6XFLAGS := -o3 
#

# Check connection with target
REMOTE_IP = $(IP)
PING_RESULT = $(shell ping -W 0.1 -c 1 ${REMOTE_IP} > /dev/null; echo $$?)

# ┌──────────┐
# │  C code  │
# └──────────┘
# Compile macros
TARGET_NAME_C := app
TARGET_C := $(BIN_PATH)/$(TARGET_NAME_C)

# Src and Obj files
SRC := $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*,.c)))
OBJ := $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))

# ┌────────────┐
# │  C++ code  │
# └────────────┘
# Compile macros
TARGET_NAME_CXX := app_cpp
TARGET_CXX := $(BIN_PATH)/$(TARGET_NAME_CXX)

# Src and Obj files
SRC_CXX := $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*,.cpp)))
OBJ_CXX := $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC_CXX)))))

# ┌───────────────┐
# │  DSP project  │
# └───────────────┘
# Compile macros
MAIN_SRC_NAME := skeleton
TARGET_NAME_CL := sgemm
TARGET_CL :=  $(BIN_PATH)/$(TARGET_NAME_CL)

SRC_CLOCL := $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*,.c)))
SRC_KERNEL := $(notdir $(basename $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*,.cl)))))
# SRC_KERNEL := dsp_compute
OBJ_CLOCL := $(addprefix $(OBJ_PATH)/, $(addsuffix .obj, $(notdir $(basename $(SRC)))))
#CLOCL: Compute Library for OpenCL

DSP_INCLUDE = -I$(INC_PATH)
DSP_INCLUDE += -I$(TI_OCL_CGT_INSTALL)/include
DSP_INCLUDE += -I$(TI_OCL_INSTALL)/usr/share/ti/opencl
DSP_INCLUDE += -I$(TARGET_ROOTDIR)/usr/share/ti/cgt-c6x/include

CL6X += -mv6600 --abi=eabi $(DSP_INCLUDE)

# Clean files list
DISTCLEAN_LIST := $(OBJ) \
				  $(OBJ_CXX) \
				  $(OBJ_CLOCL)

CLEAN_LIST := $(TARGET_C) \
			  $(TARGET_CXX) \
			  $(TARGET_CL) \
			  *.out \
			  $(OBJ_PATH)/*.o \
			  $(INC_PATH)/*.dsp_h \
			  $(DISTCLEAN_LIST) \

# Default rule
default: makedir check_settings all send

# non-phony targets

# ┌──────────┐
# │  C code  │
# └──────────┘

# Linker step (To create an "app" file from .o files)
$(TARGET_C): $(OBJ)
	@echo ""
	@echo "┌─────────────────────────────┐"
	@echo "│  Project: Regular C code    │"
	@echo "└─────────────────────────────┘"

	$(CC) -o $@ $(OBJ) $(CFLAGS)

# Compilation step (#To create .o files from .c and .h files)
$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	$(CC) $(COBJFLAGS) -o $@ $<

# ┌────────────┐
# │  C++ code  │
# └────────────┘

# Linker step (To create an "app" file from .o files)
$(TARGET_CXX): $(OBJ_CXX)
	@echo ""
	@echo "┌─────────────────────────────┐"
	@echo "│  Project: Regular C++ code  │"
	@echo "└─────────────────────────────┘"

	$(CXX) -o $@ $(OBJ_CXX) $(CXXFLAGS)

# Compilation step (#To create .o files from .c and .h files) (Rule shared with DSP project rules)
$(OBJ_PATH)/%.o: $(SRC_PATH)/%.cpp
	@echo "\n" Compiling $< to generate $@
	$(CXX) $(CXXOBJFLAGS) -o $@ $<

# ┌─────────────────────────────────────┐
# │  DSP unsing CL6X and CLOCL project  │
# └─────────────────────────────────────┘

# Create monolitic binary from main.o
# $(TARGET_CL): $(OBJ_PATH)/main.o
$(TARGET_CL): $(OBJ_PATH)/$(MAIN_SRC_NAME).o
	@echo ""
	@echo "┌──────────────────────────────┐"
	@echo "│  Project: DSP + OpenCL code  │"
	@echo "└──────────────────────────────┘"

	@echo "\n" Linking $< to generate $@
	$(CXX) $(CXXFLAGS) $< $(LDFLAGS) $(LIBS) -lrt -lpthread -lm -o $@

# Creation of kernel for DSP:
# This rule is only to trigger the pre-compiled and embedded binary (kernel.dsp_h) generation before the C++ main.cpp is compiled (since main.cpp depends on it):
# $(OBJ_PATH)/main.o: $(INC_PATH)/kernel.dsp_h
$(OBJ_PATH)/$(MAIN_SRC_NAME).o: $(INC_PATH)/$(SRC_KERNEL).dsp_h

# DSP kernel is built from this pre-compiled and embedded binary which stored in the same directory as the Makefile by default, so it's moved to "include" directory
.PRECIOUS: $(OBJ_PATH)/%.obj
$(INC_PATH)/%.dsp_h: $(SRC_PATH)/%.cl ${OBJ_CLOCL}
	@echo "\n" Compiling $< to generate $@
	$(CLOCL) $(CLOCLFLAGS) --txt $^
	mv $(@F) $(INC_PATH)
	rm -f $(patsubst $(INC_PATH)/%.dsp_h, ./%.out, $@)

# C66x Digital Signal Processor (DSP) compilation (to create .obj files from .c)
$(OBJ_PATH)/%.obj: $(SRC_PATH)/%.c
	@echo "\n" Compiling $< to generate $@
	$(CL6X) --output_file $@ -c $(CL6XFLAGS) $< 

# Creation of kernel headers .dsp_h from OpenCL source .cl files
# $(OBJ_PATH)/%.dsp_h: $(SRC_PATH)/%.cl
# 	@echo "\n" Compiling $< to generate $@
# 	$(CLOCL) $(CLOCLFLAGS) --txt $^

# phony rules
.PHONY: makedir
makedir:
	@mkdir -p $(BIN_PATH) $(OBJ_PATH) #$(DBG_PATH)


# ┌─────────────────────────┐
# │  Targets to be created  │
# └─────────────────────────┘
.PHONY: all
#Regular C project:
# all: $(TARGET_C)
#Regular C++ project:
# all: $(TARGET_CXX)
#DSP unsing CL6X and CLOCL project:
all: $(TARGET_CL)

.PHONY: clean
clean:
	@echo CLEAN $(CLEAN_LIST)
	@rm -f $(CLEAN_LIST)

.PHONY: distclean
distclean:
	@echo CLEAN $(DISTCLEAN_LIST)
	@rm -f $(DISTCLEAN_LIST)

.PHONY: send
send:
ifeq ($(PING_RESULT), 0)
	ssh -o HostKeyAlgorithms=ssh-rsa root@$(IP) 'mkdir -p /home/root/$(TARGET_NAME_CL)'
#	Send only existing binaries
	@for target in $(TARGET_C) $(TARGET_CXX) $(TARGET_CL); do \
		if [ -f $$target ]; then \
			scp -o HostKeyAlgorithms=ssh-rsa $$target root@$(IP):/home/root/$(TARGET_NAME_CL); \
		fi \
	done
	@echo "\nTransfer of binaries completed."
else
	$(error Error: Cannot connect to target with IP $(REMOTE_IP))
endif

.PHONY: check_settings
check_settings:
ifeq ($(TOOLCHAIN_PREFIX),)
	@echo "Error, "settings.sh" file has not been sourced."
	@echo "Use: source ../settings.sh"
	exit 1
endif