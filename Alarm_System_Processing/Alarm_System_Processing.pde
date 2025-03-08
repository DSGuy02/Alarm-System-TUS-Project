// Code written by Oladeji Sanyaolu (Alarm System TUS) 2025-2-27

import processing.serial.*; // Importing the serial library to communicate with the Arduino 

Serial port; // Initializing a vairable named 'port' for serial communication

String usbString; // Variable for changing the background colour

float val;

int room1 = 1, room2 = 1, room3 = 1;
int zones[];

Zone zone1;

class Zone {
  private int x, y, w, h;
  private int r, g, b, a;
  
  private String name, displayName;
  
  public Zone(String name, int x, int y, int w, int h) {
    this.name = name;
    this.displayName = name;
    
    this.x = x;
    this.y = y;
    this.w = w;
    this.h = h;
  }
  
  public void render() {
    rectMode(CENTER);
    fill(r, g, b, a);
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
  
  zone1 = new Zone("Entry", 100, 100, 100, 100);
  //port = new Serial(this, "", 9600); // Set the PORT and baud rate according to the Arduino IDE
  //port.clear();
  //port.bufferUntil('\n'); // Recieving the data from the Arduino IDE
}

// Continuously render and loop
void draw() {
  background(255, 255, 255, 1);
  
  zone1.render();
}


/*
  Serial Event Handler
*/
void serialEvent(Serial p) {
  String usbString = p.readStringUntil('\n'); // We'll change the background colour based on the data received
  
  if (usbString != null) {
    usbString = trim(usbString);
    
    int zones[] = int(split(usbString, ','));
    
    if (zones.length == 5) {
      room1 = char(zones[1]);
      room2 = char(zones[2]);
      room3 = char(zones[3]);
    }
  }
}
