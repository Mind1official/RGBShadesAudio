//   Graphical effects to run on the RGB Shades LED array
//   Each function should have the following components:
//    * Must be declared void with no parameters or will break function pointer array
//    * Check effectInit, if false then init any required settings and set effectInit true
//    * Set effectDelay (the time in milliseconds until the next run of this effect)
//    * All animation should be controlled with counters and effectDelay, no delay() or loops
//    * Pixel data should be written using leds[XY(x,y)] to map coordinates to the RGB Shades layout

// Triple Sine Waves
void threeSine() {

  static byte sineOffset = 0; // counter for current position of sine waves

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 20;
    fadeActive = 0;
  }

  // Draw one frame of the animation into the LED array
  for (byte x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {

      // Calculate "sine" waves with varying periods
      // sin8 is used for speed; cos8, quadwave8, or triwave8 would also work here
      byte sinDistanceR = qmul8(abs(y * (255 / kMatrixHeight) - sin8(sineOffset * 9 + x * 16)), 2);
      byte sinDistanceG = qmul8(abs(y * (255 / kMatrixHeight) - sin8(sineOffset * 10 + x * 16)), 2);
      byte sinDistanceB = qmul8(abs(y * (255 / kMatrixHeight) - sin8(sineOffset * 11 + x * 16)), 2);

      leds[XY(x, y)] = CRGB(255 - sinDistanceR, 255 - sinDistanceG, 255 - sinDistanceB);
    }
  }

  sineOffset++; // byte will wrap from 255 to 0, matching sin8 0-255 cycle

}


// RGB Plasma
void plasma() {

  static byte offset  = 0; // counter for radial color wave motion
  static int plasVector = 0; // counter for orbiting plasma center

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
    fadeActive = 0;
  }

  // Calculate current center of plasma pattern (can be offscreen)
  int xOffset = cos8(plasVector / 256);
  int yOffset = sin8(plasVector / 256);

  // Draw one frame of the animation into the LED array
  for (int x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {
      byte color = sin8(sqrt(sq(((float)x - 7.5) * 10 + xOffset - 127) + sq(((float)y - 2) * 10 + yOffset - 127)) + offset);
      leds[XY(x, y)] = CHSV(color, 255, 255);
    }
  }

  offset++; // wraps at 255 for sin8
  plasVector += 16; // using an int for slower orbit (wraps at 65536)

}


// Scanning pattern left/right, uses global hue cycle
void rider() {

  static byte riderPos = 0;

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 5;
    riderPos = 0;
    fadeActive = 0;
  }

  // Draw one frame of the animation into the LED array
  for (byte x = 0; x < kMatrixWidth; x++) {
    int brightness = abs(x * (256 / kMatrixWidth) - triwave8(riderPos) * 2 + 127) * 3;
    if (brightness > 255) brightness = 255;
    brightness = 255 - brightness;
    CRGB riderColor = CHSV(cycleHue, 255, brightness);
    for (byte y = 0; y < kMatrixHeight; y++) {
      leds[XY(x, y)] = riderColor;
    }
  }

  riderPos++; // byte wraps to 0 at 255, triwave8 is also 0-255 periodic

}


// Shimmering noise, uses global hue cycle
void glitter() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 15;
    fadeActive = 0;
  }

  // Draw one frame of the animation into the LED array
  for (int x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {
      leds[XY(x, y)] = CHSV(cycleHue, 255, random8(5) * 63);
    }
  }

}


// Fills saturated colors into the array from alternating directions
void colorFill() {

  static byte currentColor = 0;
  static byte currentRow = 0;
  static byte currentDirection = 0;

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 45;
    currentColor = 0;
    currentRow = 0;
    currentDirection = 0;
    currentPalette = RainbowColors_p;
    fadeActive = 0;
  }

  // test a bitmask to fill up or down when currentDirection is 0 or 2 (0b00 or 0b10)
  if (!(currentDirection & 1)) {
    effectDelay = 45; // slower since vertical has fewer pixels
    for (byte x = 0; x < kMatrixWidth; x++) {
      byte y = currentRow;
      if (currentDirection == 2) y = kMatrixHeight - 1 - currentRow;
      leds[XY(x, y)] = currentPalette[currentColor];
    }
  }

  // test a bitmask to fill left or right when currentDirection is 1 or 3 (0b01 or 0b11)
  if (currentDirection & 1) {
    effectDelay = 20; // faster since horizontal has more pixels
    for (byte y = 0; y < kMatrixHeight; y++) {
      byte x = currentRow;
      if (currentDirection == 3) x = kMatrixWidth - 1 - currentRow;
      leds[XY(x, y)] = currentPalette[currentColor];
    }
  }

  currentRow++;

  // detect when a fill is complete, change color and direction
  if ((!(currentDirection & 1) && currentRow >= kMatrixHeight) || ((currentDirection & 1) && currentRow >= kMatrixWidth)) {
    currentRow = 0;
    currentColor += random8(3, 6);
    if (currentColor > 15) currentColor -= 16;
    currentDirection++;
    if (currentDirection > 3) currentDirection = 0;
    effectDelay = 300; // wait a little bit longer after completing a fill
  }


}

