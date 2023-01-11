#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        6
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 45 // Popular NeoPixel ring size
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 1 // Time (in milliseconds) to pause between pixels
Adafruit_MPU6050 mpu;
unsigned long startTimer = 0;
int   delayShow = 1000;
unsigned long timeNow = 0;
float data[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int counts = 0;
bool doIdle =false;
//int wait = 50;
float
xAxis = 0.0,
upperXaxisF = 0.3,
lowerXaxisF = 0.0,
sum = 0.0,
average = 0.0,
upperXaxisB = 0.0,
lowerXaxisB = -0.3;
void setup(void) {
  Serial.begin(115200);
  //while (!Serial)
   // delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 and led in use ");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.show();            // Turn OFF all pixels ASAP
  pixels.setBrightness(24); // Set BRIGHTNESS to about 1/5 (max = 255)
  backward();
  //delay(1000);
  forward();
  //delay(1000);

  pixels.clear();
  //setupt motion detection
  mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
  mpu.setMotionDetectionThreshold(1);
  mpu.setMotionDetectionDuration(20);
  mpu.setInterruptPinLatch(true);  // Keep it latched.  Will turn off when reinitialized.
  mpu.setInterruptPinPolarity(true);
  mpu.setMotionInterrupt(true);
  attachInterrupt(digitalPinToInterrupt(2), motiondetected, RISING);
  Serial.println("started");
  delay(100);
  timeNow = millis();
}
void motiondetected()
{
  //output = LOW;
  Serial.print("Interrupt 1");
}
void loop() {
  startTimer = millis();
  if (doIdle) {
    idle();
  }
  bool reads=(checks());
}
bool checks() {
  if (/*mpu.getMotionInterruptStatus()*/ 1 && startTimer - timeNow > 3 ) {
    
    /* Get new sensor events with the readings */
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    Serial.print("GyroX:");
    Serial.println(g.gyro.x);

    xAxis = g.gyro.x;
    if (xAxis > upperXaxisF && xAxis > lowerXaxisF) {
      doIdle = false;
      forward();
      return true;
    }
    if (xAxis < upperXaxisB && xAxis < lowerXaxisB) {
      doIdle = false;
      backward();
      return true;
    }
    if (xAxis > -0.2 && xAxis < 0.2) {// from  -0.3 to 0.3
      //idle();
      doIdle = true;
      return false;
    }
  } delay(10);
 // doIdle = true;
}

void forward() {
  // pixels.clear(); // Set all pixel colors to 'off'
  for (int i = 0; i < 21; i++) { // For each pixel...
    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    pixels.setPixelColor(i, pixels.Color(255, 255, 255));
    pixels.show();   // Send the updated pixel colors to the hardware.
    delay(DELAYVAL); // Pause before next pass through loop
  }
  for (int i = 21; i < NUMPIXELS; i++) { // For each pixel...
    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    pixels.setPixelColor(i, pixels.Color(255, 0, 0));
    pixels.show();   // Send the updated pixel colors to the hardware.
    delay(DELAYVAL); // Pause before next pass through loop
  }
  delay(delayShow);
}


void backward() {
  // pixels.clear(); // Set all pixel colors to 'off'
  for (int i = 0; i < 21; i++) { // For each pixel...
    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    pixels.setPixelColor(i, pixels.Color(255, 0, 0));
    pixels.show();   // Send the updated pixel colors to the hardware.
    delay(DELAYVAL); // Pause before next pass through loop
  }
  for (int i = 21; i < NUMPIXELS; i++) { // For each pixel...
    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    pixels.setPixelColor(i, pixels.Color(245, 255, 245));
    pixels.show();   // Send the updated pixel colors to the hardware.
    delay(DELAYVAL); // Pause before next pass through loop
  }
  delay(delayShow);
}

void idle() {

  // Fill along the length of the strip in various colors...
  colorWipe(pixels.Color(255,   0,   0), 50); // Red
  colorWipe(pixels.Color(  0, 255,   0), 50); // Green
  colorWipe(pixels.Color(  0,   0, 255), 50); // Blue

  // Do a theater marquee effect in various colors...
  theaterChase(pixels.Color(127, 127, 127), 50); // White, half brightness
  theaterChase(pixels.Color(127,   0,   0), 50); // Red, half brightness
  theaterChase(pixels.Color(  0,   0, 127), 50); // Blue, half brightness

  rainbow(10);             // Flowing rainbow cycle along the whole strip
  theaterChaseRainbow(10); // Rainbow-enhanced theaterChase variant
}

void colorWipe(uint32_t color, int wait) {
  for (int i = 0; i < pixels.numPixels(); i++) { // For each pixel in pixels...
    pixels.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    pixels.show();                          //  Update pixels to match
    if (checks())break;
    delay(wait);                           //  Pause for a moment
  }
}
// Rainbow cycle along whole pixels. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 3 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 3*65536. Adding 256 to firstPixelHue each time
  // means we'll make 3*65536/256 = 768 passes through this outer loop:
  for (long firstPixelHue = 0; firstPixelHue < 3 * 65536; firstPixelHue += 256) {
    for (int i = 0; i < pixels.numPixels(); i++) { // For each pixel in pixels...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the pixels
      // (pixels.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / pixels.numPixels());
      // pixels.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through pixels.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      if (checks())break;
      pixels.setPixelColor(i, pixels.gamma32(pixels.ColorHSV(pixelHue)));
    }
    pixels.show(); // Update pixels with new contents
    delay(wait);  // Pause for a moment
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for (int a = 0; a < 30; a++) { // Repeat 30 times...
    for (int b = 0; b < 3; b++) { //  'b' counts from 0 to 2...
      pixels.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of pixels in increments of 3...
      for (int c = b; c < pixels.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the pixels (pixels.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / pixels.numPixels();
        uint32_t color = pixels.gamma32(pixels.ColorHSV(hue)); // hue -> RGB
        pixels.setPixelColor(c, color); // Set pixel 'c' to value 'color'
        if (checks())break;
      }
      pixels.show();                // Update pixels with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la pixels.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait) {
  for (int a = 0; a < 10; a++) { // Repeat 10 times...
    for (int b = 0; b < 3; b++) { //  'b' counts from 0 to 2...
      pixels.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of pixels in steps of 3...
      for (int c = b; c < pixels.numPixels(); c += 3) {
        pixels.setPixelColor(c, color); // Set pixel 'c' to value 'color'
        if (checks())break;
      }
      pixels.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}
