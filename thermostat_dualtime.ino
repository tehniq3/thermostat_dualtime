// thermostat for day/night - 25.10.2019 
// based on Dawn & Dusk controller. http://andydoz.blogspot.ro/2014_08_01_archive.html
// 16th August 2014. (C) A.G.Doswell 2014
// adapted sketch by niq_ro from http://nicuflorica.blogspot.ro & http://arduinotehniq.blogspot.com/

// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "RTClib.h" // from https://github.com/adafruit/RTClib
#include <LiquidCrystal_I2C.h>
#include <Encoder.h> // from http://www.pjrc.com/teensy/td_libs_Encoder.html
#include <EEPROM.h>

#define buton 4   // pin for pushbutton
#define buton1 2   // pin for pushbutton
#define buton2 3   // pin for pushbutton
#define releu 5   // pin for relay
#define ONE_WIRE_BUS 10  // pin for DS19B20

RTC_DS1307 RTC; // Tells the RTC library that we're using a DS1307 RTC
Encoder knob(buton1, buton2); //encoder connected to pins bla & bla (and ground)
LiquidCrystal_I2C lcd(0x27,16,2); // I used an odd pin combination because I need pin 2 and 3 for the interrupts.

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

//the variables provide the holding values for the set clock routine

int setyeartemp; 
int setmonthtemp;
int setdaytemp;
int sethourstemp;
int setminstemp;
int setsecs = 0;
int maxday; // maximum number of days in the given month

int sethourstempday;
int settemptempday;
int sethourstempnight;
int settemptempnight;
int settemphisteresys;

// These variables are for the push button routine
int buttonstate = 0; //flag to see if the button has been pressed, used internal on the subroutine only
int pushlengthset = 3000; // value for a long push in mS
int pushlength = pushlengthset; // set default pushlength
int pushstart = 0;// sets default push value for the button going low
int pushstop = 0;// sets the default value for when the button goes back high

int knobval; // value for the rotation of the knob
boolean buttonflag = false; // default value for the button flag

int temperatura = 0;
unsigned long timpcitire = 0;
unsigned long pauzacitire = 10000;  // 10 secunde

int temperaturalimita =0;

void setup () { 
    Serial.begin(9600); //start debug serial interface
    Wire.begin(); //start I2C interface
    RTC.begin(); //start RTC interface
 
    lcd.begin();
    lcd.backlight();
    lcd.clear(); 
    pinMode(buton,INPUT);//push button on encoder connected to A0 (and GND)
    digitalWrite(buton,HIGH); //Pull A0 high
    pinMode(releu,OUTPUT); //Relay connected to A3
    digitalWrite (releu, HIGH); //sets relay off (default condition)
    
    //Checks to see if the RTC is runnning, and if not, sets the time to the time this sketch was compiled.
    if (! RTC.isrunning()) {
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }

sethourstempday = EEPROM.read(1);
settemptempday = EEPROM.read(2);
sethourstempnight = EEPROM.read(3);
settemptempnight = EEPROM.read(4);
settemphisteresys = EEPROM.read(5);

 if (sethourstempday < 1) {
      sethourstempday = 0;
    }
 if (sethourstempday > 23) {
      sethourstempday = 23;
    }
 if (settemptempday < 40) {
      settemptempday = 40;
    }
 if (settemptempday > 70) {
      settemptempday = 70;
    }
 if (sethourstempnight < 1) {
      sethourstempnight = 0;
    }
 if (sethourstempnight > 23) {
      sethourstempnight = 23;
    }
 if (settemptempnight < 40) {
      settemptempnight = 40;
    }
 if (settemptempnight > 70) {
      settemptempnight = 70;
    }
 if (settemphisteresys < 1) {
      settemphisteresys = 1;
    }
 if (settemphisteresys > 5) {
      settemphisteresys = 5;
    }
    
// Start up the library
sensors.begin();
Serial.print("Requesting temperatures...");
sensors.requestTemperatures(); // Send the command to get temperatures
Serial.println("DONE");
Serial.print("Temperature for the device 1 (index 0) is: ");
temperatura = sensors.getTempCByIndex(0);
Serial.println(temperatura);  
timpcitire = millis(); 
}  // end setup
           