// Emulate 3D anaglyph glasses
void threeDee() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 50;
    fadeActive = 0;
  }

  for (byte x = 0; x < kMatrixWidth; x++) {
    for (byte y = 0; y < kMatrixHeight; y++) {
      if (x < 7) {
        leds[XY(x, y)] = CRGB::Blue;
      } else if (x > 8) {
        leds[XY(x, y)] = CRGB::Red;
      } else {
        leds[XY(x, y)] = CRGB::Black;
      }
    }
  }

  leds[XY(6, 0)] = CRGB::Black;
  leds[XY(9, 0)] = CRGB::Black;

}

// Random pixels scroll sideways, uses current hue
#define rainDir 0
void sideRain() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 30;
    fadeActive = 0;
  }

  scrollArray(rainDir);
  byte randPixel = random8(kMatrixHeight);
  for (byte y = 0; y < kMatrixHeight; y++) leds[XY((kMatrixWidth - 1) * rainDir, y)] = CRGB::Black;
  leds[XY((kMatrixWidth - 1)*rainDir, randPixel)] = CHSV(cycleHue, 255, 255);

}

// Pixels with random locations and random colors selected from a palette
// Use with the fadeAll function to allow old pixels to decay
void confetti() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
    selectRandomPalette();
    fadeActive = 1;
  }

  // scatter random colored pixels at several random coordinates
  for (byte i = 0; i < 4; i++) {
    leds[XY(random16(kMatrixWidth), random16(kMatrixHeight))] = ColorFromPalette(currentPalette, random16(255), 255); //CHSV(random16(255), 255, 255);
    random16_add_entropy(1);
  }

}


// Draw slanting bars scrolling across the array, uses current hue
void slantBars() {

  static byte slantPos = 0;

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 5;
    fadeActive = 0;
  }

  for (byte x = 0; x < kMatrixWidth; x++) {
    for (byte y = 0; y < kMatrixHeight; y++) {
      leds[XY(x, y)] = CHSV(cycleHue, 255, sin8(x * 32 + y * 32 + slantPos));
    }
  }

  slantPos -= 4;

}


#define NORMAL 0
#define RAINBOW 1
#define charSpacing 2
// Scroll a text string
void scrollText(byte message, byte style, CRGB fgColor, CRGB bgColor) {
  static byte currentMessageChar = 0;
  static byte currentCharColumn = 0;
  static byte paletteCycle = 0;
  static CRGB currentColor;
  static byte bitBuffer[16] = {0};
  static byte bitBufferPointer = 0;

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 35;
    currentMessageChar = 0;
    currentCharColumn = 0;
    selectFlashString(message);
    loadCharBuffer(loadStringChar(message, currentMessageChar));
    currentPalette = RainbowColors_p;
    for (byte i = 0; i < kMatrixWidth; i++) bitBuffer[i] = 0;
    fadeActive = 0;
  }

  paletteCycle += 15;

  if (currentCharColumn < 5) { // characters are 5 pixels wide
    bitBuffer[(bitBufferPointer + kMatrixWidth - 1) % kMatrixWidth] = charBuffer[currentCharColumn]; // character
  } else {
    bitBuffer[(bitBufferPointer + kMatrixWidth - 1) % kMatrixWidth] = 0; // space
  }

  CRGB pixelColor;
  for (byte x = 0; x < kMatrixWidth; x++) {
    for (byte y = 0; y < 5; y++) { // characters are 5 pixels tall
      if (bitRead(bitBuffer[(bitBufferPointer + x) % kMatrixWidth], y) == 1) {
        if (style == RAINBOW) {
          pixelColor = ColorFromPalette(currentPalette, paletteCycle+y*16, 255);
        } else {
          pixelColor = fgColor;
        }
      } else {
        pixelColor = bgColor;
      }
      leds[XY(x, y)] = pixelColor;
    }
  }

  currentCharColumn++;
  if (currentCharColumn > (4 + charSpacing)) {
    currentCharColumn = 0;
    currentMessageChar++;
    char nextChar = loadStringChar(message, currentMessageChar);
    if (nextChar == 0) { // null character at end of strong
      currentMessageChar = 0;
      nextChar = loadStringChar(message, currentMessageChar);
    }
    loadCharBuffer(nextChar);
  }

  bitBufferPointer++;
  if (bitBufferPointer > 15) bitBufferPointer = 0;

}


void scrollTextZero() {
  scrollText(0, NORMAL, CRGB::Red, CRGB::Black);
}

void scrollTextOne() {
  scrollText(1, RAINBOW, 0, CRGB::Black);
}

