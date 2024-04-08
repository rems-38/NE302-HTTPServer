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
_Token *searchTree(void *start,char *name){ // _Token == struct _token
    
    struct _token *chaine = malloc(sizeof(_Token));
    struct _token *head = chaine;

    if(start == NULL){ // on récupère la racine de l'arbre
        start = getRootTree();
    }

    struct Element *data = (struct Element *)start; // on part de l'element data qui pointe vers start


    if (strcmp(data->key,name) == 0){
        chaine->node = &data; // ajouter le noeud à l'arbre
        chaine = chaine->next;  
    }

    // On explore l'arbre

    if(data->fils == NULL && data->frere == NULL){
        return chaine;
    }

    else if(data->fils==NULL){
        chaine = searchTree(data->frere,name);
        return chaine;
    }

    else if (data->frere==NULL){
        chaine = searchTree(data->fils,name);
        return chaine;
    }
    else{
        struct _token *chaine1 = searchTree(data->frere,name);
        struct _token *chaine2 = searchTree(data->fils,name);
        chaine = chaine1;
        struct _token *fin_chaine1 = chaine1;
        while (fin_chaine1-> next != NULL){ // trouver la fin de chaine1 pour joindre chaine1 et chaine2
            fin_chaine1 = fin_chaine1 -> next;
        }
        fin_chaine1->next = chaine2;
        return chaine;
    }
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
        free(prec->node);
        free(prec);
        prec = (*r);
    }
}


// Fonction qui supprime et libere toute la mémoire associée à l'arbre.

void purgeTree(void *root){
    struct Element *data = (struct Element *)root;
    if(data->fils == NULL && data->frere != NULL){
        purgeTree((void *)data->frere);
        free(data->key);
        free(data->word);
        free(data);
    }
    else if(data->frere == NULL && data->fils != NULL){
        purgeTree((void *)data->fils);
        free(data->key);
        free(data->word);
        free(data);
    }
    else if(data->fils != NULL && data->frere != NULL){
        purgeTree((void *)data->frere);
        purgeTree((void *)data->fils);
        free(data->key);
        free(data->word);
        free(data);
    }
    else{ // data->fils == NULL && data->frere == NULL
        free(data->key);
        free(data->word);
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

    // test parseur
    char *text = "VWbG1 /m/JeAk HTTP/0.8\r\n\r\n";
    parseur(text,strlen(text));

    // arbre de test
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

    // test getRootTree(), getElementTag(), GetElementValue() et purgeTree()
    /*
    struct Element *racine = getRootTree();
    printf("racine : %s\n",racine->key);

    int *len = malloc(sizeof(int));

    printf("Tag : %s\n",getElementTag((void *)phrase,len));
    printf("Value : %s\n",getElementValue((void *)phrase,len));

    purgeTree(getRootTree());
    printArbre(getRootTree(),0);
    */

   // test searchTree()

    _Token *data = searchTree((void*)phrase,"verbe");// data non NULL

    struct Element *conversion = data->node;
    printf("word %s\n",conversion->word);

    return 0;
}