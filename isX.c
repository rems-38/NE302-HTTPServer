#include "isX.h"

/*Element *addEl(char *key, char *word, size_t length) {
    Element *el = malloc(sizeof(Element));
    
    el->key = malloc(strlen(key)+1);
    el->word = malloc(length+1);
    strncpy(el->key, key, strlen(key));
    strncpy(el->word, word, length);
    el->length = length;
    el->fils = NULL;
    el->frere = NULL;
    
    return el;
}*/

bool isRequestLine(char *text, size_t *curr, Element *head){
    bool res = true;

    /*Element *tmp = malloc(sizeof(Element));
    head->fils = tmp;                                 // on créer maintenant l'élément qu'on donne a isRequestLine 
    if(isRequestLine(text, curr, tmp)){res = true;}
    // mettre le if avant pour avoir curr pour la taille dans la start-line ??
    Element *el = addEl("start-line", text, *curr); //quelle valeure mettre pour length??
    head->fils = el;
    el->fils = tmp;*/


    Element *tmp = malloc(sizeof(Element)); //contruction du sous-arbre pour ensuite le relier a "request-line" car on ne connait pas encore la taille pour créer l'élément "request-line"
    //head->fils = tmp;

    
    if(!isMethod(text, curr, tmp)) {res = false;}
    tmp = tmp->fils; //method devient la tete
    Element *save = tmp;
    
    /*
    if(!isSP(text[*curr], curr, tmp)) {res = false;}
    tmp = tmp->frere; //SP devient la tete

    if(!isRequestTarget(text, curr, tmp)) {res = false;}
    tmp = tmp->frere; //request-target devient la tete

    if(!isSP(text[*curr], curr, tmp)) {res = false;}
    tmp = tmp->frere; //SP devient la tete

    if(!isHTTPVersion(text, curr, tmp)) {res = false;}
    tmp = tmp->frere; //HTTP-version devient la tete
    */

    //if(!isCRLF(text, curr, tmp)) {res = false;}
    
    Element *el = addEl("request-line", text, *curr);
    head->fils = el; 
    head->fils->fils = save;
    return res;
}

bool isMethod(char *text, size_t *curr, Element *head){
    bool res = false;

    Element *tmp = malloc(sizeof(Element));
    
    if(isToken(text, curr, tmp)) {res = true;}
        
    Element *el = addEl("method", text, *curr);
    head->fils = el;
    el->fils = tmp->fils;
    return res;
}


bool isToken(char *text,size_t *curr, Element *head){ //token = 1*tchar
    
    size_t icurr = 0;

    Element *c = malloc(sizeof(Element));
    Element *save_c = c; //pointeur vers l'Element c pour plus tard
    Element *tmp = malloc(sizeof(Element)); //pour l'ajout des tchar

    while(isTchar(text[icurr+(*curr)])){
        tmp = addEl("__tchar", text+icurr, 1);
        c->frere = tmp;
        c = c->frere;
        icurr += 1;                         
    }

    Element *el = addEl("token", text, icurr+1);
    head->fils = el;
    el->fils = save_c->frere;

    if(icurr == 0){
        return false;
    }
    else{
        *curr += icurr + 1;
        return true;
    }
}
/*
bool isRequestTarget(char *text, size_t *curr, Element *head){ // request-target = origin-form
    bool res = false;
    size_t curr_mem = *curr;            //sauvegarde de curr pour pouvoir définir la taille de request-target ensuite
    
    Element *el = malloc(sizeof(Element));

    if(isOriginForm(text, curr, el)){res = true;}
    el = 
    head-> // maintenant qu'on a la longueur de request-target on peut l'ajouter à l'arbre

    return res;
}

bool isOriginForm(char *text, size_t *curr, Element *head){ // origin-form = absolute-path [ "?" query ]
    
    size_t curr_mem = *curr; 
    head = head->fils;

    if(!isAbsolutePath(text, curr, head)){return false;}

    //["?" query] = ???
    head = addEl("request-target", text, (*curr)-curr_mem);
    
    
    return true;
}

bool isAbsolutePath(char *text, size_t *curr, Element *head){ // absolute-path = 1*( "/" segment )
    
    head->fils = addEl("absolute-path", text, 0);
    
    size_t icurr = 0;
    bool boucle = true;

    while(boucle){
        if(!strcmp(text+icurr+(*curr),"/")){
            boucle = false;
        }

        icurr += 1;

        if(!isSegment(text+icurr,curr,head)){
            boucle = false;
        }
    }

    if (icurr == 0){
        return false;
    }

    *curr += icurr;

    return true;
}


bool isSegment(char *text, size_t *curr, Element *head){ // segment = *pchar
    bool boucle = true;

    while(boucle){
        if(!isPchar(text,curr,head)){
            boucle = false;
        }
    }
    return true;
}

bool isPchar(char *text, size_t *curr, Element *head){

}

bool isHTTPVersion(char *text, size_t *curr, Element *head){ //HTTP-version = HTTP-name "/" DIGIT "." DIGIT
    head->frere = addEl("HTTP-version", text, 8); //HTTP-version devient la tete
    head = head->frere; ///je fais des fils  a SP

    if(!isHTTPname(text+(*curr))){
        return false;
    }

    head->fils = addEl("HTTP-name",text+(*curr), 4);
    head = head->fils; // head deviens HTTP-name
    curr += 4;

    if(!strcmp(text+(*curr),"/")){
        return false;
    }

    head->frere = addEl("ponct", text+(*curr), 1);
    head = head->frere; // head deviens ponct 
    curr += 1;

    if(!isDigit(*(text+(*curr)))){
        return false;
    }

    head->frere = addEl("DIGIT",text+(*curr), 1);
    head = head->frere; //head devient DIGIT
    curr += 1;

    if(!strcmp(text+(*curr),".")){
        return false;
    }

    head->frere = addEl("ponct",text+(*curr), 1);
    head = head->frere;
    curr += 1;

    if(!isDigit(*(text+(*curr)))){
        return false;
    }

    head->frere = addEl("DIGIT",text+(*curr), 1);
    curr += 1;

    return true;
}

bool isHTTPname(char *text){ //HTTP-name = HTTP
    return strcmp(text,"HTTP");
}

bool isMessageBody(char *text, size_t *curr){

    if(!isOCTET(text+(*curr))){
        return false;
    }

    *curr += 8;

    return true;
}

bool isOCTET(char text){
    for(int i=0; i<8 ; i++){
        if(!(text+i == 1 || text+i = 0)){
            return false;
        }
    }
    return true;
}
*/

bool isTchar(char text){ // tchar = ("!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA)
    return (text == EXCLAMATION || text == HASHTAG || text == DOLLAR || text == POURCENT || text == ESP || text == SQUOTE || text == STAR || text == PLUS || text == DASH || text == DOT || text == CIRCONFLEXE || text == UNDERSCORE || text == 96 || text == BARRE || text == VAGUE || isAlpha(text) || isDigit(text)) ;
}


bool isAlpha(char text){
    return (text >= AMAJ && text <= ZMAJ) || (text >= AMIN && text <= ZMIN);
}

bool isDigit(char text) {
    return (text >= ZERO && text <= NINE);
}
/*
bool isSP(char text, size_t *curr, Element *head){ // /!\ attention à l'ajout de l'élément SP en tant que frère de la tete
    Element *el = addEl("SP"," ",1);
    head->frere = el;
    *curr += 1;

    return (text == ' ');
}
*/