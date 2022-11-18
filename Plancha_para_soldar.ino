//Libraries
#include "max6675.h"              
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>    
//Inputs Outputs
int SSR = 3;
int thermoDO = 4;                 //Data pin for MAX6675
int thermoCS = 5;                 //CS pin for MAX6675
int thermoCLK = 6;                //Clock pin for MAX6675
int DT = 8;                       //Data pin for encoder
int CLK = 9;                      //Clock pin for encoder
int but_1 = 11;                   //Button 1 input
int buzzer = 12;                  //Buzzer output pin


MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);  //Start MAX6675 SPI communication
LiquidCrystal_I2C lcd(0x27,20,4);                     //Address could be 0x3f or 0x27 

//Variables
unsigned int millis_before, millis_before_2;
unsigned int millis_now = 0;
int refresh_rate = 1000;
int pid_refresh_rate = 300;
unsigned int seconds = 0;
int running_mode = 0;
int selected_mode = 0;
int max_modes = 1;
bool but_1_state = true;
bool but_2_state  =true;
float pwm_value = 5;
int max_pwm_value = 40;
int min_pwm_value = 2;
float temp_setpoint = 0;
float temperature = 0;

void setup() {
  //Define the OUTPUTs
  pinMode(SSR, OUTPUT);       //Start with the SSR off
  digitalWrite(SSR, LOW);
  pinMode(buzzer, OUTPUT);    //Start with buzzer off
  digitalWrite(buzzer, LOW);

  //Define the INPUTS
  pinMode(DT, INPUT_PULLUP);
  pinMode(CLK, INPUT_PULLUP);
  pinMode(but_1, INPUT_PULLUP);
  pinMode(but_2, INPUT_PULLUP);
  
  Serial.begin(9600);
  lcd.init();                 //Init the LCD
  lcd.backlight();            //Activate backlight     

  millis_before = millis();
  millis_now = millis();
  
}

void loop() { 
  millis_now = millis();
  if(millis_now - millis_before_2 > pid_refresh_rate){    
    millis_before_2 = millis();      
    temperature = thermocouple.readCelsius();
    if(running_mode == 1){       
      if(temperature < 150){
        temp_setpoint = seconds;
        if(temp_setpoint > temperature){
          pwm_value = pwm_value + 0.3;
          if(pwm_value > max_pwm_value){
            pwm_value = max_pwm_value;
          }
        }
        else{
          pwm_value--;
          if(pwm_value < min_pwm_value){
            pwm_value = min_pwm_value;
          }
        }
      }
      else if(temperature > 140){
        max_pwm_value = 60;
        temp_setpoint = 200;
        if(temp_setpoint > temperature){
          pwm_value = pwm_value + 0.3;
          if(pwm_value > max_pwm_value){
            pwm_value = max_pwm_value;
          }
        }
        else{
          pwm_value--;
          if(pwm_value < min_pwm_value){
            pwm_value = min_pwm_value;
          }
        }
      } 
               
      analogWrite(SSR, pwm_value);  //We change the Duty Cycle
        
      if(temperature > 190){
        running_mode = 10;  
        analogWrite(SSR, LOW);     
      }
    }//end of running_mode == 1
    
    if(running_mode == 10){
      lcd.clear();
      lcd.setCursor(0,1);     
      lcd.print("      COMPLETE      ");
      tone(buzzer, 1000, 1000);    
      seconds = 0;              //Reset timer
      running_mode = 11;
      delay(2000);
    }  
  }//end of pid_refresh_rate

  
  
  if(millis_now - millis_before > refresh_rate){
    millis_before = millis();   
    seconds = seconds + (refresh_rate/1000);      //We count time
    Serial.println(temperature);
    
    if(running_mode == 0){    
      digitalWrite(SSR, LOW);   
      lcd.clear();
      lcd.setCursor(0,0);     
      lcd.print("Temp: ");
      lcd.print(temperature,1);
      lcd.setCursor(0,1);     
      lcd.print("SSR: OFF");  
      lcd.setCursor(0,2);     
      lcd.print("SELECTED MODE: ");  
      lcd.print(selected_mode);  
      lcd.setCursor(0,3);     
      lcd.print("NOT RUNNING");     
    }//end of running_mode == 0

    else if(running_mode == 11){ 
      if(temperature < 40){
        running_mode = 0; 
         tone(buzzer, 1000, 100); 
      }
      digitalWrite(SSR, LOW);   
      lcd.clear();
      lcd.setCursor(0,0);     
      lcd.print("Temp: ");
      lcd.print(temperature,1);
      lcd.setCursor(0,1);     
      lcd.print("SSR: OFF");  
      lcd.setCursor(0,2);     
      lcd.print("SELECTED MODE: ");  
      lcd.print(selected_mode);  
      lcd.setCursor(0,3);     
      lcd.print("COOLDOWN");     
    }//end of running_mode == 0
  
      
    else if(running_mode == 1){            
      lcd.clear();
      lcd.setCursor(0,0);     
      lcd.print("Temp: ");
      lcd.print(temperature,1);
      lcd.setCursor(0,1);     
      lcd.print("SSR: ON ");  lcd.print("Time: "); lcd.print(seconds); 
      lcd.setCursor(0,2);     
      lcd.print("SELECTED MODE: ");  
      lcd.print(selected_mode);  
      lcd.setCursor(0,3);     
      lcd.print("RUNNING | PWM: ");  lcd.print(pwm_value,1);
    }//end of running_mode == 1
  }//end of millis_now - millis_before > refresh_rate




  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  if(!digitalRead(but_1) && but_1_state){
    but_1_state = false;
    selected_mode ++;
    millis_before = millis_before-1000;
    if(selected_mode > max_modes){
      selected_mode = 0;
    }
  }
  else if(digitalRead(but_1) && !but_1_state){
    but_1_state = true;
  }
  ///////////////////////////////////////////////////////////////////
  if(!digitalRead(but_1)==1) {
    if(running_mode == 0){
      running_mode = 1; 
    }
    
  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  
}//end of void loop
