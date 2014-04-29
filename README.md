Images and animations on 32x32 RGB LED matrix with Teensy v3.1
==============================================================

Dynamically displaying bitmap images and animations on a 32x32 RGB LED Matrix using a Teensy v3.1 and an SD card reader.

Overview
--------
Enumerates the files and directories in the root of the SD card.  Bitmap images are displayed for 2 seconds (not yet configurable).  Directories are expected to have an index.txt file, which are expected to use a simple text format for controlling animations:

filename,duration

For example:

image0.bmp,100  
image1.bmp,100  
image1.bmp,1000
