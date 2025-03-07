// Code written by Oladeji Sanyaolu (Alarm System TUS) 2025-2-27

import processing.serial.*; // Importing the serial library to communicate with the Arduino 

Serial port; // Initializing a vairable named 'port' for serial communication

String usbString; // Variable for changing the background colour

float val;

int room1 = 1, room2 = 1, room3 = 1;
int zones[];

// Setup the project
void setup() {
  size(500, 500); // Size of the serial window
  
  port = new Serial(this, "", 9600); // Set the PORT and baud rate according to the Arduino IDE
  port.clear();
  port.bufferUntil('\n'); // Recieving the data from the Arduino IDE
}

// Continuously render and loop
void draw() {
  background(255, 255, 255, 1);
  
  // Room 1
  if (room1 == 1) {
    // Initial background colour, when we will open the serial window
    
    rectMode(CENTER);
    fill(0, 255, 0);
    rect(130, 120, 100, 150);
    textAlign(CENTER);
    fill(0);
    text("Room 1", 130, 120);
  
  } else {
    fill(255, 0, 0);
    rectMode(CENTER);
    rect(130, 120, 100, 150);
    textAlign(CENTER);
    fill(0);
    text("Room 1", 130, 120);
  }
  
  // Room 2
  if (room2 == 1) {
    fill(100, 255, 100);
    rectMode(CENTER);
    rect(230, 120, 100, 150);
    
    textAlign(CENTER);
    fill(0);
    text("Room 2", 230, 120);
  } else {
    rectMode(CENTER);
    fill(255, 0, 0);
    rect(230, 120, 100, 150);
    
    textAlign(CENTER);
    fill(0);
    text("Room 2", 230, 120);
  }
  
  // Room 3
  if (room3 == 1) {
    fill(0, 255, 0);
    rectMode(CENTER);
    rect(230, 270, 100, 150);
    textAlign(CENTER);
    fill(0);
    text("Room 3", 230, 270);
  } else {
    fill(255, 0, 0);
    rectMode(CENTER);
    rect(230, 270, 100, 150);
    
    textAlign(CENTER);
    fill(0);
    text("Room 3", 230, 270);
  }
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
