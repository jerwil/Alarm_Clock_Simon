// Arduino memory game alarm clock by Jeremy Wilson
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
 
#include <Wire.h>
#include "RTClib.h"
#include <LedControl.h> // For more info see http://www.wayoda.org/arduino/ledcontrol/index.html
 
RTC_DS1307 RTC;

LedControl mydisplay = LedControl(4, 3, 2, 1); // *** Changed for board design simplicity ***

int adjust_amount = 60;    // how many seconds to adjust the time by in seconds
int multiplier = 1; // This mutliplier is used to change to hour adjustment if needed

unsigned long currentTime;
unsigned long loopTime;

int current_count;
char* mode = "time_disp";
double alarm = 28800; // Alarm default in seconds (8:00 AM)
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

// _____ Arrays to store button state: _____ //

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
int speakerPin = 5; // This is the pin used by the alarm buzzer
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


// _______ Hardware Constants: ________ //

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

// _______ Constants for Simon portion: ______ //

int gamestate = 0;
int waitingforinput = 0;
int currentlevel = 1; // This is the level (also the number of button presses to pass to next level)
long randnum = 0; //initialize long variable for random number from 0-100.
int randnumint = 0; //initialize random integer for loop. Will be from 1-4 later.
int butwait = 500; //amount of time to wait for next button input (ghetto de-bounce)
int ledtime = 500; //amount of time each LED flashes for when button is pressed
int numlevels = 11; //number of levels + 1 until the game is won (numlevels-1 = sequence length)
int pinandtone = 0; //This integer is used when the sequence is displayed
int correct = 0; //This variable must be 1 in order to go to the next level
int speedfactor = 5; //This is the final speed of the lights and sounds for the last level. This increases as more games are won
int leddelay = 200; //Initializing time for LED. This will decrease as the level increases
const int tones[] = {1915, 1700, 1519, 1432, 2700}; // The last tone is the fail tone
const int ledpins[] = {7, 9, 11, 13}; //This is the LED pin format for the simon code
const int buttonpins[] = {6, 8, 10, 12}; //This is the button pin format for the simon code
int lastbutton_states[] = {0,0,0,0}; 
int buttonPushCounter[] = {0,0,0,0}; 
int simon_timeout = 0;

void setup () {
  
randomSeed(analogRead(A0)); //By taking the random seed from an open (unwired) analog channel, it is as close as you can get to a true random number
  

  
    pinMode (speakerPin, OUTPUT);
    pinMode (alarmLEDPin, OUTPUT);
    pinMode(button1, INPUT);
    pinMode(button2, INPUT);
    pinMode(button3, INPUT);
    pinMode(button4, INPUT);
    pinMode(led1, OUTPUT);
    pinMode(led2, OUTPUT);	
    pinMode(led3, OUTPUT);	
    pinMode(led4, OUTPUT);		

    Serial.begin(57600);
    Wire.begin();
    RTC.begin();
	DateTime now = RTC.now();
	int now_second = now.second();
	old_second = now_second;
    //DateTime now = RTC.now();
	
 
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
  }
  
    // following line sets the RTC to the date & time this sketch was compiled
    // RTC.adjust(DateTime(__DATE__, __TIME__));
    
mydisplay.shutdown(0, false);  // turns on display
mydisplay.setIntensity(0, LCD_brightness); // 15 = brightest

}


void printtime(DateTime time){ // This function mainly used for debugging purposes
    Serial.print(" time: ");
    Serial.print(time.year(), DEC);
    Serial.print('/');
    Serial.print(time.month(), DEC);
    Serial.print('/');
    Serial.print(time.day(), DEC);
    Serial.print(' ');
    Serial.print(time.hour(), DEC);
    Serial.print(':');
    Serial.print(time.minute(), DEC);
    Serial.print(':');
    Serial.print(time.second(), DEC);
    Serial.println();
    Serial.print("mode = ");    
    Serial.print(mode);
    Serial.println();  
}

void time_to_ints(DateTime time, int time_array[6]){
    int year_int, month_int, day_int, hour_int, minute_int, second_int;
    year_int = time.year();
    month_int = time.month();
    day_int = time.day();
    hour_int = time.hour();
    minute_int = time.minute();
    second_int = time.second();
    time_array[0] = year_int;
    time_array[1] = month_int;
    time_array[2] = day_int;
    time_array[3] = hour_int;
    time_array[4] = minute_int;
    time_array[5] =  second_int;
}

