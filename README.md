Dynamically displaying bitmap images and animations on a 32x32 RGB LED Matrix using a Teensy v3.1 and an SD card reader.

I finally started trying to load images and animations dynamically.  I came up with a simple text format for controlling animations, and I have a sketch that reads the file and displays the images.  I do not yet have it actually walking through the images and directories yet, but will hopefully soon.

The text format so far:

filename,duration

For example:

image0.bmp,100  
image1.bmp,100  
image1.bmp,1000  

The sketch is currently hard-coded to look for /tinyb/index.txt.  My plan is to enumerate through the contents of the sd card.  If I find a directory, I'll look for an index.txt file.  If I don't find one, I'll just start displaying any bitmaps I find.  If I do, I'll just load the index file and display the images as specified.  Repeat recursively.
