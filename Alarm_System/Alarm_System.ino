/*
* Oladeji Sanyaolu (2025-2-06)
*
* Integration of the keypad to
* the Basic Alarm system
*
*/

// Libraries to use
#include <Password.h>
#include <Keypad.h>


// Pin Numbers
const int LED = 13;
const int BUZZER = 9;
const int ON_OFF = 3;
const int ENTRY_EXIT = 50;
const int ZONE_ONE = 48;


// Time constants
const int FIVE_SEC = 5000;

// Zone states
int EntryExitState;
int OnOffState;
int ZoneOneState;

// Variables for my_delay()
int x = 0;
int period = 1000;
unsigned long time_now = 0;

// Defining the Password function
Password password = Password("1234");

/**************
* Defining the Keypad Function
***************/
const byte ROWS = 4; // four rows
const byte COLS = 4; // four columns

char keys[ROWS][COLS] = { // Define the Keymap
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'} // (*) is to validate password // (#) is to reset password attempt
};

byte rowPins[ROWS] = {36, 34, 32, 30}; //{9, 8, 7, 6}; // connect to the row pinouts of the keypad (Keypad ROW0, ROW1, ROW2, and ROW3)
byte colPins[COLS] = {28, 26, 24, 22}; //{5, 4, 3, 2}; // connect to the column pinouts of the keypad (Keypad COL0, COL1, COL2, COL3)

// Create the Keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
/**************
* End of defining the Keypad Function
***************/


// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);

  // Initialize digital pin LED_BUILTIN as an output.
  //pinMode(LED_BUILTIN, OUTPUT);

  // Set the LED as an output
  pinMode(LED, OUTPUT);

  // Set the buzzer as an output
  pinMode(BUZZER, OUTPUT);

  // Set the given pins to be an input pull-up
  pinMode(ON_OFF, INPUT_PULLUP);
  pinMode(ENTRY_EXIT, INPUT_PULLUP);
  pinMode(ZONE_ONE, INPUT_PULLUP);

  // Have the keypad listen for input
  keypad.addEventListener(keypadEvent);

  // Turn off the LED
  digitalWrite(LED, LOW);
}

// the loop function runs over and over again forever
void loop() {
  read_entry_exit();
  Serial.println(EntryExitState);
}

void loops() {
  keypad.getKey();

  if (OnOffState == 1) {
    countdown();
    Serial.println("ALARM ARMED ");
  }

  while(OnOffState == 1) { // The state is set to ON
    read_entry_exit();
    read_zone_1();

    if (EntryExitState == 1) { // Entry/Exit zone has been breached
      Serial.println("TURN off Alarm");
      countdown();
      Serial.println("Entry Exit zone breeched");
      keypad.getKey();

      while(OnOffState == 1) { // The alarm is been triggered
        keypad.getKey();
        flash();
      }
    }

    while(ZoneOneState && OnOffState) { // The alarm has been triggered in Zone 1
      keypad.getKey();
      flash();
      Serial.println("Zone 1 Breached");
    }

  }

  delay(1000);
}

/*
  Flashing with a custom delay function
*/
void flash() {
  digitalWrite(LED, HIGH); // digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
  playBuzzer(BUZZER, 1500, 200);
  my_delay();//delay(1000);
  
  digitalWrite(LED, LOW); // digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
  playBuzzer(BUZZER, 500, 200);
  my_delay();//delay(1000);
}

void my_delay() {
  time_now = millis();

  while(millis() < time_now + period) { // wait approx. (period) ms
    keypad.getKey();
  }
}

/*
  Play Sound
*/
void playBuzzer(int buzzer, int freq, int time) {
  tone(buzzer, freq); // Send (freq) sound signal...
  delay(time); // ...for (time) sec
}


void stopBuzzer(int buzzer) {   
  noTone(buzzer);     // Stop sound...
}

/*
  End of flashing with a custom delay function
*/

// Countdown function
void countdown() {
  for (int z = 10; z > 0; z--) { // Start from 10, down to 0
    Serial.println("Alarm will arm in ");
    Serial.print(z - 1); Serial.println(" seconds");
    delay(1000); // Wait for one second before looping again
  }
}

// Check Password function
void checkPassword() {
  if (password.evaluate()) {
    Serial.println("Success");
    x = !x;
  } else {
    Serial.println("Wrong!");
  }

  password.reset();
}

/**************
* Digital Read States
**************/

// Checks the On/Off state
void read_on_off() {
  OnOffState = digitalRead(ON_OFF);
}

// Checks the Entry/Exit state
void read_entry_exit() {
  EntryExitState = digitalRead(ENTRY_EXIT);
}

void read_zone_1() {
  ZoneOneState = digitalRead(ZONE_ONE);
}

/**************
* End of Digital Read States
**************/



/**************
* Event handler
***************/
void keypadEvent(KeypadEvent keyEvent) {
  switch(keypad.getState()) {
    case PRESSED:
      Serial.print("Pressed: ");
      Serial.print(keyEvent);

      switch(keyEvent) {
        case '*':
          checkPassword();
          break;
        case '#':
          password.reset();
          break;
        default:
          password.append(keyEvent);
          break;
      }
  }
}
/**************
* End of Event handler
***************/
