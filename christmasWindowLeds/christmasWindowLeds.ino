#include "FastLED.h"

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few
// of the kinds of animation patterns you can quickly and easily
// compose using FastLED.
//
// This example also shows one easy way to define multiple
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    6
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB
#define NUM_LEDS    100
CRGB leds[NUM_LEDS];
CRGB ledst[NUM_LEDS];

int BRIGHTNESS=  255;
#define FRAMES_PER_SECOND  120

CRGBPalette16 stripedPallete = CRGBPalette16( CRGB::Red,  CRGB::Red,  CRGB::Red,  CRGB::Red,  CRGB::Red,
                               CRGB::Red,  CRGB::Red,  CRGB::Red, CRGB::Gray, CRGB::Gray, CRGB::Gray, CRGB::Gray, CRGB::Gray, CRGB::Gray, CRGB::Gray, CRGB::Gray);


void setup() {
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {colors, candyCane, rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm, circle,  twinkle, rainbow2, expand, runl, newKITT};

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0, gPos = 0; // rotating "base color" used by many of the patterns

void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip

  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  if (digitalRead(3))
    BRIGHTNESS = 255;
  else
    BRIGHTNESS = 96;

  if (digitalRead(2))
    BRIGHTNESS = 0;

  FastLED.setBrightness(BRIGHTNESS);

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) {
    gHue++;  // slowly cycle the "base color" through the rainbow
  }
  EVERY_N_MILLISECONDS( 5 ) {
    gPos++;  // slowly cycle the "base color" through the rainbow
  }
  EVERY_N_SECONDS( 30 ) {
    nextPattern();  // change patterns periodically
  }
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 4);
  copyHalf(); flipHalf();
}

void rainbow2()
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 5);
}

void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
  copyHalf(); flipHalf();
}

void addGlitter( fract8 chanceOfGlitter)
{
  if ( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS / 2 - 1 );
  leds[pos] += CHSV( gHue, 255, 192);
  copyHalf(); flipHalf();
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for ( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
  copyHalf(); flipHalf();
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for ( int i = 0; i < 5; i++) {
    leds[beatsin16( i + 7, 0, NUM_LEDS - 1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void twinkle() {
  fadeToBlackBy( leds, NUM_LEDS, 6);
  if ( random8() < 50) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void candyCane() {
  fill_palette( leds, NUM_LEDS,
                gPos, 8, /* higher = narrower stripes */
                stripedPallete, 255, LINEARBLEND);
  copyHalf(); flipHalf();
}

void expand() {
  if ((gHue % (NUM_LEDS / 2)) >= (NUM_LEDS / 4)) {
    for (int i = 0; i - 1 <= (gHue % (NUM_LEDS / 4)); i++) {
      leds[i] = CRGB::White;
      leds[NUM_LEDS / 2 - i] = CRGB::White;
    }
  }
  else {
    for (int i = 0; i - 1 <= (gHue % (NUM_LEDS / 4)); i++) {
      leds[i] = CRGB::Black;
      leds[NUM_LEDS / 2 - i] = CRGB::Black;
    }
  }
  copyHalf(); flipHalf();
}

void runl() {
  for (int i = 0; i < NUM_LEDS; i++) {
    int v = constrain((sin(i + gHue) * 255), 0, 255);
    leds[i] = CRGB(v, v, v);
  }
}

void circle() {
  int lastpos;
  fadeToBlackBy( leds, NUM_LEDS, 2);

  lastpos = map(gPos, 0, 255, 0, NUM_LEDS);
  lastpos = lastpos % NUM_LEDS;
  leds[lastpos] += CHSV( gHue, 255, 192);
}

void colors() {
  if ((millis() / 500) % 3 == 0)
    fill_solid(leds, NUM_LEDS, CRGB::Red);
  else if ((millis() / 500) % 3 == 1)
    fill_solid(leds, NUM_LEDS, CRGB::Green);
  else
    fill_solid(leds, NUM_LEDS, CRGB::Blue);
}

void flipHalf() {
  memcpy(ledst, leds, sizeof(leds));
  for (int i = 0; i < NUM_LEDS / 2; i++) {
    leds[NUM_LEDS / 2 + i] = ledst[NUM_LEDS - 1 - i];
  }
}


void copyHalf() {
  for (int i = 0; i < NUM_LEDS / 2; i++) {
    leds[NUM_LEDS / 2 + i] = leds[i];
  }
}






void newKITT() {
  byte red = 255, green = 0, blue = 0; int EyeSize = 7, SpeedDelay = 8, ReturnDelay = 50;
  RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
}

void CenterToOutside(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for (int i = (((NUM_LEDS / 2) - EyeSize) / 2); i >= 0; i--) {
    fadeToBlackBy( leds, (NUM_LEDS / 2), 60);

    leds[i] = CRGB(red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      leds[i + j] = CRGB(  red, green, blue);
    }
    leds[i + EyeSize + 1] = CRGB( red / 10, green / 10, blue / 10);

    leds[(NUM_LEDS / 2) - i] = CRGB(  red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      leds[(NUM_LEDS / 2) - i - j] = CRGB(  red, green, blue);
    }
    leds[(NUM_LEDS / 2) - i - EyeSize - 1] = CRGB(  red / 10, green / 10, blue / 10);
    copyHalf(); flipHalf();
    FastLED.show();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

void OutsideToCenter(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for (int i = 0; i <= (((NUM_LEDS / 2) - EyeSize) / 2); i++) {
    fadeToBlackBy( leds, (NUM_LEDS / 2), 60);

    leds[i] = CRGB(  red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      leds[i + j] = CRGB(  red, green, blue);
    }
    leds[i + EyeSize + 1] = CRGB(  red / 10, green / 10, blue / 10);

    leds[(NUM_LEDS / 2) - i] = CRGB(  red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      leds[(NUM_LEDS / 2) - i - j] = CRGB(  red, green, blue);
    }
    leds[(NUM_LEDS / 2) - i - EyeSize - 1] = CRGB(  red / 10, green / 10, blue / 10);
    copyHalf(); flipHalf();
    FastLED.show();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

void LeftToRight(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for (int i = 0; i < (NUM_LEDS / 2) - EyeSize - 2; i++) {
    fadeToBlackBy( leds, (NUM_LEDS / 2), 60);
    leds[i] = CRGB(  red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      leds[i + j] = CRGB(  red, green, blue);
    }
    leds[i + EyeSize + 1] = CRGB(  red / 10, green / 10, blue / 10);
    copyHalf(); flipHalf();
    FastLED.show();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

void RightToLeft(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for (int i = (NUM_LEDS / 2) - EyeSize - 2; i > 0; i--) {
    fadeToBlackBy( leds, (NUM_LEDS / 2), 60);
    leds[i] = CRGB(  red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      leds[i + j] = CRGB(  red, green, blue);
    }
    leds[i + EyeSize + 1] = CRGB(  red / 10, green / 10, blue / 10);
    copyHalf(); flipHalf();
    FastLED.show();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}
