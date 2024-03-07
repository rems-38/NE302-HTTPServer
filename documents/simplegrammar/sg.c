#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

#define GRAMMAIRE "test.abnf"
#define INIT message

int main(void) {
    
    FILE *gram = fopen(GRAMMAIRE, "r");
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    if (gram == NULL) {
        printf("Impossible d'ouvrir le fichier %s\n", GRAMMAIRE);
        return -1;
    }
    
    while ((read = getline(&line, &len, gram)) != -1) {
        if (line[0] == ';') { continue; }
        
        printf("%s", line);







    }

    fclose(gram);
    if (line) { free(line); }

    return 0;
}