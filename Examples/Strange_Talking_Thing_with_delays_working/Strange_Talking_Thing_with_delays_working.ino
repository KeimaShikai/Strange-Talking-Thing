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
        {"hello", "hi, good day!"},
        {"hi, good day!", "how are you"},
        {"how are you", "fine"},
        {"fine", "good"},
        {"good", "yeah"},
        {"yeah", "hello"}
    };

bool isInitDialog = false; //на 2 плате

void setup()
{
  //isInitDialog = true; // на 1 плате

  ans = mp.begin()->first;

  Serial.begin(115200);

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);

  radio.openWritingPipe(addr);
  radio.openReadingPipe(1,addr);
  
  radio.startListening();

  Serial.print(F("Hello, isInitDialog = "));
  Serial.println(isInitDialog);
}

/////////////////// функции логирования

#define DEBUG // включает дебаг логирования
#define PRINT // включает вывод сообщений

void Log(const char*m1, const char*m2 = "", const char*m3 = "", const char*m4 = "") {
  #ifdef PRINT
    #ifdef DEBUG
  Serial.print("Log: ");
  Serial.print(m1);
  Serial.print(m2);
  Serial.print(m3);
  Serial.println(m4);
    #endif
  #endif
}

void Print(const char*m1, const char*m2 = "", const char*m3 = "", const char*m4 = "") {
  #ifdef PRINT
  Serial.print(m1);
  Serial.print(m2);
  Serial.print(m3);
  Serial.println(m4);
  #endif
}

// без перевода строки
void PrintOne(const char*m1, const char*m2 = "", const char*m3 = "", const char*m4 = "") {
  #ifdef PRINT
  Serial.print(m1);
  Serial.print(m2);
  Serial.print(m3);
  Serial.print(m4);
  #endif
}

void LoopInfo(const char*m1, const char*m2 = "", const char*m3 = "", const char*m4 = "") {
  #ifdef PRINT
    #ifdef DEBUG
  Serial.print("Loop ");
  Serial.print(m1);
  Serial.print(m2);
  Serial.print(m3);
  Serial.println(m4);
    #endif
  #endif
}

/////////////////// провеки

/*
   предпалагается, что эти функции будут вызваться в циклах ожидания
   поэтому внутри них есть счётчики
*/

bool CheckAvailable() {
  static int countCall = 0;
  bool res = false;
  if(countCall++ < 5) {
    return res;
  }
  countCall = 0;

  radio.startListening();
  for(int i = 0; i < 50; ++i) {
    res = radio.available();
  }
  radio.stopListening();
  LoopInfo("CheckAvailable, text is ", (res ? "available" : "not"));
  return res;
}

bool Replay(const char* text) {
  static int countCall = 0;
  bool res = false;
  if(countCall++ < 20) {
    return res;
  }
  countCall = 0;
  
  radio.stopListening();
  res = radio.write(text, strlen(text));
  radio.startListening();
  LoopInfo("Replay, '", text, "' is ", (res ? "send" : "not send"));
  return res;  
}

///////////////////

void loop()
{
  
    char msg[bufSize+1] = {};
    if(isInitDialog) {
        radio.stopListening();

        //delay(1000);
        Log("init = ", ans);
        PrintOne("<< ");
        while(!radio.write(ans, strlen(ans))) { 
          LoopInfo(" init"); 
        }
        isInitDialog = false;

        Print("(inited) ", ans);
    }

    // recieve
    delay(500);
    Log("waiting msg");
    radio.startListening();
    PrintOne(">> ");
    while(!radio.available()) {  // ожидание сообщения
      LoopInfo(" recieve"); 
      Replay(ans);
    }
    radio.read(msg, bufSize); // чтение сообщения

    Print(msg);

    auto it = mp.find(msg);
    if(it != mp.end()) {
        // у нас в map есть ответ
        ans = it->second;
    }
    // иначе переотправляем ответ
    
    radio.stopListening();
    delay(500);
    Log("sending");
    PrintOne("<< ");
    while(!radio.write(ans, strlen(ans))) { 
      LoopInfo(" sending"); 
      if(CheckAvailable()) break;
    }

    Print(ans);

    radio.startListening();
  
}
