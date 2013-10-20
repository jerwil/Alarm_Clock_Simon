// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
 
#include <Wire.h>
#include "RTClib.h"
#include <LedControl.h> // For more info see http://www.wayoda.org/arduino/ledcontrol/index.html
 
RTC_DS1307 RTC;

LedControl mydisplay = LedControl(3, 4, 5, 1);

int adjust_amount = 60;    // how many seconds to adjust the time by
int multiplier = 1; // This mutliplier is used to change to hour adjustment
unsigned long currentTime;
unsigned long loopTime;
const int pin_A = 11;  // Rotary encoder pin A
const int pin_B = 10;  // Rotary encoder pin B

const int button_pin = 8;
unsigned char encoder_A;
unsigned char encoder_B;
unsigned char encoder_A_prev=0;
int current_count;
char* mode = "time_disp";
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
int time_set_tick = 100;
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
int speakerPin = 9; // This is the pin used by the alarm buzzer
int alarmLEDPin = 2; // This is the pin used to indicate if the alarm is on
boolean alarm_on = false;
unsigned long double_click_timeout;
int click_once = 0; // This integer is used to store the fact that we have one click and are looking for the second to occur before the delay
int double_clicked = 0; // This tells us if a double click occured. 

boolean Alarm_DP = false; //Indicate if the alarm decimal point should be lit
boolean PM_DP = false; //Indicate if the PM decimal point should be lit
boolean DP = false; //A general decimal point variable
int LCD_brightness = 7;
int rotary = 0;

int button1 = 10;
int button2 = 11;
int button3 = 12;
int button4 = 13;

int button_states[4] = {0,0,0,0};
int button_presses[4] = {0,0,0,0};

