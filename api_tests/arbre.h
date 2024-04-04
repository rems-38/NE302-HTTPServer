#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>

typedef struct Element {
    char *key;
    char *word;
    size_t length;
    struct Element *fils;
    struct Element *frere;
} Element;

void printArbre(Element *head, int level);
Element *addEl(char *key, char *word, size_t length);
void updateLength(Element *data, size_t length);