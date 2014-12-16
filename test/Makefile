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
TARGETS = test-malloc test-rbtree
#~=======================================================================~#

all: $(TARGETS)

test-%: $(ROOT)/test-%

$(ROOT)/test-malloc: $(BUILD)/test-malloc.o $(BUILD)/local-malloc.o main.o
	$(CC) $(CFLAGS) -o $@ $(BUILD)/test-malloc.o $(BUILD)/local-malloc.o main.o

$(ROOT)/test-rbtree: $(BUILD)/test-rbtree.o $(BUILD)/local-rbtree.o main.o
	$(CC) $(CFLAGS) -o $@ $(BUILD)/test-rbtree.o $(BUILD)/local-rbtree.o main.o

# build all of the source files
$(BUILD)/local-%.o: $(SRC)/%.c $(BUILD)
	$(CC) $(CFLAGS) -o $@ -c $<

# build all of the tests
$(BUILD)/test-%.o: $(TEST)/%.c $(BUILD)
	$(CC) $(CFLAGS) -o $@ -c $<

$(BUILD):
	mkdir -p $@

clean:
	rm -f $(COBJ)
	rm -f $(TESTOBJ)
	rm -f $(addprefix $(ROOT)/, $(TARGETS))

