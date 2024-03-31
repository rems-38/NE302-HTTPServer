#include "arbre.h"

/*typedef struct Element {
    char *key;
    char *word;
    size_t length;
    struct Element *fils;
    struct Element *frere;
} Element;*/

void printArbre(Element *head, int level) {
    if (head == NULL) { return; }
    for (int i = 0; i < level; i++) { printf("\t"); }
    printf("%s: ", head->key);
    for (int j = 0; j < head->length; j++) {
        printf("%c", head->word[j]);
    }
    printf("\n");
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