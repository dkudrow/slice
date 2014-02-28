#
# Makefile
#

TOOLS = /opt/raspberrypi/tools/arm-bcm2708/arm-bcm2708-linux-gnueabi/bin/arm-bcm2708-linux-gnueabi
CC = $(TOOLS)-gcc
AS = $(TOOLS)-as
LD = $(TOOLS)-ld
OBJCOPY = $(TOOLS)-objcopy
OBJDUMP = $(TOOLS)-objdump

SOURCE = src/
INCLUDE = include/
BUILD = build/

CFLAGS = -I$(INCLUDE)
LDFLAGS = --no-undefined --section-start=.init=0x8000

LINKER = kernel.ld

COBJ := $(patsubst $(SOURCE)%.c,$(BUILD)%.o,$(wildcard $(SOURCE)*.c))
ASMOBJ := $(patsubst $(SOURCE)%.S,$(BUILD)%.o,$(wildcard $(SOURCE)*.S))
OBJECTS := $(COBJ) $(ASMOBJ)

TARGET = kernel
ELF = $(BUILD)$(TARGET).elf
IMAGE = $(TARGET).img
LIST = $(TARGET).list
MAP = $(TARGET).map

all: $(IMAGE) $(LIST)

rebuild: all

# build the kernel listing
$(LIST): $(ELF)
	$(OBJDUMP) -d $(ELF) > $@

# build the kernel image
$(IMAGE): $(ELF)
	$(OBJCOPY) $(ELF) -O binary $@

# build the elf file
$(ELF): $(OBJECTS) $(LINKER)
	$(LD) $(LDFLAGS) $(OBJECTS) -Map $(MAP) -o $@

# build all of the c source files
$(BUILD)%.o: $(SOURCE)%.c $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

# build all of the assembly source files
$(BUILD)%.o: $(SOURCE)%.S $(BUILD)
	$(AS) -c $< -o $@

# make the build directory if it doesn't exist
$(BUILD):
	mkdir -p $@

clean:
	rm -rf $(BUILD)/*
	rm -f $(IMAGE)
	rm -f $(LIST)
	rm -f $(MAP)
