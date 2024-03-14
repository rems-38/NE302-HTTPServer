#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define GRAMMAIRE "test.abnf"
#define MAX_LINE_LENGTH 256

typedef struct {
    char type;
    union {
        char alpha;
        char digit;
        char separator;
        char istring[256];
        char *sequence_elements[10];  // Arbitraire : pour stocker des éléments de séquence
    } data;
} JSONElement;

bool isDigit(char c) {
    return (c >= '0' && c <= '9');
}
bool isAlpha(char c) {
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

JSONElement createAlphaElement(char alpha) {
    JSONElement element;
    element.type = 'a';
    element.data.alpha = alpha;
    return element;
}
JSONElement createDigitElement(char digit) {
    JSONElement element;
    element.type = 'd';
    element.data.digit = digit;
    return element;
}
JSONElement createSeparatorElement(char separator) {
    JSONElement element;
    element.type = 's';
    element.data.separator = separator;
    return element;
}
JSONElement createStringElement(char *istring) {
    JSONElement element;
    element.type = 'i';
    strncpy(element.data.istring, istring, sizeof(element.data.istring) - 1);
    element.data.istring[sizeof(element.data.istring) - 1] = '\0';
    return element;
}
JSONElement createSequenceElement(char **sequence_elements) {
    JSONElement element;
    element.type = 'sequence';
    for (int i = 0; i < 10; i++) {
        if (sequence_elements[i] == NULL) {
            break;
        }
        element.data.sequence_elements[i] = sequence_elements[i];
    }
    return element;
}

JSONElement convertAbnfToJson(char *rule) {
    // ....

    if (isAlpha(rule[0])) {
        return createAlphaElement(rule[0]);
    } else if (isDigit(rule[0])) {
        return createDigitElement(rule[0]);
    } else if (rule[0] == '-') {
        return createSeparatorElement(rule);
    } else {
        return createStringElement(rule);
    }
}

void jsonFromAbnf(FILE *gram) {
    char line[MAX_LINE_LENGTH];

    while (fgets(line, MAX_LINE_LENGTH, gram) != NULL) {
        line[strcspn(line, "\n")] = '\0';
        JSONElement jsonElement = convertAbnfToJson(line);    
    }
}


int main(void) {
    FILE *gram = fopen(GRAMMAIRE, "r");
    if (gram == NULL) {
        printf("Impossible d'ouvrir le fichier %s\n", GRAMMAIRE);
        return -1;
    }

    jsonFromAbnf(gram);
    
    fclose(gram);
    return 0;
}