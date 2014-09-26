# src/
Below is a brief summary of each file. They are listed loosely in order of
inclusion in the OS.

*src/start.S* -- The entry point into the OS - the first code that gets
executed once the kernel image is loaded into memory. It does some
low-level initialization before passing control off to the OS main.

*src/main.c* -- The `main' function for the OS.

*src/irq.c* -- IRQ interrupt handler.

*src/timer.c* -- An interface to the system timer (not the ARM Core timer!)

*src/mailbox.c* -- Mailbox communication interface between the ARM Core and
the VideoCore. Facilitates communication with on-chip peripherals.

*src/gpio.c* -- Interface to the BCM2835 GPIO controller.

*src/emmc.c* -- Interface to the BCM2835 EMMC (external mass media
controller)

*src/framebuffer.c* -- Interface to the GPU framebuffer.

*src/console.c* -- Console driver, built around the framebuffer interface.

*src/printf.c* -- A simple printf function for debugging.

*src/memory_utils.c* -- Memory utilities.

*src/led.c* -- Some routines to control the BCM2835 LED's. Handy for
debugging before the framebuffer is online.

*src/malloc.c* -- An implementation of malloc to allow dynamic memory allocation.
