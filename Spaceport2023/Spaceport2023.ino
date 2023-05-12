#include <TimeLib.h>
#include <SD.h>
#include <SPI.h>

const int chipSelect = BUILTIN_SDCARD;

const int A1xInput = A16;
const int A1yInput = A1;
const int A1zInput = A0;

const int A2xInput = A2;
const int A2yInput = A3;
const int A2zInput = A4;

const int A3xInput = A12;
const int A3yInput = A11;
const int A3zInput = A10;

int A1xRawMin = 380;
int A1xRawMax = 410;

int A1yRawMin = 382;
int A1yRawMax = 412;

int A1zRawMin = 385;
int A1zRawMax = 414;

int A2xRawMin = 380;
int A2xRawMax = 410;

int A2yRawMin = 381;
int A2yRawMax = 411;

int A2zRawMin = 382;
int A2zRawMax = 412;

int A3xRawMin = 381;
int A3xRawMax = 410;

int A3yRawMin = 586;
int A3yRawMax = 616;

int A3zRawMin = 589;
int A3zRawMax = 620;

const int sampleSize = 10;

File logger;
char filename[15];
void setup()  {
  // set the Time library to use Teensy 3.0's RTC to keep time
  setSyncProvider(getTeensy3Time);
  analogReadAveraging(sampleSize);
  Serial.begin(115200); 
  if (timeStatus()!= timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }
  //Check if the SD card can begin on the specified pin, if not then print an error message
  if(!SD.begin(chipSelect)) {
    Serial.println("Card initalization failed!");
  }
  //Because strings in C are literals, we have to use strcpy to set our file name up for use in the file system.
  strcpy(filename, "/DATA00.TXT");
  //This is a handy filesystem created by the folks at Adafruit that allows for a new file to be opened and written to each time the device is reset
  for (uint8_t i = 0; i < 100; i++) {
    filename[5] = '0' + i/10;
    filename[6] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (!SD.exists(filename)) {
      break;
    }
  }
  //Initalize our logging variable by opening it the first time and testing a write
  logger = SD.open(filename, FILE_WRITE);
  //If it wasn't opened, then alert the user
  if(!logger) {
    Serial.println("Couldn't open file!");
  }
  //Close the file after the test
  logger.close();
  //Allows for LED light on Teensy to be turned on
  pinMode(LED_BUILTIN, OUTPUT);
  time_t t = processSyncMessage();
  if (t != 0) {
    Teensy3Clock.set(t); // set the RTC
    setTime(t);
  }
}

