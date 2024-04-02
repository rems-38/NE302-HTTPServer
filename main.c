#include "isX.h"


int main(int argc, char *argv[]) {
    
    if(argc != 2){
        printf("Erreur dans le nombre d'arguments\n");
        return 0;
    }

    FILE *ftest = fopen(argv[1], "r");
    char *line = NULL;
    int len = 0;

    if (ftest == NULL) {
        printf("Impossible d'ouvrir le fichier %s\n", argv[1]);
        return -1;
    }

    // ----------------------------------------------------------
    // /!\ TROUVER FUNCTION QUI LIT TOUT LE FICHIER D'UN COUP !!
    // ----------------------------------------------------------
    
    // if ((line = fgets(&line, len, ftest)) != NULL) {
    //     Element *req = isHTTPMessage(line, len);

    //     if (req == NULL) {
    //         printf("Erreur dans la lecture du message\n");
    //         exit(1);
    //     } else { printArbre(req, 0); }
    // }

    fclose(ftest);
    if (line) free(line);
    return 0;
}