double time_to_double(DateTime time){
    double year_int, month_int, day_int, hour_int, minute_int, second_int;
    year_int = time.year();
    month_int = time.month();
    day_int = time.day();
    hour_int = time.hour();
    minute_int = time.minute();
    second_int = time.second();
    double seconds = hour_int*3600+minute_int*60+second_int;
    return seconds;
}

void print_time_array_separated(int time_array[6]){
  Serial.println();
  Serial.print("Hours: ");
  Serial.println(time_array[3]);
  Serial.print("Minutes: ");
  Serial.println(time_array[4]);
  Serial.print("Seconds: ");
  Serial.println(time_array[5]);
  
}

int tick(int delay, double timekeeper[1]){
currentTime = millis();
if(currentTime >= (timekeeper[0] + delay)){
	timekeeper[0] = currentTime;
	return 1;
  }
else {return 0;}
}

void secs_to_hms(double secs_in, int time_array[6]){
    double hours = floor(secs_in/3600);
    double minutes = floor(((secs_in - hours*3600)/60));
    double seconds = floor((secs_in - hours*3600 - minutes*60));
    int hour_int = hours;
    int minute_int = minutes;
    int second_int = seconds;
    time_array[3] = hour_int;
    time_array[4] = minute_int;
    time_array[5] = second_int;
}

double time_array_to_secs(int time_array[6]){
    return time_array[3]*60*60 + time_array[4]*60 + time_array[5];
}

void time_array_to_digit_array(int time_array[6], int digit_array[6]){
  int hours = time_array[3];
  int minutes = time_array[4];
  int seconds = time_array[5];
  PM = 0; // AM until proven PM
  
  if (time_format == 12){
	if (hours == 12){PM = 1;}
	else if (hours > 12) {
		hours -= 12;
		PM = 1;
	}
	else if (hours == 0){
	hours = 12;
	PM = 0;
	}
  }
  
  if (hours < 10 && hours > 0){
    digit_array[0] = 10; // 10 will be the designation for not displaying anything
    digit_array[1] = hours;
  }
  else {
    digit_array[0] = hours/10;
    digit_array[1] = hours%10;
  }
  if (minutes < 10){
  digit_array[2] = 0; // 10 will be the designation for not displaying anything
  digit_array[3] = minutes;
  }
  else {
    digit_array[2] = minutes/10;
    digit_array[3] = minutes%10;
  }
  if (time_format == 24){PM = 0;} // The PM LED is not needed in 24 hour time format

}

int button_press (int button_indicator, int button_press_initiated[1], int button_press_complete[1]){
	if (button_indicator == 0 && button_press_initiated[0] == 1) {
	button_press_complete[0] = 1;
	Serial.print("Button press complete");
	Serial.println();
	button_press_initiated[0] = 0;
	}
	else if (button_indicator == 1){
	button_press_initiated[0] = 1;
	button_press_complete[0] = 0;
	}
	else {button_press_complete[0] = 0;}
return button_press_complete[0];
}

void buzz(int tone) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);
}

void buttoncheck(int button_states[4]){
//  for (int i=0;i<4;i++){button_states[i]=0;}
  if(digitalRead(button1)){button_states[0] = 1;}
  else {button_states[0] = 0;}
  if(digitalRead(button2)){button_states[1] = 1;}
  else {button_states[1] = 0;}
  if(digitalRead(button3)){button_states[2] = 1;}
  else {button_states[2] = 0;}
  if(digitalRead(button4)){button_states[3] = 1;}
  else {button_states[3] = 0;}
}

int double_click(int delay, int button_to_check){
int double_click_complete;
	if (button_to_check == 1){
		if (click_once == 1 && millis() <= double_click_timeout + delay){
		double_click_complete = 1;
		Serial.print("Second click of couble click");
		Serial.println();
		}
		else if (click_once == 0) {
		click_once = 1;
		double_click_complete = 0;
		double_click_timeout = millis();
		Serial.print("First click of couble click");
		Serial.println();
		}
	}
	if (button_to_check == 0 && millis() >= double_click_timeout + delay){
	click_once = 0;
	double_click_complete = 0;
	}
return double_click_complete;
}