void loop () {

 if (timpcitire - millis() > pauzacitire)
{
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  Serial.print("Temperature for the device 1 (index 0) is: ");
  temperatura = sensors.getTempCByIndex(0);
  Serial.println(temperatura);  
  timpcitire = millis();
}

    DateTime now = RTC.now(); //get time from RTC
    //Display current time
    lcd.setCursor (0,0);
    if (now.day() <10) 
      {
        lcd.print("0");
      }
    lcd.print(now.day(), DEC);
    lcd.print('/');
    if (now.month() <10) 
      {
        lcd.print("0");
      }
    lcd.print(now.month());
    lcd.print('/');
    lcd.print(now.year(), DEC);
    lcd.print(" ");
    lcd.setCursor (0,1);
    if (now.hour() <10) 
      {
        lcd.print("0");
      }
    lcd.print(now.hour(), DEC);
    lcd.print(':');
    if (now.minute() <10) 
      {
        lcd.print("0");
      }
    lcd.print(now.minute(), DEC);
  if (millis()%500 == 0) lcd.print(':');
    else lcd.print(' ');
    if (now.second() <10) 
      {
        lcd.print("0");
      }
    lcd.print(now.second());
//    lcd.print("     ");

if ((now.hour() >= sethourstempday) and (now.hour() < sethourstempnight)) // day
{
  temperaturalimita = settemptempday;
  lcd.setCursor (15,0);
  lcd.print("*");
}
else  // night
{
  temperaturalimita = settemptempnight;
  lcd.setCursor (15,0);
  lcd.print("/");
}

lcd.setCursor(11,0);
lcd.print(temperatura);
lcd.write(0b11011111);
lcd.print("C");
lcd.setCursor(11,1);
lcd.print(temperaturalimita);
lcd.write(0b11011111);
lcd.print("C");

if (temperatura > temperaturalimita)
{
  digitalWrite(releu, HIGH);
  lcd.setCursor(15,1);
  lcd.print("*");
}
if (temperatura < temperaturalimita - settemphisteresys)
{
  digitalWrite(releu, LOW);
lcd.setCursor(15,0);
lcd.print(" ");
}

    pushlength = pushlengthset;
    pushlength = getpushlength ();
    delay (10);
    
    if (pushlength <pushlengthset) {     
      ShortPush ();   
    }
    
       
       //This runs the setclock routine if the knob is pushed for a long time
       if (pushlength > pushlengthset) {
         lcd.clear();
         DateTime now = RTC.now();
         setyeartemp=now.year(),DEC;
         setmonthtemp=now.month(),DEC;
         setdaytemp=now.day(),DEC;
         sethourstemp=now.hour(),DEC;
         setminstemp=now.minute(),DEC;
         setclock();
         pushlength = pushlengthset;
       };
delay(100);       
} // end main loop

//sets the clock
void setclock (){
   setyear ();
   lcd.clear ();
   setmonth ();
   lcd.clear ();
   setday ();
   lcd.clear ();
   sethours ();
   lcd.clear ();
   setmins ();
   lcd.clear();
   sethoursday ();
   lcd.clear ();
   settempday ();
   lcd.clear();
   sethoursnight ();
   lcd.clear ();
   settempnight ();
   lcd.clear();
   sethisteresys ();
   lcd.clear();   
  
   RTC.adjust(DateTime(setyeartemp,setmonthtemp,setdaytemp,sethourstemp,setminstemp,setsecs));
   EEPROM.write(1, sethourstempday);
   EEPROM.write(2, settemptempday); 
   EEPROM.write(3, sethourstempnight);
   EEPROM.write(4, settemptempnight);
   EEPROM.write(5, settemphisteresys);
   
   delay (1000); 
}

// subroutine to return the length of the button push.
int getpushlength () {
  buttonstate = digitalRead(buton);  
       if(buttonstate == LOW && buttonflag==false) {     
              pushstart = millis();
              buttonflag = true;
          };
          
       if (buttonstate == HIGH && buttonflag==true) {
         pushstop = millis ();
         pushlength = pushstop - pushstart;
         buttonflag = false;
       };
       return pushlength;
}

