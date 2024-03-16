#include <SPI.h>
#include <SD.h>
#include "RTClib.h"
#include <Adafruit_MLX90614.h>
#include <time.h>
#include "HX711.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define VIN A3 // define the Arduino pin A0 as voltage input (V in)
const float VCC   = 5.0;// supply voltage 5V or 3.3V. If using PCB, set to 5V only.
const int model = 0;   // enter the model (see below)

float cutOffLimit = 1.00;// reading cutt off current. 1.00 is 1 Amper


float sensitivity[] ={
          40.0,// for ACS758LCB-050B
          60.0,// for ACS758LCB-050U
          20.0,// for ACS758LCB-100B
          40.0,// for ACS758LCB-100U
          13.3,// for ACS758KCB-150B
          16.7,// for ACS758KCB-150U
          10.0,// for ACS758ECB-200B
          20.0,// for ACS758ECB-200U     
         }; 
float quiescent_Output_voltage [] ={
          0.5,// for ACS758LCB-050B
          0.12,// for ACS758LCB-050U
          0.5,// for ACS758LCB-100B
          0.12,// for ACS758LCB-100U
          0.5,// for ACS758KCB-150B
          0.12,// for ACS758KCB-150U
          0.5,// for ACS758ECB-200B
          0.12,// for ACS758ECB-200U            
          };
const float FACTOR = sensitivity[model]/1000;// set sensitivity for selected model
const float QOV =   quiescent_Output_voltage [model] * VCC;// set quiescent Output voltage for selected model
float voltage;// internal variable for voltage
float cutOff = FACTOR/cutOffLimit;// convert current cut off to mV




bool LOG_FLAG = false;
int File_index = 0;
//int myFilename = 0;


Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 5;
const int LOADCELL_SCK_PIN = 4;

HX711 scale;
float THRUST;


File myFile;
String myFilename;

RTC_DS1307 rtc;
//RTC_DS3231 rtc;
String FOLDER_NAME;
String FILE_NAME;

String Data_line;
const int chip_select_SD = 53;


float Ambient_temp;
float Object_temp;


//--------------------------------DISPLAY BITMAP--------------------------------------------------//


// 'airbotix_technology_logo(1)', 128x32px
const unsigned char epd_bitmap_airbotix_technology_logo_1_ [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x38, 0x42, 0xe0, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xfe, 0x0f, 0xfc, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0x03, 0xfc, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x07, 0xff, 0xff, 0xfe, 0x0f, 0xfc, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x01, 0xff, 0xff, 0xf8, 0x3f, 0xfc, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xe1, 0xff, 0xfc, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, 0x1f, 0xf3, 0xfc, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf0, 0xff, 0x8f, 0xfc, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x01, 0xc0, 0x00, 0x0e, 0x0f, 0xf8, 0x7f, 0xfc, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x03, 0xe0, 0x00, 0x01, 0xff, 0x83, 0xff, 0xfc, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xff, 0x80, 0x00, 0x0f, 0xf8, 0x00, 0x3f, 0xf8, 0x3f, 0xff, 0xfc, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x1f, 0xfc, 0x07, 0xff, 0x03, 0xff, 0xff, 0xfc, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x7f, 0xc1, 0xff, 0xe0, 0x1f, 0xff, 0xff, 0xfc, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xf8, 0x00, 0x00, 0xf0, 0x7f, 0xfc, 0x00, 0x1f, 0xff, 0xff, 0xfc, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x1f, 0xff, 0x00, 0x00, 0x07, 0xff, 0xff, 0xfc, 0x00, 
	0x00, 0x1f, 0xef, 0xff, 0xe0, 0x00, 0x1f, 0xff, 0x80, 0x20, 0x00, 0x03, 0xff, 0xff, 0xfc, 0x00, 
	0x00, 0x1f, 0xc1, 0xff, 0x80, 0x3f, 0xff, 0xc0, 0x0f, 0xf8, 0x00, 0x00, 0xff, 0xff, 0xfc, 0x00, 
	0x00, 0x1f, 0xfe, 0x07, 0xff, 0xff, 0xe0, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xff, 0x80, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 
	0x00, 0x1f, 0xff, 0xf8, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 
	0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 528)
const int epd_bitmap_allArray_LEN = 1;
const unsigned char* epd_bitmap_allArray[1] = {
	epd_bitmap_airbotix_technology_logo_1_
};



String create_folder()
{
  DateTime now = rtc.now();
  String foldername = String (now.year(), DEC) + String (now.month(), DEC) + String (now.day(), DEC);
  return foldername;
}

String get_time()
{
  DateTime now = rtc.now();
  String RTC_TIME = String(now.hour(), DEC) + ":" + String(now.minute(), DEC) + ":" + String(now.second(), DEC);
  return RTC_TIME;

}

String get_filename()
{
  DateTime now = rtc.now();
  String filename = String(now.hour(), DEC) + String(now.minute(), DEC) + String(now.second(), DEC) + ".txt";
  //String filename = String(now.hour(), DEC) + String(now.minute(), DEC) + ".txt";
  return filename;
}

