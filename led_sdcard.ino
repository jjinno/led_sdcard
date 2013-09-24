#include <SdFat.h>
#include <SdFatUtil.h>
#include <ctype.h>

// Include the LED library...
#include <Adafruit_NeoPixel.h>
#include <SPI.h>

#define DATA_PIN 6
#define SD_PIN   10

// Create the variables to be used by SdFat Library.
SdFat sd;
ifstream* is;

Adafruit_NeoPixel* strip;

bool initError = false;

char name[] = "test.led"; // Create an array that contains the name of our file.
uint8_t r = 0; // The 0-255 value of red.
uint8_t g = 0; // The 0-255 value of green.
uint8_t b = 0; // The 0-255 value of blue.

uint8_t tmp1;
uint8_t tmp2;

uint8_t orientation = 0;
uint8_t wait = 0;
uint16_t rows = 0; // The number of LED Rows.
uint16_t cols = 0; // The number of LED Columns.

int index = 0; // Index will keep track of our position within the contents buffer.

void setup() {
  pinMode(DATA_PIN, OUTPUT); // The data pin for the LEDs must be an output.
  pinMode(SD_PIN, OUTPUT); // Pin 10 must be set as an output for the SD communication to work.

  Serial.begin(57600); // Start a serial connection.

  // Initialize the SD card and configure the I/O pins.
  if (!sd.begin(SD_PIN, SPI_FULL_SPEED)) {
    initError = true;
    return;
  }

  sd.chdir("/"); // Open the root directory in the volume.

  initializeFile();
}

void initializeFile() {
  // Open for read...
  is = new ifstream(name, ios::binary);

  // Check for open error...
  if (!is->is_open()) {
    initError = true;
    return;
  }

  index = 0;
  readHeader();
  strip = new Adafruit_NeoPixel((rows * cols), DATA_PIN, NEO_GRB + NEO_KHZ800);
}

void readHeader() {
  (*is) >> orientation >> wait;

  (*is) >> tmp1 >> tmp2;
  rows = (tmp2 << 8) + tmp1;

  (*is) >> tmp1 >> tmp2;
  cols = (tmp2 << 8) + tmp1;
}

void loop() {
  if (initError) {
    Serial.println("Error.");
    while (1);
  }

  // NOTE: Until I actually add the code to support LRUD/LRDU/RLUD/RLDU, etc,
  //       either 'rows' or 'cols' should be set to '1', to indicate a single
  //       strand of LEDs.

  // Keep reading characters from the file until we get an error or reach the end
  // of the file. (This will output the entire contents of the file).
  (*is) >> r >> g >> b;

  strip->setPixelColor(index, r, g, b);

  index++;

  if (index >= (rows * cols)) {
    strip->show();
    index = 0;
    r = g = b = 0;
    delay(10 * wait);
  }

  if (is->eof()) {
    is->close();
    delete is;
    delete strip;
    initializeFile();
  }
}
