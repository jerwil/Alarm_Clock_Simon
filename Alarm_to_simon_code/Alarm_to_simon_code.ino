 
#include <Wire.h>
#include "RTClib.h"
#include <LedControl.h> // For more info see http://www.wayoda.org/arduino/ledcontrol/index.html
 
RTC_DS1307 RTC;

LedControl mydisplay = LedControl(2, 3, 4, 1);


int adjust_amount = 60;    // how many seconds to adjust the time by
int multiplier = 1; // This mutliplier is used to change to hour adjustment
unsigned long currentTime;
unsigned long loopTime;

int current_count;
char* mode = "alarm_sound";
char* sub_mode = "minute_set";
char* mode_str[] = {"Tacos","Clock","Alarm Set"};
double alarm = 10800; // Alarm default in seconds
int alarm_array[6];
int current_time_array[6];
int old_second = 0; //This is used for the tick mechanism
int now_second = 0;
int unixtime_int = 0;
int display_array[6];
int time_format = 12;
boolean button_hi = false;
int button_state = 0;
int button_counter = 0; // This is used to detect how long the button is held for
int timeout = 0; // Time out for not pushing the button for a while
int blink = 1; // This is used for blinking numbers while adjusting time
double second_timer[1] = {0}; // This is used to keep track of the timer used to tick for each second
double half_second_timer[1] = {0}; // This is used to keep track of the timer used to tick for each second
double time_set_timer[1] = {0}; // This is used to keep track of the timer used for time setting
double button_hold_timer[1] = {0}; // This is used to keep track of the timer used for time setting
int time_set_tick = 200;
int PM = 0; // This is the indicator that time is in PM

int button1_press_initiate[1] = {0}; 
int button1_press_completed[1] = {0}; 
int button2_press_initiate[1] = {0}; 
int button2_press_completed[1] = {0}; 
int button3_press_initiate[1] = {0}; 
int button3_press_completed[1] = {0}; 
int button4_press_initiate[1] = {0}; 
int button4_press_completed[1] = {0}; 

int button_pushed = 0; // This is the indicator that the button was pushed and released
int alarm_tone = 1000; // This is the frequency for the alarm buzzer
int alarmLEDPin = 2; // This is the pin used to indicate if the alarm is on
boolean alarm_on = false;
unsigned long double_click_timeout;
int click_once = 0; // This integer is used to store the fact that we have one click and are looking for the second to occur before the delay
int double_clicked = 0; // This tells us if a double click occured.
int tone_happening = 0;

boolean Alarm_DP = false; //Indicate if the alarm decimal point should be lit
boolean PM_DP = false; //Indicate if the PM decimal point should be lit
boolean DP = false; //A general decimal point variable
int LCD_brightness = 7;
int rotary = 0;

int button1 = 6;
int button2 = 8;
int button3 = 10;
int button4 = 12;

int led1 = 7;
int led2 = 9;
int led3 = 11;
int led4 = 13;

int button_states[4] = {0,0,0,0};
int button_presses[4] = {0,0,0,0};

const int speakerPin = 5;    // the pin that will be used for speaker output
const int tones[] = {1915, 1700, 1519, 1432, 2700}; // The last tone is the fail tone
const int ledpins[] = {7, 9, 11, 13}; //This was most convenient for soldering
const int buttonpins[] = {6, 8, 10, 12}; //This made soldering easy since I skipped every other pin

// Variables will change:
int buttonState[] = {0,0,0,0};         // current state of the button
int lastButtonState[] = {0,0,0,0}; 
int buttonPushCounter[] = {0,0,0,0}; 

void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);
  }
}

void setup() {
  // initialize the button pin as a input:
  randomSeed(analogRead(A0));
  int i;
  for (i = 0; i < 4; i = i + 1){
    pinMode(buttonpins[i], INPUT);
  }
  // initialize the LED as an output:
  for (i = 0; i < 4; i = i + 1){
    pinMode(ledpins[i], OUTPUT);
  }
  pinMode(speakerPin, OUTPUT);
  // initialize serial communication:
//  Serial.begin(9600);
  pinMode (alarmLEDPin, OUTPUT);
Serial.begin(57600);

mydisplay.shutdown(0, false);  // turns on display
mydisplay.setIntensity(0, LCD_brightness); // 15 = brightest

}
int gamestate = 0;
int waitingforinput = 0;
int currentlevel = 1; // This is the level (also the number of button presses to pass to next level)
long randnum = 0; //initialize long variable for random number from 0-100.
int randnumint = 0; //initialize random integer for loop. Will be from 1-4 later.
int butwait = 500; //amount of time to wait for next button input (ghetto de-bounce)
int ledtime = 500; //amount of time each LED flashes for when button is pressed
int numlevels = 10; //number of levels until the game is won
int pinandtone = 0; //This integer is used when the sequence is displayed
int correct = 0; //This variable must be 1 in order to go to the next level
int speedfactor = 5; //This is the final speed of the lights and sounds for the last level. This increases as more games are won
int leddelay = 200; //Initializing time for LED. This will decrease as the level increases

