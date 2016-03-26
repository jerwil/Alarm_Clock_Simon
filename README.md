# Memory Game Alarm Clock

A device to make waking up easier by challenging your mind in the morning.

![](https://raw.githubusercontent.com/jerwil/Alarm_Clock_Simon/master/Media/Simon%20Alarm%20Gif.gif "Alarm Clock Animation")

See the blog post here: [http://www.jeremyadamwilson.com/memory-game-alarm-clock/](http://www.jeremyadamwilson.com/memory-game-alarm-clock/)

## Features

* Requires a memory game to be beaten to turn off alarm
* All functions of alarm clock are controlled by 4 memory game buttons
* Buttons light up whenever pressed for feedback
* Intuitive alarm and time setting
* Adjustable brightness
* Adjustable difficulty
* 12/24 hour time display modes

## Demo Video

See all the features in the [demo video](https://www.youtube.com/watch?v=Q9e83-DTT6o).

## How to Use

![](https://raw.githubusercontent.com/jerwil/Alarm_Clock_Simon/master/Media/Buttons_Labeled.png "Buttons labeled")

* The leftmost button controls the alarm time, while the next button to the right controls the current time
* The right two buttons are for + and -
* Without holding down any other button, + and - control the display brightness.
* With the alarm set botton held, + and - change the alarm time
* With the time button held, + and - change the current time
* With the leftmost two buttons held, + and - change the difficulty level
* The alarm set button is double-pressed to toggle the alarm on or off


## Requirements

### Subsystems

This project brought together a lot of smaller projects I had worked on in the past. The below sections describe the various subsystems that all came together to make this project.

*Real Time Clock*

The DS 1307 real time clock requires a crystal, resistors, and a 3V battery. It communicates to the ATMega328 by the I²C protocol.I started out using a breakout board kit for prototyping, for for the final design, all the support hardware was put onto the alarm clock circuit board.

*ATMega238, buttons, lights, and memory game*

The ability to create my own smart device based on the ATMega328 without an Arduino was learned during my [Arduino independence project](http://www.jeremyadamwilson.com/arduino-independence-simon-game/). Using the same support hardware, I was able to build onto that project to form the complete alarm clock circuit board.

The logic for the memory game was pulled from my [Arduino Simon](https://github.com/jerwil/Simon_Etch) project.

*LED Display*

The circuit board design for the LED display was pulled from my [MAX7219 7 segment LED driver board project](https://github.com/jerwil/MAX7219_Hello_World).

### Hardware

* ATMega328
* DS 1307 Real Time Clock
* Coin battery holder
* 220Ω resistors
* 10kΩ resistors
* 100Ω resistor
* 10 mF capacitors
* 16 mHz oscillator for ATMega328
* Buzzer
* Buttons with built-in LEDs
* Power jack
* Body/housing
* Male and female headers for stacking design

![](https://raw.githubusercontent.com/jerwil/Alarm_Clock_Simon/master/Media/IMG_20140727_231136121_HDR.jpg "Fully assembled alarm clock")

### Schematics

![](https://raw.githubusercontent.com/jerwil/Alarm_Clock_Simon/master/Media/Alarm_Clock_Simon_Schematic_Stack_pcb.png "Alarm Clock Schematic")

See the included Fritzing diagram (.fzz file) for details.

### License

<a rel="license" href="http://creativecommons.org/licenses/by-nc/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc/4.0/88x31.png" /></a><br /><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">Memory Game Alarm Clock</span> by <a xmlns:cc="http://creativecommons.org/ns#" href="http://www.JeremyAdamWilson.com">Jeremy Wilson</a> is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc/4.0/">Creative Commons Attribution-NonCommercial 4.0 International License</a>.


