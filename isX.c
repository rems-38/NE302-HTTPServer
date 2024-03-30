#include "isX.h"



bool isRequestLine(char *text, size_t *curr, Element *head){
    bool res = true;

    Element *tmp = malloc(sizeof(Element)); //contruction du sous-arbre pour ensuite le relier a "request-line" car on ne connait pas encore la taille pour créer l'élément "request-line"
    Element *save = tmp;
    
    if(!isMethod(text, curr, tmp)) {res = false;}
    tmp = tmp->fils; //method devient la tete

    if(!isSP(text[*curr], curr, tmp)) {res = false;}
    tmp = tmp->frere; //SP devient la tete

    if(!isRequestTarget(text, curr, tmp)) {res = false;}
    tmp = tmp->frere; //request-target devient la tete

    if(!isSP(text[*curr], curr, tmp)) {res = false;}
    tmp = tmp->frere; //SP devient la tete

    if(!isHTTPVersion(text, curr, tmp)) {res = false;}
    tmp = tmp->frere; //HTTP-version devient la tete

    //if(!isCRLF(text, curr, tmp)) {res = false;}
    
    Element *el = addEl("request-line", text, *curr);
    head->fils = el; 
    head->fils->fils = save->frere;
    return res;
}

bool isMethod(char *text, size_t *curr, Element *head){
    bool res = false;

    Element *tmp = malloc(sizeof(Element));
    
    if(isToken(text, curr, tmp)) {res = true;}
        
    Element *el = addEl("method", text, *curr);
    head->fils = el;
    el->fils = tmp;
    return res;
}


bool isToken(char *text,size_t *curr, Element *head){ //token = 1*tchar
    
    size_t icurr = 0;

    Element *c = malloc(sizeof(Element));
    Element *save_c = c; //pointeur vers l'Element c pour plus tard
    Element *tmp = malloc(sizeof(Element)); //pour l'ajout des tchar

    while(isTchar(text[*curr], curr)){
        tmp = addEl("__tchar", text+icurr, 1);
        c->frere = tmp;
        c = c->frere;
        icurr += 1;                          
    }

    Element *el = addEl("token", text, icurr+1);
    head->frere = el;
    head->frere->fils = save_c->frere;

    if(icurr == 0){
        return false;
    }
    else{
        *curr += icurr + 1;
        return true;
    }
}

bool isRequestTarget(char *text, size_t *curr, Element *head){
    bool res = false;

    Element *el = addEl("request-target", text, 0);
    head->fils = el;
    head = head->fils;

    if(isOriginForm(text, curr, head)){res = true;}

    return res;
}

bool isOriginForm(char *text, size_t *curr, Element *head){

    Element *el = addEl("origin-form", text, 0);
    head->fils = el;
    head = head->fils;

    if(!isAbsolutePath(text, curr, head)){return false;}

    //["?" query] = ???

    return true;
}

bool isAbsolutePath(char *text, size_t *curr, Element *head){
    size_t icurr = 0;
    bool boucle = true;

   /*while(boucle){
        if(isSLASH(text, &curr, head)){
            
        }

    }*/

    
}

bool isHTTPVersion(char *text, size_t *curr, Element *head){ //HTTP-version = HTTP-name "/" DIGIT "." DIGIT
    head->frere = addEl("HTTP-version", text, 8); //HTTP-version devient la tete
    head = head->frere; ///je fais des fils  a SP

    if(!isHTTPname(text,curr)){
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

bool isHTTPname(char *text, size_t *curr){ //HTTP-name = HTTP
    return strcmp(text,"HTTP");
}


bool isTchar(char text, size_t *curr){ // tchar = ("!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA)
    return (text[curr] == EXCLAMATION || text[curr] == HASHTAG || text[curr] == DOLLAR || text[curr] == POURCENT || text[curr] == ESP || text[curr] == SQUOTE || text[curr] == STAR || text[curr] == PLUS || text[curr] == DASH || text[curr] == DOT || text[curr] == CIRCONFLEXE || text[curr] == UNDERSCORE || text[curr] == 96 || text[curr] == BARRE || text[curr] == VAGUE || isAlpha(text[curr]) || isDigit(text[curr])) ;
}

bool isAlpha(char text){
    return (text >= AMAJ && text <= ZMAJ) || (text >= AMIN && text <= ZMIN);
}

bool isDigit(char text) {
    return (text >= ZERO && text <= NINE);
}

bool isSP(char text, size_t *curr, Element *head){ // /!\ attention à l'ajout de l'élément SP en tant que frère de la tete
    Element *el = addEl("SP"," ",1);
    head->frere = el;
    *curr ++;

    return (text == ' ');
}
