/*
*
* Code written by Oladeji Sanyaolu (2025-2-06)
*
* Arduino Home Alarm System
*
* Project for BEng Computer Engineer 2nd Year (TUS Athlone)
*
*/

// Libraries to use
#include <Password.h>
#include <Keypad.h>

// For the OLED
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Define the Screen Width, Height - in pixels
#define OLED_SCREEN_WIDTH 128
#define OLED_SCREEN_HEIGHT 64
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define OLED_SCREEN_ADDRESS 0x3C

// Pin Numbers
const int RED_LED = 13;
const int GREEN_LED = 23;
const int BUZZER = 9;
const int ON_OFF_BUTTON = 52;

// const int ENTRY_EXIT = 53; // no longer using buttons for Entry/Exit
const int ENTRY_EXIT_ULTRASONIC_TRIG = 7;
const int ENTRY_EXIT_ULTRASONIC_ECHO = 6;

const int ZONE_ONE_PIR = 48;
#define ZONE_ONE_SOUND A2

const int ZONE_TWO_IR_DIGITAL = 46;
#define ZONE_TWO_TEMP A0

// Time constants
const int FIVE_SEC = 5000;

// Zone states
int EntryExitState;
int OnOffState;

int ZoneOneState;
int ZoneTwoState;

// Boolean to arm alarm
bool alarmArmed = false;

// Variables for ENTRY_EXIT_Ultrasonic
long entry_exit_duration;
float entry_exit_distance;

// Variables for my_delay()
//int period = 1000;
unsigned long time_now = 0;

// Defining the Password function
Password password = Password("1234");

// Defining the OLED function (SSD1306 connected to I2C)
Adafruit_SSD1306 oled(OLED_SCREEN_WIDTH, OLED_SCREEN_HEIGHT, &Wire, OLED_SCREEN_ADDRESS);

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

byte rowPins[ROWS] = {22, 24, 26, 28};//{9, 8, 7, 6}; // connect to the row pinouts of the keypad (Keypad ROW0, ROW1, ROW2, and ROW3)
byte colPins[COLS] = {30, 32, 34, 36}; //{5, 4, 3, 2}; // connect to the column pinouts of the keypad (Keypad COL0, COL1, COL2, COL3)

// Create the Keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
/**************
* End of defining the Keypad Function
***************/


// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);

  //I2CScanner();
  // Initialize digital pin LED_BUILTIN as an output.
  //pinMode(LED_BUILTIN, OUTPUT);

  // Set the LEDs as an output
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  // Set the buzzer as an output
  pinMode(BUZZER, OUTPUT);

  // Set the given pins to be an input pull-up
  pinMode(ON_OFF_BUTTON, INPUT_PULLUP);
  // pinMode(ENTRY_EXIT, INPUT_PULLUP); // no longer using a button for Entry/Exit
  pinMode(ENTRY_EXIT_ULTRASONIC_TRIG, OUTPUT);
  pinMode(ENTRY_EXIT_ULTRASONIC_ECHO, INPUT);

  // Setup for Zone 1
  pinMode(ZONE_ONE_PIR, INPUT);
  pinMode(ZONE_ONE_SOUND, INPUT);

  // Setup for Zone 2
  pinMode(ZONE_TWO_IR_DIGITAL, INPUT);
  pinMode(ZONE_TWO_TEMP, INPUT);

  // Have the keypad listen for input
  keypad.addEventListener(keypadEvent);

  // Turn off the LEDs
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);

  initOLED();
}

