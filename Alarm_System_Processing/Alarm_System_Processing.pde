/*
 * Code written by Oladeji Sanyaolu 2025-2-27
 *
 * Processing Integration for Alarm System TUS
 *
*/

import processing.serial.*; // Importing the serial library to communicate with the Arduino 

Serial port; // Initializing a vairable named 'port' for serial communication

String serialString; // Variable for getting data from the serial communication

// Check if the alarm has been enabled
boolean alarmEnabled = false;

// Check if the zones have been breached
boolean entryExitBreach = false;
boolean zoneOneBreach = false;
boolean zoneTwoBreach = false;


// The three zones
Zone entryExit;
Zone zoneOne;
Zone zoneTwo;

class Zone {
  private int x, y, w, h;
  private int r, g, b, a;
  
  private String name, displayName;
  
  // Constructor
  public Zone(String name, int x, int y, int w, int h) {
    this.name = name;
    this.displayName = name;
    
    this.x = x;
    this.y = y;
    this.w = w;
    this.h = h;
    
    this.r = 200;
    this.g = 200;
    this.b = 200;
    this.a = 255;
  }
  
  // To draw the zone
  public void render() {
    rectMode(CENTER);
    fill(color(r, g, b, a));
    stroke(10);
    rect(x, y, w, h);
    fill(0);
    textAlign(CENTER);
    text(displayName, x, y);
  }
  
  
  /*
    Adjusting the Name
  */
  public void changeName(String name) {
    this.name = name;
  }
  
  public void changeDisplayName(String displayName) {
    this.displayName = displayName;
  }
  
  /*
    Adjusting the Colours
  */
  public void changeRed(int r) {
    this.r = r;
  }
  
  public void changeGreen(int g) {
    this.g = g;
  }
  
  public void changeBlue(int b) {
    this.b = b;
  }
  
  public void changeAlpha(int a) {
    this.a = a;
  }
  
  /*
    Set to a colour
  */
  public void setRed() {
    this.r = 200;
    this.g = 0;
    this.b = 0;
  }
  
  public void setGreen() {
    this.r = 0;
    this.g = 200;
    this.b = 0;
  }
  
  public void setBlue() {
    this.r = 0;
    this.g = 0;
    this.b = 200;
  }
  
  // The default colour
  public void defaultColour() {
    this.r = 200;
    this.g = 200;
    this.b = 200;
    this.a = 255;
  }
  
  public void setColour(int r, int g, int b) {
    this.r = r;
    this.g = g;
    this.b = b;
  }
  
  public void setColour(int r, int g, int b, int a) {
    this.r = r;
    this.g = g;
    this.b = b;
    this.a = a;
  }
   
}

// Setup the project
void setup() {
  size(500, 500); // Size of the serial window
  
  // Instance our zones
  entryExit = new Zone("Entry", 100, 100, 200, 200);
  zoneOne = new Zone("Room 1", 300, 100, 200, 200);
  zoneTwo = new Zone("Room 2", 100, 300, 200, 200);
  
  port = new Serial(this, "/dev/ttyACM0", 9600); // Set the PORT and baud rate according to the Arduino IDE
  port.clear();
  port.bufferUntil('\n'); // Recieving the data from the Arduino IDE
}

// Continuously render and loop
void draw() {
  background(255, 255, 255, 255);
  
  if (alarmEnabled) { // The Alarm has been enabled
    if (entryExitBreach) { // Indicate that the Entry/Exit zone has been breached
      entryExit.setRed();
      zoneOne.setColour(155, 0, 0);
      zoneTwo.setColour(155, 0, 0);
    } else if (zoneOneBreach) { // Indicate that Zone 1 has been breached
       entryExit.setColour(155, 0, 0);
       zoneOne.setRed();
       zoneTwo.setColour(155, 0, 0);
    } else if (zoneTwoBreach) { // Indicate that Zone 2 has been breached
        entryExit.setColour(155, 0, 0);
        zoneOne.setColour(155, 0, 0);
        zoneTwo.setRed();
    } else { // No Zones have been breached but indicate that the alarm has been enabled
      entryExit.setColour(155, 200, 200);
      zoneOne.setColour(155, 200, 200);
      zoneTwo.setColour(155, 200, 200);
    }
  } else { // The alarm is disabled
    entryExit.defaultColour();
    zoneOne.defaultColour();
    zoneTwo.defaultColour();
  }
  
  // Draw the zones
  entryExit.render();
  zoneOne.render();
  zoneTwo.render();
 
  serialStringData();
  text("Serial Data: " + serialString, 390, 490);
}


/*
  Serial Event Handler
*/
void serialStringData() {
  // Read the data coming from the Serial port
  while (port.available() > 0) {
    if (serialString != null) {
      serialString = trim(serialString);
      //if (serialString.equals("ALARM_ENABLED")) {
        //alarmEnabled = true;
      //}
      switch(serialString) {
        case "ALARM_ENABLED":
          alarmEnabled = true;
          break;
        case "ALARM_DISABLED":
          alarmEnabled = false;
          entryExitBreach = false;
          zoneOneBreach = false;
          zoneTwoBreach = false;
          break;
        case "ENTRY_EXIT_BREACH":
          entryExitBreach = true;
          break;
        case "ENTRY_EXIT_SLEEP":
          entryExitBreach = false;
          break;
        case "ZONE_1_BREACH":
          zoneOneBreach = true;
          break;
        case "ZONE_1_SLEEP":
          zoneOneBreach = false;
          break;
        case "ZONE_2_BREACH":
          zoneTwoBreach = true;
          break;
        case "ZONE_2_SLEEP":
          zoneTwoBreach = false;
          break;
        case "KEYPAD_SUCCESS":
          break;
        case "KEYPAD_FAIL":
          break;
        default:
          break;
      }
    }
  }
}

void serialEvent(Serial p) {
  serialString = p.readStringUntil('\n'); // We'll change the background colour based on the data received
  
}