void setup()
{
  Serial.begin(9600);
  //-----------------Display Setup WITH LOGO-------------------------------------//
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();

  display.drawBitmap(0,0,epd_bitmap_airbotix_technology_logo_1_,128,32,WHITE);
  display.display();
  delay(4000);
  display.clearDisplay();
  display.display();
  


//------------------------LOAD CELL SETUP---------------------------------//

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(-710.f);         // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();


  // SD CARD initialization
  Serial.print("Initializing SD card...");
  display.setTextSize(1);
  display.setCursor(0, 9);
  display.setTextColor(WHITE);
  display.print("Initializing SD-Card");
  display.display();
  //delay(3000);
  //display.clearDisplay();
  //display.display();
  pinMode(chip_select_SD, OUTPUT);

  if (!SD.begin(chip_select_SD)) {
    Serial.println("initialization failed!");
    display.setTextSize(1);
    display.setCursor(0, 20);
    display.setTextColor(WHITE);
    display.print("Insert SD-card");
    display.display();
    delay(10000);
    display.clearDisplay();
    display.display();
    return;
  }

  // RTC initialization
  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    display.clearDisplay();
    display.display();
    display.setTextSize(1);
    display.setCursor(0, 9);
    display.setTextColor(WHITE);
    display.print("RTC not found");
    display.setCursor(0, 16);
    display.println("Reset after connecting RTC");
    display.display();
    //delay(10000);
    //display.clearDisplay();
    //display.display();
    while (1);
  }
  //Date time update
  if (! rtc.isrunning()){
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  // MLX initialization

  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    display.clearDisplay();
    delay(1000);
    display.display();
    display.setTextSize(1);
    display.setCursor(0, 9);
    display.setTextColor(WHITE);
    display.print("MLX Sensor not found");
    display.display();
    //delay(20000);
    //display.clearDisplay();
    //display.display();
    while (1);
  }
  

  Serial.println("initialization done.");
    //display.clearDisplay();
    delay(1000);
    display.display();
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.setTextColor(WHITE);
    display.print("Initialization done");
    display.display();
    delay(3000);

  FOLDER_NAME = create_folder();
  FILE_NAME = get_filename();

  SD.mkdir(FOLDER_NAME);

  myFile = SD.open(FOLDER_NAME + "/" + FILE_NAME, FILE_WRITE);
  myFilename = FILE_NAME ;

  //myFile.println(THRUST);
  //myFile.println("hello2");
  //Serial.println(myFilename);

  //Emergency button test
  pinMode(14,INPUT_PULLUP);

    display.clearDisplay();
    display.display();


    display.setCursor(0, 9);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.print("Start motors");
    display.display(); 

}



void loop()
{

   display.setCursor(0, 0);
   display.setTextColor(WHITE);
   display.setTextSize(1);
   display.print("Thrust Bench V1.0.4");
   display.display();

  //-------------------Current Reading---------------------------//

  float voltage_raw =   (5.0 / 1023.0)* analogRead(VIN);// Read the voltage from sensor
  voltage =  voltage_raw - QOV + 0.007 ;// 0.007 is a value to make voltage zero when there is no current
  float current = voltage / FACTOR;
  if(abs(voltage) > cutOff ){
    Serial.print("V: ");
    Serial.print(voltage,3);// print voltage with 3 decimal places
    Serial.print("V, I: ");
    Serial.print(current,2); // print the current with 2 decimal places
    Serial.println("A");
  }
  /*}else{
    //Serial.println("No Current");
    
  }*/
  delay(500);
  
  // Reading motor thrust
  THRUST = scale.get_units(5);

  // Reading Temperature from MLX
  Ambient_temp = mlx.readAmbientTempC();
  Object_temp = mlx.readObjectTempC();

  Data_line = get_time() + "\t" + String(Ambient_temp) + "\t" + String(Object_temp) + "\t" + String(THRUST) + "\t" + String(current,2); 
  Serial.println(Data_line);

  if (THRUST >= 10 && THRUST < 500)
  {
    LOG_FLAG = true;
    //myFile = SD.open(FOLDER_NAME + "/" + FILE_NAME, FILE_WRITE);
  }
  else if (THRUST < 10 )
  {
    //myFile.close();
  }
      
  /*if (!LOG_FLAG){
    display.setCursor(0, 9);
         display.setTextColor(WHITE);
         display.setTextSize(1);
         display.print("Start motors");
         display.display();  
  }*/

  if (LOG_FLAG)
  {
    if (!SD.exists(FOLDER_NAME))
    {
      Serial.println("File does not exists!!!");
    }
    else
    {
      int button_state = digitalRead(14);

      if (button_state == HIGH) {
      // close the file:
         
         Serial.println("NORMAL");
         Serial.print("logging");
         myFile.println(Data_line);
         Serial.println(myFilename); 
         display.clearDisplay(); 
         display.display();
         display.setCursor(0, 9);
         display.setTextColor(WHITE);
         display.setTextSize(1);
         display.print("Logging in");
         display.setCursor(0, 16);
         display.print(myFilename);
         display.display();       
        /* Serial.println("Ambient_temp");
         Serial.print(Ambient_temp);
         Serial.println("Object_temp");
         Serial.print(Object_temp);
         Serial.println("Thrust");
         Serial.print(THRUST);*/
         //myFile.println(THRUST);
         //delay(100);

      } 
      if (button_state == LOW){


       //Serial.println("EMERGENCY");
       myFile.close();
       Serial.println("done writing.");
       display.clearDisplay();
       display.display();
       display.setCursor(0, 9);
       display.setTextColor(WHITE);
       display.setTextSize(1);
       display.print("done writing in");
       display.setCursor(0, 16);
       display.print(myFilename);
       display.setCursor(0, 24);
       display.print("Reset for new file");
       display.display();  

       LOG_FLAG = false;
       
      

      }

    }

  }
}