// demo of Starter kit V2.0 - Grove - Light Sensor
// when the value of light sensor ledd than a certain value
// led will on, you can set this certain value via change thresholdvalue
// such as when you cover light sensor with your hand, you'll find led on

#include <NewPing.h>
#include <Wire.h>
#include "rgb_lcd.h"
#include <WaveUtil.h>
#include <WaveHC.h>

byte norm_eye_0[8] = {
	0b00000,
	0b00000,
	0b00001,
	0b00110,
	0b01100,
	0b01000,
	0b01000,
	0b10000
};
byte norm_eye_1[8] = {
	0b00000,
	0b00000,
	0b11100,
	0b00011,
	0b00000,
	0b00000,
	0b00000,
	0b00000
};
byte norm_eye_2[8] = {
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b10000,
	0b10000,
	0b10000,
	0b01000
};
byte norm_eye_3[8] = {
	0b10000,
	0b10000,
	0b01000,
	0b01000,
	0b01100,
	0b00110,
	0b00001,
	0b00000
};
byte norm_eye_4[8] = {
	0b11100,
	0b11100,
	0b11100,
	0b00000,
	0b00000,
	0b00011,
	0b11100,
	0b00000
};
byte norm_eye_5[8] = {
	0b01000,
	0b01000,
	0b10000,
	0b10000,
	0b10000,
	0b00000,
	0b00000,
	0b00000
};

 
SdReader card;    // This object holds the information for the card
FatVolume vol;    // This holds the information for the partition on the card
FatReader root;   // This holds the information for the volumes root directory
FatReader f;      // This holds the information for the file we're playing
WaveHC wave;      // This is the only wave (audio) object, since we will only play one at a time

uint8_t dirLevel; // indent level for file/dir names    (for prettyprinting)
dir_t dirBuf;     // buffer for directory reads

#define error(msg) error_P(PSTR(msg))
void play(FatReader &dir);

#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.


rgb_lcd lcd;
int fear;
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.


const int pinLight = A0;
const int pinTemp = A1;
const int pinSound = A2;
int lastTemp = 0;
int lastBright = 0;
int lastLoud = 0;

void setup() {
  lastTemp = 0;
  lastBright = 0;
  // set up serial port
  Serial.begin(9600);
  putstring_nl("WaveHC with 6 buttons");
  
   putstring("Free RAM: ");       // if this is under 150 bytes it may spell trouble!
  
  // Set the output pins for the DAC control. This pins are defined in the library
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
 
  // pin13 LED
  pinMode(13, OUTPUT);
 
  // enable pull-up resistors on switch pins (analog inputs)
  digitalWrite(14, HIGH);
  digitalWrite(15, HIGH);
  digitalWrite(16, HIGH);
  digitalWrite(17, HIGH);
  digitalWrite(18, HIGH);
  digitalWrite(19, HIGH);
 
  //  if (!card.init(true)) { //play with 4 MHz spi if 8MHz isn't working for you
  if (!card.init()) {         //play with 8 MHz spi (default faster!)  
    putstring_nl("Card init. failed!");  // Something went wrong, lets print out why
    sdErrorCheck();
    while(1);                            // then 'halt' - do nothing!
  }
  
  // enable optimize read - some cards may timeout. Disable if you're having problems
  card.partialBlockRead(true);
 
// Now we will look for a FAT partition!
  uint8_t part;
  for (part = 0; part < 5; part++) {     // we have up to 5 slots to look in
    if (vol.init(card, part)) 
      break;                             // we found one, lets bail
  }
  if (part == 5) {                       // if we ended up not finding one  :(
    putstring_nl("No valid FAT partition!");
    sdErrorCheck();      // Something went wrong, lets print out why
    while(1);                            // then 'halt' - do nothing!
  }
  
  // Lets tell the user about what we found
  putstring("Using partition ");
  Serial.print(part, DEC);
  putstring(", type is FAT");
  Serial.println(vol.fatType(),DEC);     // FAT16 or FAT32?
  
  // Try to open the root directory
  if (!root.openRoot(vol)) {
    putstring_nl("Can't open root dir!"); // Something went wrong,
    while(1);                             // then 'halt' - do nothing!
  }
  
  // Whew! We got past the tough parts.
  putstring_nl("Ready!");
  dirLevel = 0;
  lcd.begin(16,2);
  lcd.createChar(0,norm_eye_0);
  lcd.createChar(1,norm_eye_1);
  lcd.createChar(2,norm_eye_2);
  lcd.createChar(3,norm_eye_3);
  lcd.createChar(4,norm_eye_4);
  lcd.createChar(5,norm_eye_5);
  fear = 0;
  lcd.setCursor(0,0);
  lcd.setCursor(4,0);
  lcd.write((byte)0);
  lcd.setCursor(5,0);
  lcd.write((byte)1);
  lcd.setCursor(6,0);
  lcd.write((byte)2);
  lcd.setCursor(4,1);
  lcd.write((byte)3);
  lcd.setCursor(5,1);
  lcd.write((byte)4);
  lcd.setCursor(5,1);
  lcd.write((byte)5);
  
  lcd.setCursor(0,0);
  lcd.setCursor(10,0);
  lcd.write((byte)0);
  lcd.setCursor(11,0);
  lcd.write((byte)1);
  lcd.setCursor(12,0);
  lcd.write((byte)2);
  lcd.setCursor(10,1);
  lcd.write((byte)3);
  lcd.setCursor(11,1);
  lcd.write((byte)4);
  lcd.setCursor(12,1);
  lcd.write((byte)5);
  Serial.begin(9600);
  
}

