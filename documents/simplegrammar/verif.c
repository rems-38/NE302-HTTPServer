#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

bool verifDebut(char *text) {
    return strcmp(text, "start") == 0;
}

bool verifFin(char *text) {
    return strcmp(text, "fin") == 0;
}

bool isPonct(char c) {
    return (c == ',' || c == '.' || c == '!' || c == '?' || c == ':');
}

bool isSeparateur(char c) {
    return (c == ' ' || c == '-' || c == '_');
}

bool isAlpha(char c) {
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

bool isDigit(char c) {
    return (c >= '0' && c <= '9');
}


bool verifMot(char *text, size_t curr) {
    size_t curr_comp = curr;

    while (isAlpha(text[curr])) {
        curr++;
    }

    if (curr == curr_comp)
        return false;

    if (!isSeparateur(text[curr]))
        return false;

    return true;
}

bool verifNombre(char *text, size_t curr) {
    int i = 0;

    while (isDigit(text[curr])) {
        i++;
        curr++;
    }

    if (i <= 1)
        return false;

    return true;
}

bool verifMessage(char *text) {

    size_t curr = 0; // curseur qui permet de parcourir la ligne sans modifier le pointeur texte initial

    char *starting = malloc(5 * sizeof(char)); // on crée une chaine de caractère pour stocker les 5 premiers caractères de notre ligne
    strncpy(starting, text, 5); // on copie le contenu des 5 premier caractères de la ligne
    
    if (!verifDebut(starting)) // on vérifie si notre ligne commence par "start"
        return false;

    curr += 5; // on a vérifié "start", on passe au 6ième caractère de la ligne avec +5 car sizeof(char) = 1

    int i = 0; // indicateur pour vérifier qu'on a bien au moins 2 séquences (mot ponct/nombre separateur)
    bool boucle = true; // passe à false dès qu'on a plus de séquences

    while (boucle) { // vérifier si on a au moins 2 séquences (mot ponct/nombre separateur)
        if (verifMot(text, curr)) {
            if (isPonct(text[curr])) {
                i++;
            } else {
                return false;
            }
        } else if (verifNombre(text, curr)) {
            if (isSeparateur(text[curr])) {
                i++;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    if (i < 2) // vérifiér qu'on ai au moins 2 séquences
        return false;

    while (isPonct(text[curr])) { // optionnel on parcoure la ligne jusqu'à ce qu'il n'y ait plus de ponct
        curr++;
    }

    char *fin = malloc(3 * sizeof(char)); // on vérifie si le message se termine par "fin" de la même manière qu'on a vérifié "début"
    strncpy(fin, text + curr, 3);
    
    if (!verifFin(fin))
        return false;

    curr += 3;

    if (text[curr] != 10) // LF = Line feed (retour à la ligne) vaut 10 en ASCII
        return false;

    return true; // notre message est correcte
}

int main(int argc, char *argv[]) {
    
    if(argc != 2){
        printf("Erreur d'argument\n");
        return 0;
    }

    FILE *ftest = fopen(argv[1], "r");
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    
    if ((read = getline(&line, &len, ftest)) != -1) { 
        printf("Verification du message : %d\n", verifMessage(line));
    }
    
    return 0;
}