void scrollTextTwo() {
  scrollText(2, NORMAL, CRGB::Green, CRGB(0,0,8));
}



#define analyzerFadeFactor 5
#define analyzerScaleFactor 1.5
#define analyzerPaletteFactor 2
void drawAnalyzer() {
  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
    selectRandomAudioPalette();
    audioActive = true;
    fadeActive = 0;
  }

  CRGB pixelColor;

  const float yScale = 255.0 / kMatrixHeight;

  for (byte x = 0; x < kMatrixWidth / 2; x++) {
    byte newX = x;
    int freqVal;
    if (x < 2) {
      newX = 0;
      freqVal = spectrumDecay[newX] / 2;
    } else {
      newX = x - 1;
      freqVal = spectrumDecay[newX];
    }
    
    for (byte y = 0; y < kMatrixHeight; y++) {
      if (x > 6) {
        pixelColor = ColorFromPalette(currentPalette, 0, 0);
      } else {
        int senseValue = freqVal / analyzerScaleFactor - yScale * (kMatrixHeight - 1 - y);
        int pixelBrightness = senseValue * analyzerFadeFactor;
        if (pixelBrightness > 255) pixelBrightness = 255;
        if (pixelBrightness < 0) pixelBrightness = 0;

        int pixelPaletteIndex = senseValue / analyzerPaletteFactor - 15;
        if (pixelPaletteIndex > 240) pixelPaletteIndex = 240;
        if (pixelPaletteIndex < 0) pixelPaletteIndex = 0;

        pixelColor = ColorFromPalette(currentPalette, pixelPaletteIndex, pixelBrightness);
      }
      leds[XY(x, y)] = pixelColor;
      leds[XY(kMatrixWidth - x - 1, y)] = pixelColor;
    }
  }


}

#define VUFadeFactor 5
#define VUScaleFactor 2.0
#define VUPaletteFactor 1.5
void drawVU() {
  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
    selectRandomAudioPalette();
    audioActive = true;
    fadeActive = 0;
  }

  CRGB pixelColor;

  const float xScale = 255.0 / (kMatrixWidth / 2);
  float specCombo = (spectrumDecay[0] + spectrumDecay[1] + spectrumDecay[2] + spectrumDecay[3]) / 4.0;

  for (byte x = 0; x < kMatrixWidth / 2; x++) {
    int senseValue = specCombo / VUScaleFactor - xScale * x;
    int pixelBrightness = senseValue * VUFadeFactor;
    if (pixelBrightness > 255) pixelBrightness = 255;
    if (pixelBrightness < 0) pixelBrightness = 0;

    int pixelPaletteIndex = senseValue / VUPaletteFactor - 15;
    if (pixelPaletteIndex > 240) pixelPaletteIndex = 240;
    if (pixelPaletteIndex < 0) pixelPaletteIndex = 0;

    pixelColor = ColorFromPalette(currentPalette, pixelPaletteIndex, pixelBrightness);

    for (byte y = 0; y < kMatrixHeight; y++) {
      leds[XY(x, y)] = pixelColor;
      leds[XY(kMatrixWidth - x - 1, y)] = pixelColor;
    }
  }


}


void RGBpulse() {

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 1;
    audioActive = true;
    fadeActive = 1;
  }

  static byte RGBcycle = 0;

  if (beatDetect()) {

    switch (RGBcycle) {
      case 0:
        fillAll(CRGB::Red);
        break;
      case 1:
        fillAll(CRGB::Lime);
        break;
      case 2:
        fillAll(CRGB::Blue);
        break;
    }

    RGBcycle++;
    if (RGBcycle > 2) RGBcycle = 0;
  }

}

// RGB Plasma
void audioPlasma() {

  static byte offset  = 0; // counter for radial color wave motion
  static int plasVector = 0; // counter for orbiting plasma center

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
    selectRandomAudioPalette();
    audioActive = true;
    fadeActive = 0;
  }

  // Calculate current center of plasma pattern (can be offscreen)
  int xOffset = (cos8(plasVector / 256)-127)/2;
  int yOffset = (sin8(plasVector / 256)-127)/2;

  //int xOffset = 0;
  //int yOffset = 0;


  // Draw one frame of the animation into the LED array
  for (int x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {
      byte color = sin8(sqrt(sq(((float)x - 7.5) * 12 + xOffset) + sq(((float)y - 2) * 12 + yOffset)) + offset);
      leds[XY(x, y)] = ColorFromPalette(currentPalette, color, 255);
    }
  }

  offset++; // wraps at 255 for sin8
  plasVector += (spectrumDecay[0] + spectrumDecay[1] + spectrumDecay[2]); // using an int for slower orbit (wraps at 65536)

}



