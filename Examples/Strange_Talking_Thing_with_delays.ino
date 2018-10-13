#include <SPI.h>
#include "RF24.h"
#include <ArduinoSTL.h>
#include <map>
#include <cstring>

RF24 radio(7, 8);

const uint64_t addr = 0xF0F0BACE64;

const uint8_t bufSize = 32;
const char* ans = nullptr;

struct cmp_str
{
   bool operator()(const char *a, const char *b)
   {
      return std::strcmp(a, b) < 0;
   }
};

std::map<const char*, const char*, cmp_str> mp = {
        {"hello", "hi"},
        {"hi", "how are you"},
        {"how are you", "fine"},
        {"fine", "good"},
        {"good", "yeah"},
        {"yeah", "hello"}
    };

     //bool isInitDialog = false; //на 2 плате
     bool isInitDialog = true; // на 1 плате

void setup()
{

  ans = mp.begin()->first;

  Serial.begin(115200);

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);

  radio.openWritingPipe(addr);
  radio.openReadingPipe(1,addr);
  
  radio.startListening();

  Serial.println(F("Hello"));
}
void loop()
{
  
    char msg[bufSize+1] = {};
    radio.stopListening();
    if(isInitDialog) {

        //Serial.println(F("Init begins"));
        delay(1000);
        while(!radio.write(ans, strlen(ans)));
        //Serial.println("Init ends");
        isInitDialog = false;
        
    }

    // recieve
    radio.startListening();
    while(!radio.available()); // ожидание сообщения
    delay(1000);
    radio.read(msg, bufSize); // чтение сообщения

    auto it = mp.find(msg);
    if(it != mp.end()) {
        // у нас в map есть ответ
        // посылаем ответ
        ans = it->second;
        Serial.println(ans);
    }
    else if(strcmp(msg, "retry") == 0) {
        // переотправить ответ
        Serial.println(ans);
    }
    else {
        // не нашлось ответа
        // 1) шум
        // 2) подавление
         ans = "retry";
         Serial.println(ans);
    }
    // send

    //Serial.println(ans);
    
    radio.stopListening();
    delay(1000);
    while(!radio.write(ans, strlen(ans)));
 
    Serial.println(ans);

    radio.startListening();
  
}
