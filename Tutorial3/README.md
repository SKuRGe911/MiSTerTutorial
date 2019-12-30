Geranium MiSTer Tutorial Project
By SKuRGe911(Troy Marchant)

Tutorial 3 - CPU and DDRRAM controller

In this tutorial I have moved all the system modules into a single module called geranium_system which is instantiated once in the main file.

The cpu is a riscv implementation which can be found at https://github.com/cliffordwolf/picorv32 . The riscv toolchain can be found at https://gnutoolchains.com/risc-v/ for windows.
Remember to add it to your PATH variable.

I also use visual studio 2019 Preview for the editing and compiling of the ROM files. So you should get that too. You might need Cygwin too if you are using windows for the MAKE utility.

For this tutorial, the system from the previous tutorial is used to download the menu selected .ROM file into DDRRAM. Once it is loaded it then passes of control to the CPU. The high memory addresses determine what the cpu is accessing. I have set one constant to DDR RAM and one to accessing IO(The video memory). The demo rom is just a simple program that draws some brenham lines to the screen. not much really. But it proves the ram and cpu are both working together.

In the next tutorial I will add SDCARD functionality to the system.