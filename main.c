#include "verifMessage.h"
//#include "isX.h"


int main(int argc, char *argv[]) {
    
    if(argc != 2){
        printf("Erreur dans le nombre d'arguments\n");
        return 0;
    }

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
        printf("Résultat de la vérification du message : %d\n", output);

        printArbre(message, 0);
    }

    fclose(ftest);
    if (line) free(line);
    return 0;
}


