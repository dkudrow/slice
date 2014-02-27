/******************************************************************************
*	kernel.ld
*	 by Alex Chadwick
*
*	A linker script for generation of raspberry pi kernel images.
******************************************************************************/

SECTIONS {

	. = 0x8000;
	
	.init : { *(.init) }

	.text : { *(.text) }

	.data : { *(.data) }

	/DISCARD/ : { *(*) }
}