void audioCirc() {
  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
    audioActive = true;
    fadeActive = 0;
  }

  int lowfreq; int medfreq; int hifreq;

  for (byte x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {
      lowfreq = spectrumValue[0]/(hypot(x-7.5, y-2)*1.5);
      medfreq = spectrumDecay[2]/(hypot(x-7.5, y-2)*1.1);
      hifreq = spectrumDecay[5]/(hypot(x-7.5, y-2)*1.2);

      if (lowfreq < 90) lowfreq = 0;
      if (lowfreq > 255) lowfreq = 255;

      if (medfreq < 60) medfreq = 0;
      if (medfreq > 255) medfreq = 255;

      if (hifreq < 60) hifreq = 0;
      if (hifreq > 255) hifreq = 255;
      
      leds[XY(x,y)] = CRGB(lowfreq, medfreq, hifreq);
    }
  }

  
}

// RGB Plasma
void audioSpin() {

  static byte offset  = 0; // counter for radial color wave motion
  static int plasVector = 0; // counter for orbiting plasma center

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
    selectRandomAudioPalette();
    audioActive = true;
    fadeActive = 0;
  }

  // Calculate current center of plasma pattern (can be offscreen)
  int xOffset = 0;
  int yOffset = 0;

  //int xOffset = 0;
  //int yOffset = 0;


  // Draw one frame of the animation into the LED array
  for (int x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {
      //byte color = sin8(sqrt(sq(((float)x - 7.5) * 12 + xOffset) + sq(((float)y - 2) * 12 + yOffset)) + offset);
      float tanxy = ((float)(x-7.5)/(float)((y-2))*2);
      byte color = sin8(tanxy*10+plasVector/100);
      leds[XY(x, y)] = ColorFromPalette(currentPalette, color, 255);
    }
  }

  offset++; // wraps at 255 for sin8
  plasVector += (spectrumDecay[0] + spectrumDecay[1] + spectrumDecay[2]); // using an int for slower orbit (wraps at 65536)

}


void audioStripes() {
  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 25;
    selectRandomAudioPalette();
    fadeActive = 0;
    audioActive = true;
  }

  CRGB linecolor;
  int audioLevel;
  int brightLevel;

  for (byte y = 0; y < 5; y++) {

    //audioLevel = spectrumDecay[y+1] / 2.0;
    audioLevel = spectrumPeaks[y+1] / 1.8;
    if (y == 0) audioLevel /= 2;
    if (audioLevel > 239) audioLevel = 239;


    
    for (byte x = 0; x < kMatrixWidth; x++) {
          brightLevel = (audioLevel-(abs(7.5-x)*20)) * 3.0;
          if (brightLevel < 0) brightLevel = 0;
          if (brightLevel > 254) brightLevel = 254;
          linecolor = ColorFromPalette(currentPalette, audioLevel, brightLevel);
      leds[XY(x, 4-y)] = linecolor;
    }
  }
}

void audioStripesB() {
  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 20;
    selectRandomAudioPalette();
    fadeActive = 0;
    audioActive = true;
  }

  CRGB linecolor;
  int audioLevel;
  int brightLevel;

  // Updated frequency mapping for better visibility:
  // y0 (top) = high freq (band 5)
  // y1 = bass (band 1) 
  // y2 (center) = low beat (band 0)
  // y3 = bass (band 2)
  // y4 (bottom) = highest freq (band 6)
  
  byte frequencyMap[5] = {5, 1, 0, 2, 5}; // Maps y position to spectrum band
  
  for (byte y = 0; y < 5; y++) {
    audioLevel = spectrumPeaks[frequencyMap[y]] / 1.8;  // Note: removed +1 since band 6 is max
    
    // Special handling for different frequency ranges
    if (y == 2) audioLevel /= 2;  // Low beat in center
    if (y == 0 || y == 4) audioLevel *= 2.5;  // Boost high frequencies for visibility
    
    if (audioLevel > 239) audioLevel = 239;

    // Left lens (x = 0 to 6)
    for (byte x = 0; x < 7; x++) {
      brightLevel = (audioLevel-(abs(3-x)*20)) * 2.0;  // Center at x=3 for left lens
      if (brightLevel < 0) brightLevel = 0;
      if (brightLevel > 254) brightLevel = 254;
      linecolor = ColorFromPalette(currentPalette, audioLevel, brightLevel);
      leds[XY(x, 4-y)] = linecolor;
    }

    // Right lens (x = 9 to 15) 
    for (byte x = 9; x < kMatrixWidth; x++) {
      brightLevel = (audioLevel-(abs(12-x)*20)) * 2.0;  // Center at x=12 for right lens
      if (brightLevel < 0) brightLevel = 0;
      if (brightLevel > 254) brightLevel = 254;
      linecolor = ColorFromPalette(currentPalette, audioLevel, brightLevel);
      leds[XY(x, 4-y)] = linecolor;
    }

    // Bridge area (x = 7 to 8) - keep dark or use minimal effect
    for (byte x = 7; x <= 8; x++) {
      leds[XY(x, 4-y)] = CRGB::Black;
    }
  }
}


