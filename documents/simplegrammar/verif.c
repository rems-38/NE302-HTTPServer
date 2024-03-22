#include<stdlib.h>
#include<stdio.h>


int verifMessage(char *text, Element *data) {
    size_t curr = 0;

    char *starting = malloc(5 * sizeof(char));
    strncpy(starting, text, 5);
    
    if (verifDebut(starting) == false) return -1;
    ajoutFils(data, starting, len);
    curr += len;


}

bool verifDebut(char *text){
    if (strcmp(text, "start")) return true;
    return false;
}

bool verifMot(char *text){
    char *textbis = *text
    while(isSeparateur(textbis)==false){
        *textbis += sizeof(char);
    }
    if(verifALPHA(text) && verifSeparateur(textbis)){ return true;}
    return false
}


bool verifALPHA(char *text){
    while(isSeparateur(text)==false){
        if (isAlpha(text)==false){
            return false;
        }
        *text += sizeof(char);
    }
    return true
}

bool verifSeparateur(char *text){
    
}


bool isSeparateur(char *text){
    if (strcmp(text[0]," ")||strcmp(text[0],"   ")||strcmp(text[0],"-"||strcmp(text[0],"_"))){return true;}
    return false
}

bool isAlpha(char *text){
    if ((*text >= 65 && *text <= 90)||(*text >= 97 && *text <= 122)){ // table ascii : 65-90 = A-Z et 97-122 = a-z
        return true;}
    return false
}

bool isDigit(char *text){
    if (*text >= 48 && *text <= 57){ // table ascii : 48-57 = 0-9
        return true;}
    return false
}

/*
Table ASCII :
SP = 32
HTAB = 9 
LF = 10
*/

