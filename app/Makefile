#This make files assumes the "setup.sh" script was sourced. If not, an error message is displayed.

# tool macros
CC = ${TOOLCHAIN_PREFIX}gcc
CXX = ${TOOLCHAIN_PREFIX}g++

CFLAGS := 
DBGFLAGS := -g
COBJFLAGS := $(CFLAGS) -c

CXXFLAGS := -lOpenCL -locl_util
CXXOBJFLAGS := -std=c++11 -O3 -c
# -std=c++11 specifies the C++ standard to be used (in this case, C++11).
# -O3 is a compiler optimization flag in C++ that tells the compiler to perform additional optimization passes, beyond those performed by -O2. 
# -c is the flag that tells the compiler to generate object files instead of a binary.

#Check connection with target
REMOTE_IP = $(IP)
PING_RESULT = $(shell ping -W 0.1 -c 1 ${REMOTE_IP} > /dev/null; echo $$?)

# path macros
BIN_PATH := bin
OBJ_PATH := obj
SRC_PATH := src
DBG_PATH := debug

# C code:
# compile macros
TARGET_NAME_C := app
TARGET_C := $(BIN_PATH)/$(TARGET_NAME_C)
# TARGET_DEBUG := $(DBG_PATH)/$(TARGET_NAME_C)

# src files & obj files
SRC := $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*,.c)))
OBJ := $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))
# OBJ_DEBUG := $(addprefix $(DBG_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))

# C++ code:
TARGET_NAME_CXX := app_cpp
TARGET_CXX := $(BIN_PATH)/$(TARGET_NAME_CXX)

# src files & obj files
# SRC_CXX := $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*,.cpp)))
SRC_CXX := /src/matmul_arm.cpp 
OBJ_CXX := $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC_CXX)))))

# clean files list
DISTCLEAN_LIST := $(OBJ) \
				  $(OBJ_CXX)
                #   $(OBJ_DEBUG)
CLEAN_LIST := $(TARGET_C) \
			  $(DISTCLEAN_LIST) \
			#   $(TARGET_DEBUG)
# default rule
default: makedir check_settings all send

host: 
	g++ -std=c++11 ./src/matmul_arm.cpp -o matmul

# non-phony targets

# C code:

#Linker step (To create an "app" file from .o files)
$(TARGET_C): $(OBJ)
	$(CC) -o $@ $(OBJ) $(CFLAGS)

#Compilation step (#To create .o files from .c and .h files)
$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	$(CC) $(COBJFLAGS) -o $@ $<

# $(DBG_PATH)/%.o: $(SRC_PATH)/%.c*
#	$(CC) $(COBJFLAGS) $(DBGFLAGS) -o $@ $<

# $(TARGET_DEBUG): $(OBJ_DEBUG)
#	$(CC) $(CFLAGS) $(DBGFLAGS) $(OBJ_DEBUG) -o $@

# C++ code:
#Linker step (To create an "app" file from .o files)
$(TARGET_CXX): $(OBJ_CXX)
	$(CXX) -o $@ $(OBJ_CXX) $(CXXFLAGS)

#Compilation step (#To create .o files from .c and .h files)
$(OBJ_PATH)/%.o: $(SRC_PATH)/%.cpp
	$(CXX) $(CXXOBJFLAGS) -o $@ $<

# phony rules
.PHONY: makedir
makedir:
	@mkdir -p $(BIN_PATH) $(OBJ_PATH) #$(DBG_PATH)

.PHONY: all
all: $(TARGET_C) $(TARGET_CXX)

.PHONY: debug
debug: $(TARGET_DEBUG)

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
	ssh -o HostKeyAlgorithms=ssh-rsa root@$(IP) 'mkdir -p /home/root/apps'
	scp -o HostKeyAlgorithms=ssh-rsa $(TARGET_C) $(TARGET_CXX) root@$(IP):/home/root/apps
	@echo "\nTransfer of binaries completed."
else
	$(error Error: Cannot connect to target with IP $(REMOTE_IP))
endif

.PHONY: check_settings
check_settings:
ifeq ($(TOOLCHAIN_PREFIX),)
	@echo "Error, "settings.sh" file has not been sourced."
	exit 1
endif