//leds run around the periphery of the shades, changing color every go 'round
void shadesOutline() {
  
  static uint8_t x = 0;
  
  //startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 25;
    FastLED.clear();
    currentPalette = RainbowColors_p;
    fadeActive = 2;
  }

  CRGB pixelColor = CHSV(cycleHue, 255, 255);
  leds[OutlineMap(x)] = pixelColor;

  x++;
  if (x > (OUTLINESIZE-1)) x = 0;
  
}

void audioShadesOutline() {
  
  static float x = 0;
  
  //startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 15;
    FastLED.clear();
    currentPalette = RainbowColors_p;
    fadeActive = 10;
    audioActive = true;
  }

  static uint8_t beatcount = 0;

  int brightness = (spectrumDecay[0] + spectrumDecay[1]);
  if (brightness > 255) brightness = 255;

  CRGB pixelColor = CHSV(cycleHue, 255, brightness);
  
  for (byte k = 0; k < 4; k++) {
    leds[OutlineMap(x+(OUTLINESIZE/4-1)*k)] += pixelColor;
  }

  float xincr = (spectrumDecay[0] + spectrumDecay[1]) / 600.0;
  if (xincr > 1.0) xincr = 1.0;
  if (xincr < 0.1) xincr = 0.1;




  if (beatDetect()) {
    beatcount++;
    if (beatcount >= 32 ) beatcount = 0;
  }

  if (beatcount < 16 ) {
    x += xincr;
  } else {
    x -= xincr;
  }
  
  if (x > (OUTLINESIZE-1)) x = 0;
  if (x < 0) x = OUTLINESIZE - 1;
  
}

void audioShadesOutlineB() {
  
  static float x = 0;
  
  //startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 15;
    FastLED.clear();
    currentPalette = currentPalette = CRGBPalette16(CRGB::Black, CRGB::Crimson, CRGB::Red);
    fadeActive = 10;
    audioActive = true;
  }

  static uint8_t beatcount = 0;

  int brightness = (spectrumDecay[0] + spectrumDecay[1]);
  if (brightness > 255) brightness = 255;

  CRGB pixelColor = CHSV(cycleHue, 255, brightness);
  
  for (byte k = 0; k < 4; k++) {
    leds[OutlineMap(x+(OUTLINESIZE/4-1)*k)] += pixelColor;
  }

  float xincr = (spectrumDecay[0] + spectrumDecay[1]) / 600.0;
  if (xincr > 1.0) xincr = 1.0;
  if (xincr < 0.1) xincr = 0.1;




  if (beatDetect()) {
    beatcount++;
    if (beatcount >= 32 ) beatcount = 0;
  }

  if (beatcount < 16 ) {
    x += xincr;
  } else {
    x -= xincr;
  }
  
  if (x > (OUTLINESIZE-1)) x = 0;
  if (x < 0) x = OUTLINESIZE - 1;
  
}

