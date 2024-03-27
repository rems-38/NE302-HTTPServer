#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>

#define INIT "message"

#define AMAJ 65
#define ZMAJ 90
#define AMIN 97
#define ZMIN 122
#define ZERO 48
#define NINE 57
#define SP 32           // espace
#define HTAB 9          // \t
#define DASH 45         // -
#define UNDERSCORE 95   // _
#define COMMA 44        // ,
#define DOT 46          // .
#define EXCLAMATION 33  // !
#define QUESTION 63     // ?
#define COLON 58        // :
#define LF 10           // \n

/*
Table ASCII :
A-Z = 65-90
a-z = 97-122
0-9 = 48-57
SP = 32
HTAB = 9
- = 45
_ = 95
, = 44
. = 46
! = 33
? = 63
: = 58
LF = 10
*/

typedef struct Element {
    char *word;
    size_t length;
    struct Element *fils;
    struct Element *frere;
} Element;



/* Element *ajoutFils(Element *main, char *text, size_t len) {
    
}
*/

/* Element *ajoutFrere(Element *data, char *text size_t len) {

}
*/

bool isDebut(char *text){
    return strcmp(text, "start");
}

bool isAlpha(char text){
    return (text >= AMAJ && text <= ZMAJ) || (text >= AMIN && text <= ZIN);
}

bool isDigit(char text) {
    return (text >= ZERO && text <= NINE);
}

bool isSeparateur(char text) {
    return (text == SP || text == HTAB || text == DASH || text == UNDERSCORE);
}

bool isPonct(char text) {
    return (text == COMMA || text == DOT || text == EXCLAMATION || text == QUESTION || text == COLON);
}

bool isFin(char *text) {
    return strcmp(text, "fin");
}

bool isLF(char text) {
    return (text == LF);
}

bool isMot(char *text, size_t *curr){
    size_t icurr = 0;

    while (isAlpha(*(text+icurr))) { 
        icurr += 1;
    }

    if (icurr != 0) {
        if (isSeparateur(*(text+icurr))) {
            *curr += icurr;
            *curr += 1;
            return true;
        }
    }

    return false;
}

bool isNombre(char *text, size_t *curr){
    size_t icurr = 0;
    while (isDigit(*(text+icurr))) {
        icurr += 1;
    }
    *curr += icurr;
    return icurr != 0;
}

int verifMessage(Element *data) {
    size_t curr = 0;
    
    if (isDebut(data->word) == false) { return -1; }

    // ajoutFils(data, starting, len);
    curr += 5;

    int i = 0;
    bool boucle = true;

    while(boucle) {
        if (isMot(data->word+curr, &curr)) {
            if (isPonct(*(data->word+curr))) {
                curr++;
                i++;
            }
            else { boucle = false; }
        }
        else if (isNombre(data->word+curr, &curr)) {
            if (isSeparateur(*(data->word+curr))) {
                curr++;
                i++;
            }
            else { boucle = false; }
        }
        else { boucle = false; }

        // if (i < 2) { return false; } // vérifier si on a au moins 2 séquences

        // while(isPonct(data->word+curr,curr)){ // optionnel
        //     curr += 1;
        // }
        
    }

    if (isFin(data->word+curr) == false) { return -1; }
    curr += 3;
    
    if (isLF(*(data->word+curr)) == false) { return -1; }
    curr += 1;
    
    return 1;
}


int main(int argc, char *argv[]) {
    
    FILE *ftest = fopen(argv[1], "r");
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    Element *message = malloc(sizeof(Element));

    if (ftest == NULL) {
        printf("Impossible d'ouvrir le fichier %s\n", argv[1]);
        return -1;
    }

    if ((read = getline(&line, &len, ftest)) != -1) {
        message->word = line;
        message->length = read; // 27 (26 + 1)
        int output = verifMessage(message);
        printf("output : %d\n", output);
    }

    fclose(ftest);
    if (line) free(line);
    return 0;
} 