/*
  Nyan Cat Lightup Project 
   for Zach & Sally
    - love knitcore
    
  Designed for an Arduino Pro Mini 5V ATMega328
 */
 
// PWM Lights
int pin_star1  = 5;
int pin_star2  = 6;
int pin_star3  = 9;

// Audio Trigger In
int pin_audiotrigger_in = 10;
// Audio Trigger Out
int pin_audiotrigger_out = 11;


unsigned long trigger_time;
int stfu;
int stfu_end;
int mlight;


// Setup Values
int star1_delay = 0;
int star1_value = 0;
bool star1_up = true;
int star2_delay = 0;
int star2_value = 0;
bool star2_up = true;
int star3_delay = 0;
int star3_value = 0;
bool star3_up = true;

void setup() {
  // Only for Debugging if you need it
  Serial.begin(57600);
  
  // initialize light pins - All out
  pinMode(pin_star1, OUTPUT);
  pinMode(pin_star2, OUTPUT);
  pinMode(pin_star3, OUTPUT);
  
  // Audio Pins
  pinMode(pin_audiotrigger_out, OUTPUT);
  pinMode(pin_audiotrigger_in, INPUT_PULLUP);
  digitalWrite(pin_audiotrigger_out, HIGH);
  trigger_time = 0;
  stfu = 0;
  stfu_end = 200; // This will get a random value after each setting
  mlight = 0;
  
  // if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));
  
  // Random Starting delays
  star1_delay = random(0, 500);
  star2_delay = random(0, 500);
  star3_delay = random(0, 500);
  
}

// the loop function runs over and over again forever
void loop() {
  
  // PWM Fade Lights
  if(trigger_time > 0) {
    music_lights();
  } else {
    pwm_star(pin_star1, star1_delay, star1_value, star1_up);
    pwm_star(pin_star2, star2_delay, star2_value, star2_up);
    pwm_star(pin_star3, star3_delay, star3_value, star3_up);
  }
  
  //Check Audio
  check_audio_trigger();
  
  //Debugging if you need this
  //   Serial.println(trigger_time);
  
  //Cause this ran crazy fast, slow shit down
  delay(10);              
}

//////////////////////////////////////////////////
//////////////////////////////////////////////////
//      LIGHT DURING MUSIC                      //
//////////////////////////////////////////////////
//////////////////////////////////////////////////

void music_lights(){
  // Only using the 3 PWM lights for this
  mlight++;
  if(mlight < 20){
    analogWrite(pin_star1, 250);
    analogWrite(pin_star2, 0);
    analogWrite(pin_star3, 0);    
  } else if(mlight < 40){
    analogWrite(pin_star1, 0);
    analogWrite(pin_star2, 250);
    analogWrite(pin_star3, 0);    
  } else if(mlight < 60){
    analogWrite(pin_star1, 0);
    analogWrite(pin_star2, 0);
    analogWrite(pin_star3, 250);    
  } else {
    mlight = 0;
  }
}


//////////////////////////////////////////////////
//////////////////////////////////////////////////
//      Audio                                   //
//////////////////////////////////////////////////
//////////////////////////////////////////////////

void check_audio_trigger(){
  // Force a delay if we just shut down the audio
  if(stfu > 0){
    stfu++;
    if(stfu > stfu_end){
      stfu = 0;
    }  
  }
  
  // This is the main button detection to start the music
  if((digitalRead(pin_audiotrigger_in) == LOW) && (trigger_time < 1) && (stfu == 0)){
    digitalWrite(pin_audiotrigger_out,LOW);
    trigger_time = 1;
    Serial.println("Triggering Music");
  }
  
  if(trigger_time > 0){
    trigger_time++;
    
    if(trigger_time > 300){
      // Ok, if they hit it again now, lets turn it off
      if(digitalRead(pin_audiotrigger_in) == LOW){
        digitalWrite(pin_audiotrigger_out,HIGH);
        trigger_time = 0;
        stfu = 1;
        stfu_end = random(70, 80);
        
        Serial.println("STFU Rnd: ");
        Serial.println(stfu_end);
      }
      
      // Damn, still going? Lets shut this down
      // This should end as the music fades out around 3:30
      if(trigger_time > 21000){
        digitalWrite(pin_audiotrigger_out,HIGH);
        trigger_time = 0;   
        Serial.println("Long Stop");     
      }

    }
  }
  
}


//////////////////////////////////////////////////
//////////////////////////////////////////////////
//      FADE STARS                              //
//////////////////////////////////////////////////
//////////////////////////////////////////////////

void pwm_star(int &pin, int &delay, int &value, bool &up){
  if(delay > 0){
    delay--;
    return;
  }
  
  if(up){
    value++;
  } else {
    value--;
  }

  if(value < 1){
    up = true;
    analogWrite(pin, 0);
    
    // Set the delay between fades
    delay = random(100, 500);
    return;
  }
  if(value > 250){
    up = false;
  }
  
  analogWrite(pin, value);
}
