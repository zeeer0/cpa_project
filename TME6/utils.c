#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "utils.h"

char** splitWords(char* string){
 int nb=1, inAWord=0;

 // number of words
 for(char*s = string; *s; s++){
  if( (*s!=' ' || *s!='\t') && inAWord==0){
   inAWord=1;
  }else if(inAWord==1 && (*s==' ' || *s=='\t') ){
   inAWord=0;
   nb++;
  }
 }

// build tab of words
 char** result = (char**) malloc(sizeof(char*)*nb);
 int lastSpace = 0, curSpace=0;
 nb=0;inAWord=0;

// for each word build string
 for(char*s = string; *s; s++, curSpace++){
  if( (*s!=' ' || *s!='\t') && inAWord==0){
   inAWord=1;
  }else if(inAWord==1 && (*s==' ' || *s=='\t') ){
   inAWord=0;
   result[nb] = (char*) malloc(sizeof(char)*(curSpace-lastSpace)+1);   
   result[nb] = strncpy(result[nb], string+lastSpace, curSpace-lastSpace);
   result[nb][curSpace-lastSpace]='\0';
   lastSpace=curSpace+1;
   nb++;
  }
 }

 //last word
 result[nb] = (char*) malloc(sizeof(char)*(curSpace-lastSpace)+1);   
 result[nb] = strncpy(result[nb], string+lastSpace, curSpace-lastSpace);
 result[nb][curSpace-lastSpace]='\0';
 result[nb+1]=NULL;

 return result;
}