// the loop function runs over and over again forever
void loop() {
  keypad.getKey();
  toggleAlarm(); // Check if the alarm has been enabled
  displayOLEDText("Alarm Disabled");

  while(alarmArmed == true) { // The state is set to ON
    keypad.getKey(); // Just in case the home-owner wishes to disarm the alarm
    Serial.println("ALARM_ENABLED"); // For Processing

    // Clear the OLED
    oled.clearDisplay();

    // Check the zones
    read_entry_exit();
    read_zone_1();
    read_zone_2();

    if (EntryExitState == 1) { // Entry/Exit zone has been breached
      displayOLEDText("Entry/Exit zone accessed."); // TODO: Use for OLED
      countdown(10);
      // Serial.println("Entry Exit zone breeched"); // TODO: Use for OLED
      keypad.getKey();

      while(alarmArmed == true) { // The alarm has been triggered
        keypad.getKey();
        flash();

        displayOLEDTextPositon("Entry/Exit Breached!", 10, 10);
        Serial.println("ENTRY_EXIT_BREACH"); // For Processing
      }
    }



    if (ZoneOneState == 1) { // Zone 1 has been breached
      countdown(5);
      keypad.getKey();

      while(alarmArmed == true) { // The alarm has been triggered by Zone 1
        keypad.getKey();
        flash();

        displayOLEDTextPositon("Zone 1 Breached!", 10, 10); // Serial.println("Zone 1 Breached"); // TODO: Use for OLED
        Serial.println("ZONE_1_BREACH"); // For Processing
      }
    }

    if (ZoneTwoState == 1) { // Zone 2 has been breached
      countdown(5);
      keypad.getKey();

      while(alarmArmed == true) { // The alarm has been triggered by Zone 2
        keypad.getKey();
        flash();

        displayOLEDTextPositon("Zone 2 Breached!", 10, 10); // OLED Display
        Serial.println("ZONE_2_BREACH");
      }
    }
  }

  delay(1000);
}


/*
  Flashing with a custom delay function
*/
void flash() {
  changeGreenLED(false); // Make sure the Green LED is off

  changeRedLED(true); // digitalWrite(RED_LED, HIGH); // digitalWrite(LED_BUILTIN, HIGH); // turn the RED LED on (HIGH is the voltage level)
  playBuzzer(BUZZER, 1500, 200);
  custom_delay(200);//delay(1000);
  
  changeRedLED(false); // digitalWrite(RED_LED, LOW); // digitalWrite(LED_BUILTIN, LOW); // turn the RED LED off by making the voltage LOW
  playBuzzer(BUZZER, 500, 200);
  custom_delay(200);//delay(1000);
}

void custom_delay(int period) {
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
  noTone(buzzer); // Stop sound...
}

/*
  End of flashing with a custom delay function
*/

// Countdown function
void countdown(int timer) {
  for (int z = timer; z > 0; z--) { // Start from 10, down to 0
    //Serial.println("Alarm will arm in ");
    displayOLEDTextPositon("Alarm trigger in " + String(z - 1) + " seconds", 10, 10);
    //Serial.print(z - 1); Serial.println(" seconds");
    custom_delay(1000);//delay(1000); // Wait for one second before looping again

    if (alarmArmed == false) // Stop the countdown
      break;
  }
}

// Check Password function
void checkPassword() {
  if (password.evaluate()) {
    Serial.println("KEYPAD_SUCCESS"); // For Processing
    displayOLEDTextPositon("Password Success!", 10, 30);
    alarmArmed = false;
  } else {
    Serial.println("KEYPAD_FAIL"); // For Processing
    displayOLEDTextPositon("Invalid Password!", 10, 30);
    oled.clearDisplay(); // Clear the OLED;
  }

  password.reset();
}

// Turn on/off the LED depending on situation
void changeRedLED(bool on) {
  if (on == true) {
    digitalWrite(RED_LED, HIGH); // turn the RED LED on by making the voltage HIGH
  } else {
    digitalWrite(RED_LED, LOW); // turn the RED LED off by making the voltage LOW

  }
}

void changeGreenLED(bool on) {
  if (on == true) {
    digitalWrite(GREEN_LED, HIGH); // turn the GREEN LED on by making the voltage HIGH
  } else {
    digitalWrite(GREEN_LED, LOW); // turn the GREEN LED off by making the voltage LOW

  }
}

/*
  OLED Functions
*/
void initOLED() {
  // initialise OLED display with address 0x3C for 128x64
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }

  delay(2000); // wait for init
}

// Clears the display and shows text at (0, 10)
void displayOLEDText(String text) {
  oled.clearDisplay(); // clear display

  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0, 10);
  oled.println(text);
  oled.display();
}

// Does not clear display and adds text to a selected coord
void displayOLEDTextPositon(String text, int x, int y) {
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(x, y);
  oled.println(text);
  oled.display();
}

