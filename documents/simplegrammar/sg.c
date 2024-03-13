#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>

#define GRAMMAIRE "test.abnf"
#define INIT "message"

#define SP ' '
#define HTAB '\t'
#define LF '\n'
#define DIGIT_RANGE_START '0'
#define DIGIT_RANGE_END '9'
#define ALPHA_UPPER_START 'A'
#define ALPHA_UPPER_END 'Z'
#define ALPHA_LOWER_START 'a'
#define ALPHA_LOWER_END 'z'

typedef struct {
    char type; // Type de l'élément : 'a' pour ALPHA, 'd' pour DIGIT, 's' pour séparateur, 'p' pour ponctuation, 'i' pour istring
    union {
        char alpha;         // Caractère pour ALPHA
        char digit;         // Caratcère pour DIGIT
        char separator;     // Caractère pour séparateur
        char punctuation;   // Caractère pour ponctuation
        char istring[256];  // Chaîne de caractère pour istring
    } data;
} GrammarElement;


bool isDigit(char c) {
    return (c >= DIGIT_RANGE_START && c <= DIGIT_RANGE_END);
}

bool isAlpha(char c) {
    return (c >= ALPHA_UPPER_START && c <= ALPHA_UPPER_END) || (c >= ALPHA_LOWER_START && c <= ALPHA_LOWER_END);
}

void logElement(GrammarElement *el) {
    switch (el->type) {
        case 'a':
            printf("__alpha: %c\n", el->data.alpha);
            break;
        case 'd':
            printf("__digit: %c\n", el->data.digit);
            break;
        case 's':
            printf("__%c: %c\n", el->data.separator == ' ' ? 'sp' : 'htab' , el->data.separator);
            break;
        case 'p':
            printf("__icar: %c\n", el->data.punctuation);
            break;
        case 'i':
            printf("__istring: %s\n", el->data.istring);
            break;
        default:
            break;
    }
}

int main(int argc, char *argv[]) {
    
    FILE *gram = fopen(GRAMMAIRE, "r");
    FILE *ftest = fopen(argv[1], "r")
    char *line = NULL;
    size_t len = 0;
    size_t read;

    if (gram == NULL) {
        printf("Impossible d'ouvrir le fichier %s\n", GRAMMAIRE);
        return -1;
    }
    if (ftest == NULL) {
        printf("Impossible d'ouvrir le fichier %s\n", argv[1]);
        return -1;
    }
    

    while ((read = getline(&line, &len, gram)) != -1) {
        if (line[0] == ';') { continue; }

        char *ligne_split;
        
        ligne_split = strtok(line," = ");


        printf("%d : %d\n",ligne_split[0],ligne_split[1]);
       /* char *type = ligne_split[0];

        int i = 2;
        while (ligne_split[i] != '\0'){

        }
*/
        printf("%s\n", line);

        }
    

    fclose(gram);
    if (line) { free(line); }

    return 0;
} 