#include <SPI.h>
#include "RF24.h"

//place for future global variables
//make sense to put everything in other file
const uint8_t bufSize = 32;

RF24 radio(7, 8);
const uint64_t addr = 0xFFFFBACE64;

//bool bRole = 0;
bool status_is_emitter = 1;

void setup() {
  Serial.begin(115200);

  radio.begin();

  radio.setPALevel(RF24_PA_LOW);

  radio.openWritingPipe(addr);
  radio.openReadingPipe(1, addr);

  radio.startListening();
}

void loop() {

  char data_out1[bufSize + 1] = "1This is a time to exterminatus!";
  char data_out2[bufSize + 1] = "0Check out this epic drop!";
  char data_in[bufSize + 1] = "";

  //place for two if constractions
  if (status_is_emitter)
  {
    if (Serial.available())
    {
      const uint8_t szPayload = radio.getPayloadSize();
      radio.stopListening();

      size_t len = Serial.readBytes(data_out1, bufSize);

      //Serial.print(F("< "));
      //Serial.println(data);

      for (uint8_t i = 0; i < (len / szPayload) + 1; ++i)
      {
        if (!radio.write( data_out1, min(len, szPayload) ))
        {
          Serial.println(F("failed send"));
        }
      }

      radio.startListening();
    }
  }
  else
  {
    if ( radio.available() )
    {
      Serial.print(F("> "));
      while ( radio.available() )
      {
        radio.read( data_in, bufSize );
        Serial.print(data_in);
      }
    }
    if (data_in[0] == '1') //maybe we need to check out
                            //this place for mistakes
    {
      status_is_emitter = 0;
    }
    else
    {
      status_is_emitter = 1;
    }
  }
}
