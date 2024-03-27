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
    char *key;
    char *word;
    size_t length;
    struct Element *fils;
    struct Element *frere;
} Element;

void printArbre(Element *head, int level) {
    if (head == NULL) { return; }
    for (int i = 0; i < level; i++) { printf("\t"); }
    printf("%s: %s\n", head->key, head->word);
    printArbre(head->fils, level+1);
    printArbre(head->frere, level);
}

Element *addEl(char *key, char *word, size_t length) {
    Element *el = malloc(sizeof(Element));
    
    el->key = malloc(strlen(key)+1);
    el->word = malloc(length+1);
    strncpy(el->key, key, strlen(key));
    strncpy(el->word, word, length);
    el->length = length;
    el->fils = NULL;
    el->frere = NULL;
    
    return el;
}

bool isDebut(char *text, Element *head){
    Element *el = addEl("debut", "start", 5);
    Element *sub = addEl("__istring", "start", 5);
    el->fils = sub;
    head->fils = el;    

    return strcmp(text, "start");
}

bool isAlpha(char text, Element *pere){
    return (text >= AMAJ && text <= ZMAJ) || (text >= AMIN && text <= ZMIN);
}

bool isDigit(char text, Element *pere) {
    return (text >= ZERO && text <= NINE);
}

bool isSeparateur(char text, Element *head) {
    Element *el = addEl("separateur", &text, 1);
    Element *sub;
    if (text == SP) { sub = addEl("__sp", &text, 1); }
    else if (text == HTAB) { sub = addEl("__htab", &text, 1); }
    else { sub = addEl("__icar", &text, 1); }
    el->fils = sub;
    head->frere = el;

    return (text == SP || text == HTAB || text == DASH || text == UNDERSCORE);
}

bool isPonct(char text, Element *head) {
    Element *el = addEl("ponct", &text, 1);
    Element *sub = addEl("__icar", &text, 1);
    el->fils = sub;
    head->frere = el;

    return (text == COMMA || text == DOT || text == EXCLAMATION || text == QUESTION || text == COLON);
}

bool isFin(char *text, Element *head) {
    Element *el = addEl("fin", "fin", 3);
    Element *sub = addEl("__istring", "fin", 3);
    el->fils = sub;
    head->frere = el;

    return strcmp(text, "fin");
}

bool isLF(char text, Element *head) {
    Element *el = addEl("__lf", &text, 1);
    head->frere = el;

    return (text == LF);
}

bool isMot(char *text, size_t *curr, Element *head) {
    size_t icurr = 0;
    bool res = false;

    Element *c = malloc(sizeof(Element));
    Element *save_c = c;
    Element *tmp = malloc(sizeof(Element));
    while (isAlpha(*(text+icurr), head)) { 
        tmp = addEl("__alpha", text+icurr, 1);
        c->frere = tmp;
        c = c->frere;
        icurr += 1;
    }

    if (icurr != 0) {
        if (isSeparateur(*(text+icurr), tmp)) {
            *curr += icurr;
            *curr += 1;
            res = true;
        }
    }
    
    Element *el = addEl("mot", text, icurr);
    head->frere = el;
    head->frere->fils = save_c->frere;

    return res;
}

bool isNombre(char *text, size_t *curr, Element *head) {
    size_t icurr = 0;

    Element *c = malloc(sizeof(Element));
    Element *save_c = c;
    Element *tmp = malloc(sizeof(Element));
    while (isDigit(*(text+icurr), head)) {
        tmp = addEl("__digit", text+icurr, 1);
        c->frere = tmp;
        c = c->frere;
        icurr += 1;
    }
    *curr += icurr;

    Element *el = addEl("nombre", text, icurr);
    head->frere = el;
    head->frere->fils = save_c->frere;

    return icurr != 0;
}

int verifMessage(Element *data) {
    Element *head = data;
    size_t curr = 0;
    
    if (isDebut(head->word, data) == false) { return -1; }
    data = data->fils;
    curr += 5;

    int i = 0;
    bool boucle = true;

    while(boucle) {
        if (isMot(head->word+curr, &curr, data)) {
            data = data->frere;
            if (isPonct(*(head->word+curr), data)) {
                data = data->frere;
                curr++;
                i++;
            }
            else { boucle = false; }
        }
        else if (isNombre(head->word+curr, &curr, data)) {
            data = data->frere;
            if (isSeparateur(*(head->word+curr), data)) {
                data = data->frere;
                curr++;
                i++;
            }
            else { boucle = false; }
        }
        else { boucle = false; }

        // if (i < 2) { return false; } // vérifier si on a au moins 2 séquences
        
    }

    while (isPonct(*(head->word+curr), data)) {
        data = data->frere;
        curr++;
    }

    if (isFin(head->word+curr, data) == false) { return -1; }
    data = data->frere;
    curr += 3;
    
    if (isLF(*(head->word+curr), data) == false) { return -1; }
    curr += 1;

    data = head;
    return 1;
}


int main(int argc, char *argv[]) {
    
    FILE *ftest = fopen(argv[1], "r");
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    Element *message = malloc(sizeof(Element));
    message->key = INIT;

    if (ftest == NULL) {
        printf("Impossible d'ouvrir le fichier %s\n", argv[1]);
        return -1;
    }

    if ((read = getline(&line, &len, ftest)) != -1) {
        message->word = line;
        message->length = read;
        int output = verifMessage(message);
        printf("%d\n", output);

        printArbre(message, 0);
    }

    fclose(ftest);
    if (line) free(line);
    return 0;
} 