Images and animations on 32x32 RGB LED matrix with Teensy v3.1
==============================================================

Dynamically displaying bitmap images and animations on a [32x32 RGB LED Matrix](http://www.adafruit.com/products/1484) using a [Teensy v3.1](https://www.pjrc.com/store/teensy31.html), an [SD card adaptor](https://www.pjrc.com/store/sd_adaptor.html), and the SmartMatrix libraries by [Pixelmatix](http://docs.pixelmatix.com/SmartMatrix).

**Disclaimer:** This is my first Arduino project, and really the first C code I've written.  Please let me know if you find any bugs or have any suggestions, comments, etc you'd like to contribute.  Thanks to kjetilei for the modified bmpDraw function!

Overview
--------
Enumerates the files and directories in the root of the SD card.  Bitmap images are displayed for 3 seconds (not yet configurable).  Directories are expected to have an index.txt file, which are expected to use a simple text format for controlling animations.  If the root of the SD card has an index.txt file, only the images and directories listed in it are displayed.

Index file format
-----------------

Index files are expected to be named index.txt, and contain any combination of one or more relative directory names and/or relative image file names.  Each line entry should be terminated with a CRLF (/r/n), *including the last entry*.

directory  
directory  
directory  
image,duration  
image,duration  
image,duration  

Example:  

/index.txt  
anim1  
anim2  
image0.bmp,100  
image1.bmp,100  
image1.bmp,1000  

/anim1/index.txt  
image0.bmp,100  
image1.bmp,100  
image2.bmp,1000  
image1.bmp,100  
