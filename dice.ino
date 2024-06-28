#include "dice.h"
#include <tapSensor.h>
#include <squareWave.h>
#include <timeObj.h>
#include <mapper.h>
#include <serialStr.h>
#include <strTools.h>


#define  THINK_MS 1000
#define  SHOW_MS  5000
#define  TAP_MIN  20


enum        states { waiting, thinking, showing };
states      ourState;
tapSensor   ourTapSensor(3);
dice        ourDie[2];
bool        dreidelMode;
serialStr   serialMgr;


void setup(void) {
   
  timeObj  serialTimeout(5000);

  randomSeed(analogRead(A1));
  Serial.begin(57600);
  while(!Serial&&!serialTimeout.ding());
  serialMgr.setCallback(gotStr);
  dreidelMode = false;
  ourDie[0].begin(0x70);
  ourDie[0].clearDie();
  ourDie[1].begin(0x71);
  ourDie[1].clearDie();
  ourTapSensor.begin();
  ourState = waiting;
}


void gotStr(char* inStr) {

  lwrCase(inStr);
  if (!strcmp("dreidel",inStr)) {
    dreidelMode = true;
    Serial.println("Set to dreidel mode");
  } else if (!strcmp("dice",inStr)) {
    dreidelMode = false;
    Serial.println("Set to dice mode");
  } else {
    Serial.print("Hello! I'm in ");
    if (dreidelMode) {
      Serial.print("dreidel mode right now.");
      Serial.println("To change to dice mode, type dice."); 
    } else {
      Serial.print("dice mode right now.");
      Serial.println("To change to dreidel mode, type dreidel.");
    }
  }
}


void loop(void) {

   int  num;
   char letter;
   
   idle();
   switch(ourState) {
      case waiting   :
         if (ourTapSensor.getTapVal()>TAP_MIN) {
            ourDie[0].doFuzz(THINK_MS);
            ourDie[1].doFuzz(THINK_MS);
            ourState = thinking;
         }
      break;
      case thinking  :
         if (ourTapSensor.getTapVal()>TAP_MIN) {
            ourDie[0].doFuzz(THINK_MS);
            ourDie[1].doFuzz(THINK_MS);
         } else if (ourDie[0].isWaiting()) {
            if (dreidelMode) {
              num = random(1,5);
              if (num==1) letter = 'S';
              else if (num==2) letter = 'H';
              else if (num==3) letter = 'G';
              else if (num==4) letter = 'N';
              else letter = 'E';
              ourDie[0].showASCII(letter,SHOW_MS);
              ourDie[1].showASCII(letter,SHOW_MS);
              ourState = showing;
            } else {
              num = random(1,7);
              ourDie[0].showNum(num,SHOW_MS);
              ourDie[1].showNum(num,SHOW_MS);
              ourState = showing;
            }
         }
      break;
      case showing   :
         if (ourTapSensor.getTapVal()>TAP_MIN) {
            ourDie[0].doFuzz(THINK_MS);
            ourDie[1].doFuzz(THINK_MS);
            ourState = thinking;
         } else if (ourDie[0].isWaiting()) {
            ourState = waiting;
         }
      break;
   } 
}
