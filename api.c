#include "api.h"
#include "isX.h"

struct Element *data; //racine de l'arbre


void *getRootTree(){
    extern Element *head;
    return head;
}


// Fonction qui recherche dans l'arbre tous les noeuds dont l'etiquette est egale à la chaine de caractères en argument.   
// Par convention si start == NULL alors on commence à la racine 
// sinon on effectue une recherche dans le sous-arbre à partir du noeud start 
_Token *searchTree(void *start,char *name){
    
    struct _token *tree;

    struct _token *racine = tree; // on veut conserver le début de l'arbre


    if(start == NULL){ // on partira du principe qu'on part toujours de la racine
        start = getRootTree();
    }

    while(!(data->frere==NULL && data->fils==NULL)){
        while(data->frere != NULL){
            if(strcmp(data->word,name)){
                tree->node = data;
                tree = tree->next;
            }
            else{

            }
        }
    }
}


// Fonction qui supprime et libere la liste chainée de reponse. 

void purgeElement(_Token **r){
    struct _token *prec = *r;
    while((*r)->next != NULL){
        r = (*r)->next;
        free(prec);
        prec = *r;
    }
}


// Fonction qui supprime et libere toute la mémoire associée à l'arbre .

void purgeTree(void *root){}


int parseur(char *req, int len){
    
    extern Element *isHTTPMessage(char *text, ssize_t len);

    Element *line = isHTTPMessage(req, len);

    if (line == NULL) {
        printf("Erreur dans la lecture du message\n");
        exit(1);
    }
    else { printArbre(line, 0); }
}