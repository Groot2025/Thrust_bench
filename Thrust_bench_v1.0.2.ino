#include <SPI.h>
#include <SD.h>
#include "RTClib.h"
#include <Adafruit_MLX90614.h>
#include <time.h>
#include "HX711.h"




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
  //String filename = String(now.hour(), DEC) + String(now.minute(), DEC) + String(now.second(), DEC) + ".txt";
  String filename = String(now.hour(), DEC) + String(now.minute(), DEC) + ".txt";
  return filename;
}

void setup()
{
  Serial.begin(9600);

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(-710.f);         // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();


  // SD CARD initialization
  Serial.print("Initializing SD card...");
  pinMode(chip_select_SD, OUTPUT);

  if (!SD.begin(chip_select_SD)) {
    Serial.println("initialization failed!");
    return;
  }

  // RTC initialization
  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  //Date time update
  if (! rtc.isrunning()){
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  // MLX initialization

  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    while (1);
  }
  

  Serial.println("initialization done.");

  FOLDER_NAME = create_folder();
  FILE_NAME = get_filename();

  SD.mkdir(FOLDER_NAME);

  myFile = SD.open(FOLDER_NAME + "/" + FILE_NAME, FILE_WRITE);
  myFilename = FILE_NAME ;

  //myFile.println(THRUST);
  myFile.println("hello2");
  Serial.println(myFilename);

  //Emergency button test
  pinMode(14,INPUT_PULLUP);

}


void loop()
{
   
   
  
  // Reading motor thrust
  THRUST = scale.get_units(5);

  // Reading Temperature from MLX
  Ambient_temp = mlx.readAmbientTempC();
  Object_temp = mlx.readObjectTempC();

  Data_line = get_time() + "\t" + String(Ambient_temp) + "\t" + String(Object_temp) + "\t" + String(THRUST);
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

  if (LOG_FLAG)
  {
    if (!SD.exists(FOLDER_NAME + "/" + File_index + ".txt"))
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

       LOG_FLAG = false;
       
      

      }

    }

  }
}