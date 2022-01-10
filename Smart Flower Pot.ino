#include <LiquidCrystal.h>
#include <stdio.h>
#include <DS1302.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//For LCD Display
bool clockMode=true;  //For showing clock or humidity, True means clock
int counter=0;

//For RTC Module
const int kCePin   = 8;  // Chip Enable
const int kIoPin   = 6;  // Input/Output
const int kSclkPin = 7;  // Serial Clock
DS1302 rtc(kCePin, kIoPin, kSclkPin);


//For Push Button
const int buttonPin = 9;    // the number of the pushbutton pin
const int ledPin = 13;      // the number of the LED pin

int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin


// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0, lastClockTime=0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

const int moisture=A0;

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  pinMode(moisture, INPUT);

//  Uncomment for setting the time and date to rtc module
//  Time t(2019, 11, 29, 11, 14, 50, Time::kFriday);
//  rtc.time(t);

//For Push button
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  // set initial LED state
  digitalWrite(ledPin, ledState);
}


String dayAsString(const Time::Day day) {
  switch (day) {
    case Time::kSunday: return "Sun";
    case Time::kMonday: return "Mon";
    case Time::kTuesday: return "Tue";
    case Time::kWednesday: return "Wed";
    case Time::kThursday: return "Thu";
    case Time::kFriday: return "Fri";
    case Time::kSaturday: return "Sat";
  }
  return "---";
}


void printTime() {
  // Get the current time and date from the chip.
  Time t = rtc.time();

  // Name the day of the week.
  const String day = dayAsString(t.day);

  char buf1[16], buf2[16];
  snprintf(buf1, sizeof(buf1), " %s %02d-%02d-%04d",
           day.c_str(),
           t.date, t.mon, t.yr);
  
  snprintf(buf2, sizeof(buf2),"    %02d:%02d:%02d", t.hr, t.min, t.sec);

  if(t.hr==6 and t.min==0 and t.sec<6)
    ledState=HIGH;
  else if(t.hr==18 and t.min==0 and t.sec<6)
    ledState=LOW;
  
//  Serial.println(buf1,buf2);

  if(counter==0)
    lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(buf1);

  lcd.setCursor(0, 1);
  lcd.print(buf2);
}


void printHumidity()
{
  int humidity=analogRead(moisture)*0.09765625;

  if(counter==0)
    lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Moisture : ");
  lcd.print(humidity);
  lcd.print("%");

  lcd.setCursor(0,1);
  lcd.print("  All Out: ");
  
  if(ledState==HIGH)
    lcd.print("OFF");
  else
    lcd.print("ON ");
}

void loop() {
  if ((millis() - lastClockTime) > 1000){
    if(clockMode)
    {
      printTime();    
    }
    else
    {
      printHumidity();
    }
  
    ++counter;
    if((counter>2 && !clockMode) || (counter>6 && clockMode))
    {
      counter=0;
      clockMode=!clockMode;
    }
    lastClockTime=millis();
  }

//For Push Button
//=========================

// read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        ledState = !ledState;
      }
    }
  }

  // set the LED:
  digitalWrite(ledPin, ledState);

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;

//=========================
  
//  delay(1000);
}
