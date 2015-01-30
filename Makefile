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
#ROOT = $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
ROOT = .

SRC = $(ROOT)/src
INC = $(ROOT)/include
BUILD = $(SRC)/build

TEST = $(ROOT)/test
TESTBUILD = $(TEST)/build

#~==== ARM cross compilation tools ======================================~#
ARMDIR = /opt/raspberrypi/tools/arm-bcm2708/arm-bcm2708-linux-gnueabi
ARM = $(ARMDIR)/bin/arm-bcm2708-linux-gnueabi

ARMCC = $(ARM)-gcc
ARMCFLAGS = -I$(INC) -ffreestanding -nostartfiles -DLOGGING=INFO
LIBGCC = $(ARMDIR)/lib/gcc/arm-bcm2708-linux-gnueabi/4.7.1/libgcc.a

ARMAS = $(ARM)-as
ARMASFLAGS = --warn

ARMLD = $(ARM)-ld
ARMLDFLAGS = --no-undefined --fatal-warnings

OBJCOPY = $(ARM)-objcopy

OBJDUMP = $(ARM)-objdump

#~==== test compilation tools ===========================================~#
TESTCC = clang
TESTCFLAGS = -Wall -g -I$(TEST) -I$(INC)

#~==== targets ==========================================================~#
IMAGE = $(ROOT)/kernel.img
LIST = $(ROOT)/kernel.list
ELF = $(BUILD)/kernel.elf
MAP = $(ROOT)/kernel.map
TARGETS = $(IMAGE) $(LIST) $(ELF) $(MAP)

COBJ :=
COBJ += console.o
COBJ += emmc.o
#COBJ += filesystem.o
COBJ += framebuffer.o
COBJ += gpio.o
COBJ += irq.o
COBJ += led.o
COBJ += log.o
COBJ += mailbox.o
COBJ += main.o
COBJ += malloc.o
COBJ += kprintf.o
COBJ += rbtree.o
COBJ += string.o
COBJ += timer.o
COBJ := $(addprefix $(BUILD)/, $(COBJ))

ASMOBJ := 
ASMOBJ += start.o
ASMOBJ += division.o
ASMOBJ := $(addprefix $(BUILD)/, $(ASMOBJ))

OBJ := $(COBJ) $(ASMOBJ)

#~==== rules ============================================================~#
all: image list

image: $(IMAGE)

list: $(LIST)

$(LIST): $(ELF)
	$(OBJDUMP) -d $(ELF) > $@

$(IMAGE): $(ELF)
	$(OBJCOPY) $(ELF) -O binary $@

$(ELF): $(OBJ) $(LIBGCC)
	$(ARMLD) $(ARMLDFLAGS) -T kernel.ld -Map $(MAP) -o $@ $^

$(BUILD)/%.o: $(SRC)/%.c
	$(ARMCC) $(ARMCFLAGS) -MD -o $@ -c $<

$(BUILD)/%.o: $(SRC)/%.S
	$(ARMAS) $(ARMASFLAGS) -o $@ -c $<

#~==== test targets =====================================================~#
TEST_OBJ = main-test.o string.o kprintf.o dummy_console-test.o
MALLOC_OBJ := $(TEST_OBJ) malloc-test.o malloc.o
RBTREE_OBJ := $(TEST_OBJ) rbtree-test.o rbtree.o
KPRINTF_OBJ := $(TEST_OBJ) kprintf-test.o
FS_OBJ := $(TEST_OBJ) filesystem-test.o filesystem.o emmc.o

TESTS = malloc-test rbtree-test fs-test kprintf-test

#~==== test rules =======================================================~#
test: tests
	for t in $(TESTS); do $(TEST)/$$t; done

tests: $(TESTS)

rbtree-test: $(addprefix $(TESTBUILD)/, $(RBTREE_OBJ))
	$(TESTCC) $(TESTCFLAGS) -o $(TEST)/$@ $^

malloc-test: $(addprefix $(TESTBUILD)/, $(MALLOC_OBJ))
	$(TESTCC) $(TESTCFLAGS) -o $(TEST)/$@ $^

fs-test: $(addprefix $(TESTBUILD)/, $(FS_OBJ))
	$(TESTCC) $(TESTCFLAGS) -o $(TEST)/$@ $^

kprintf-test: $(addprefix $(TESTBUILD)/, $(KPRINTF_OBJ))
	$(TESTCC) $(TESTCFLAGS) -o $(TEST)/$@ $^

$(TESTBUILD)/emmc.o: $(TEST)/dummy_emmc.c
	$(TESTCC) $(TESTCFLAGS) -MD -o $@ -c $<

$(TESTBUILD)/%-test.o: $(TEST)/%.c
	$(TESTCC) $(TESTCFLAGS) -MD -o $@ -c $<

$(TESTBUILD)/%.o: $(SRC)/%.c
	$(TESTCC) $(TESTCFLAGS) -MD -o $@ -c $<

#~==== clean ============================================================~#
clean-deps:
	rm -f $(TESTBUILD)/*.d
	rm -f $(BUILD)/*.d

clean:
	rm -f $(TESTBUILD)/*.o
	rm -f $(TEST)/*-test
	rm -f $(BUILD)/*.o
	rm -f $(TARGETS)


