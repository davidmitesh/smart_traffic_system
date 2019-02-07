#include "SoftwareSerial.h"

SoftwareSerial serial_connection(11, 10);

#define BUFFER_SIZE 64

char inData[BUFFER_SIZE];
char inChar=-1;
int count=0;
int i=0;

void setup()
{
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  
  Serial.begin(9600);
  serial_connection.begin(38400);
  serial_connection.println("Ready!!!");
  Serial.println("Started");
}
void loop()
{
  byte byte_count=serial_connection.available();
  if(byte_count)
  {
    Serial.println("Incoming Data");
    int first_bytes=byte_count; 
    int remaining_bytes=0;
    if(first_bytes>=BUFFER_SIZE-1)
    {
      remaining_bytes=byte_count-(BUFFER_SIZE-1);
    }
    for(i=0;i<first_bytes;i++)
    {
      inChar=serial_connection.read();
      inData[i]=inChar;
    }
    inData[i]='\0';
    if(String(inData)=="0")
    {
      Serial.println("********* Start *********");
      digitalWrite(4,LOW);
      digitalWrite(3,HIGH);
      delay(7000);
      digitalWrite(3,LOW);
 
      digitalWrite(2,HIGH);
    }
    else if(String(inData)=="1")
    {
      Serial.println("********* STOP *********");
      digitalWrite(2,LOW);
      digitalWrite(3,LOW);
      //digitalWrite(4,HIGH);
      //delay(1000);
      //digitalWrite(4,LOW);
      
      digitalWrite(4,HIGH);
     
    }
    for(i=0;i<remaining_bytes;i++)
    {
      inChar=serial_connection.read();
    }
    Serial.println(inData);
  }
  delay(10); 
}
