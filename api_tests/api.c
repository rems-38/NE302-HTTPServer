#include "api.h"
#include "isX.h"

struct Element *line = NULL;

// Fonction qui retourne un pointeur (type opaque) vers la racine de l'arbre construit. 
void *getRootTree(){
    return line;
}

// Fonction qui recherche dans l'arbre tous les noeuds dont l'etiquette est egale à la chaine de caractères en argument.   
// Par convention si start == NULL alors on commence à la racine 
// sinon on effectue une recherche dans le sous-arbre à partir du noeud start 
_Token *searchTree(void *start,char *name){
    
    struct _token *tree = malloc(sizeof(struct _token));

    if(start == NULL){ // on de la racine de l'arbre
        start = getRootTree();
    }

    struct Element *data = start; // on part de l'element data qui pointe vers start


    if (strcmp(data->word,name)){
        tree->node = &data; // ajouter le noeud à l'arbre
        tree = tree->next;  
    }

    // On explore l'arbre

    if(data->fils == NULL && data->frere == NULL){
        return tree;
    }

    else if(data->fils==NULL){
        tree = searchTree(data->frere,name);
    }

    else if (data->frere==NULL){
        tree = searchTree(data->fils,name);
    }
    else{
        struct _token *tree1 = searchTree(data->frere,name);
        struct _token *tree2 = searchTree(data->fils,name);
        tree = tree1;
        struct _token *fin_tree1 = tree1;
        while (fin_tree1-> next != NULL){ // trouver la fin de tree1 pour joindre tree1 et tree2
            fin_tree1 = fin_tree1 -> next;
        }
        fin_tree1->next = tree2;
    }
    return NULL;
}

// fonction qui renvoie un pointeur vers char indiquant l'etiquette du noeud. (le nom de la rulename, intermediaire ou terminal) 
// et indique (si len!=NULL) dans *len la longueur de cette chaine.
char *getElementTag(void *node,int *len){
    struct Element *data = node;
    *len = data -> length;
    return data->key;
}

// fonction qui renvoie un pointeur vers char indiquant la valeur du noeud. (la partie correspondnant à la rulename dans la requete HTTP ) 
// et indique (si len!=NULL) dans *len la longueur de cette chaine.
char *getElementValue(void *node,int *len){
    struct Element *data = node;
    *len = data -> length;
    return data->key;
}

// Fonction qui supprime et libere la liste chainée de reponse. 

void purgeElement(_Token **r){
    struct _token *prec = (*r);
    while((*r)->next != NULL){
        (*r) = (*r)->next;
        free(prec);
        prec = (*r);
    }
}


// Fonction qui supprime et libere toute la mémoire associée à l'arbre.

void purgeTree(void *root){
    struct Element *data = root;
    if(data->fils == NULL && data->frere != NULL){
        purgeTree(data->frere);
        free(data);
    }
    else if(data->frere == NULL && data->fils != NULL){
        purgeTree(data->fils);
        free(data);
    }
    else if(data->fils != NULL && data->frere != NULL){
        purgeTree(data->frere);
        purgeTree(data->fils);
        free(data);
    }
}


int parseur(char *req, int len){

    line = isHTTPMessage(req, len);

    if (line == NULL) {
        printf("Erreur dans la lecture du message\n");
        return -1;
    }
    else { 
        printArbre(line, 0);
        return 0;
        }
}

int main(){ //test
    char *text = "GET / HTTP/1.1\r\nUser-Agent: Wget/1.16 (linux-gnu)\r\nAccept: */*\r\nHost: www.google.com\r\nConnection: Keep-Alive\r\n";
    parseur(text,153);

    searchTree(line,"pchar");

    return 0;
}