#
# test/Makefile
#
# Makefile for unit tests
#
# Author:	Daniel Kudrow (dkudrow@cs.ucsb.edu)
# Date:		December 14, 2014
#
# Copyright (c) 2014, Daniel Kudrow
# All rights reserved, see LICENSE.txt for details.
#

include ../Makefile.common

#~==== define objects ===================================================~#
COBJ := $(patsubst $(SRC)/%.c,$(BUILD)/local-%.o,$(wildcard $(SRC)/*.c))
TESTOBJ := $(patsubst $(TEST)/%.c,$(BUILD)/test-%.o,$(wildcard $(TEST)/*.c))
#~=======================================================================~#

#~==== define targets ===================================================~#
TARGETS = test-fs test-rbtre etest-malloc
#~=======================================================================~#

all: $(TARGETS)

test-%: $(ROOT)/test-%
	
FS_OBJ = test-filesystem.o local-filesystem.o local-emmc.o local-string.o
$(ROOT)/test-fs: $(addprefix $(BUILD)/, $(FS_OBJ))
	$(CC) $(CFLAGS) -o $@ $^

RBTREE_OBJ = test-rbtree.o local-rbtree.o test-main.o
$(ROOT)/test-rbtree: $(addprefix $(BUILD)/, $(RBTREE_OBJ))
	$(CC) $(CFLAGS) -o $@ $^

MALLOC_OBJ = test-malloc.o local-malloc.o test-main.o
$(ROOT)/test-malloc: $(addprefix $(BUILD)/, $(MALLOC_OBJ))
	$(CC) $(CFLAGS) -o $@ $^

# build all of the source files
$(BUILD)/local-%.o: $(TESTSRC)/%.c $(BUILD)
	$(CC) $(CFLAGS) -o $@ -c $<

# build all of the tests
$(BUILD)/test-%.o: %.c $(BUILD)
	$(CC) $(CFLAGS) -o $@ -c $<

$(BUILD):
	mkdir -p $@

clean:
	rm -f $(COBJ)
	rm -f $(TESTOBJ)
	rm -f $(addprefix $(ROOT)/, $(TARGETS))