void mind() {
  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 25;
    audioActive = true;
    fadeActive = 0;
  }

  static byte growthStage = 0;  // 0=off, 1=center, 2=3x3, 3=5x5, 4=all
  static unsigned long lastStageChange = 0;
  static byte stageHoldTime = 100; // ms to hold each stage
  
  // Calculate bass level for growth trigger
  float bassLevel = (spectrumDecay[0] + spectrumDecay[1] + spectrumDecay[2]) / 3.0;
  
  // Beat detection for stage progression
  static boolean lastBeatState = false;
  boolean currentBeat = beatDetect();
  boolean beatTriggered = (currentBeat && !lastBeatState);
  lastBeatState = currentBeat;
  
  // Define lens centers
  byte leftCenterX = 3, rightCenterX = 12, centerY = 2;
  
  // Clear display
  FastLED.clear();
  
  // Determine color based on stage and bass level
  CRGB stageColor;
  switch(growthStage) {
    case 0: // Off/Black
      stageColor = CRGB::Black;
      break;
    case 1: // Center pixel - Crimson
      stageColor = CRGB::Crimson;
      break;
    case 2: // 3x3 - Crimson to Red transition
      if (bassLevel > 150) {
        stageColor = CRGB::Red;
      } else {
        stageColor = CRGB::Crimson;
      }
      break;
    case 3: // 5x5 - Red
      stageColor = CRGB::Red;
      break;
    case 4: // All on - Bright Red
      stageColor = CRGB::Red;
      stageColor.nscale8(255); // Full brightness
      break;
    default:
      stageColor = CRGB::Black;
      break;
  }
  
  // Scale brightness based on bass level
  byte brightness = map(bassLevel, 0, 400, 50, 255);
  if (brightness > 255) brightness = 255;
  if (brightness < 50) brightness = 50;
  stageColor.nscale8(brightness);
  
  // Draw current stage pattern
  switch(growthStage) {
    case 0: // Off - do nothing
      break;
      
    case 1: // Center pixel
      leds[XY(leftCenterX, centerY)] = stageColor;
      leds[XY(rightCenterX, centerY)] = stageColor;
      break;
      
    case 2: // 3x3 pattern
      for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
          // Left lens
          if (leftCenterX + dx >= 0 && leftCenterX + dx < 7 && 
              centerY + dy >= 0 && centerY + dy < 5) {
            leds[XY(leftCenterX + dx, centerY + dy)] = stageColor;
          }
          // Right lens
          if (rightCenterX + dx >= 9 && rightCenterX + dx < 16 && 
              centerY + dy >= 0 && centerY + dy < 5) {
            leds[XY(rightCenterX + dx, centerY + dy)] = stageColor;
          }
        }
      }
      break;
      
    case 3: // 5x5 pattern
      for (int dx = -2; dx <= 2; dx++) {
        for (int dy = -2; dy <= 2; dy++) {
          // Left lens
          if (leftCenterX + dx >= 0 && leftCenterX + dx < 7 && 
              centerY + dy >= 0 && centerY + dy < 5) {
            leds[XY(leftCenterX + dx, centerY + dy)] = stageColor;
          }
          // Right lens
          if (rightCenterX + dx >= 9 && rightCenterX + dx < 16 && 
              centerY + dy >= 0 && centerY + dy < 5) {
            leds[XY(rightCenterX + dx, centerY + dy)] = stageColor;
          }
        }
      }
      break;
      
    case 4: // All on (full lenses)
      for (byte x = 0; x < 7; x++) {
        for (byte y = 0; y < 5; y++) {
          leds[XY(x, y)] = stageColor;
        }
      }
      for (byte x = 9; x < 16; x++) {
        for (byte y = 0; y < 5; y++) {
          leds[XY(x, y)] = stageColor;
        }
      }
      break;
  }
  
  // Stage progression logic
  if (beatTriggered && currentMillis - lastStageChange > stageHoldTime) {
    growthStage++;
    if (growthStage > 4) {
      growthStage = 0; // Reset to off/black
    }
    lastStageChange = currentMillis;
    
    // Adjust hold time based on bass level for dynamic timing
    stageHoldTime = map(bassLevel, 0, 400, 200, 50);
  }
  
  // Auto-reset if no beat for a while (return to black)
  if (currentMillis - lastStageChange > 2000) {
    growthStage = 0;
    lastStageChange = currentMillis;
  }
}

