Geranium MiSTer Tutorial Project
By SKuRGe911(Troy Marchant)

Tutorial 4 - SDCARD and TEXT CONSOLE

In this tutorial we wire up the HPIOS sdcard system and allow an sdcard image to be mounted in the software and then read from like a normal file system in C.

I have used the petite fat file system project to implement the higher level of the system. This can be found at
http://elm-chan.org/fsw/ff/00index_p.html

I have also put in a character display console in the video controller so we can use the C code to display messages on the screen. For now the Text console is hardwired to be allways on. but in the next tutorial I will add keyboard and interupts to the cpu so we can turn it on or off with a key.

The ROM file that contains the data of the text needs the program 'srec_cat' to be converted into a .mif file for reading in the synthesis. you can google that one yourself.

You will also now need to make a disk image for mounting the fs. To do this I personally just make a RAM disk that is small then read it into an image using the appropriate software. The image needs to be put in the same dir as the .ROM file for ease of use. Note that now when running you must first mount the disk then load the rom in the F12 menu. it wont work without mounting first. The supplied image is in the ROM directory.