void check_button_presses()
{
  button_presses[0] = button_press (button_states[0], button1_press_initiate, button1_press_completed); 
  button_presses[1] = button_press (button_states[1], button2_press_initiate, button2_press_completed); 
  button_presses[2] = button_press (button_states[2], button3_press_initiate, button3_press_completed); 
  button_presses[3] = button_press (button_states[3], button4_press_initiate, button4_press_completed);  
}

boolean compare_array(int a,int b,int c,int d, int array_to_compare[4]){
return(a == array_to_compare[0] && b == array_to_compare[1] && c == array_to_compare[2] && d == array_to_compare[3]);
}

void LEDs_with_buttons(int button_status[4]){
 if (button_status[0] == 1){digitalWrite(led1, HIGH);}
 else{digitalWrite(led1, LOW);}
 if (button_status[1] == 1){digitalWrite(led2, HIGH);}
 else{digitalWrite(led2, LOW);}
 if (button_status[2] == 1){digitalWrite(led3, HIGH);}
 else{digitalWrite(led3, LOW);}
 if (button_status[3] == 1){digitalWrite(led4, HIGH);}
 else{digitalWrite(led4, LOW);}
}

void update_screen(int array_to_display[4]){
  for (int digit = 0; digit < 4; digit++)  {
   if (digit == 3){DP = PM_DP;} // For the last digit, light up decimal point if time is PM
   else if (digit == 0){DP = Alarm_DP;} // For the first digit, light up decimal point if the alarm is on
   else {DP = false;}
 
   if (array_to_display[digit] == 10){          // This is the convention for a blank digit
    mydisplay.setChar(0,3-digit,' ', DP); // This is how you print a blank digit (as a space character)
   }
   else {mydisplay.setDigit(0, 3-digit, array_to_display[digit], DP);} 
 }  
 mydisplay.setIntensity(0, LCD_brightness); // 15 = brightest 
}

// Functions for Simon portion:

void playTone(int tone, int duration) {
  tone_happening = 1;
  for (long dur = 0; dur < duration * 1000L; dur += tone * 2) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);
//    Serial.print("Playing tone:");
//    Serial.print(tone);
//    Serial.print("    Duration is:");
//    Serial.println(dur);
    tone_happening = 1;
  }
  tone_happening = 0;
}

// _____________ PROGRAM STARTS HERE: _____________//

