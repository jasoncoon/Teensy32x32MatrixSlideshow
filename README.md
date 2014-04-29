Images and animations on 32x32 RGB LED matrix with Teensy v3.1
==============================================================

Dynamically displaying bitmap images and animations on a [32x32 RGB LED Matrix](http://www.adafruit.com/products/1484) using a [Teensy v3.1](https://www.pjrc.com/store/teensy31.html), an [SD card adaptor](https://www.pjrc.com/store/sd_adaptor.html), and the SmartMatrix libraries by [Pixelmatix](http://docs.pixelmatix.com/SmartMatrix).

**Disclaimer:** This is my first Arduino project, and really the first C code I've written.  Please let me know if you find any bugs or have any suggestions, comments, etc you'd like to contribute.  Thanks to kjetilei for the modified bmpDraw function!

Overview
--------
Enumerates the files and directories in the root of the SD card.  Bitmap images are displayed for 2 seconds (not yet configurable).  Directories are expected to have an index.txt file, which are expected to use a simple text format for controlling animations:

filename,duration

For example:

image0.bmp,100  
image1.bmp,100  
image1.bmp,1000
