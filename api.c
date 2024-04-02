#include "api.h"
#include "isX.h"

struct Element *data; //racine de l'arbre


// Fonction qui recherche dans l'arbre tous les noeuds dont l'etiquette est egale à la chaine de caractères en argument.   
// Par convention si start == NULL alors on commence à la racine 
// sinon on effectue une recherche dans le sous-arbre à partir du noeud start 
_Token *searchTree(void *start,char *name){
    
    struct _token start;

    if(start == NULL){
        start = getRootTree();
    }

    while(!(data->frere==NULL && data->fils==NULL)){
        struct Element *racine = data;
        while(data->frere != NULL){
            if(strcmp(data->word,name)){
                start = data;
                start = start->next;
            }

        }
    }
}


// Fonction qui supprime et libere la liste chainée de reponse. 

void purgeElement(_token **r){
    struct _token *prec = *r;
    while(*r->next != NULL){
        r = r->suivant;
        free(prec);
        prec = *r
    }
}

// Fonction qui supprime et libere toute la mémoire associée à l'arbre .

void purgeTree(void *root); 