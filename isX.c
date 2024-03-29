#include "isX.h"



bool isRequestLine(char *text, size_t *curr, Element *head){
    bool res = true;
    
    Element *el = addEl("request-line", &text, 0);
    head->fils = el;
    head = head->fils;

    if(!isMethod(text, &curr, head)) {res = false;}
    head = head->fils; //method devient la tete

    if(!isSP(text, &curr, head)) {res = false;}
    head = head->frere; //SP devient la tete

    if(!isRequestTarget(text, &curr, head)) {res = false;}
    head = head->frere; //request-target devient la tete

    if(!isSP(text, &curr, head)) {res = false;}
    head = head->frere; //SP devient la tete

    if(!isHttpVersion(text, &curr, head)) {res = false;}
    head = head->frere; //HTTP-version devient la tete

    if(!isCRLF(text, &curr, head)) {res = false;}
    
    return res;
}

bool isMethod(char *text, size_t *curr, Element *head){
    bool res = false;
    
    Element *el = addEl("method", &text, 0);
    head->fils = el;

    if(isToken(text, &curr, head->fils)) {res = true};
    
    
    return res;
}


bool isToken(char *text,size_t *curr, Element *head){ //token = 1*tchar
    
    size_t icurr = 0;

    Element *c = malloc(sizeof(Element));
    Element *save_c = c; //pointeur vers l'Element c pour plus tard
    Element *tmp = malloc(sizeof(Element)); //pour l'ajout des tchar

    while(isTchar(text, &curr, head)){
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

    Element *el = addEl("request-target", &text, 0);
    head->fils = el;
    head = head->fils;

    if(isOriginForm(text, size_t *curr, Element *head)){res = true;}

    return res;
}

bool isOriginForm(char *text, size_t *curr, Element *head){

    Element *el = addEl("origin-form", &text, 0);
    head->fils = el;
    head = head->fils;

    if(!isAbsolutePath(text, &curr, head)){return false;}

    //["?" query] = ???

    return true;
}

bool isAbsolutePath(char *text, size_t *curr, Element *head){
    size_t icurr = 0;
    bool boucle = true;

    while(boucle){
        if(isSLASH(text, &curr, head))

    }

    
}




bool isTchar(char text){ // tchar = ("!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA)
    return (text[curr] == EXCLAMATION || text[curr] == HASHTAG || text[curr] == DOLLAR || text[curr] == POURCENT || text[curr] == ESP || text[curr] == SQUOTE || text[curr] == STAR || text[curr] == PLUS || text[curr] == DASH || text[curr] == DOT || text[curr] == CIRCONFLEXE || text[curr] == UNDERSCORE || text[curr] == 96 || text[curr] == BARRE || text[curr] == VAGUE || isAlpha(text[curr]) || isDigit(text[curr])) ;
}

bool isAlpha(char text){
    return (text >= AMAJ && text <= ZMAJ) || (text >= AMIN && text <= ZMIN);
}

bool isDigit(char text) {
    return (text >= ZERO && text <= NINE);
}

bool isSP(char text, size_t *curr, Element *head){ // /!\ attention à l'ajout de l'élément SP en tant que frère de la tete
    Element *el = addEl("SP"," ",1)
    head->frere = el;
    *curr ++;

    return (text == " ");
}
