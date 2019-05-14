#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h> //allows to communicate with I2C devices
#include <Adafruit_GFX.h> //library for oled
#include <Adafruit_SSD1306.h> //library for oled
#define OLED_RESET LED_BUILTIN
#define buzzer 2 //connecting D4 pin to buzzer

// Replace with your network credentials
char* ssid = "nishchala";
char* password = "12345678";
 
ESP8266WebServer server(80);   //instantiate server at port 80 (http port)
 
String page = "";
String page1="";

// --------------------------------------------------------------------------DetectionCode---------------------------------------------------------

Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

int i =0;

constexpr uint8_t RST_PIN = 5;     // Connecting D1 to RST pin
constexpr uint8_t SS_PIN = 4;     // Connecting D2 to Slave Select pin

//uint8_t is the same as a byte. its shorthand for: a type of unsigned integer of length 8 bits.

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class,rfid is the object
 
MFRC522::MIFARE_Key key; 
 
// Init array that will store new NUID 
byte nuidPICC[4];
//Programmable Interface Controller Chip
 //Non Unique Identification Number
//The setup() function is called when a sketch starts. 
//Use it to initialize variables, pin modes, start using libraries, etc. 
//The setup() function will only run once, after each powerup or reset of the Arduino board.

// ----------------------------------------------------------------------------------------------------------------------------------------
double data;
String pictype="";
String nuid="";
String inhex = "";
String hex= "";
String indec = "";
String dec = "";
String match = "";
String detect = ""; 

void setup(){

  Serial.begin(115200);//Sets the data rate in bits per second for serial data transmission
  WiFi.begin(ssid, password); //begin WiFi connection
  Serial.println("");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
   
  server.on("/", [](){
      page = "<h1>BUS IDENTIFICATION FOR THE BLIND USING RFID SYSTEM</h1><p>PICC type:"+String(pictype)+"</p><p>"+String(detect)+"</p><p>"+String(nuid)+"</p><p>"+String(inhex)+"</p><p>"+String(hex)+"</p><p>"+String(indec)+"</p><p>"+String(dec)+"</p><p>"+String(match)+"</p>";  
    server.send(200, "text/html", page);
  });
  
  server.begin();
Wire.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();

  display.setTextSize(1.6);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("BUS IDENTIFICATION \nFOR THE BLIND \nUSING RFID SYSTEM\n");
  display.setTextSize(1.5);
  display.println("Kindly scan your card");
  display.display();


  
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 
  pinMode(buzzer, OUTPUT); // setting the respective pin to buzzer and setting the mode to output
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF; //reading the key byte by byte?
  }
  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
}

//loop() After creating a setup() function, which initializes and sets the initial values, 
//the loop() function does precisely what its name suggests, and loops consecutively, allowing 
//your program to change and respond. 
 
void loop(){
  
  server.handleClient(); //used to handle incoming http requests
  // Look for new cards
  if ( ! rfid.PICC_IsNewCardPresent())
    return;
 
  // Verify if the NUID has been read
  if ( ! rfid.PICC_ReadCardSerial())
    return;
 
  Serial.print(F("PICC type: ")); //PIC: Peripheral Interface COntroller
  
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));
 pictype = rfid.PICC_GetTypeName(piccType);
  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    pictype = "Your tag is not of type MIFARE Classic.";
    return;
  }
 //taking the uid from the object(named rfid) which is read byte by byte
 
  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3] ) {
    Serial.println(F("A new card has been detected."));
    detect = "A new card has been detected.";
 
    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
   
    Serial.println(F("The NUID tag is:"));
    nuid = "The NUID tag is:";
    Serial.print(F("In hex: "));
    inhex = "In hex: ";
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    Serial.print(F("In dec: "));
    indec = "In dec: ";
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    String str1;
   //checking if the registered rfid is matching with the tag swiped
   if (nuidPICC[0]==9 && nuidPICC[1]==173&& nuidPICC[2]==112 && nuidPICC[3]==96) 
    {
       
      Serial.println("RFID MATCHED!");
      Serial.println("Bus Arrived.");
      Serial.println("Location: DSCE->Majestic \n\n");
       Serial.println(WiFi.localIP());
      match = "RFID MATCHED!------>Bus Arrived------>Location: DSCE->Majestic \n\n";
      digitalWrite(buzzer, HIGH);
  delay(5000);
      digitalWrite(buzzer, LOW);

  }
  else{
 
    Serial.println("RFID MISMATCH!");
    Serial.println("Incorrect Bus Boarded.\n\n");
    match = "RFID MISMATCH!\n Incorrect Bus Boarded.\n\n";
  digitalWrite(buzzer, LOW);  
  delay(2000); 

    }
  }
  else 
  {
    Serial.println(F("Card read previously.\n\n"));
    match = "Card read previously.\n\n";
  }
 
  // Halt PICC
  rfid.PICC_HaltA();
 
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
  
}


void printHex(byte *buffer, byte bufferSize) {
  hex = "";
  for (byte i = 0; i < bufferSize; i++) {
   Serial.print(buffer[i] < 0x10 ? " 0" : " ");
   String temp = buffer[i] < 0x10 ? " 0" : " ";
   hex = hex + temp;
    Serial.print(buffer[i], HEX);
    hex = hex + String(buffer[i], HEX);
  }
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize) {
  dec = "";
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    String temp = buffer[i] < 0x10 ? " 0" : " ";
    dec = dec + temp;
    Serial.print(buffer[i], DEC);
     dec = dec + String(buffer[i], HEX);
  }
}
