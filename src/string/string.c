#include "string.h"

int strlen(const char* str){
  int len = 0;
  while(str != 0){
    len++;
    str++;
  }
  return len;
}

int strnlen(const char* str, int max){
  int len = 0;  
  for (len = 0; len < max; len++){
    if(str[len] == 0){
      break;
    }    
  }
  return len;
}


bool isdigit(char c){
  return c >= 48 && c <= 57;
}

int tonumericdigit(char c){
  return c - 48;
}