void loop () {

if (PM == 1){PM_DP = true;}
else{PM_DP = false;}



if(mode == "time_disp"){ // This is current time mode

  DateTime now = RTC.now();
  time_to_ints(now, current_time_array);
  time_array_to_digit_array(current_time_array, display_array);
 
 // The following checks the alarm condition:
 
  if (alarm == time_to_double(now) && alarm_on == true){
  mode = "alarm_sound";
  }
 
  buttoncheck(button_states); // Checks all 4 buttons and updates the array

  LEDs_with_buttons(button_states);

  check_button_presses();

  currentTime = millis();


// _____________ Setting Display Brightness: _____________//

if(compare_array(0,0,0,1,button_presses) && compare_array(0,0,0,0,button_states) && LCD_brightness < 15){LCD_brightness ++;} // If the 4th button is pressed and no others are, increase brightness up to max
if(compare_array(0,0,1,0,button_presses) && compare_array(0,0,0,0,button_states) && LCD_brightness >= 1){LCD_brightness --;} // If the 3rd button is pressed and no others are, decrease brightness down to min
if(compare_array(0,0,1,0,button_presses) && compare_array(0,0,0,0,button_states) && LCD_brightness == 0){mydisplay.shutdown(0, true);}  // turns off display below min brightness
if (LCD_brightness >= 1){mydisplay.shutdown(0, false);}

// _____________ Setting Current Time: _____________//

if (button_states[1])
  {
  if (button_counter >= 8){time_set_tick = 5;} // As the button is held down longer, the time adjustment speed increases
  else if (button_counter >= 5){time_set_tick = 10;}
  else if (button_counter >= 3){time_set_tick = 50;}
  else {time_set_tick = 200;}
  time_array_to_digit_array(current_time_array, display_array); 
  now = RTC.now();
    if (compare_array(0,1,1,0,button_states))
    {
      if (tick(time_set_tick, time_set_timer) == 1)
      {
      DateTime then(now.unixtime() - adjust_amount*multiplier);
      RTC.adjust(then);
      now = RTC.now();
      }
      if (tick(1000,button_hold_timer) == 1){button_counter += 1;}
    }
    else if (compare_array(0,1,0,1,button_states)){
      if (tick(time_set_tick, time_set_timer) == 1)
      {
      DateTime then(now.unixtime() + adjust_amount*multiplier);
      RTC.adjust(then);
      now = RTC.now();
      }
      if (tick(1000,button_hold_timer) == 1){button_counter += 1;}
    }
    else { button_counter = 0;}
  time_array_to_digit_array(current_time_array, display_array);
  }


// _____________ Setting Alarm: _____________//

if (button_states[0]){
  if (button_counter >= 8){time_set_tick = 5;} // As the button is held down longer, the time adjustment speed increases
  else if (button_counter >= 5){time_set_tick = 10;}
  else if (button_counter >= 3){time_set_tick = 50;}
  else {time_set_tick = 200;}
  secs_to_hms(alarm, alarm_array);
  if (compare_array(1,0,1,0,button_states))
    {
    if (tick(1000,button_hold_timer) == 1){button_counter += 1;}
    if (tick(time_set_tick, time_set_timer) == 1){alarm -= adjust_amount*multiplier;}
    }
  else if (compare_array(1,0,0,1,button_states))
    {
    if (tick(1000,button_hold_timer) == 1){button_counter += 1;}
    if (tick(time_set_tick, time_set_timer) == 1){alarm += adjust_amount*multiplier;}
    }
  else { button_counter = 0;}  
  if (alarm > 86400) {alarm -= 86400;} //Alarm will roll over
  if (alarm < 0) {alarm += 86400;}
  time_array_to_digit_array(alarm_array, display_array);
}

// _____________ Toggling Alarm: _____________//
// The alarm is toggled on and off by double-clicking the first button
if (alarm_on == true){Alarm_DP = true;} // This indicates if the alarm is on (Alarm_DP means alarm decimal point)
else {Alarm_DP = false;}

double_clicked = double_click(250, button_presses[0]);
if (double_clicked == 1){
  alarm_on = !alarm_on;
  click_once = 0;
  double_clicked = 0;
}

// _____________ Toggling 12/24 hour mode: _____________//
// The clock is switched between 12 and 24 hour mode by double-clicking the 2nd button

double_clicked = double_click(250, button_presses[1]);
if (double_clicked == 1){
  if (time_format == 12){time_format = 24;}
  else if (time_format == 24){time_format = 12;}
  click_once = 0;
  double_clicked = 0;
}


// _____________ Setting Difficulty: _____________//
// This allows the user to choose the sequence length required to turn off the alarm. This is done by holding down the first two buttons then using the right two buttons as + and -.
// To keep it challenging, a sequence of 5 is the minimum, but 16 is the max (10 is really hard anyway!)
if (button_states[0] && button_states[1]){
  if(compare_array(0,0,0,1,button_presses) && numlevels < 17){numlevels ++;} // If the 4th button is pressed and no others are, increase number of levels up to max of 16 (5 in sequence)
  if(compare_array(0,0,1,0,button_presses) && numlevels > 6){numlevels --;} // If the 3rd button is pressed and no others are, decrease number of levels down to min of 6 (5 in sequence)
  display_array[0] = 10; // First two digits are blank
  display_array[1] = 10;
  if ((numlevels-1) < 10){
  display_array[2] = 0;
  display_array[3] = numlevels-1;
  }
  else {
    display_array[2] = (numlevels-1)/10;
    display_array[3] = (numlevels-1)%10;
  }
}

// _____________ Voluntary Simon Mode: _____________//
// This will allow the user to play Simon on-demand by pushing all 4 buttons at once when the alarm is off
if (button_states[0] && button_states[1] && button_states[2] && button_states[3] && alarm_on == false){
mode = "alarm_sound";
for(int k=0; k<4; k++){digitalWrite(ledpins[k], LOW);}
delay(500);
}

update_screen(display_array);

}

// _______ End of time display mode ______//

// _____________ Alarm Mode: _____________//
 
if(mode == "alarm_sound"){ // This is alarm mode
  
//The following clears out then initializes both arrays for new games  
  
int numarray[numlevels];
int userarray[numlevels];
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
  leddelay = ledtime;
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
  while (simon_timeout >= 5){ // This code will cause the game to timeout and sound the alarm if the user doesn't respond within 5 seconds
  correct = 1;
  for (i = 0; i < 4; i = i + 1){ 
  button_states[i] = digitalRead(buttonpins[i]);
  buttonchange = buttonchange + button_states[i];
  }
  i = 4;
  if (blink == 0){
  buzz(alarm_tone);
  for(int k=0; k<4; k++){digitalWrite(ledpins[k], HIGH);}
  }
  else if (blink == 1) {for(int k=0; k<4; k++){digitalWrite(ledpins[k], LOW);}}
    if (tick(500, half_second_timer) == 1){
        if (blink == 0){blink = 1;}
	else if (blink == 1){blink = 0;}   
    }
  if (buttonchange != 0){ // This gets the alarm out of the timeout state and back into the game
    simon_timeout = 0;
      for(int k=0; k<4; k++){digitalWrite(ledpins[k], LOW);}
    gamestate = 0;
    currentlevel = 0;
    j = currentlevel;
  }  
        DateTime now = RTC.now();
        time_to_ints(now, current_time_array);
        time_array_to_digit_array(current_time_array, display_array);
        update_screen(display_array);   
  }
 if (gamestate == 1){
    while (buttonchange == 0 && simon_timeout < 5){
          for (i = 0; i < 4; i = i + 1){ 
          button_states[i] = digitalRead(buttonpins[i]);
          buttonchange = buttonchange + button_states[i];
        }

        DateTime now = RTC.now();
        time_to_ints(now, current_time_array);
        time_array_to_digit_array(current_time_array, display_array);
        update_screen(display_array); 
        if (tick(1000, second_timer) == 1 && alarm_on == true){ //Simon will only time out if the alarm is on (not in voluntary Simon mode
        simon_timeout += 1; // The timeout timer will fill up as long as there is no input
        Serial.print("Simon timeout: ");
        Serial.println(simon_timeout);
        }     
    }
    if (simon_timeout < 5){
     for (i = 0; i < 4; i = i + 1){
        if (button_states[i] == HIGH) {
            simon_timeout = 0;
            digitalWrite(ledpins[i], HIGH);
            playTone(tones[i], ledtime);
            Serial.print("Input into tones function:");
            Serial.println(tones[i]);
            digitalWrite(ledpins[i], LOW);
            waitingforinput = 0;
            userarray[j]=i; 
            button_states[i] = LOW;
            buttonchange = 0;
         }
       } 
        if (userarray[j] == numarray[j]){
            Serial.println("Correct!");
            j++;  
            correct = 1;
            }
        else{
            Serial.println("Wrong!");
            correct = 0;
            i = 4;
            j = currentlevel;
            waitingforinput = 0;
        }
 }
 }
}

if (gamestate == 1){

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

Serial.println("The user sequence is");
for (i = 0; i < numlevels; i = i + 1) {
   Serial.println(userarray[i]);
}
Serial.println("The answers are");
for (i = 0; i < numlevels; i = i + 1) {
   Serial.println(numarray[i]);
}

if (correct == 1){
    currentlevel++;
    waitingforinput = 0;
    }
if (currentlevel == numlevels){
  Serial.println("VICTORY!");
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
simon_timeout = 0;
mode = "time_disp";
    }
}

}

// At the end of each cycle, send display array to the 4 digit display
/*

 for (int digit = 0; digit < 4; digit++)  {
   if (digit == 3){DP = PM_DP;} // For the last digit, light up decimal point if time is PM
   else if (digit == 0){DP = Alarm_DP;} // For the first digit, light up decimal point if the alarm is on
   else {DP = false;}
 
   if (display_array[digit] == 10){          // This is the convention for a blank digit
    mydisplay.setChar(0,3-digit,' ', DP); // This is how you print a blank digit (as a space character)
   }
   else {mydisplay.setDigit(0, 3-digit, display_array[digit], DP);} 
 } 
 
 mydisplay.setIntensity(0, LCD_brightness); // 15 = brightest
*/
}