void setup () {
  
mydisplay.shutdown(0, false);  // turns on display
mydisplay.setIntensity(0, LCD_brightness); // 15 = brightest
  
    pinMode(pin_A, INPUT);
    pinMode(pin_B, INPUT);
    pinMode(button_pin, INPUT);
    pinMode (speakerPin, OUTPUT);
    pinMode (alarmLEDPin, OUTPUT);
    pinMode(button1, INPUT);
    pinMode(button2, INPUT);
    pinMode(button3, INPUT);
    pinMode(button4, INPUT);	

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
    //RTC.adjust(DateTime(__DATE__, __TIME__));
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

// _____________ PROGRAM STARTS HERE: _____________//

void loop () {

DateTime now = RTC.now();
time_to_ints(now, current_time_array);

buttoncheck(button_states); // Checks all 4 buttons and updates the array

button_state = digitalRead(button3);

check_button_presses();

if (button_state == HIGH){
 button_hi = true;
 timeout = 0; 
}
else {
 button_hi = false; 
}

if (PM == 1){PM_DP = true;}
else{PM_DP = false;}

//time_to_ints(now, current_time_array);

// The following checks the alarm condition:
if (alarm == time_to_double(now) && alarm_on == true){
  mode = "alarm_sound";
}



 if(mode == "time_disp"){ // This is current time mode
   currentTime = millis();
   time_array_to_digit_array(current_time_array, display_array); // Show current time unless otherwise specified. This is the default.
   
// New tick mechanism:


if (tick(1000, second_timer) == 1){
  Serial.print("Unix time: ");
  Serial.print(now.unixtime());
  Serial.println();
  printtime(now);
  old_second = now_second;
  Serial.print("Time as a double: ");
  Serial.print(time_to_double(now));
  Serial.println();
  if (button_hi == true){ // This code checks to see if the button has been held down long enough to set alarm
//    if (button_counter >= 3) {
//      mode = "alarm_set";
//	  sub_mode = "hour_set";
//    Serial.print("Switched mode to:");
//    Serial.print(mode);
//    Serial.println();
//	button_pushed = 0;
//    }
//    button_counter += 1;
  }
  else {
   button_counter = 0; 
  }
  Serial.print("Button held for:");
  Serial.print(button_counter);
  Serial.println();
}

// _____________ Setting Display Brightness: _____________//

if(compare_array(0,0,0,1,button_presses) && compare_array(0,0,0,0,button_states) && LCD_brightness < 15)  // If the 4th button is pressed and no others are, increase brightness up to max
  {LCD_brightness ++;
  mydisplay.shutdown(0, false);}
if(compare_array(0,0,1,0,button_presses) && compare_array(0,0,0,0,button_states) && LCD_brightness > 0){LCD_brightness --;} // If the 3rd button is pressed and no others are, decrease brightness down to min
if(compare_array(0,0,1,0,button_presses) && compare_array(0,0,0,0,button_states) && LCD_brightness == 1){mydisplay.shutdown(0, true);}  // turns off display below min brightness


// _____________ Setting Current Time: _____________//

if (button_states[1])
  {
  if (button_counter >= 10){time_set_tick = 5;}
  else if (button_counter >= 5){time_set_tick = 10;}
  else if (button_counter >= 3){time_set_tick = 50;}
  else {time_set_tick = 100;}
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
      if (tick(1000,second_timer) == 1){button_counter += 1;}
    }
    else if (compare_array(0,1,0,1,button_states)){
      if (tick(time_set_tick, time_set_timer) == 1)
      {
      DateTime then(now.unixtime() + adjust_amount*multiplier);
      RTC.adjust(then);
      now = RTC.now();
      }
      if (tick(1000,second_timer) == 1){button_counter += 1;}
    }
    else {button_counter = 0;}
  time_array_to_digit_array(current_time_array, display_array);
  }


// _____________ Setting Alarm: _____________//

if (button_states[0]){
  secs_to_hms(alarm, alarm_array);
  if (compare_array(1,0,1,0,button_states)){if (tick(500, half_second_timer) == 1){alarm -= adjust_amount*multiplier;}}
  if (compare_array(1,0,0,1,button_states)){if (tick(500, half_second_timer) == 1){alarm += adjust_amount*multiplier;}}
  time_array_to_digit_array(alarm_array, display_array);
}

// _____________ Toggling Alarm: _____________//

if (alarm_on == true){Alarm_DP = true;} // This indicates if the alarm is on
else {Alarm_DP = false;}

double_clicked = double_click(1000, button_presses[0]);
if (double_clicked == 1){
alarm_on = !alarm_on;
click_once = 0;
double_clicked = 0;
}

// if (button_states == {0,0,0,0}){if (LCD_brightness < 15){LCD_brightness ++;}} Work on brightness code

}

  if(mode == "time_set"){ // This is time set mode
	if (button_pushed == 1 && sub_mode == "minute_set") {sub_mode = "hour_set";}
	else if (button_pushed == 1 && sub_mode == "hour_set") {
		sub_mode = "minute_set";
		button_pushed = 0;
	}
	time_array_to_digit_array(current_time_array, display_array); 
 	if (sub_mode == "minute_set"){multiplier = 1;}
	else if (sub_mode == "hour_set"){multiplier = 60;}
        if (rotary == 1){
	  timeout = 0;
	  blink = 1; // This ensures the digits are visible while adjusting time			  
	  now = RTC.now();
	  DateTime then(now.unixtime() + adjust_amount*multiplier); // one hour later
	  RTC.adjust(then);
	  now = RTC.now();
	  printtime(now);   
	}
        else if (rotary == -1){
       	  timeout = 0;
	  blink = 1;			  
	  now = RTC.now();
	  DateTime then(now.unixtime() - adjust_amount*multiplier); // one hour later
	  RTC.adjust(then);
	  now = RTC.now();
	  printtime(now);        
	}   
 
	time_array_to_digit_array(current_time_array, display_array);
		
	if (blink == 0 && sub_mode == "minute_set"){
		display_array[2] = 10;
		display_array[3] = 10;
		}
	if (blink == 0 && sub_mode == "hour_set"){
		display_array[0] = 10;
		display_array[1] = 10;
		}

	if (tick(1000, second_timer) == 1){  
	  timeout += 1;
	  if (timeout >= 10){ // If the button is not pressed for 10 seconds
		mode = "time_disp";
		timeout = 0;
		Serial.print("Timeout, switching to clock mode");
		Serial.println();
	  }
	if (button_hi == true){ // This code checks to see if the button has been held down long enough to set alarm
		if (button_counter >= 3) {
			mode = "alarm_set";
			sub_mode = "hour_set";
			Serial.print("Switched mode to:");
			Serial.print(mode);
			Serial.println();
			button_pushed = 0;
			}
		button_counter += 1;
	}
	else {button_counter = 0;}
	  // Serial.print("Time Set Mode");
	  // Serial.println();  
	  // print_time_array_separated(current_time_array);
	}
	
	if (tick(500, half_second_timer) == 1){  
		if ((sub_mode == "minute_set" || sub_mode == "hour_set") && blink == 0){
			blink = 1;
		}
		else if ((sub_mode == "minute_set" || sub_mode == "hour_set") && blink == 1){
			blink = 0;
		}
	}
}

 if(mode == "alarm_set"){ // This is alarm set mode

// digitalWrite(alarmLEDPin, HIGH); // This shows that we are setting the alarm. This will blink.
 
  if (button_pushed == 1 && sub_mode == "minute_set") {sub_mode = "hour_set";}
  else if (button_pushed == 1 && sub_mode == "hour_set") {sub_mode = "minute_set";}
 
  secs_to_hms(alarm, alarm_array);

  if (sub_mode == "minute_set"){multiplier = 1;}
  else if (sub_mode == "hour_set"){multiplier = 60;}

  if (rotary != 0){
    digitalWrite(alarmLEDPin, HIGH);
    timeout = 0;   
    blink = 1;
  }
   if (rotary == 1){alarm += adjust_amount*multiplier;}
   else if (rotary == -1){alarm -= adjust_amount*multiplier;}   
   if (alarm > 86400) {alarm -= 86400;}
   if (alarm < 0) {alarm += 86400;}


  time_array_to_digit_array(alarm_array, display_array);

if (blink == 0 && sub_mode == "minute_set"){
	display_array[2] = 10;
	display_array[3] = 10;
	// digitalWrite(alarmLEDPin, LOW);
	}
if (blink == 0 && sub_mode == "hour_set"){
	display_array[0] = 10;
	display_array[1] = 10;
	// digitalWrite(alarmLEDPin, LOW);
	}

	
	
	if (tick(1000, second_timer) == 1){
		Serial.print("Alarm as int:");
		Serial.println(alarm);
	  timeout += 1;
	  if (timeout >= 10){ // If the button is not pressed for 10 seconds
		mode = "time_disp";
		timeout = 0;
		Serial.print("Timeout, switching to clock mode");
		Serial.println();
	  }
	if (button_hi == true){ // This code checks to see if the button has been held down long enough to set alarm
		if (button_counter >= 3) {
			mode = "time_set";
			sub_mode = "minute_set";
			Serial.print("Switched mode to:");
			Serial.print(mode);
			Serial.println();
			button_pushed = 0;
			}
		button_counter += 1;
	}
	else {button_counter = 0;}
	  Serial.print("Alarm Set Mode");
	  Serial.println();  
	  print_time_array_separated(alarm_array);
	}
	  
	if (tick(500, half_second_timer) == 1){  
		if ((sub_mode == "minute_set" || sub_mode == "hour_set") && blink == 0){
		blink = 1;
		digitalWrite(alarmLEDPin, HIGH);
		}
		else if ((sub_mode == "minute_set" || sub_mode == "hour_set") && blink == 1){
		blink = 0;
		digitalWrite(alarmLEDPin, LOW);
		}
	}
}
 
if(mode == "alarm_sound"){ // This is alarm mode. Current problem is display_array doesn't get updated
  
time_array_to_digit_array(current_time_array, display_array);

	if (blink == 0){
           for (int digit = 0; digit < 4; digit++)  {display_array[digit] = 10;}
		buzz(alarm_tone);
	}  
if (tick(500, second_timer) == 1){
	if (blink == 0){blink = 1;}
	else if (blink == 1){blink = 0;}
		Serial.print("Alarm!!!!");
		Serial.println();
	}
	if (button_pushed == 1){mode = "time_disp";}
}

// At the end of each cycle, send display array to the 4 digit display

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

}