// The following subroutines set the individual clock parameters
int setyear () {
//lcd.clear();
    lcd.setCursor (0,0);
    lcd.print ("Set Year");
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return setyeartemp;
    }

    lcd.setCursor (0,1);
    knob.write(0);
    delay (50);
    knobval=knob.read();
    if (knobval < -1) { //bit of software de-bounce
      knobval = -1;
    }
    if (knobval > 1) {
      knobval = 1;
    }
    setyeartemp=setyeartemp + knobval;
    if (setyeartemp < 2019) { //Year can't be older than currently, it's not a time machine.
      setyeartemp = 2019;
    }
    lcd.print (setyeartemp);
    lcd.print("  "); 
    setyear();
}
  
int setmonth () {
//lcd.clear();
   lcd.setCursor (0,0);
    lcd.print ("Set Month");
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return setmonthtemp;
    }

    lcd.setCursor (0,1);
    knob.write(0);
    delay (50);
    knobval=knob.read();
    if (knobval < -1) {
      knobval = -1;
    }
    if (knobval > 1) {
      knobval = 1;
    }
    setmonthtemp=setmonthtemp + knobval;
    if (setmonthtemp < 1) {// month must be between 1 and 12
      setmonthtemp = 1;
    }
    if (setmonthtemp > 12) {
      setmonthtemp=12;
    }
    lcd.print (setmonthtemp);
    lcd.print("  "); 
    setmonth();
}

int setday () {
  if (setmonthtemp == 4 || setmonthtemp == 5 || setmonthtemp == 9 || setmonthtemp == 11) { //30 days hath September, April June and November
    maxday = 30;
  }
  else {
  maxday = 31; //... all the others have 31
  }
  if (setmonthtemp ==2 && setyeartemp % 4 ==0) { //... Except February alone, and that has 28 days clear, and 29 in a leap year.
    maxday = 29;
  }
  if (setmonthtemp ==2 && setyeartemp % 4 !=0) {
    maxday = 28;
  }
//lcd.clear();  
   lcd.setCursor (0,0);
    lcd.print ("Set Day");
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return setdaytemp;
    }

    lcd.setCursor (0,1);
    knob.write(0);
    delay (50);
    knobval=knob.read();
    if (knobval < -1) {
      knobval = -1;
    }
    if (knobval > 1) {
      knobval = 1;
    }
    setdaytemp=setdaytemp+ knobval;
    if (setdaytemp < 1) {
      setdaytemp = 1;
    }
    if (setdaytemp > maxday) {
      setdaytemp = maxday;
    }
    lcd.print (setdaytemp);
    lcd.print("  "); 
    setday();
}

int sethours () {
//lcd.clear();
    lcd.setCursor (0,0);
    lcd.print ("Set Hours");
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return sethourstemp;
    }

    lcd.setCursor (0,1);
    knob.write(0);
    delay (50);
    knobval=knob.read();
    if (knobval < -1) {
      knobval = -1;
    }
    if (knobval > 1) {
      knobval = 1;
    }
    sethourstemp=sethourstemp + knobval;
    if (sethourstemp < 1) {
      sethourstemp = 0;
    }
    if (sethourstemp > 23) {
      sethourstemp=23;
    }
    lcd.print (sethourstemp);
    lcd.print("  "); 
    sethours();
}

int setmins () {
//lcd.clear();
   lcd.setCursor (0,0);
    lcd.print ("Set Mins");
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return setminstemp;
    }

    lcd.setCursor (0,1);
    knob.write(0);
    delay (50);
    knobval=knob.read();
    if (knobval < -1) {
      knobval = -1;
    }
    if (knobval > 1) {
      knobval = 1;
    }
    setminstemp=setminstemp + knobval;
    if (setminstemp < 0) {
      setminstemp = 0;
    }
    if (setminstemp > 59) {
      setminstemp=59;
    }
    lcd.print (setminstemp);
    lcd.print("  "); 
    setmins();
}

