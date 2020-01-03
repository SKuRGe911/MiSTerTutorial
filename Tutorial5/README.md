Geranium MiSTer Tutorial Project
By SKuRGe911(Troy Marchant)

Tutorial 5 - IRQs and Keyboard Input

In this tutorial we add an IRQ controller and connect it to the CPU. We create a Verticle Blanking Interval(VBI) interupt and an interupt for keyboard input.

Now by pushing the Escape key we can toggle on and off our text display console. And I have added a command line system which you can enter commands and push Enter to have the c code run.

We leave the first 3 interupts empty because the CPU implementation we are using uses them for special purposes. Also the interupts MASKS on the CPU definition show that we are using an interupt without having to ACK it.

In the next tutorial I will add an sdram controller, DMA controller and get the video module to read its frame buffer from the sdram.