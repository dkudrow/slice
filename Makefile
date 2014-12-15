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
ROOT = $(abspath $(dir $(MAKEFILE_LIST)))
SRC = $(ROOT)/src
INC = $(ROOT)/include
TEST = $(ROOT)/test
BUILD = $(ROOT)/build
#~=======================================================================~#

#~==== cross compilation tools for ARM ==================================~#
ARMDIR = /opt/raspberrypi/tools/arm-bcm2708/arm-bcm2708-linux-gnueabi
ARMLIB = $(ARMDIR)/lib/gcc/arm-bcm2708-linux-gnueabi/4.7.1
ARM = $(ARMDIR)/bin/arm-bcm2708-linux-gnueabi
ARMCC = $(ARM)-gcc
ARMAS = $(ARM)-as
ARMLD = $(ARM)-ld
OBJCOPY = $(ARM)-objcopy
OBJDUMP = $(ARM)-objdump
ARMINC = $(ARMDIR)/lib/gcc/arm-bcm2708-linux-gnueabi/4.7.1/include
ARMCFLAGS = -I$(INC) -I$(ARMINC)\
		   	-ffreestanding -nostartfiles\
		   	-DPRINT_WARN -DPRINT_ERROR -DPRINT_DEBUG -DDEBUG_LEVEL=2
ARMASFLAGS =
ARMLDFLAGS = --no-undefined -T $(ROOT)/kernel.ld --fatal-warnings
#~=======================================================================~#

#~==== local compilation tools ==========================================~#
CC = clang
CFLAGS = -Wall -g -I$(INC)
#~=======================================================================~#

#~==== define objects ===================================================~#
COBJ := $(patsubst $(SRC)/%.c,$(BUILD)/%.o,$(wildcard $(SRC)/*.c))
ASMOBJ := $(patsubst $(SRC)/%.S,$(BUILD)/%.o,$(wildcard $(SRC)/*.S))
OBJECTS := $(COBJ) $(ASMOBJ)
#~=======================================================================~#

#~==== define targets ===================================================~#
BASE = kernel
ELF = $(BUILD)/$(BASE).elf
IMAGE = $(ROOT)/$(BASE).img
LIST = $(ROOT)/$(BASE).list
MAP = $(ROOT)/$(BASE).map
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
$(BUILD)/%.o: $(SRC)/%.c $(BUILD)
	$(ARMCC) $(ARMCFLAGS) -c $< -o $@

# build all of the assembly source files
$(BUILD)/%.o: $(SRC)/%.S $(BUILD)
	$(ARMAS) $(ARMASFLAGS) -c $< -o $@

# make the build directory if it doesn't exist
$(BUILD):
	mkdir -p $@

#~==== define test objects ==============================================~#
TESTOBJ := $(patsubst $(TEST)/%,%,$(wildcard $(TEST)/*.c))
TESTOBJ := $(filter-out main.c, $(TESTOBJ))
TESTOBJ := $(foreach NAME, $(TESTOBJ), $(wildcard $(SRC)/$(NAME)))
TESTOBJ := $(patsubst $(SRC)/%.c,$(BUILD)/local-%.o, $(TESTOBJ))
TESTOBJ += $(patsubst $(TEST)/%.c,$(BUILD)/test-%.o,$(wildcard $(TEST)/*.c))
#~=======================================================================~#

#~==== define test targets ==============================================~#
TESTS = run_tests
#~=======================================================================~#

test: $(TESTS)

$(TESTS): $(TESTOBJ)
	$(CC) $(CFLAGS) $(TESTOBJ) -o run_tests

$(BUILD)/test-%.o: $(TEST)/%.c $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/local-%.o: $(SRC)/%.c $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD)/*
	rm -f $(IMAGE)
	rm -f $(LIST)
	rm -f $(MAP)
	rm -f $(TESTS)
