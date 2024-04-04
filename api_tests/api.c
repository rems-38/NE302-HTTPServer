#include "api.h"
#include "isX.h"

/*
typedef struct Element {
    char *key;
    char *word;
    size_t length;
    struct Element *fils;
    struct Element *frere;
} Element;
*/


struct Element *line;

// Fonction qui retourne un pointeur (type opaque) vers la racine de l'arbre construit. 
void *getRootTree(){
    return (void *)line;
}

// Fonction qui recherche dans l'arbre tous les noeuds dont l'etiquette est egale à la chaine de caractères en argument.   
// Par convention si start == NULL alors on commence à la racine 
// sinon on effectue une recherche dans le sous-arbre à partir du noeud start 
_Token *searchTree(void *start,char *name){
    
    struct _token *tree = malloc(sizeof(struct _token));

    if(start == NULL){ // on de la racine de l'arbre
        start = getRootTree();
    }

    struct Element *data = (struct Element *)start; // on part de l'element data qui pointe vers start


    if (strcmp(data->key,name)==0){
        tree->node = &data; // ajouter le noeud à l'arbre
        tree = tree->next;  
    }

    // On explore l'arbre

    if(data->fils == NULL && data->frere == NULL){
        return tree;
    }

    else if(data->fils==NULL){
        tree = searchTree(data->frere,name);
        return tree;
    }

    else if (data->frere==NULL){
        tree = searchTree(data->fils,name);
        return tree;
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
        return tree;
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
    return data->word;
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
    struct Element *data = (struct Element *)root;
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

    line = malloc(sizeof(Element));
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
    char *text = "VWbG1 /m/JeAk HTTP/0.8\r\n\r\n";
    parseur(text,strlen(text));

    struct Element *phrase = malloc(sizeof(Element));
    struct Element *sujet = malloc(sizeof(Element));
    struct Element *verbe = malloc(sizeof(Element));
    struct Element *complement = malloc(sizeof(Element));

    phrase->key = "phrase";
    phrase->word = "je m'appelle Pierre";
    phrase->length = 7;

    phrase->fils = sujet;
    sujet->key = "sujet";
    sujet->word = "je";

    sujet->frere = verbe;
    verbe->key = "verbe";
    verbe->word = "m'appelle";

    verbe->frere = complement;
    complement->key = "complement";
    complement->word = "Pierre";

    struct Element *racine = getRootTree();
    printf("racine : %s\n",racine->key);

    _Token *data = searchTree((void*)phrase,"verbe");
    struct Element *conversion = data->node;
    char *word=conversion->word; //SEGFAULT ici
    printf("word %s\n",word);

    int *len = malloc(sizeof(int));

    printf("Tag : %s\n",getElementTag((void *)phrase,len));
    printf("Value : %s\n",getElementValue((void *)phrase,len));

    return 0;
}