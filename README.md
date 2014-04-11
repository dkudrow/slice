# Slice OS
Slice is (going to be) a very basic operating system for the Raspberry Pi,
written from the ground up.

## Progress
Below I will report progress on the various sub-systems as I write them. They appear roughly in order of conception.

#### Framebuffer
I have basically written enough of a framebuffer driver to dump basic text output on the screen. It supports a built-in bitmapped font and scrolling.

#### Console
The console doesn't do a whole lot. This code will act as a thin layer around the framebuffer driver when it comes time to implement a terminal driver. Basic responsibilities involve managing the cursor position, text wrapping and scrolling when necessary.

#### SD Card Controller
Although the EMMC supports MMC as well, I am only implementing SD to keep things simple. This driver detects and initializes the SD card and provides a simple interface for reading and writing.

## References
This undertaking would not be possible without the vast corpus of freely available tutorials, literature, and forums online. Here is a
comprehensive list of sources that I consulted in writing Slice. All of these documents are (as far as I'm aware) completely free although
some of the hardware manuals require you to _register_ with the vendor - usually just and e-mail will suffice.

#### Technical Manuals
* [ARM Architecture Reference Manual](http://infocenter.arm.com/help/topic/com.arm.doc.ddi0301h/DDI0301H_arm1176jzfs_r0p7_trm.pdf)
* [BCM2835 Peripherals](https://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/downloads/SoC-Peripherals.pdf)
* [Arm System Developers Guide](http://www.amazon.com/ARM-System-Developers-Guide-Architecture/dp/1558608745)
* [MMC 4.4 Specification](http://www.jedec.org/standards-documents/docs/jesd-84-a44)
* [SDHCI 3.0 Specification (simplified)]()

#### Tutorials
* [Baking Pi Tutorial](https://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/)
* [David Welch's excellent tutorials](https://github.com/dwelch67/raspberrypi)

#### Existing Code
* [FreeBSD (BCM2835-specific code)](https://github.com/freebsd/freebsd/tree/master/sys/arm/broadcom/bcm2835)
* [Linux (BCM2835-specific code)](https://github.com/raspberrypi/linux/tree/rpi-3.10.y/arch/arm/mach-bcm2708)
* [GameIcon](https://github.com/XainFaith/Gameicon)

#### Miscellany
* [Raspberry Pi Baremetal forum](http://www.raspberrypi.org/forum/viewforum.php?f=72)
