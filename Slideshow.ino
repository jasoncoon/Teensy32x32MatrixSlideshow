#include "SmartMatrix_32x32.h"
#include "SdFat.h"
#include "SdFatUtil.h"

SmartMatrix matrix;

//const int defaultBrightness = 25*(255/100);
const int defaultBrightness = 255;
const int defaultScrollOffset = 6;
const rgb24 defaultBackgroundColor = {
  0, 0, 0};

// Teensy 3.0 has the LED on pin 13
const int ledPin = 13;

//------------------
#define SD_CS 15
SdFat sd; // set filesystem
SdFile bmpFile; // set filesystem

//for image processing
#define BUFFPIXEL 64

//------------------

// the setup() method runs once, when the sketch starts
void setup() {
  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);
  //    digitalWrite(ledPin, HIGH);
  Serial.begin(38400);

  delay(3000);

  matrix.begin();
  matrix.setBrightness(defaultBrightness);

  matrix.setScrollOffsetFromEdge(defaultScrollOffset);

  matrix.setColorCorrection(cc24);

  Serial.print("Initializing SD card...");
  if (!sd.begin(SD_CS, SPI_FULL_SPEED)) {
    Serial.println("failed!");
    return;
  }
  Serial.println("SD OK!");
}

void loop() {
  
  SdFile indexFile;
  indexFile.open("/index.txt", O_READ);
  
  if(indexFile.isOpen() && indexFile.isFile()) {
    Serial.print("found /index.txt, loading...");
    loadAnimation("/");
    indexFile.close();
  } else {
    SdFile file;
    uint32_t pos = 0;  
    while(1) {
      sd.vwd()->seekSet(pos);
      if(!file.openNext(sd.vwd(), O_READ)) {
        file.close();
        break;
      }
      pos = sd.vwd()->curPosition();
      char name[13];
      file.getFilename(name);
      if(file.isDir()) {
        String dirName = "/";
        dirName.concat(name);
        dirName.concat("/");
        loadAnimation(dirName);
      } else {
        loadImage(name);
      }
      file.close();
    }
  }
}

void loadImage(char *name) {
  uint32_t loadTime;
  uint32_t waitTime = 3000;
  
  loadTime = bmpDraw(name, 0, 0);
  if(loadTime < waitTime) delay(waitTime - loadTime);
}

void loadAnimation(String path) {
  
  SdFile indexFile;
  String indexPath = path;
  indexPath.concat("index.txt");
  char indexPathChar[100];
  indexPath.toCharArray(indexPathChar, sizeof(indexPathChar));
  indexFile.open(indexPathChar, O_READ);
//  Serial.print("");
  Serial.print("opened index file: ");
  Serial.println(indexPath);
  
  int16_t c;
  String imageFilename = path;
  char imageFilenameChar[100];
  String delayString;
  long waitTime;
  uint32_t loadTime;
  boolean foundComma = false;
  char character;
  
  uint32_t startTime = millis();
  
  Serial.println("reading index file");
  
  while ((c = indexFile.read()) > 0) {  
    Serial.print("read character: ");
    character = (char)c;
    Serial.println(character);
    if(character == ',') {
      Serial.println("found comma");
      foundComma = true;
    } else if(character == '\r') {
      indexFile.read(); // throw away \n
      Serial.println("found line ending");
      imageFilename.toCharArray(imageFilenameChar, sizeof(imageFilenameChar));
      
      if(delayString.length() < 1 && !imageFilename.endsWith(".bmp")) {
        imageFilename.concat("/");
        loadAnimation(imageFilename);
        foundComma = false;
        imageFilename = path;
        delayString = "";
        startTime = millis();
      } else {
        waitTime = delayString.toInt();
        loadTime = millis() - startTime;
        loadTime += bmpDraw(imageFilenameChar, 0, 0);
        if(loadTime < waitTime) delay(waitTime - loadTime);
        foundComma = false;
        imageFilename = path;
        delayString = "";
        startTime = millis();
      }
    } else if (isdigit(character) && foundComma) {
      Serial.println("found delay digit");
      delayString.concat(character);
    } else {
      Serial.println("found image filename character");
      imageFilename.concat(character);      
    }
  }
  
  indexFile.close();
}

uint32_t bmpDraw(char *filename, uint8_t x, uint8_t y) {

  SdFile   bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  //  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();

  if((x >= matrix.getScreenWidth()) || (y >= matrix.getScreenHeight())) return 0;

  Serial.println();
  Serial.print("Loading image '");
  Serial.print(filename);
  Serial.println('\'');

  // Open requested file on SD card
  bmpFile.open(filename, O_READ);

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.print("File size: "); 
    Serial.println(read32(bmpFile));
    read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print("Image Offset: "); 
    Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print("Header size: "); 
    Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print("bmpDepth: "); 
      Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print("Image size: ");
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;

        if((x+w-1) >= matrix.getScreenWidth())  w = matrix.getScreenWidth()  - x;
        if((y+h-1) >= matrix.getScreenHeight()) h = matrix.getScreenWidth() - y;

        for (row=0; row<h; row++) { // For each scanline...
          //          tft.goTo(x, y+row);


          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
          pos = bmpImageoffset + row * rowSize;
          if(bmpFile.curPosition() != pos) { // Need seek?
            bmpFile.seekSet(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          // optimize by setting pins now
          for (col=0; col<w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to Matrix format, push to display
            rgb24 color;
            color.blue = sdbuffer[buffidx++];
            color.green = sdbuffer[buffidx++];
            color.red = sdbuffer[buffidx++];

            matrix.drawPixel(x+col, y+row, color);

          } // end pixel
        } // end scanline
        matrix.swapBuffers(true);
        Serial.print("Loaded in ");
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) {
    Serial.println("BMP format not recognized.");
    return 0;
  }
  else {
    return millis() - startTime;
  }
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(SdFile& f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(SdFile& f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}
