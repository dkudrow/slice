# Slice OS
Slice is (going to be) a very basic operatnig system for the Raspberry Pi,
written from the ground up.

## Progress
Below I will report progress on the various sub-systems as I write them. They appear roughly in order of conception.

#### Framebuffer
I have basically written enough of a framebuffer driver to dump basic output on the screen. It supports a built-in bitmapped font and scrolling.

#### Console
The console doesn't do a whole lot. This code will act as a thin layer around the framebuffer driver when it comes time to implement a terminal driver. Basic responsibilities involve managing the cursor position, text wrapping and scrolling when necessary.

#### Keyboard
Currently implementing a cut-down version of FreeBSD's keyboard driver.

#### USB
Implementing the full USB stack is somewhat involved so I am starting with the keyboard driver and will implement only as much of the USB sub-system as the keyboard requires. Have not really started this yet.

## References
This undertaking would not be possible without the vast corpus of freely available tutorials, literature, and forums online. Of particular note are:

#### Technical Manuals
* [ARM Architecture Reference Manual](http://ecee.colorado.edu/ecen3000/labs/lab3/files/DDI0419C_arm_architecture_v6m_reference_manual.pdf)
* [BCM2835 Peripherals](https://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/downloads/SoC-Peripherals.pdf)
* [Arm System Developers Guide](http://www.amazon.com/ARM-System-Developers-Guide-Architecture/dp/1558608745)

#### Tutorials
* [Baking Pi Tutorial](https://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/)
* [David Welch's excellent tutorials](https://github.com/dwelch67/raspberrypi)

#### Existing Code
* [FreeBSD (BCM2835-specific code)](https://github.com/freebsd/freebsd/tree/master/sys/arm/broadcom/bcm2835)
* [Linux (BCM2835-specific code)](https://github.com/raspberrypi/linux/tree/rpi-3.10.y/arch/arm/mach-bcm2708)

#### Miscellany
* [Raspberry Pi Baremetal forum](http://www.raspberrypi.org/forum/viewforum.php?f=72)
