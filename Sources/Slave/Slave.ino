#include <SPI.h>
#include "RF24.h"
#include "Slave.h"

void setup() {
  Serial.begin(115200);

  radio.begin();

  radio.setPALevel(RF24_PA_LOW);

  radio.openWritingPipe(addr);
  radio.openReadingPipe(1, addr);

  radio.startListening();
}

void loop() {

  if (status_is_emitter)
  {
    if (Serial.available())
    {
      const uint8_t szPayload = radio.getPayloadSize();
      radio.stopListening();

      size_t len = Serial.readBytes(data_out1, bufSize);

      for (uint8_t i = 0; i < (len / szPayload) + 1; ++i)
      {
        if (!radio.write(data_out1, min(len, szPayload)))
        {
          Serial.println(F("failed send"));
        }
      }

      status_is_emitter = 0;

      radio.startListening();
    }
  }
  else
  {
    if (radio.available())
    {
      while (radio.available())
      {
        radio.read(data_in, bufSize);
        Serial.print(data_in);
      }
    }

    //we need a condition to switch between statuses instead of this
    if (data_in[0] == '1') status_is_emitter = 1;
    //maybe we need to check out this place for mistakes
  }
}