void mindAudio() {
  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 20;
    audioActive = true;
    fadeActive = 0;
  }

  // Static variables for smoothing
  static float smoothedIntensity = 0;
  static byte lastExpansion = 0;
  static unsigned long lastExpansionChange = 0;
  static byte targetExpansion = 0;
  
  // Calculate overall audio intensity
  float totalIntensity = 0;
  for (byte i = 0; i < 7; i++) {
    totalIntensity += spectrumDecay[i];
  }
  totalIntensity = totalIntensity / 7.0;
  
  // Smooth the intensity with exponential averaging
  smoothedIntensity = smoothedIntensity * 0.85 + totalIntensity * 0.15;
  
  // Define lens centers
  byte leftCenterX = 3, rightCenterX = 12, centerY = 2;
  
  // Clear display
  FastLED.clear();
  
  // Beat detection for intensity boost (no white)
  static byte beatBoost = 0;
  if (beatDetect()) {
    beatBoost = 80; // Reduced from 100 for smoother effect
  }
  
  // Fade beat boost more gradually
  if (beatBoost > 0) {
    beatBoost -= 3; // Slower fade (was 6)
  }
  
  // Determine color and brightness based on smoothed intensity
  CRGB ringColor;
  byte baseBrightness = map(smoothedIntensity, 0, 400, 0, 200);
  if (baseBrightness > 200) baseBrightness = 200;
  
  // Add beat boost to brightness
  byte totalBrightness = baseBrightness + beatBoost;
  if (totalBrightness > 255) totalBrightness = 255;
  
  // Color progression: Off -> Dark Red -> Red -> Crimson
  if (smoothedIntensity < 30) {
    ringColor = CRGB::Black; // Off
  } else if (smoothedIntensity < 200) {
    // Dark Red
    ringColor = CRGB(totalBrightness/2, 0, 0); // Pure red, dimmed
  } else if (smoothedIntensity < 300) {
    // Red
    ringColor = CRGB(totalBrightness, 0, 0); // Pure red
  } else {
    // Crimson (red with slight blue tint)
    ringColor = CRGB(totalBrightness, 0, totalBrightness/8); // Mostly red with tiny blue
  }
  
  // Calculate target expansion based on smoothed intensity + beat boost
  targetExpansion = map(smoothedIntensity + beatBoost, 0, 500, 0, 4);
  if (targetExpansion > 4) targetExpansion = 4;
  
  // Smooth expansion changes with delay
  byte currentExpansion = lastExpansion;
  if (targetExpansion != lastExpansion) {
    // Only change expansion if enough time has passed (prevents jitter)
    if (currentMillis - lastExpansionChange > 50) { // 50ms delay between changes
      if (targetExpansion > lastExpansion) {
        currentExpansion = lastExpansion + 1; // Grow one ring at a time
      } else {
        currentExpansion = lastExpansion - 1; // Shrink one ring at a time
      }
      lastExpansion = currentExpansion;
      lastExpansionChange = currentMillis;
    }
  }
  
  // Add fade effect for smooth transitions
  static byte fadeAmount = 0;
  if (currentExpansion != targetExpansion) {
    fadeAmount = 50; // Partial brightness during transitions
  } else {
    fadeAmount = 0; // Full brightness when stable
  }
  
  // Apply fade to ring color
  CRGB fadedColor = ringColor;
  if (fadeAmount > 0) {
    fadedColor.nscale8(255 - fadeAmount);
    fadeAmount -= 2; // Gradually remove fade
    if (fadeAmount < 0) fadeAmount = 0;
  }
  
  // Ring 1: Center pixel (always on if any audio)
  if (currentExpansion >= 1) {
    leds[XY(leftCenterX, centerY)] = fadedColor;
    leds[XY(rightCenterX, centerY)] = fadedColor;
  }
  
  // Ring 2: 3x3 pattern
  if (currentExpansion >= 2) {
    for (int dx = -1; dx <= 1; dx++) {
      for (int dy = -1; dy <= 1; dy++) {
        // Skip center pixel (already handled above)
        if (dx == 0 && dy == 0) continue;
        
        // Left lens
        if (leftCenterX + dx >= 0 && leftCenterX + dx < 7 && 
            centerY + dy >= 0 && centerY + dy < 5) {
          leds[XY(leftCenterX + dx, centerY + dy)] = fadedColor;
        }
        // Right lens
        if (rightCenterX + dx >= 9 && rightCenterX + dx < 16 && 
            centerY + dy >= 0 && centerY + dy < 5) {
          leds[XY(rightCenterX + dx, centerY + dy)] = fadedColor;
        }
      }
    }
  }
  
  // Ring 3: 5x5 outline
  if (currentExpansion >= 3) {
    for (int dx = -2; dx <= 2; dx++) {
      for (int dy = -2; dy <= 2; dy++) {
        // Only draw outline - skip inner 3x3 area
        if (abs(dx) < 2 && abs(dy) < 2) continue;
        
        // Left lens
        if (leftCenterX + dx >= 0 && leftCenterX + dx < 7 && 
            centerY + dy >= 0 && centerY + dy < 5) {
          leds[XY(leftCenterX + dx, centerY + dy)] = fadedColor;
        }
        // Right lens
        if (rightCenterX + dx >= 9 && rightCenterX + dx < 16 && 
            centerY + dy >= 0 && centerY + dy < 5) {
          leds[XY(rightCenterX + dx, centerY + dy)] = fadedColor;
        }
      }
    }
  }
  
  // Ring 4: Full lens fill
  if (currentExpansion >= 4) {
    for (byte x = 0; x < 7; x++) {
      for (byte y = 0; y < 5; y++) {
        // Only fill areas not already covered by inner rings
        int dx = x - leftCenterX;
        int dy = y - centerY;
        if (abs(dx) > 1 || abs(dy) > 1) {
          leds[XY(x, y)] = fadedColor;
        }
      }
    }
    for (byte x = 9; x < 16; x++) {
      for (byte y = 0; y < 5; y++) {
        // Only fill areas not already covered by inner rings
        int dx = x - rightCenterX;
        int dy = y - centerY;
        if (abs(dx) > 1 || abs(dy) > 1) {
          leds[XY(x, y)] = fadedColor;
        }
      }
    }
  }
}

//hearts that start small on the bottom and get larger as they grow upward
const uint8_t SmHeart[] = {46, 48, 53, 55, 60, 65};
const uint8_t MedHeart[] = {31, 32, 34, 35, 38, 39,
                            41, 42, 46, 47, 48, 55, 54, 53, 54, 55, 60, 65
                           };
const uint8_t LrgHeart[] = {15, 16, 18, 19, 24, 25,
                            27, 28, 31, 32, 33, 34, 35, 38, 39, 40, 41, 42,
                            46, 47, 48, 53, 54, 55, 60, 65
                           };
const uint8_t HugeHeart[] = {0, 1, 3, 4, 9, 10, 12,
                             13, 14, 15, 16, 17, 18, 19, 20, 23, 24, 25, 26,
                             27, 28, 29, 31, 32, 33, 34, 35, 38, 39, 40, 41,
                             42, 46, 47, 48, 53, 54, 55, 60, 65
                            };