void loop() {
  /*long A1xRaw = analogRead(A1xInput);
  long A1yRaw = analogRead(A1yInput);
  long A1zRaw = analogRead(A1zInput);

  long A1xScaled = map(A1xRaw,A1xRawMin,A1xRawMax, -1000,1000);
  long A1yScaled = map(A1yRaw,A1yRawMin,A1yRawMax, -1000,1000);
  long A1zScaled = map(A1zRaw,A1zRawMin,A1zRawMax, -1000,1000);
  float A1xAccel = A1xScaled / 1000.0;
  float A1yAccel = A1yScaled / 1000.0;
  float A1zAccel = A1zScaled / 1000.0;
  Serial.print(" X1:");
  Serial.print(A1xAccel);
  Serial.print("G Y1:");
  Serial.print(A1yAccel);
  Serial.print("G Z1:");
  Serial.print(A1zAccel);
  Serial.print("G ");
  long A2xRaw = analogRead(A2xInput);
  long A2yRaw = analogRead(A2yInput);
  long A2zRaw = analogRead(A2zInput);

  long A2xScaled = map(A2xRaw,A2xRawMin,A2xRawMax, -1000,1000);
  long A2yScaled = map(A2yRaw,A2yRawMin,A2yRawMax, -1000,1000);
  long A2zScaled = map(A2zRaw,A2zRawMin,A2zRawMax, -1000,1000);
  float A2xAccel = A2xScaled / 1000.0;
  float A2yAccel = A2yScaled / 1000.0;
  float A2zAccel = A2zScaled / 1000.0;
  Serial.print(" X2:");
  Serial.print(A2xAccel);
  Serial.print("G Y2:");
  Serial.print(A2yAccel);
  Serial.print("G Z2:");
  Serial.print(A2zAccel);
  Serial.print("G ");

  long A3xRaw = analogRead(A3xInput);
  long A3yRaw = analogRead(A3yInput);
  long A3zRaw = analogRead(A3zInput);

  long A3xScaled = map(A3xRaw,A3xRawMin,A3xRawMax, -1000,1000);
  long A3yScaled = map(A3yRaw,A3yRawMin,A3yRawMax, -1000,1000);
  long A3zScaled = map(A3zRaw,A3zRawMin,A3zRawMax, -1000,1000);
  float A3xAccel = A3xScaled / 1000.0;
  float A3yAccel = A3yScaled / 1000.0;
  float A3zAccel = A3zScaled / 1000.0;
  Serial.print(" X3:");
  Serial.print(A3xAccel);
  Serial.print("G Y3:");
  Serial.print(A3yAccel);
  Serial.print("G Z3:");
  Serial.print(A3zAccel);
  Serial.print("G ");
  */
  //printAccOutput(A1xInput,A1yInput,A1zInput, A1xRawMin,A1xRawMax, A1yRawMin, A1yRawMax, A1zRawMin, A1zRawMax);
  //printAccOutput(A2xInput,A2yInput,A2zInput, A2xRawMin,A2xRawMax, A2yRawMin, A2yRawMax, A2zRawMin, A2zRawMax);
  //printAccOutput(A3xInput,A3yInput,A3zInput, A3xRawMin,A3xRawMax, A3yRawMin, A3yRawMax, A3zRawMin, A3zRawMax);
  //digitalClockDisplay();
  logger = SD.open(filename, FILE_WRITE);  
  printSDAccOutput(logger,A1xInput,A1yInput,A1zInput, A1xRawMin,A1xRawMax, A1yRawMin, A1yRawMax, A1zRawMin, A1zRawMax);
  printSDAccOutput(logger,A2xInput,A2yInput,A2zInput, A2xRawMin,A2xRawMax, A2yRawMin, A2yRawMax, A2zRawMin, A2zRawMax);
  printSDAccOutput(logger,A3xInput,A3yInput,A3zInput, A3xRawMin,A3xRawMax, A3yRawMin, A3yRawMax, A3zRawMin, A3zRawMax);
  SDClockDisplay();
  logger.close();
  digitalWrite(LED_BUILTIN,HIGH);
}
void SDClockDisplay() {
  // digital clock display of the time
  logger.print(hour());
  printSDDigits(minute());
  printSDDigits(second());
  logger.print(",");
  logger.print(day());
  logger.print(",");
  logger.print(month());
  logger.print(",");
  logger.print(year()); 
  logger.println(); 
}
void digitalClockDisplay() {
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}

time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}

/*  code to process time sync messages from the serial port   */
#define TIME_HEADER  "T"   // Header tag for serial time sync message

unsigned long processSyncMessage() {
  unsigned long pctime = 0L;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013 

  if(Serial.find(TIME_HEADER)) {
     pctime = Serial.parseInt();
     return pctime;
     if( pctime < DEFAULT_TIME) { // check the value is a valid time (greater than Jan 1 2013)
       pctime = 0L; // return 0 to indicate that the time is not valid
     }
  }
  return pctime;
}
void printSDDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  logger.print(":");
  if(digits < 10)
    logger.print('0');
  logger.print(digits);
}
void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
void printAccOutput(int xInput, int yInput, int zInput, int xRawMin, int xRawMax, int yRawMin, int yRawMax, int zRawMin, int zRawMax) {
  long xRaw = analogRead(xInput);
  long yRaw = analogRead(yInput);
  long zRaw = analogRead(zInput);

  long xScaled = map(xRaw,xRawMin,xRawMax, -1000,1000);
  long yScaled = map(yRaw,yRawMin,yRawMax, -1000,1000);
  long zScaled = map(zRaw,zRawMin,zRawMax, -1000,1000);
  float xAccel = xScaled / 1000.0;
  float yAccel = yScaled / 1000.0;
  float zAccel = zScaled / 1000.0;
  Serial.print(xAccel);
  Serial.print("G ");
  Serial.print(yAccel);
  Serial.print("G ");
  Serial.print(zAccel);
  Serial.print("G ");
}
void printSDAccOutput(File logger, int xInput, int yInput, int zInput, int xRawMin, int xRawMax, int yRawMin, int yRawMax, int zRawMin, int zRawMax) {
  long xRaw = analogRead(xInput);
  long yRaw = analogRead(yInput);
  long zRaw = analogRead(zInput);

  long xScaled = map(xRaw,xRawMin,xRawMax, -1000,1000);
  long yScaled = map(yRaw,yRawMin,yRawMax, -1000,1000);
  long zScaled = map(zRaw,zRawMin,zRawMax, -1000,1000);
  float xAccel = xScaled / 1000.0;
  float yAccel = yScaled / 1000.0;
  float zAccel = zScaled / 1000.0;
  logger.print(xAccel);
  logger.print(",");
  logger.print(yAccel);
  logger.print(",");
  logger.print(zAccel);
  logger.print(",");
}