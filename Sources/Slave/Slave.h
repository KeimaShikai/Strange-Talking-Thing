//global variables
RF24 radio(7, 8);
const uint64_t addr = 0xFFFFBACE64;
const uint8_t bufSize = 32;

//status initialisation
bool status_is_emitter = 0;

//dialog
char data_out1[bufSize + 1] = "1This is a time to exterminatus!";
//char data_out2[bufSize + 1] = "0Check out this epic drop!";
char data_in[bufSize + 1] = "";
