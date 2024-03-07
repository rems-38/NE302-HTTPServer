#include <stdlib.h>
#include <stdio.h>


int main(int argc, char *argv[]) {

    FILE *test_file = fopen(argv[1], "r");

    if (test_file == NULL) {
        printf("Impossible d'ouvrir le fichier %s\n", argv[1]);
        return -1;
    }

    

    return 0;

}