void loop() {
  
int numarray[numlevels];
Serial.println("numarray!");
int userarray[numlevels];
Serial.println("userarray!");

//The following clears out both arrays for new games
int i;

if (gamestate == 0){
for (i = 0; i < numlevels; i = i + 1) {
   numarray[i]=0;
   userarray[i]=0;
  randnum = random(1,100);
    if (randnum <= 25) {
      randnumint = 0;
    }
    if (randnum > 25){
    randnumint = 1;
  }
    if (randnum > 50){
    randnumint = 2;
  }
    if (randnum > 75){
    randnumint = 3;
  }
  numarray[i]=randnumint;
//  Serial.println(numarray[i]);
}
if (i == numlevels){
 gamestate = 1; 
}
}


//The following will show the user the current sequence
if (waitingforinput == 0){
 delay (200);
i = 0;
for (i = 0; i < currentlevel; i= i + 1){
  leddelay = ledtime/(1+(speedfactor/numlevels)*(currentlevel - 1));
      pinandtone = numarray[i];
      digitalWrite(ledpins[pinandtone], HIGH);
      playTone(tones[pinandtone], leddelay);
      digitalWrite(ledpins[pinandtone], LOW);
      delay(100/speedfactor);
    }
    waitingforinput = 1;
}
i = 0;
int buttonchange = 0;    
int j = 0; // This is the current position in the sequence
while (j < currentlevel){    
    while (buttonchange == 0){
          for (i = 0; i < 4; i = i + 1){ 
          buttonState[i] = digitalRead(buttonpins[i]);
          buttonchange = buttonchange + buttonState[i];
        }
    }
     for (i = 0; i < 4; i = i + 1){
        if (buttonState[i] == HIGH) {
            digitalWrite(ledpins[i], HIGH);
            playTone(tones[i], ledtime);
            digitalWrite(ledpins[i], LOW);
            waitingforinput = 0;
            userarray[j]=i; 
            buttonState[i] = LOW;
            buttonchange = 0;
         }
       } 
        if (userarray[j] == numarray[j]){
            j++;  
            correct = 1;
            }
        else{
          
            correct = 0;
            i = 4;
            j = currentlevel;
            waitingforinput = 0;
        }
}

if (correct == 0){
  delay(300);
  i = 0;
  gamestate = 0;
  currentlevel = 1;
  for (i = 0; i < 4; i = i + 1){
         digitalWrite(ledpins[i], HIGH);
      }
          playTone(tones[4], ledtime);
   for (i = 0; i < 4; i = i + 1){
         digitalWrite(ledpins[i], LOW);   
       }
       delay (200);
  for (i = 0; i < 4; i = i + 1){
         digitalWrite(ledpins[i], HIGH);
      }
          playTone(tones[4], ledtime);
   for (i = 0; i < 4; i = i + 1){
         digitalWrite(ledpins[i], LOW);   
       }
       
       delay(500);
       gamestate = 0;
}

//Serial.println("The user sequence is");
//for (i = 0; i < numlevels; i = i + 1) {
//   Serial.println(userarray[i]);
//}
//Serial.println("The answers are");
//for (i = 0; i < numlevels; i = i + 1) {
//   Serial.println(numarray[i]);
//}

if (correct == 1){
    currentlevel++;
    waitingforinput = 0;
    }
if (currentlevel == numlevels){
  delay(500);
  // The following is the victory anmiation/sound:
  int notes[] = {2, 2, 2, 2, 0, 1, 2, 1, 2};
  int note = 0;
  int tempo[] = {200, 200, 200, 400, 400, 400, 200, 200, 600}; 
  int breaks[] = {100, 100, 100, 200, 200, 200, 300, 100, 200}; 
  for (i = 0; i < 9; i = i + 1){
  note = notes[i];
    digitalWrite(ledpins[note], HIGH);
    playTone(tones[note], tempo[i]);
    digitalWrite(ledpins[note], LOW);
    delay(breaks[i]);
  }
gamestate = 0;
currentlevel = 1;
numlevels = numlevels + 2;
speedfactor = speedfactor + 1;
    }
 
}
