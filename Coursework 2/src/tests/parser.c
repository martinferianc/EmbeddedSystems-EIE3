#include <stdio.h>

int isdigit(const char c) {
  if ((c - '0' > -1) && (c - '0' < 10))
    return 1;
  else
    return 0;
}
// The output sequence determines the type of the output
// 1 --- FLOAT
// 0 --- INT
short int parseRegex(char* regex, char type){
    short int result_type = 0;
    if (regex[1]== '-'){
        for (size_t i = 1; i < 23; i++) {
            if (regex[i]=='.'){
                // Number is a float
                result_type = 1;
            }
            regex[i-1] = regex[i];
        }
    }
    return result_type;
}

int main(int argc, char const *argv[]) {
    printf("Potato\n");
    char regex[24];
    regex[0] = 'R';
    regex[1] = '-';
    regex[2] = '5';
    short int result_type = parseRegex(regex, 'R');
    if (result_type){
        printf("%f\n", atof(regex));
    } else {
        printf("%d\n", atoi(regex));
    }
    return 0;
}
