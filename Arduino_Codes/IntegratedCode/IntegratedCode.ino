#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10  
SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
const int rs = 5, en = 4, d4 = 6, d5 = 7, d6 = A5, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);// Configurable, see typical pin layout above
const byte ROWS = 2; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'2','3','A'},
  {'5','6','B'},
};
byte rowPins[ROWS] = {A0, A1}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {A2, A3, A4}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
byte deduction;

MFRC522::MIFARE_Key key;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!Serial);
  SPI.begin();
  mfrc522.PCD_Init();
  lcd.begin(16,2);

  for (byte i=0;i<6;i++){
      key.keyByte[i] = 0xFF;
    }
  
     finger.begin(57600);
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }
  Serial.println("Choose Mode");
  lcd.setCursor(0,0);
  lcd.print("A: Payment Mode");
  Serial.println("A: Payment Mode");
  lcd.setCursor(0,1);
  lcd.print("B: Recharge Mode");
  Serial.println("B: Recharge Mode");
  //dump_byte_array(key.keyByte,MFRC522::MF_KEY_SIZE);
  Serial.println();
}

void loop() {
  // put your main code here, to run repeatedly:
  char button = keypad.getKey();

  if (button){
      
      if(button == 'A'){
        lcd.clear();
        lcd.print("Payment Mode");
        Serial.println("Chosen Payment Mode");
        delay(1500);
        lcd.clear();
        lcd.print("Scan the card");
        Serial.println("Waiting for card...");
        //delay(1000);
       //if ( ! mfrc522.PICC_IsNewCardPresent())
         // return;
          while (!mfrc522.PICC_IsNewCardPresent());
          //Serial.println("Card is read");
  
      // Select one of the cards
      if ( ! mfrc522.PICC_ReadCardSerial())
          return;

       
  
      // Show some details of the PICC (that is: the tag/card)
     // Serial.print(F("Card UID:"));
      //dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
      Serial.println();
     // Serial.print(F("PICC type: "));
      MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
     // Serial.println(mfrc522.PICC_GetTypeName(piccType));
  
    if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
          &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
          &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
          Serial.println(F("This sample only works with MIFARE Classic cards."));
          return;
      }
        
        //---------------Scan General vs Student Card--------------
        MFRC522::StatusCode status;
        byte buffer[18];
        byte size = sizeof(buffer);
        int AuthAddr = 15;
        int BlockAddr = 12;
        int CheckSector = 3;
         // Authenticate using key A
        status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,AuthAddr , &key, &(mfrc522.uid));
        if (status != MFRC522::STATUS_OK) {
            Serial.print(F("PCD_Authenticate() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
            return;
        }
       // Authenticate using key B
        status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, AuthAddr, &key, &(mfrc522.uid));
        if (status != MFRC522::STATUS_OK) {
            Serial.print(F("PCD_Authenticate() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
            return;
        }

        // Read Student or Gen Public
         status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(BlockAddr, buffer, &size);
        if (status != MFRC522::STATUS_OK) {
            Serial.print(F("MIFARE_Read() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
        }
        if(buffer[0] == 1){
          lcd.clear();
          lcd.print("Student Card");
          Serial.println("Student Card");
          delay(1500);
          //-----------------Fingerprint Begin---------------------
          lcd.clear();
          lcd.print("Scan Fingerprint");
          Serial.print("Waiting for fingerprint...");
          Serial.println();
          
          
            while(! getFingerprint());
          
          //-----------------Fingerprint End-----------------------
          delay(1500);
          lcd.clear();
          lcd.print("Student Number");
          char NumberofStudents;
          while(! (NumberofStudents = keypad.getKey()));
          
          Serial.println(NumberofStudents);
          if(NumberofStudents == '2' || NumberofStudents == '3' || NumberofStudents == '5' || NumberofStudents == '6'){
          deduction = 10*(NumberofStudents-48);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Students: ");
          lcd.print(NumberofStudents);
          Serial.print("Students: ");
          Serial.println(NumberofStudents);
          lcd.setCursor(0,1);
          lcd.print("Deduction: ");
          lcd.print(deduction);
          Serial.print("Deduction: ");
          Serial.println(deduction);
          }
          else{
          lcd.clear();
          lcd.print("Invalid Input");
          Serial.println("Invalid Input");
          }
          
          
          
          }else{
            lcd.clear();
            lcd.print("General Card");
            Serial.println("General Card");
            delay(1000);
            lcd.clear();
            lcd.print("People: ");
            char NumberofPeople;
            while(! (NumberofPeople = keypad.getKey()));
            
            Serial.println(NumberofPeople);
            if(NumberofPeople == '2' || NumberofPeople == '3' || NumberofPeople == '5' || NumberofPeople == '6'){
            deduction = 20*(NumberofPeople-48);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("People: ");
            lcd.print(NumberofPeople);
            Serial.print("People: ");
            Serial.println(NumberofPeople);
            lcd.setCursor(0,1);
            lcd.print("Deduction: ");
            lcd.print(deduction);
            Serial.print("Deduction: ");
            Serial.println(deduction);
            }
            else{
            lcd.clear();
            lcd.print("Invalid Input");
            Serial.println("Invalid Input");
            }
            
            }
        //---------------Scan General Vs Student Card End----------

        //---------------Deduction Start---------------------------
        byte MoneyBlockAddr = 8;
        byte MoneySector = 2;
        byte MoneyAuthAddr = 11;
        byte MoneyBlock[16]={0};

        status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, MoneyAuthAddr, &key, &(mfrc522.uid));
        if (status != MFRC522::STATUS_OK) {
            Serial.print(F("PCD_Authenticate() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Error Reading");
            lcd.setCursor(0,1);
            lcd.print("Card. Try again");
            return;
        }
        
     
        status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, MoneyAuthAddr, &key, &(mfrc522.uid));
        if (status != MFRC522::STATUS_OK) {
            Serial.print(F("PCD_Authenticate() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Error Reading");
            lcd.setCursor(0,1);
            lcd.print("Card. Try again");
            return;
        }
        
        status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(MoneyBlockAddr, buffer, &size);
        if (status != MFRC522::STATUS_OK) {
            Serial.print(F("MIFARE_Read() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Error Reading");
            lcd.setCursor(0,1);
            lcd.print("Card. Try again");
        }
        if(buffer[0] >=deduction){
        MoneyBlock[0] = buffer[0]-deduction;
        status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(MoneyBlockAddr, MoneyBlock, 16);
        if (status != MFRC522::STATUS_OK) {
            Serial.print(F("MIFARE_Write() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Error Reading");
            lcd.setCursor(0,1);
            lcd.print("Card. Try again");
        }else{
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Deduced: ");
            lcd.print(deduction);
            Serial.print("Deduced: ");
            Serial.println(deduction);
            lcd.setCursor(0,1);
            lcd.print("Remaining: ");
            lcd.print(buffer[0]-deduction);
            Serial.print("Remaining: ");
            Serial.println(buffer[0]-deduction);
            
          }
          }else{
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Insufficient");
            lcd.setCursor(0,1);
            lcd.print("Money");
            Serial.print("Insufficient Money");
            Serial.println();
            }
            delay(2000);
        
        
        //---------------Deduction End-----------------------------
        // Halt PICC
            mfrc522.PICC_HaltA();
            // Stop encryption on PCD
            mfrc522.PCD_StopCrypto1();
            Serial.println("Choose Mode");
            lcd.setCursor(0,0);
            lcd.print("A: Payment Mode");
            Serial.println("A: Payment Mode");
            lcd.setCursor(0,1);
            lcd.print("B: Recharge Mode");
            Serial.println("B: Recharge Mode");
            //dump_byte_array(key.keyByte,MFRC522::MF_KEY_SIZE);
            Serial.println();

        
        }else if(button == 'B'){
          Serial.println("Chosen Recharge Mode");
          lcd.clear();
          lcd.print("Recharge Mode");
          delay(1500);
          lcd.clear();
          lcd.print("Scan the card");
          MFRC522::StatusCode status;
          byte buffer[18];
          byte size = sizeof(buffer);


        while (!mfrc522.PICC_IsNewCardPresent());
          //Serial.println("Card is read");
  
          // Select one of the cards
          if ( ! mfrc522.PICC_ReadCardSerial())
              return;
//          if ( ! mfrc522.PICC_IsNewCardPresent())
//          return;
//  
//        // Select one of the cards
//        if ( ! mfrc522.PICC_ReadCardSerial())
//            return;
    
        // Show some details of the PICC (that is: the tag/card)
        //Serial.print(F("Card UID:"));
        //dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
        Serial.println();
       // Serial.print(F("PICC type: "));
        MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
       // Serial.println(mfrc522.PICC_GetTypeName(piccType));
    
      if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
            &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
            &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
            Serial.println(F("This sample only works with MIFARE Classic cards."));
            return;
        }
          
           
          //----------------------Read Current Balance----------------------

          byte MoneyBlockAddr = 8;
          byte MoneySector = 2;
          byte MoneyAuthAddr = 11;
          byte MoneyBlock[16]={0};

          status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, MoneyAuthAddr, &key, &(mfrc522.uid));
        if (status != MFRC522::STATUS_OK) {
            Serial.print(F("PCD_Authenticate() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Error Reading");
            lcd.setCursor(0,1);
            lcd.print("Card. Try again");
            return;
        }
        
     
        status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, MoneyAuthAddr, &key, &(mfrc522.uid));
        if (status != MFRC522::STATUS_OK) {
            Serial.print(F("PCD_Authenticate() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Error Reading");
            lcd.setCursor(0,1);
            lcd.print("Card. Try again");
            return;
        }
        status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(MoneyBlockAddr, buffer, &size);
        if (status != MFRC522::STATUS_OK) {
            Serial.print(F("MIFARE_Read() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Error Reading");
            lcd.setCursor(0,1);
            lcd.print("Card. Try again");
        }
          //----------------------Read Current Balance End------------------

          //----------------------Update Balance Start----------------------

          int recharge = 250;
          Serial.print("Buffer val: ");
          Serial.println(buffer[0]);
          int sum = buffer[0]+ (250 - buffer[0]);
          
          MoneyBlock[0] = sum;
          Serial.println(MoneyBlock[0]);
          if(MoneyBlock[0] >= 255 ){
              Serial.print("Enough Balance");
              lcd.clear();
              lcd.print("Enough Balance");
            }else{

          status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(MoneyBlockAddr, MoneyBlock, 16);
          if (status != MFRC522::STATUS_OK) {
              Serial.print(F("MIFARE_Write() failed: "));
              Serial.println(mfrc522.GetStatusCodeName(status));
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Error Reading");
              lcd.setCursor(0,1);
              lcd.print("Card. Try again");
          }
          lcd.clear();
          lcd.print("Recharged: ");
          lcd.print(250-buffer[0]);
          delay(1000);
          lcd.clear();
          lcd.print("Recharged: ");
          lcd.print(250-buffer[0]);
          delay(1000);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Current Balance");
          lcd.setCursor(0,1);
          lcd.print(MoneyBlock[0]);
          delay(1000);
          lcd.clear();
          lcd.print("Return: ");
          lcd.print(buffer[0]);
          Serial.print("Recharged: ");
          Serial.println(250-buffer[0]);
          Serial.print("Current Balance: ");
          Serial.println(MoneyBlock[0]);
          Serial.print("Return: ");
          Serial.println(buffer[0]);
          delay(1500);
          
            }
          //----------------------Update Balance End-----------------------
        // Halt PICC
            mfrc522.PICC_HaltA();
            // Stop encryption on PCD
            mfrc522.PCD_StopCrypto1();
            Serial.println("Choose Mode");
            lcd.setCursor(0,0);
            lcd.print("A: Payment Mode");
            Serial.println("A: Payment Mode");
            lcd.setCursor(0,1);
            lcd.print("B: Recharge Mode");
            Serial.println("B: Recharge Mode");
            //dump_byte_array(key.keyByte,MFRC522::MF_KEY_SIZE);
            Serial.println();
          } 
    }
}
uint8_t getFingerprint(){
            int p = -1;
            while(p != FINGERPRINT_OK){
              p = finger.getImage();
              switch(p){
                  case FINGERPRINT_OK:
                  Serial.println("Image Taken");
                  lcd.clear();
                  lcd.setCursor(0,0);
                  lcd.print("Fingerprint");
                  lcd.setCursor(0,1);
                  lcd.print("Scanned");
                  delay(1000);
                  return 1;
                  break;
                  case FINGERPRINT_IMAGEFAIL:
                  case FINGERPRINT_PACKETRECIEVEERR:
                  Serial.println("Error scanning fingerprint");
                  lcd.clear();
                  lcd.setCursor(0,0);
                  lcd.print("Scan Error");
                  lcd.setCursor(0,1);
                  lcd.print("Try again");
                  break;
                  case FINGERPRINT_NOFINGER:
                  Serial.println(".");
                  lcd.clear();
                  lcd.setCursor(0,0);
                  lcd.print("Place your thumb");
                  lcd.setCursor(0,1);
                  lcd.print("on scanner");
                  break;
                  default:
                  Serial.println("Unknown Error");
                  lcd.clear();
                  lcd.print("Unknown Error");
                  
                }
              }
            }
void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}
