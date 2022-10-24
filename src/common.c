/*
 * @Author: FYB
 * @Description: GitHub: https://github.com/magil0
 */

#include "common.h"
#include "stdio.h"

extern _Bool DBG;

void debug(char* str){
    
    if (DBG) {
        puts(str);
    }
}




