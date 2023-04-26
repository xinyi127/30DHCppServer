#include "util.h"
#include <stdio.h>
#include <stdlib.h>

void effif(bool condition, const char* errmsg){
    if(condition){
        perror(errmsg);
        exit(EXIT_FAILURE);
    }
}