void loop()
{
  
  /**
  sounds in terror order
  2
  5
  4
  1
  3
  */
    /**Serial.println("Playing ");
    playcomplete("SCARED.WAV");
    Serial.println("Playing 2");
    playcomplete("SLIGHT~1.WAV");
    Serial.println("Playing 3");
    playcomplete("TERRIF~1.WAV");
    Serial.println("Playing 4");
    playcomplete("FEARFUL.WAV");
    Serial.println("Playing 5");
    playcomplete("NERVOUS.WAV");*/
    if(fear > 800){
      playcomplete("TERRIF~1.WAV");
    }
    else if(fear > 500){
      playcomplete("SCARED.WAV");
    }
    else if(fear > 350){
      playcomplete("FEARFUL.WAV");
    }
    else if(fear > 250){
      playcomplete("NERVOUS.WAV");
    }
    else if(fear>100){
      playcomplete("SLIGHT~1.WAV");
    }
    else{
      delay(1500);
    }
    int i = 0;
    int uS = (sonar.ping() / US_ROUNDTRIP_CM);
    int brightness =  analogRead(pinLight);    //the light sensor is attached to analog 0
    int temperature = analogRead(pinTemp);
    int loudness = analogRead(pinSound);
    
    fear = fear-(fear/10);
    Serial.print("Darkness: ");
    Serial.println(brightness);
    Serial.print("AddFear: ");
    Serial.println(addDarkFear(lastBright,brightness));
    int darkFear = addDarkFear(lastBright, brightness);
    fear = fear+darkFear; 
    lastBright = brightness;
    loudness = lastLoud;
    Serial.print("Fear level: ");
    Serial.println(fear);
}


int addTemperatureFear(){
  return 1;
}

int addLoudFear(int lastLoud, int loudness){
  /**f(loudness-lastLoud > 400){
   return 200; 
  }
  else if(loudness>600){
    return 400;
  }
  else if(loudness>500){
    return 150;
  }
  else if(loudness>400){
    return 100;
  }
  else if(loudness>300){
    return 50;
  }
  else if (loudness > 200){
    return 25;
  }
  else{
   return 0; 
  }
  */
  return 1;
}
int addDarkFear(int lastBright, int brightness){
  
  Serial.println("mrp");
  Serial.println(brightness);
  Serial.println(lastBright);
  if(lastBright-brightness > 350){
     return 4000;
     Serial.println("It Happened!");
   }
   else if(brightness < 100){
     return 500;
   }
   else if(brightness < 200){
     return 350; 
   }
   else if(brightness < 300){
     return 100;
   }
   else if(brightness < 400){
     return 60;
   }
   else if(brightness < 500){
     return 40;
   }   
   else if(brightness < 600){
     return 20;
   }
      else if(brightness < 700){
     return 100;
   }
   else{
   return 0; 
  }
 }


/////////////////////////////////// HELPERS
void playfile(char *name) {
  // see if the wave object is currently doing something
  if (wave.isplaying) {// already playing something, so stop it!
    wave.stop(); // stop it
  }
  // look in the root directory and open the file
  if (!f.open(root, name)) {
    putstring("Couldn't open file "); Serial.print(name); return;
  }
  // OK read the file and turn it into a wave object
  if (!wave.create(f)) {
    putstring_nl("Not a valid WAV"); return;
  }
  
  // ok time to play! start playback
  wave.play();
}

void playcomplete(char *name) {
  // call our helper to find and play this name
  playfile(name);
  while (wave.isplaying) {
  // do nothing while its playing
  }
  // now its done playing
}

/*
 * print error message and halt
 */
void error_P(const char *str) {
  PgmPrint("Error: ");
  SerialPrint_P(str);
  sdErrorCheck();
  while(1);
}
/*
 * print error message and halt if SD I/O error, great for debugging!
 */
void sdErrorCheck(void) {
  if (!card.errorCode()) return;
  PgmPrint("\r\nSD I/O error: ");
  Serial.print(card.errorCode(), HEX);
  PgmPrint(", ");
  Serial.println(card.errorData(), HEX);
  while(1);
}
/*
 * play recursively - possible stack overflow if subdirectories too nested
 */
void play(FatReader &dir) {
  FatReader file;
  while (dir.readDir(dirBuf) > 0) {    // Read every file in the directory one at a time
  
    // Skip it if not a subdirectory and not a .WAV file
    if (!DIR_IS_SUBDIR(dirBuf)
         && strncmp_P((char *)&dirBuf.name[8], PSTR("WAV"), 3)) {
      continue;
    }

    Serial.println();            // clear out a new line
    
    for (uint8_t i = 0; i < dirLevel; i++) {
       Serial.write(' ');       // this is for prettyprinting, put spaces in front
    }
    if (!file.open(vol, dirBuf)) {        // open the file in the directory
      error("file.open failed");          // something went wrong
    }
    
    if (file.isDir()) {                   // check if we opened a new directory
      putstring("Subdir: ");
      printEntryName(dirBuf);
      Serial.println();
      dirLevel += 2;                      // add more spaces
      // play files in subdirectory
      play(file);                         // recursive!
      dirLevel -= 2;    
    }
    else {
      // Aha! we found a file that isnt a directory
      putstring("Playing ");
      printEntryName(dirBuf);              // print it out
      if (!wave.create(file)) {            // Figure out, is it a WAV proper?
        putstring(" Not a valid WAV");     // ok skip it
      } else {
        Serial.println();                  // Hooray it IS a WAV proper!
        wave.play();                       // make some noise!
        
        uint8_t n = 0;
        while (wave.isplaying) {// playing occurs in interrupts, so we print dots in realtime
          putstring(".");
          if (!(++n % 32))Serial.println();
          delay(100);
        }       
        sdErrorCheck();                    // everything OK?
        // if (wave.errors)Serial.println(wave.errors);     // wave decoding errors
      }
    }
  }
}
