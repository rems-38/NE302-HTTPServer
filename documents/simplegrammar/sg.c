#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>

#define INIT "message"

#define SP ' '
#define HTAB '\t'
#define LF '\n'

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
        verifMessage(line, message);
    }

    fclose(ftest);
    if (line) free(line);
    return 0;
} 