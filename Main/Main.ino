//RSS CMG Prototype command program
//A script to control the RSS CMG Prototype based on
//symbols recieved from the IR remote.
//Planned behaviour is RW operation, and servo operation.
//Written by Jasper Grant
//Written March 20th 2023 for the RSS CMG Capstone
//Last edited March 23rd 2023

//Include library for IR remote
#include <IRremote.h>

//Define onboard LED pin
#define LED_PIN 13

//Define pin for IR reciever
#define IR_PIN 8

//Define pins for Reaction Wheel
#define RW_ENABLE_PIN 19 //ESCON pin J5-2
#define RW_PWM_PIN 20 //ESCON pin J5-1
//Connect GND to ESCON pin J5-5 and J6-7 (Not 100% sure this is necessary but will not hurt)

//Define pins for servo motor
#define SERVO_PWM_PIN 16

//Button Mappings
#define POWER_BUTTON         69
#define VOLUME_PLUS_BUTTON   70
#define VOLUME_MINUS_BUTTON  21
#define REWIND_BUTTON        68
#define FAST_FORWARD_BUTTON  67    
//Not a button but ensures that a 0 shows up
//as a continued press as opposed to an
//unrecognized key
#define CONTINUED_PRESS       0

//Servo angles
#define SERVO_90_DEG 127
#define SERVO_0_DEG 0
#define SERVO_45_DEG 63
#define SERVO_135_DEG 190
#define INCREMENT 3

//RW speeds
#define FULL_SPEED 255
#define STATIONARY 127

//Global to track current CMG position
byte cmg_pwm = 0;

void setup() {
  //Start Serial
  Serial.begin(9600);
  //Start receiving from IR remote
  IrReceiver.begin(IR_PIN);

  //Keep LED on for debugging purposes
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  //Define output pins for motor control
  pinMode(RW_ENABLE_PIN, OUTPUT);
  pinMode(RW_PWM_PIN, OUTPUT);
  pinMode(SERVO_PWM_PIN, OUTPUT);

}

//Functions for specific buttons on the IR remote

void power_button(){
  Serial.println("Emergency Stop");
  //Turn off RW
  analogWrite(RW_ENABLE_PIN, LOW);
  digitalWrite(RW_PWM_PIN, 127);
}

void volume_plus_button(){
  Serial.println("Turning on CMG rotation");
  //Move gimbal to 90 degrees
  digitalWrite(SERVO_PWM_PIN, SERVO_90_DEG);
  cmg_pwm = SERVO_90_DEG;
  //Wait 5 seconds
  delay(5);
  //Spin up CMG
  analogWrite(RW_ENABLE_PIN, HIGH);
  digitalWrite(RW_PWM_PIN, FULL_SPEED);
}

void volume_minus_button(){
  Serial.println("Turning off CMG rotation");
  //Stop spinning CMG
  analogWrite(RW_ENABLE_PIN, LOW);
  digitalWrite(RW_PWM_PIN, STATIONARY);
  //Move gimbal to starting positin
  digitalWrite(SERVO_PWM_PIN, SERVO_0_DEG);
  cmg_pwm = SERVO_0_DEG;
}

void rewind_button(){
  Serial.println("Turning CMG CCW");
  //If angle is greater than 45 degrees
  if(cmg_pwm > SERVO_45_DEG){
    //Decrement angle variable
    cmg_pwm-=INCREMENT;
    //Write this decremented angle to CMG
    digitalWrite(SERVO_PWM_PIN, cmg_pwm);
  }
}

void fast_forward_button(){
  Serial.println("Turning CMG CW");
  //If angle is less than 135 degrees
  if(cmg_pwm < SERVO_135_DEG){
    //Increment angle variable
    cmg_pwm+=INCREMENT;
    //Write this incremented angle to CMG
    digitalWrite(SERVO_PWM_PIN, cmg_pwm);
  }
}

void unrecognized_key(){
  ;
}

//Main decoding loop

void loop() {
  //Decode once
  if(IrReceiver.decode()){
    //Go back to listening
    IrReceiver.resume();
    //Decode command
    int command = IrReceiver.decodedIRData.command;
    //if(command == 0) command = last_command;
    
    //Switch to deal with different buttons
    //Each button is mapped to a function
    //Unmapped buttons will have no effect
    switch(command){
      case POWER_BUTTON:
        power_button();
        break;
      case VOLUME_PLUS_BUTTON:
        volume_plus_button();
        break;
      case VOLUME_MINUS_BUTTON:
        volume_minus_button();
        break;
      case REWIND_BUTTON:
        rewind_button();
        break;
      case FAST_FORWARD_BUTTON:
        fast_forward_button();
        break;
      default:
        unrecognized_key();
    }
  }
}