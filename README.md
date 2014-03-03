# Slice OS
Slice is (going to be) a very basic operatnig system for the Raspberry Pi,
written from the ground up.

## Progress
Below I will report progress on the various sub-systems as I write them. They appear roughly in order of conception.

### Framebuffer
I have basically written enough of a framebuffer driver to dump basic output on the screen.

### Console
The console doesn't do a whole lot. This code will act as a thin layer around the framebuffer driver when it comes time to implement a terminal driver.

### Keyboard
Currently implementing a cut-down version of FreeBSD's keyboard driver.

### USB
Implementing the full USB stack is somewhat involved so I am starting with the keyboard driver and will implement only as much of the USB sub-system as the keyboard requires. Have
not really started this yet.
