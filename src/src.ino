#include <Arduino.h>
#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2SNoDAC.h"
#include <NewPing.h>
//include sounds 

#include "front.h"
#include "left.h"
#include "right.h"
#include "open.h"
#include "close.h"
//

//#define LEFT_BUTTON_PIN D1
//#define RIGHT_BUTTON_PIN  D7
#define FRONT_BUTTON_PIN D2

#define LEFT_SONAR_TRIGGER_PIN D3
#define LEFT_SONAR_ECHO_PIN D4
#define RIGHT_SONAR_TRIGGER_PIN D5 // blue
#define RIGHT_SONAR_ECHO_PIN D6 // hoyeri 
#define FRONT_SONAR_TRIGGER_PIN D1
#define FRONT_SONAR_ECHO_PIN D7

#define MAX_DISTANCE 1200 // Maximum distance to measure in centimeters
#define TIMEOUT 9000 // 10 seconds timeout for button press

NewPing leftSonar(LEFT_SONAR_TRIGGER_PIN, LEFT_SONAR_ECHO_PIN, MAX_DISTANCE);
NewPing rightSonar(RIGHT_SONAR_TRIGGER_PIN, RIGHT_SONAR_ECHO_PIN, MAX_DISTANCE);
NewPing frontSonar(FRONT_SONAR_TRIGGER_PIN, FRONT_SONAR_ECHO_PIN, MAX_DISTANCE);

//unsigned long lastButtonPressTime = 0;
static unsigned long lastButtonPressTime = 0;  // Variable to store the previous timestamp
const long interval = 10000;

// Define audio files corresponding to directions

AudioGeneratorWAV *wav;
AudioFileSourcePROGMEM *file;
AudioOutputI2SNoDAC *out;

void setup()
{
  Serial.begin(115200);
  delay(1000);

  audioLogger = &Serial;
  out = new AudioOutputI2SNoDAC();
  wav = new AudioGeneratorWAV();
 
  pinMode(FRONT_BUTTON_PIN, INPUT_PULLUP);
}

void loop()
{
    unsigned long currentMillis = millis(); 
  if (currentMillis - lastButtonPressTime >= interval) {
    int test1,test2,test3,sensor=0,maxdistance;
    test1=leftSonar.ping_cm();
    test2=rightSonar.ping_cm();
    test3=frontSonar.ping_cm();
       Serial.print("Left  :");
       Serial.println(test1);
       Serial.print("Right : ");
       Serial.println(test2);
       Serial.print("Front :");
       Serial.println(test3);
       Serial.print("times:");
       Serial.println(currentMillis - lastButtonPressTime);
       if((test1>test2) && (test1>test3)){
        maxdistance=test1;
        sensor=1;
       }
       else if(test2>test3){
        maxdistance=test2;
        sensor=2;
       }
       else {
          maxdistance=test3;
           sensor=3;
       }

    if (maxdistance > 100 ) {
      if (sensor == 1 ) {
       playSound("leftSound");
       playSound("openSound");
       // lastButtonPressTime = millis();
         lastButtonPressTime = currentMillis;
      } else if (sensor == 2) {
        playSound("rightSound");
         playSound("openSound");
       // lastButtonPressTime = millis();
         lastButtonPressTime = currentMillis;
      } else if(sensor == 3) {
       // Serial.println("playing front sound ...");
        playSound("frontSound");
         playSound("openSound");
      //  lastButtonPressTime = millis();
         lastButtonPressTime = currentMillis;
      }
    } else if(test1 == 0 && test2 == 0 && test3 == 0){
         playSound("frontSound");
         playSound("openSound");
      
    }
      else {
          playSound("closeSound");
      Serial.printf(" automatic call : all side is full  \n");
       lastButtonPressTime = currentMillis;
    } 
  }   else {
    if (digitalRead(FRONT_BUTTON_PIN) == LOW ) {
      checkAndPlaySound(frontSonar, "frontSound");
      lastButtonPressTime = currentMillis;
    } 
    
  } 
}

void checkAndPlaySound(NewPing &sonar,  const char * sound)
{
  int distance = sonar.ping_cm();
  if (distance > 100) {
     Serial.println(sound);
  Serial.print(" side  open ");
    playSound(sound);
    playSound("openSound");
  } else {
  //Serial.println(sound);
  Serial.print(" side not open ");
  playSound(sound);
  playSound("closeSound");
 // Serial.println(sound);
  }
}

void playSound( const char * sound)
{
  Serial.print("Playing sound :");
  Serial.println(sound);
  Serial.printf("\n");
    qqqg      ewqus               aa  Za87
  if (wav->isRunning())
  {
    wav->stop();
  }
   if(strcmp(sound, "frontSound") == 0) {
  file = new AudioFileSourcePROGMEM(front, sizeof(front));
} else if(strcmp(sound, "rightSound") == 0) {
  file = new AudioFileSourcePROGMEM(right, sizeof(right));
} else if(strcmp(sound, "leftSound") == 0) {
  file = new AudioFileSourcePROGMEM(left, sizeof(left));
} else if(strcmp(sound, "openSound") == 0) {
  file = new AudioFileSourcePROGMEM(open, sizeof(open));
} else if(strcmp(sound, "closeSound") == 0) {
  file = new AudioFileSourcePROGMEM(close, sizeof(close));
}

  wav->begin(file, out);

  while (wav->isRunning())
  {
    if (!wav->loop())
      wav->stop();
  }
}
