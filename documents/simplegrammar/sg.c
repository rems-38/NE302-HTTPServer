#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>

#define GRAMMAIRE "test.abnf"
#define INIT message

int main(void) {
    
    FILE *gram = fopen(GRAMMAIRE, "r");
    char *line = NULL;
    size_t len = 0;
    size_t read;

    if (gram == NULL) {
        printf("Impossible d'ouvrir le fichier %s\n", GRAMMAIRE);
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