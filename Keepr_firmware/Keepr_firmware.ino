/*
* Title: Keepr firmware v01
*
* Author: Simon
* 
* Date: Aug 23 2021
*
*/
#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/power.h>

#define potentiometer_pin A0 // reads the potentiomenter input
#define battery_voltage_pin A1 // reads the battery level voltage
#define battery_low_pin 2 // connects to rgb led to show when battery is low
#define enable_pin 3  // should be a pwm pin 
#define in1A_pin 4 
#define in2A_pin 5

float pot_val = 0;
int mapped_pot_val = 0;

float calibration_voltage = 4.029; // Voltage when battery is fully charged
float R1 = 20000;
float R2 = 10000;
float voltage_raw_val = 0.00;
float voltage_out = 0.00;
float battery_voltage = 0.00;
float correction_factor = 0.13;
float charge_ok = 3.3; // 20% - 100%
float alert = 3.1;


void setup() {
  Serial.begin(9600);
  Serial.println("Keepr Version 1 tests!");
  // Initializes the pins as Outputs 
  pinMode(battery_low_pin, OUTPUT);
  pinMode(enable_pin, OUTPUT);
  pinMode(in1A_pin, OUTPUT);
  pinMode(in2A_pin, OUTPUT);
  digitalWrite(battery_low_pin,HIGH); // ensures bat low indicator is off at start
}

void loop() {
power_management(); // Invoked Function puts software based measures to optimize powwer consumption
motor_control(); // function controls the motor based on pot input
}

void take_readings(){
  pot_val = analogRead(potentiometer_pin);
  mapped_pot_val = map(potentiometer_pin, 0,1023, 0, 255);
  Serial.print("Raw Pot Val is:");
  Serial.println(pot_val);
  Serial.print("Mapped Pot Val is:");
  Serial.println(pot_val);
}

void motor_control(){
  take_readings();
  digitalWrite(in1A_pin, HIGH);
  digitalWrite(in2A_pin, LOW);
  analogWrite(enable_pin, mapped_pot_val);
}


void check_battery_voltage(){
      voltage_raw_val = analogRead(battery_voltage_pin);
      voltage_out = (voltage_raw_val * 5) / 1024; 
      battery_voltage = voltage_out / (R2/(R1+ R2)) - correction_factor;
      Serial.print("Measured Voltage is: ");
      Serial.print(battery_voltage);
      Serial.println("V");
  }

// shows battery is low 
void check_battery_low(){
  check_battery_voltage(); 
    if(battery_voltage <= alert){
        digitalWrite(battery_low_pin,LOW); // Red led stays on!
      } else {
        digitalWrite(battery_low_pin,HIGH); // Red led stays Off!
      }
}

void power_management()
{
  // turn off brown-out enable in software
  MCUCR = bit(BODS) | bit(BODSE);
  MCUCR = bit(BODS);

  interrupts(); // guarantees next instruction executed
 
  // Turns off I2C
  power_twi_disable(); // TWI (I2C)

  // Turns off unused timers
  power_timer1_disable(); // Timer 1

    // turn off brown-out enable in software
  MCUCR = bit(BODS) | bit(BODSE);
  MCUCR = bit(BODS);
  sleep_cpu();

  // cancel sleep as a precaution
  sleep_disable();
  interrupts(); // guarantees next instruction executed
}
