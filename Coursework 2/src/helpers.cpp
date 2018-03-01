#include "helpers.h"

// Checks if the input is a digit
int isdigit(const char c) {
  if ((c - '0' > -1) && (c - '0' < 10))
    return 1;
  else
    return 0;
}
// The output sequence determines the type of the output
// 1 --- FLOAT
// 0 --- INT
uint8_t parseRegex(char* regex, char type){
    uint8_t result_type = 0;
    if (regex[1]== '-'){
        for (uint8_t i = 1; i < 23; i++) {
            if (regex[i]=='.'){
                // Number is a float
                result_type = 1;
            }
            regex[i-1] = regex[i];
        }
    }
    return result_type;
}
