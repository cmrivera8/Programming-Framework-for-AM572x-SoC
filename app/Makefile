CC=$(BR_DIR)/output/host/usr/bin/arm-linux-gcc
CFLAGS=

out_file = app
src_files = main.c
obj_files = main.o

#WSL:
PATH:=$(PATH):/home/cmrivera/Project_M2/buildroot_BeagleBoard_X15/output/host/usr/bin/

#Linker step:
$(out_file): $(obj_files) #To create an "app" file from .o files
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@
	scp $@ root@$(IP):/root

.PHONY: all #To avoid implicit rules

#Compilation step:
$(obj_files): %.o: %.c #To create .o files from .c files
	$(CC) -c $(CFLAGS) $^ -o $@

#To compile modules:
host_modules:
	make -C /home/cmrivera/WSL2-Linux-Kernel M=$(PWD) modules

target_modules:
	make ARCH=arm CROSS_COMPILE=arm-linux- -C $(BR_DIR)/output/build/linux-5.17.15 \
	M=$(PWD) modules 
	scp ./*.ko root@$(IP):/root

clean:
	rm -f ./*.mod* ./*.ko ./*.symvers ./*.order ./*.o ./$(out_file)