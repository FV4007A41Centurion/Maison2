// Include Particle Device OS APIs
#include "Particle.h"
// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);


 
// Define the symbols on the buttons of the keypad
char keys[12] = {'*', '7', '4', '1', '0', '8', '5', '2', '#', '9', '6', '3'};
char keypress;
char keyPressed;
// Save the correct password
char passWord[] = {'7', '3', '5', '5', '6', '0', '8'};
bool InputAvailable = false;
bool next = true;
bool waiting = true;
// setup() runs once, when the device is first turned on
void setup() {
  // Put initialization like pinMode and begin functions here
   Serial.begin(9600);    // Initialize the serial port and set the baud rate to 9600
   pinMode(A2, INPUT_PULLUP);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
   static char keyIn[7];    // Save the input characters
  static byte keyInNum = 0; // Save the number of input characters
int clavout = analogRead(A2);

if(next == true && waiting){
Serial.println("Waiting for input");
waiting = false;
} //imprimer attente input

    if(clavout >= 0 && clavout <500 && next){
    keypress = '*';
    next = false;
    } //*

    else if(clavout >= 500 && clavout <600 && next){
    keypress = '7';
    next = false;
    } //7

    else if(clavout >= 600 && clavout <800 && next){
    keypress = '4';
    next = false;
    } //4

    else if(clavout >= 800 && clavout <900 && next){
    keypress = '1';
    next = false;
    } //1

    else if(clavout >= 900 && clavout <1100 && next){
    keypress = '0';
    next = false;
    } //0

    else if(clavout >= 1100 && clavout <1400 && next){
    keypress = '8';
    next = false;
    } //8

    else if(clavout >= 1400 && clavout <1900 && next){
    keypress = '5';
    next = false;
    } //5

    else if(clavout >= 1900 && clavout <2100 && next){
    keypress = '2';
    next = false;
    } //2

    else if(clavout >= 2100 && clavout <2500 && next){
    //keypress = '#';
    next = false;
    } //#

    else if(clavout >= 2500 && clavout <2890 && next){
    keypress = '9';
    next = false;
    } //9

    else if(clavout >= 2890 && clavout <2950 && next){
    keypress = '6';
    next = false;
    } //6

    else if(clavout >= 2950 && clavout <3100 && next){
    keypress = '3';
    next = false;
    } //3

    else if(clavout >=3100 && next == false){
        next = true;
        waiting = true;
    } 
 keyPressed = keypress;
 keypress = '\0';
//Serial.print("Input available: ");
//Serial.println(InputAvailable);
  if (keyPressed) {
    // If a key is pressed, print it on the LCD and serial monitor
    Serial.print("Key Pressed: ");
    Serial.println(keyPressed);
    // Save the input characters
    keyIn[keyInNum++] = keyPressed;
 
    // If the user has input 4 characters
    if (keyInNum == 7) {
      bool isRight = true;  // Flag to check if the password is correct
      for (int i = 0; i < 7; i++) {
        if (keyIn[i] != passWord[i]) {
          isRight = false;  // Set flag to false if password is incorrect
        }
      }
      // If the password is correct, print YES, otherwise print wrong
      if (isRight) {
        Serial.println("Password Correct");
        
        keyInNum = 0;
        memset(keyIn, '\0', sizeof(keyIn));
        delay(1000);

      } else {
        Serial.println("Password Incorrect");

       // Reset the input for the next password entry
       keyInNum = 0;
        memset(keyIn, '\0', sizeof(keyIn));  // Clear the keyIn array

       delay(1000);  // Wait for 2 seconds before clearing the LCD
       Serial.println("Try Again");
      }
 
    }
  }
}