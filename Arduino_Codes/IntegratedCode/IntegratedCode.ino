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

MFRC522::MIFARE_Key key;
void setup() {
  
   Serial.begin(9600); // Initialize serial communications with the PC
    while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    SPI.begin();        // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522 card
    lcd.begin(16,2);

    // Prepare the key (used both as key A and as key B)
    // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }

    Serial.println(F("Scan a MIFARE Classic PICC to demonstrate read and write."));
    Serial.print(F("Using key (for A and B):"));
    dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);
    Serial.println();

    Serial.println(F("BEWARE: Data will be written to the PICC, in sector #1"));
     finger.begin(57600);
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

    

}

void loop() {
  // put your main code here, to run repeatedly:
  char key = keypad.getKey();
  
  if (key){
    Serial.println(key);
    
    if (key == 'A' ){
      lcd.clear();
      lcd.print("Payment Mode");
       if ( ! mfrc522.PICC_IsNewCardPresent())
        return;

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
        return;

    // Show some details of the PICC (that is: the tag/card)
    Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();
    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    Serial.println(mfrc522.PICC_GetTypeName(piccType));

  if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        Serial.println(F("This sample only works with MIFARE Classic cards."));
        return;
    }
    //-------------------------Payment---------------------------------------------------
     MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer);
    byte identityBlock = 15;
    byte identitySector = 3;
    byte identityBlockAddr = 12;
    

    // Authenticate using key A
    Serial.println(F("Authenticating using key A..."));
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, identityBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    
 // Authenticate using key B
    Serial.println(F("Authenticating again using key B..."));
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, identityBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    Serial.println(F("Current data in sector:"));
    mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, identitySector);
    Serial.println();

    Serial.print(F("Reading data from block ")); Serial.print(identityBlockAddr);
    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(identityBlockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    Serial.print(F("Data in block ")); Serial.print(identityBlockAddr); Serial.println(F(":"));
    dump_byte_array(buffer, 16); Serial.println(buffer[0]);

    byte tempo = buffer[0];
    if(tempo == 1){
      Serial.println("For Student: ");
      lcd.clear();
      lcd.print("Student Card");
      //-------------------Fingerprint--Begin------------------
      
        while (!  getFingerprintEnroll() );


uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Waitinf For");
  lcd.setCursor(0,1);
  lcd.print("Fingerprint...");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }
}

      
      //-------------------Fingerprint--End---------------------

      //------------------------Deduction-Start-----------------------
      byte sector2        = 2;
      byte blockAddr2     = 8;
      byte AuthBlock      = 11; 
      byte dataBlocks3[16];
      
      Serial.print(F("Reading data from block ")); Serial.print(blockAddr2);
    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr2, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    Serial.print(F("Data in block ")); Serial.print(blockAddr2); Serial.println(F(":"));
    dump_byte_array(buffer, 16); Serial.println();
    lcd.clear();
    
    Serial.println(buffer[0]);
    byte temp = buffer[0];
    
    int deduction = 10;
    if(buffer[0]< deduction){Serial.println("Insufficient Balance");
    lcd.setCursor(0,0);
    lcd.print("Insufficient");
    lcd.setCursor(0,1);
    lcd.print("Balance");
    }else{
    temp -= deduction; 
     lcd.setCursor(0,0);
    lcd.print("Balance- ");
    lcd.print(temp);
    lcd.setCursor(0,1);
    lcd.print("Deducted- ");
    lcd.print(deduction);
    dataBlocks3[0] = temp;
     Serial.print(F("Writing data into block ")); Serial.print(blockAddr2);
    Serial.println(F(" ..."));
    dump_byte_array(dataBlocks3, 16); Serial.println();
    
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr2, dataBlocks3, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    Serial.println();
    }

    Serial.println(F("Current data in sector:"));
    mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector2);
    Serial.println();
    
    delay(3000);
    lcd.clear();
      //------------------------Deduction-Stop------------------------
       // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
      
      }else{
        Serial.println("For General Public: ");
        lcd.clear();
        lcd.print("General Card");
        //-------------------Deduction-For-General-Public--------------------
        byte blockAddr2     = 8;
         Serial.print(F("Reading data from block ")); Serial.print(blockAddr2);
    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr2, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    Serial.print(F("Data in block ")); Serial.print(blockAddr2); Serial.println(F(":"));
    dump_byte_array(buffer, 16); Serial.println();
    
    Serial.println(buffer[0]);
    byte temp = buffer[0];
    
    int deduction = 25;
    if(buffer[0]< deduction){Serial.println("Insufficient Balance");
    lcd.setCursor(0,0);
    lcd.print("Insufficient");
    lcd.setCursor(0,1);
    lcd.print("Balance");
    }else{
    temp -= deduction; 
     lcd.setCursor(0,0);
    lcd.print("Balance- ");
    lcd.print(temp);
    lcd.setCursor(0,1);
    lcd.print("Deducted- ");
    lcd.print(deduction);
    dataBlocks3[0] = temp;
    check = 1;
     Serial.print(F("Writing data into block ")); Serial.print(blockAddr2);
    Serial.println(F(" ..."));
    dump_byte_array(dataBlocks3, 16); Serial.println();
    
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr2, dataBlocks3, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    Serial.println();
    }

    Serial.println(F("Current data in sector:"));
    mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector2);
    Serial.println();
    
    delay(5000);
    lcd.clear();
    //-----------------------------------------------
    // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
        //-------------------End-of-Deduction-For-General-Public-------------
        }
    
    

    
    
    //---------------------------------------------------------------------------
      
      
      }
      else if (key == 'B'){
        lcd.clear();
       lcd.print("Recharge Mode");
        }
  }

  

}
 void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}