void clearOLED() {
  oled.clearDisplay();
}

// Check if the Alarm has been turned on
void toggleAlarm() {
  read_on_off();

  if (OnOffState == 0)
    alarmArmed = !alarmArmed;

  if (alarmArmed == true) {
    // Serial.println("ALARM ARMED"); // TODO: Use for OLED
    // Serial.println("ALARM_ENABLED"); // For Processing
    changeGreenLED(true); // Green LED indicates the the alarm is armed
  } else {
    // Serial.println("ALARM DISARMED"); // TODO: Use for OLED
    Serial.println("ALARM_DISABLED"); // For Processing
    changeGreenLED(false); // Turn off the Green LED
    stopBuzzer(BUZZER); // Stop the Buzzer

  }
}

/**************
* Digital Read States
**************/

// Checks the On/Off state
void read_on_off() {
  OnOffState = digitalRead(ON_OFF_BUTTON);
}

// Checks the Entry/Exit state
void read_entry_exit() {
  // Clear the trigPin by setting it LOW:
  digitalWrite(ENTRY_EXIT_ULTRASONIC_TRIG, LOW);
  delayMicroseconds(10);

  // Trigger the sensor by setting the trigPin high for 10 microseconds:
  digitalWrite(ENTRY_EXIT_ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ENTRY_EXIT_ULTRASONIC_TRIG, LOW);

  // Read the echoPin, pulseIn() returns the duration (length of the pulse) in microseconds:
  entry_exit_duration = pulseIn(ENTRY_EXIT_ULTRASONIC_ECHO, HIGH);
  // Calculate the distance:
  entry_exit_distance = entry_exit_duration * 0.034 / 2;

  // Print the distance on the Serial Monitor (Ctrl+Shift+M):
  // Serial.print(entry_exit_distance); // Debugging
  // Serial.println(" cm"); // Debugging

  if (entry_exit_distance > 0.00 && entry_exit_distance <= 6.0) { // Someone has potentially breached the entry/exit zone
    Serial.println("ENTRY_EXIT_ULTRASONIC_DETECT");
    EntryExitState = 1;
  } else {
    EntryExitState = 0;
  }

}

void read_zone_1() { // Zone 1 is operated by a PIR and a sound detector
  int pirState = digitalRead(ZONE_ONE_PIR);
  int soundValue = analogRead(ZONE_ONE_SOUND);

  //Serial.print("Sound Value: ");
  //Serial.println(soundValue);

  if (pirState == 1) {
    Serial.println("ZONE_1_PIR_DETECT");
    ZoneOneState = 1;
  } else if (soundValue >= 70) {
    Serial.println("ZONE_1_SOUND_DETECT");
    ZoneOneState = 1;
  } else {
    ZoneOneState = 0;
  }
}

void read_zone_2() { // Zone 2 
  int irTwoState = digitalRead(ZONE_TWO_IR_DIGITAL);
  int tempValue = analogRead(ZONE_TWO_TEMP);

  //Serial.print("Temp. Value: ");
  //Serial.println(tempValue);

  if (irTwoState == 0) {
    Serial.println("ZONE_2_IR_DETECT");
    ZoneTwoState = 1;
  } else if (tempValue >= 1030) {
    Serial.println("ZONE_2_TEMP_DETECT");
    ZoneTwoState = 1;
  } else {
    ZoneTwoState = 0;
  }
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
      Serial.print("Keypad: ");
      Serial.print(keyEvent);
      Serial.println();
      displayOLEDTextPositon("Key: " + String(keyEvent), 10, 20);

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


// I2C Scanner (to find the OLED)
void I2CScanner() {
  Serial.println();
  Serial.println ("I2C scanner. Scanning ...");

  byte count = 0;

  pinMode(12, OUTPUT);

  digitalWrite(12, HIGH);
  Wire.begin();

  for (byte i = 1; i < 120; i++) {
    Wire.beginTransmission(i);

    if (Wire.endTransmission() == 0) {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.println (")");
      count++;
      delay (1);
    } 
  }

  Serial.println("Done.");
  Serial.print("Found ");
  Serial.print(count, DEC);
  Serial.println(" device(s).");

}