int sethoursday () {
//lcd.clear();
    lcd.setCursor (0,0);
    lcd.print ("Day: Start Hours");
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return sethourstempday;
    }

    lcd.setCursor (0,1);
    knob.write(0);
    delay (50);
    knobval=knob.read();
    if (knobval < -1) {
      knobval = -1;
    }
    if (knobval > 1) {
      knobval = 1;
    }
    sethourstempday = sethourstempday + knobval;
    if (sethourstempday < 1) {
      sethourstempday = 0;
    }
    if (sethourstempday > 23) {
      sethourstempday = 23;
    }
    lcd.print (sethourstempday);
    lcd.print("  "); 
    sethoursday();
}

int settempday () {
//lcd.clear();
   lcd.setCursor (0,0);
    lcd.print ("Day: Set Temp.");
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return settemptempday;
    }

    lcd.setCursor (0,1);
    knob.write(0);
    delay (50);
    knobval=knob.read();
    if (knobval < -1) {
      knobval = -1;
    }
    if (knobval > 1) {
      knobval = 1;
    }
    settemptempday = settemptempday + knobval;
    if (settemptempday < 40) {
      settemptempday = 40;
    }
    if (settemptempday > 70) {
      settemptempday = 70;
    }
    lcd.print (settemptempday);
    lcd.write(0b11011111);
    lcd.print("C");
    lcd.print("  "); 
    settempday();
}

int sethoursnight () {
//lcd.clear();
    lcd.setCursor (0,0);
    lcd.print ("Night: Set Hours");
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return sethourstempnight;
    }

    lcd.setCursor (0,1);
    knob.write(0);
    delay (50);
    knobval=knob.read();
    if (knobval < -1) {
      knobval = -1;
    }
    if (knobval > 1) {
      knobval = 1;
    }
    sethourstempnight = sethourstempnight + knobval;
    if (sethourstempnight < 1) {
      sethourstempnight = 0;
    }
    if (sethourstempnight > 23) {
      sethourstempnight = 23;
    }
    lcd.print (sethourstempnight);
    lcd.print("  "); 
    sethoursnight();
}

int settempnight () {
//lcd.clear();
   lcd.setCursor (0,0);
    lcd.print ("Night: Set Temp.");
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return settemptempnight;
    }

    lcd.setCursor (0,1);
    knob.write(0);
    delay (50);
    knobval=knob.read();
    if (knobval < -1) {
      knobval = -1;
    }
    if (knobval > 1) {
      knobval = 1;
    }
    settemptempnight = settemptempnight + knobval;
    if (settemptempnight < 40) {
      settemptempnight = 40;
    }
    if (settemptempnight > 70) {
      settemptempnight = 70;
    }
    lcd.print (settemptempnight);
    lcd.write(0b11011111);
    lcd.print("C");
    lcd.print("  "); 
    settempnight();
}

int sethisteresys () {
//lcd.clear();
   lcd.setCursor (0,0);
    lcd.print ("Histeresys");
    pushlength = pushlengthset;
    pushlength = getpushlength ();
    if (pushlength != pushlengthset) {
      return settemphisteresys;
    }

    lcd.setCursor (0,1);
    knob.write(0);
    delay (50);
    knobval=knob.read();
    if (knobval < -1) {
      knobval = -1;
    }
    if (knobval > 1) {
      knobval = 1;
    }
    settemphisteresys = settemphisteresys + knobval;
    if (settemphisteresys < 1) {
      settemphisteresys = 1;
    }
    if (settemphisteresys > 5) {
      settemphisteresys = 5;
    }
    lcd.print (settemphisteresys);
    lcd.write(0b11011111);
    lcd.print("C");
    lcd.print("  "); 
    sethisteresys();
}


void ShortPush () {
  //This displays the calculated sunrise and sunset times when the knob is pushed for a short time.
lcd.clear();
for (long Counter = 0; Counter < 604 ; Counter ++) { //returns to the main loop if it's been run 604 times 
                                                     //(don't ask me why I've set 604,it seemed like a good number)
  lcd.setCursor (1,0);
  lcd.print ("made by niq_ro");
  lcd.setCursor (1,1);
  lcd.print ("(Nicu FLORICA)");
    
  //If the knob is pushed again, enter the mode set menu
  pushlength = pushlengthset;
  pushlength = getpushlength ();
  if (pushlength != pushlengthset) {
    lcd.clear ();
    lcd.print("new push?");
  }
  
}
lcd.clear();
}
