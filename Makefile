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

include Makefile.common

IMAGE = $(ROOT)/$(KERNEL).img
LIST = $(ROOT)/$(KERNEL).list
ELF = $(BUILD)/$(KERNEL).elf

#~==== targets ==========================================================~#
.PHONY: test

all: image list test

image: $(IMAGE)

list: $(LIST)

test:
	$(MAKE) -C $(TEST) all

test-%:
	$(MAKE) -C $(TEST) $@

rebuild: all
#~=======================================================================~#

# build the kernel listing
$(LIST): $(ELF)
	$(OBJDUMP) -d $(ELF) > $@

# build the kernel image
$(IMAGE): $(ELF)
	$(OBJCOPY) $(ELF) -O binary $@

$(ELF):
	$(MAKE) -C $(SRC)

# make the build directory if it doesn't exist
$(BUILD):
	mkdir -p $@

clean:
	$(MAKE) -C $(TEST) clean
	$(MAKE) -C $(SRC) clean
	rm -f $(IMAGE)
	rm -f $(LIST)

