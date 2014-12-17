#
# Makefile
#
# Top level makefile
#
# Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
# Date:		March 7, 2014
#
# Copyright (c) 2014, Daniel Kudrow
# All rights reserved, see LICENSE.txt for details.
#

include ../Makefile.common

ELF = $(BUILD)/$(KERNEL).elf
MAP = $(ROOT)/$(KERNEL).map
COBJ := $(patsubst %.c, $(BUILD)/%.o, $(wildcard *.c))
ASMOBJ := $(patsubst %.S, $(BUILD)/%.o, $(wildcard *.S))
OBJ := $(COBJ) $(ASMOBJ)

all: $(ELF)

# build the elf file
$(ELF): $(OBJ) $(LINKER) $(SRC)
	$(ARMLD) $(ARMLDFLAGS) -T kernel.ld $(OBJ) -Map $(MAP) -o $@

# build all of the c source files
$(BUILD)/%.o: %.c $(BUILD)
	$(ARMCC) $(ARMCFLAGS) -o $@ -c $<

# build all of the assembly source files
$(BUILD)/%.o: %.S $(BUILD)
	$(ARMAS) $(ARMASFLAGS) -o $@ -c $<

clean:
	rm -f $(OBJ)
	rm -f $(ELF) $(MAP)
