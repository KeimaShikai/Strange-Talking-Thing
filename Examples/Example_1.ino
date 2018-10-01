#include <SPI.h>
#include "RF24.h"

//bool radioNumber = 0;

const uint8_t bufSize = 32; // :( я не знаю, как изменить внутренний размер буфера в драйвере
                            // 

RF24 radio(7,8);
//byte addresses[][6] = {"Node1","Node1"};
const uint64_t addr = 0xF0F0F0F0AA;

void setup() {
  Serial.begin(115200);

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  
  Serial.print(F("Hello! I'm "));
  Serial.println(reinterpret_cast<const char*>(addr));
  Serial.println(F("Just enter text"));
  Serial.print(F("Max size one message = "));
  Serial.println(radio.getPayloadSize());
  

  radio.openWritingPipe(addr);
  radio.openReadingPipe(1,addr);
  
  radio.startListening();
}

void ReadAndSend();
void RecieveAndPrint();

void loop() {
  
  RecieveAndPrint();
  ReadAndSend();

} // Loop

void ReadAndSend() {
  if(Serial.available()) {
    const uint8_t szPayload = radio.getPayloadSize();
    radio.stopListening();
    char data[bufSize+1] = {};
    
    size_t len = Serial.readBytes(data, bufSize);
    
    Serial.print(F("< "));
    Serial.println(data);

    for(uint8_t i = 0; i < (len / szPayload) + 1; ++i) {
      if (!radio.write( data, min(len, szPayload) )){
        Serial.println(F("failed send"));
      }
    }
        
    radio.startListening();
  }
}

void RecieveAndPrint() {
  char data[bufSize+1] = {};
  
  if( radio.available() ) {
    Serial.print(F("> "));
    while ( radio.available() ) {
      radio.read( data, bufSize );
      Serial.print(data);
    }
    //Serial.println();
  }
}
