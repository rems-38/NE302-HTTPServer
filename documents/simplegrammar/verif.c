#include<stdlib.h>
#include<stdio.h>


int verifMessage(char *text, Element *data) {
    size_t curr = 0;

    char *starting = malloc(5 * sizeof(char));
    strncpy(starting, text, 5);
    
    if (verifDebut(starting) == false){return -1};

    ajoutFils(data, starting, len);
    curr += len;

    *text += curr;

    int i = 0;
    bool boucle = true;

    while(boucle){
    if(verifMot(&text+curr,curr)){
        if(verifPonct(&text+curr,curr)){
            i++;
        }
        else{
            boucle = false;
        }
    }
    else if (verifNombre(&text+curr,curr)){
        if(isSeparateur(&text+curr,curr)){
            i++;
        }
        else{
            boucle = false;
        }
    }
    else{
        boucle = false;
    }

    if(i<2){return false;} // vérifier si on a au moins 2 séquences

    while(isPonct(&text+curr,curr)){ // optionnel
        curr += sizeof(char);
    }

    // Manque fin LF
}

bool verifDebut(char *text){
    if (strcmp(text, "start")) return true;
    return false;
}

bool verifMot(char *text,size_t curr){

    curr_comp = curr;

    while(isALPHA(text+curr)){ 
        curr += sizeof(char);
        }

    if (curr == curr_comp){
        return false;
    }


    if (isSeparateur(text+curr)==false){
        return false;
    }

    return true;
}

bool verifPonct(char *text,size_t curr){

}

bool verifNombre(char *text,size_t curr){

}

// Verif caractère par caractère

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

