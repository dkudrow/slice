#
# Makefile
#
# Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
# Date:		March 7, 2014
#
# Copyright (c) 2014, Daniel Kudrow
# All rights reserved, see LICENSE.txt for details.
#

#~==== source tree layout ===============================================~#
SOURCE = src/
INCLUDE = include/
TEST = test/
BUILD = build/
#~=======================================================================~#

#~==== cross compilation tools for ARM ==================================~#
ARMDIR = /opt/raspberrypi/tools/arm-bcm2708/arm-bcm2708-linux-gnueabi/
ARMLIB = $(ARMDIR)lib/gcc/arm-bcm2708-linux-gnueabi/4.7.1/
ARM = $(ARMDIR)bin/arm-bcm2708-linux-gnueabi
ARMCC = $(ARM)-gcc
ARMAS = $(ARM)-as
ARMLD = $(ARM)-ld
OBJCOPY = $(ARM)-objcopy
OBJDUMP = $(ARM)-objdump
ARMINCLUDE = $(ARMDIR)lib/gcc/arm-bcm2708-linux-gnueabi/4.7.1/include

ARMCFLAGS = -I$(INCLUDE) -I$(ARMINCLUDE)\
		   	-ffreestanding -nostartfiles\
		   	-DPRINT_WARN -DPRINT_ERROR -DPRINT_DEBUG -DDEBUG_LEVEL=2
ARMASFLAGS =
ARMLDFLAGS = --no-undefined -T kernel.ld --fatal-warnings
#~=======================================================================~#

#~==== local compilation tools ==========================================~#
CC = gcc

CFLAGS = -g -I$(INCLUDE)
#~=======================================================================~#

#~==== define objects ===================================================~#
COBJ := $(patsubst $(SOURCE)%.c,$(BUILD)%.o,$(wildcard $(SOURCE)*.c))
ASMOBJ := $(patsubst $(SOURCE)%.S,$(BUILD)%.o,$(wildcard $(SOURCE)*.S))
OBJECTS := $(COBJ) $(ASMOBJ)
#~=======================================================================~#

#~==== define targets ===================================================~#
TARGET = kernel
ELF = $(BUILD)$(TARGET).elf
IMAGE = $(TARGET).img
LIST = $(TARGET).list
MAP = $(TARGET).map
#~=======================================================================~#

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
	$(ARMLD) $(ARMLDFLAGS) $(OBJECTS) -Map $(MAP) -o $@

# build all of the c source files
$(BUILD)%.o: $(SOURCE)%.c $(BUILD)
	$(ARMCC) $(ARMCFLAGS) -c $< -o $@

# build all of the assembly source files
$(BUILD)%.o: $(SOURCE)%.S $(BUILD)
	$(ARMAS) $(ARMASFLAGS) -c $< -o $@

# make the build directory if it doesn't exist
$(BUILD):
	mkdir -p $@

test_console:
	$(CC) $(CFLAGS) $(SOURCE)console.c $(TEST)framebuffer.c $(TEST)main.c -o $@

clean:
	rm -rf $(BUILD)*
	rm -f $(IMAGE)
	rm -f $(LIST)
	rm -f $(MAP)
	rm -f test_*
