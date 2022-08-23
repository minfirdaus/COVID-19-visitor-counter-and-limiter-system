#include <Servo.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(12,11,5,4,3,2); 
Servo servo1;
const unsigned int NOTE_C4 = 262;
const unsigned int NOTE_E4 = 330;
const unsigned int NOTE_G4 = 392;

const int PIR1_PIN = 8;
const int PIR2_PIN = 9;
const int PIEZO_PIN = 7;
const bool SERIAL_PRINT = true;
int inputval=0;

int visitors = 0;
int lastRIPdetected = 0;
bool b_PIR1_active = false;
bool b_PIR2_active = false;

void setup()  {
  lcd.begin(16,2);
  servo1.attach(10);
  pinMode(A2,INPUT);
  pinMode(PIR1_PIN, INPUT);
  pinMode(PIR2_PIN, INPUT);
  pinMode(PIEZO_PIN, OUTPUT);
  
  
  Serial.begin(9600); 
  DisplayMsg("Visitor counter", "Welcome", SERIAL_PRINT);
}

// -------- Message display on LCD and Serial (optional) ------
void DisplayMsg(String s1, String s2, bool ab_serial_print) {

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(s1);

  lcd.setCursor(0, 1);  
  lcd.print(s2);
  
  if (ab_serial_print) 
    Serial.println(s1 + ". " + s2);
} 


// -------- Increase/Decrease visitors ---------
void UpdateVisitorsCounter(int x){ 
int duration=100;
  
  visitors = visitors + x;
  
  lastRIPdetected = 0;  // reset detected PIR
  
  if (x>0)
    PlayNote(NOTE_G4, duration);
  else
    PlayNote(NOTE_C4, duration);
    	
}

// -------- Play note ---------
void PlayNote(int note, int duration) {
  tone(PIEZO_PIN, note, duration);
  delay(duration * 1.3);
  noTone(PIEZO_PIN);
}


void loop() {    
  inputval=analogRead(A2);
  
  // ----------- check PIR1 ----------------
  if (digitalRead(PIR1_PIN) == HIGH) {
    
    if ( !b_PIR1_active ) {
      b_PIR1_active = true;
      
      if (lastRIPdetected == 0 && !b_PIR2_active && inputval<358 && inputval > 20 && visitors <= 2) {  // new start
        lastRIPdetected = 1;    
 
        DisplayMsg("In PIR1", "Visit started", SERIAL_PRINT);
        servo1.write(0);

      } else if (lastRIPdetected == 2) { // if we were in PIR2 before
        UpdateVisitorsCounter(-1);  
      
        DisplayMsg("Visitor exited", "Visitors:" + String(visitors), SERIAL_PRINT);
        servo1.write(90);
      } 
      else if (visitors>2){
        DisplayMsg("Too much visitors", "Please wait", SERIAL_PRINT);
      }
    }
  }else 
    b_PIR1_active = false ;  // reenable PIR1
  	
  
  // ----------- check PIR2 ----------------  
  if (digitalRead(PIR2_PIN) == HIGH ) {
    
    if ( !b_PIR2_active ) {
      b_PIR2_active = true;
      
      if (lastRIPdetected == 0 && !b_PIR1_active) { // new start
        if (visitors > 0) {
          lastRIPdetected = 2;   

          DisplayMsg("In PIR2", "Exit started", SERIAL_PRINT);
          servo1.write(0);
        } else 
          DisplayMsg("No more visitors", "to exit", SERIAL_PRINT);
                
      } else if (lastRIPdetected == 1) {  // if we were in PIR1 before
        UpdateVisitorsCounter(1);      

        DisplayMsg("Visitor entered", "Visitors:" + String(visitors), SERIAL_PRINT);
        servo1.write(90);
      }
    }
  } else 
    b_PIR2_active = false; // reenable PIR2
  Serial.println (analogRead(A2));
}
