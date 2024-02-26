#include <AFMotor.h>
#include <Stepper.h> //Include the Arduino Stepper Library
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
// Number of steps per internal motor revolution for motors 2 and 3
const float STEPS_PER_REV = 200; 
//  Amount of Gear Reduction
const float GEAR_RED = 4;
// Number of steps per geared output rotation
const float STEPS_PER_OUT_REV = STEPS_PER_REV * GEAR_RED;
 //*****************Variable Definition**************************//
int StepsRequired;
int count;
int data_count;
float babyWeight=0;
int syringeVolume = 15;
int totalBloodVolume;
int numberOfCycles;
int minutesPerCycle;
int number_of_steps;
int number_of_revs;
float rpmDraw;
float rpmDeliver;
char key;
char Data[3];
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
int getSyringeVolume();
 float ExchangeTransfusion();
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'.','0','#','D'}
};
byte rowPins[ROWS] = {A3, A2, A1,A0}; //connect to the row pinouts of the keypad
byte colPins[COLS] = { 13, 12,11,10}; //connect to the column pinouts of the keypad                                                                                                                                                    
////Create an object of keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
// Steps per revolution for bipolar stepper motors
const int stepsPerRevolution= 200;
AF_Stepper motor1(stepsPerRevolution, 2);      //connect bipolar motor to port #1 (M1 and M2)

Stepper motor3(STEPS_PER_REV, 8, 7, 6, 5);  //connect unipolar stepper motor 1 to digital pins 30 to 33
Stepper motor2(STEPS_PER_REV, 5, 3, 2, 1);  //connect unipolar stepper motor 2 to digital pins 34 to 37

//LiquidCrystal_I2C lcd(0x3F,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27,20,4);
int data= 2;



  // put your setup code here, to run once:
void setup() {
  
  Serial.begin(9600);
  lcd.init();                      // initialize the lcd 
  lcd.init();
  lcd.backlight();
  lcd.setCursor(5,0);
  lcd.print("WELCOME");
  delay(4000);
  lcd.setCursor(5,0);
  lcd.print("ANET 4.0");
  delay(4000);
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("AMEYAW ");
  delay(2000);
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("BANDOMA ");
  delay(2000);
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("and CANN");
  delay(4000); 
  lcd.clear();
  lcd.setCursor(0, 0); 
  lcd.print("Input weight of");
  lcd.setCursor(0, 1); 
  lcd.print("baby in kg:");
}
    


