#ifndef COMMON_H
#define COMMON_H

#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "prodcons.h"

char* arrayToString(int* array){
	int i;
	char * myString= malloc(sizeof(array)/sizeof(int));
	for(i=0; i<sizeof(array)/sizeof(int); i++){
		strcpy(myString, myString);
		strcat(myString, (char*)array[i]);
		strcpy(myString, myString);
		strcat(myString, " ");
	}
	return myString;
}

#endif //COMMON_H