void hearts() {
  static boolean erase = false;
  static uint8_t x, y = 0;
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 150;
    FastLED.clear();
    fadeActive = 0;

    x = 0;
    y = 0;
  }
  if (y == 5)
    y = 0;
  if (y == 0)
    for (x = 0; x < 6; x++)
      leds[SmHeart[x]] = CRGB::Salmon; //Tried to transition from pink-ish to red. Kinda worked.
  if (y == 1)
    for (x = 0; x < 18; x++)
      leds[MedHeart[x]] = CRGB::Tomato;
  if (y == 2)
    for (x = 0; x < 26; x++)
      leds[LrgHeart[x]] = CRGB::Crimson;
  if (y == 3) {
    for (x = 0; x < 40; x++)
      leds[HugeHeart[x]] = CRGB::Red;
  } //set the delay slightly longer for HUGE heart.
  if (y == 4)
    FastLED.clear();
  y++;
}



// Ring pulser


void drawRing(int xCenter, int yCenter, float radius, CRGB color) {
  int brightness;
  CRGB tempColor;

  if (radius > 13) radius = 13;
  
  for (int x = 0; x < kMatrixWidth; x++) {
    for (int y = 0; y < kMatrixHeight; y++) {
      brightness = 255 - abs((hypot((float)x - 7.5 - xCenter/256.0, (float)y - 2.0 - yCenter/256.0) - radius)*192.0);
      if (brightness > 255) brightness = 255;
      if (brightness < 0) brightness = 0;
      tempColor = color;
      leds[XY(x, y)] += tempColor.nscale8(brightness);
    }
  }
}

void rings() {

  static float offset  = 0; // counter for radial color wave motion
  static uint16_t plasVector = 0; // counter for orbiting plasma center


  #define RING_GAINSMOOTH 0.01
  static float ringavg1 = 500, ringavg2 = 500, ringavg3 = 500;
  float ringval1, ringval2, ringval3;

  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
    fadeActive = 0;
    audioActive = true;
  }


  // Calculate current center of plasma pattern (can be offscreen)
  int xOffset;
  int yOffset;

  fillAll(0);



  ringval1 = spectrumDecay[0] + spectrumDecay[1] + spectrumDecay[2];
  ringval2 = spectrumDecay[3] + spectrumDecay[4];
  ringval3 = spectrumDecay[5] + spectrumDecay[6];

  ringavg1 = ringavg1 * (1.0 - RING_GAINSMOOTH) + ringval1 * RING_GAINSMOOTH;
  ringavg2 = ringavg2 * (1.0 - RING_GAINSMOOTH) + ringval2 * RING_GAINSMOOTH;
  ringavg3 = ringavg3 * (1.0 - RING_GAINSMOOTH) + ringval3 * RING_GAINSMOOTH;

  float ring_gain1 = (6.0/ringavg1);
  float ring_gain2 = (6.0/ringavg2);
  float ring_gain3 = (5.0/ringavg3);

  if (ring_gain1 > 0.05) ring_gain1 = 0.05;
  if (ring_gain2 > 0.05) ring_gain2 = 0.05;
  if (ring_gain3 > 0.05) ring_gain3 = 0.05;

  xOffset = cos16(plasVector);
  yOffset = sin16(plasVector);
  drawRing(xOffset/24,yOffset/24, ringval1 * ring_gain1, CRGB::Red);

  xOffset = cos16(plasVector+65535*0.33);
  yOffset = sin16(plasVector+65535*0.33);
    drawRing(xOffset/24,yOffset/24, ringval2 * ring_gain2, CRGB::Green);


  xOffset = cos16(plasVector+65535*0.66);
  yOffset = sin16(plasVector+65535*0.66);
  drawRing(xOffset/24,yOffset/24, ringval3 * ring_gain2, CRGB::Blue);


  offset += 0.2;
  plasVector += 256; // using an int for slower orbit (wraps at 65536)

}

// Noise flyer

void noiseFlyer() {
  // startup tasks
  if (effectInit == false) {
    effectInit = true;
    effectDelay = 10;
    selectRandomNoisePalette();

    audioActive = true;
  }

  static byte hueOffset = 0;
  static byte heading = 0;

  fillnoise8();
  float kph = (spectrumDecay[0] + spectrumDecay[1] + spectrumDecay[2]) / 3.0;
  
  int brightness;

  for (byte x = 0; x < kMatrixWidth; x++) {
    for (byte y = 0; y < kMatrixHeight; y++) {
      brightness = kph/4 - 72;
      brightness += noise[x][y];
      if (brightness > 240) brightness = 240;
      if (brightness < 0) brightness = 0;
      CRGB pixelColor = ColorFromPalette(currentPalette, brightness, 255);
      leds[XY(x,y)] = pixelColor;
    }
  }
  
  //hueOffset++;

  heading += random8(5) - 2;


  
  nx += sin8(heading) * kph / 6000 + 5;
  ny += cos8(heading) * kph / 6000 + 5;

  
}