void loop() {
  // put your main code here, to run repeatedly:

  //Prompt user to input weight of baby in kg
 do{
    key = keypad.getKey();
    if(key){
      // Press key
      Data[data_count]='\0';
      Data[data_count] =  key;// store char into data array
      lcd.setCursor(12 + data_count, 1); // move cursor to show each new char
      lcd.print(Data[data_count]); // print char at said cursor
      data_count++;
   }
  } while(data_count<3);
   
babyWeight=atof(Data);
getSyringeVolume();
ExchangeTransfusion();
}  
//Function for getting Syringe Volume
int getSyringeVolume(){
  if(babyWeight < 1.5){
    lcd.clear();
    lcd.setCursor(0, 0); 
    lcd.print("Use 5ml syringe");
    delay(5000);
    syringeVolume = 5;
    number_of_steps = 800;
  }
else if(babyWeight>=1.5 & babyWeight <= 2.5){
  lcd.clear();
    lcd.setCursor(0, 0); 
    lcd.print("Use 10ml syringe");
    delay(5000);
  syringeVolume = 10;
  number_of_steps = 1600;
}
else if(babyWeight>2.5 & babyWeight <= 3.5){
   lcd.clear();
    lcd.setCursor(0, 0); 
    lcd.print("Use 15ml syringe");
    delay(5000);
  syringeVolume = 15;
  number_of_steps = 2400;
 }
else{
   lcd.clear();
    lcd.setCursor(0, 0); 
    lcd.print("Use 20ml syringe");
    delay(5000);
  syringeVolume = 20;
  number_of_steps = 3200;
}
}
//***************Function for exchange transfusion*************************//
float ExchangeTransfusion(){
  //Pre-transfusion calculations
   float totalBloodVolume = babyWeight * 160;
   int numberOfCycles = ceil(totalBloodVolume/syringeVolume);
   float minutesPerCycle = 120/numberOfCycles;
   float minsForDrawing = ceil(0.4167*minutesPerCycle);     
   //Minutes for drawing blood is 5/12 of the total minutes per cycle
   float minsForDelivering = ceil(0.5833*minutesPerCycle);  
   //Minutes for drawing blood is 7/12 of the total minutes per cycle
   float number_of_revs = ceil(number_of_steps/200);
    int rpmDraw = number_of_revs / minsForDrawing;
    int rpmDeliver = number_of_revs / minsForDelivering;
    //Confirmation of transfusion details
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Total volume for");   //print to LCD
    lcd.setCursor(0,1);
    lcd.print("exchange:" );
    lcd.print(totalBloodVolume);
    lcd.print("ml");  
    delay(5000);
lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Number of cycles:");
    lcd.setCursor(0,1);
    lcd.print(numberOfCycles);
    delay(5000);
  for(count = 1; count <= numberOfCycles; count++ ) {
  

     //STEP 1, DRAWING BLOOD FROM THE BABY
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Drawing blood");
  lcd.setCursor(0,1);
  lcd.print("from baby");
  lcd.setCursor(10,1);
  lcd.print("C" + String(count) + "/C" + String(numberOfCycles));
  motor1.setSpeed(rpmDraw); // calculated rpm 
  motor1.step(number_of_steps, BACKWARD, DOUBLE);  
   //main motor moves backwards to draw blood from baby

  //STEP 2, DISCARDING BLOOD INTO WASTE BAG
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Discarding blood");
  lcd.setCursor(9,1);
  lcd.print("C" + String(count) + "/C" + String(numberOfCycles));
 // Motor2 rotates in a clockwise direction to open valve to waste bag
  StepsRequired  =  -STEPS_PER_OUT_REV / 3.5;   
  motor2.setSpeed(1000);  
  motor2.step(StepsRequired);
  motor1.setSpeed(35);  // 10 rpm 
  motor1.step(number_of_steps, FORWARD, DOUBLE);  //main motor moves backwards to draw blood from baby

  //STEP 3, DRAWING FRESH BLOOD FROM DONOR BAG
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Drawing fresh");
  lcd.setCursor(0,1);
  lcd.print("blood");
  lcd.setCursor(8,1);
  lcd.print("C" + String(count) + "/C" + String(numberOfCycles));
  StepsRequired  =  STEPS_PER_OUT_REV / 3.5;   
  // Motor2 rotates in a counter clockwise direction to close valve to waste bag
  motor2.setSpeed(1000);  
  motor2.step(StepsRequired);
  StepsRequired  =  -STEPS_PER_OUT_REV / 3.5;    
  // // Motor3 rotates in a clockwise direction to open valve to waste bag
  motor3.setSpeed(1000);  
  motor3.step(StepsRequired);
  delay(200);
  
   motor1.setSpeed(35);  // 10 rpm 
   motor1.step(number_of_steps, BACKWARD, DOUBLE);  
   //main motor moves backwards to draw blood from baby


  //STEP 4, DELIVERING FRESH BLOOD TO BABY
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Delivering blood");
  lcd.setCursor(0,1);
  lcd.print("to baby");
  lcd.setCursor(10,1);
  lcd.print("C" + String(count) + "/C" + String(numberOfCycles));
  StepsRequired  =  STEPS_PER_OUT_REV / 3.5;   
  motor3.setSpeed(1000);  
  motor3.step(StepsRequired); // Motor3 rotates in a counter clockwise direction to close valve to donor bag
  motor1.setSpeed(rpmDeliver);  // 10 rpm 
  motor1.step(number_of_steps, FORWARD, DOUBLE);  //main motor moves backwards to draw blood from baby
//Prompt user
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Numb of complete");
  lcd.setCursor(0,1);
  lcd.print("cycles:");
  lcd.setCursor(8,1);
  lcd.print( String(count) + "/" + String(numberOfCycles));
    delay(5000);  
}
//Completion message
lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Transfusion ");
  lcd.setCursor(0,1);
  lcd.print("complete");
  lcd.setCursor(9,1);
  lcd.print(count);
    delay(5000); 
}
