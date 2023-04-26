#include <TimeLib.h>
#include <SD.h>
#include <SPI.h>

const int chipSelect = BUILTIN_SDCARD;

const int A1xInput = A2;
const int A1yInput = A3;
const int A1zInput = A4;

int A1xRawMin = 381;
int A1xRawMax = 410;

int A1yRawMin = 382;
int A1yRawMax = 412;

int A1zRawMin = 385;
int A1zRawMax = 414;

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
}

void loop() {
  time_t t = processSyncMessage();
  if (t != 0) {
    Teensy3Clock.set(t); // set the RTC
    setTime(t);
  }
  long A1xRaw = analogRead(A1xInput);
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
  Serial.print("G");
  digitalClockDisplay();
  logger = SD.open(filename, FILE_WRITE);  
  SDClockDisplay();
  logger.close();
  //delay(1000);
}
void SDClockDisplay() {
  // digital clock display of the time
  logger.print(hour());
  printSDDigits(minute());
  printSDDigits(second());
  logger.print(" ");
  logger.print(day());
  logger.print(" ");
  logger.print(month());
  logger.print(" ");
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
//This function will take in a given pin and return the raw reading of the axis given as an integer
int getAxis(int pin) {
  //Create a result to store each sample added up
  long result = 0;
  //Go through and read off the pin the set number of times (10) and add them up
  for(int i = 0; i < sampleSize; i++) {
    result += analogRead(pin);
  }
  //Finally return the average (divided by the sample size)
  return result / sampleSize;
}