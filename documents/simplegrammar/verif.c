#include<stdlib.h>
#include<stdio.h>

int main(){


  
  return 0;
}

bool verifMot(char *text){
    char *textbis = *text
    while(isSeparateur(textbis)==false){
        *textbis += sizeof(char);
    }
    if(verifALPHA(text) && verifSeparateur(textbis)){ return true;}
    return false
}

bool isSeparateur(char *text){
    if (strcmp(text[0]," ")||strcmp(text[0],"   ")||strcmp(text[0],"-"||strcmp(text[0],"_")){return true;}
    return false
}

bool verifSeparateur(char *text){}
