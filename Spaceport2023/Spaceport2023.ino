#include <TimeLib.h>
#include <SD.h>
#include <SPI.h>
#include <InternalTemperature.h>

const int chipSelect = BUILTIN_SDCARD;

const int A1xInput = A16;
const int A1yInput = A1;
const int A1zInput = A0;

const int A2xInput = A2;
const int A2yInput = A3;
const int A2zInput = A4;

const int A3xInput = A6;
const int A3yInput = A7;
const int A3zInput = A8;

const int A1xRawMin = 380;
const int A1xRawMax = 410;

const int A1yRawMin = 382;
const int A1yRawMax = 412;

const int A1zRawMin = 385;
const int A1zRawMax = 414;

const int A2xRawMin = 380;
const int A2xRawMax = 410;

const int A2yRawMin = 381;
const int A2yRawMax = 411;

const int A2zRawMin = 382;
const int A2zRawMax = 412;

const int A3xRawMin = 382;
const int A3xRawMax = 412;

const int A3yRawMin = 382;
const int A3yRawMax = 413;

const int A3zRawMin = 385;
const int A3zRawMax = 415;

const int sampleSize = 10;

bool liftoffDetected = false;

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
  //check to see if we have some type of liftoff detected, if so start outputting to sd card
  //if(liftoffDetected == true) {
  //open the SD card for writing
  logger = SD.open(filename, FILE_WRITE);  
  //print the output of the 3 accelerometer
  printSDAccOutput(logger,A1xInput,A1yInput,A1zInput, A1xRawMin,A1xRawMax, A1yRawMin, A1yRawMax, A1zRawMin, A1zRawMax);
  printSDAccOutput(logger,A2xInput,A2yInput,A2zInput, A2xRawMin,A2xRawMax, A2yRawMin, A2yRawMax, A2zRawMin, A2zRawMax);
  printSDAccOutput(logger,A3xInput,A3yInput,A3zInput, A3xRawMin,A3xRawMax, A3yRawMin, A3yRawMax, A3zRawMin, A3zRawMax);
  float temp = InternalTemperature.readTemperatureC();
  logger.print(temp);
  logger.print(",");
  //printAccOutput(A1xInput,A1yInput,A1zInput, A1xRawMin,A1xRawMax, A1yRawMin, A1yRawMax, A1zRawMin, A1zRawMax);
  //printAccOutput(A2xInput,A2yInput,A2zInput, A2xRawMin,A2xRawMax, A2yRawMin, A2yRawMax, A2zRawMin, A2zRawMax);
  //printAccOutput(A3xInput,A3yInput,A3zInput, A3xRawMin,A3xRawMax, A3yRawMin, A3yRawMax, A3zRawMin, A3zRawMax);
  //Serial.println();
  //print the time to the sdcard
  SDClockDisplay();
  logger.close();
  //turn on the led to indicate start of writing 
  digitalWrite(LED_BUILTIN,HIGH);  
  //}
  //else {
  //  checkForLiftoff(A3xInput,A3yInput,A3zInput, A3xRawMin,A3xRawMax, A3yRawMin, A3yRawMax, A3zRawMin, A3zRawMax);
  //}

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
  Serial.print(",");
  Serial.print(yAccel);
  Serial.print(",");
  Serial.print(zAccel);
  Serial.print(",");
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
void checkForLiftoff(int xInput, int yInput, int zInput, int xRawMin, int xRawMax, int yRawMin, int yRawMax, int zRawMin, int zRawMax) {
  long xRaw = analogRead(xInput);
  long yRaw = analogRead(yInput);
  long zRaw = analogRead(zInput);

  long xScaled = map(xRaw,xRawMin,xRawMax, -1000,1000);
  long yScaled = map(yRaw,yRawMin,yRawMax, -1000,1000);
  long zScaled = map(zRaw,zRawMin,zRawMax, -1000,1000);
  float xAccel = xScaled / 1000.0;
  float yAccel = yScaled / 1000.0;
  float zAccel = zScaled / 1000.0;
  float posX = xAccel * xAccel;
  float posY = yAccel * yAccel;
  float posZ = zAccel * zAccel;
  float magAccel = sqrt(posX + posY + posZ);
  if(magAccel >= 3) {
    liftoffDetected = true;
  }
}