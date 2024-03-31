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

bool isSP(char text, size_t *curr, Element *head){ // /!\ attention à l'ajout de l'élément SP en tant que frère de la tete
    Element *el = addEl("__sp", &text, 1);
    head->frere = el;
    *curr += 1;

    return (text == ' ');
}
/*
bool isHTAB(char text, size_t *curr, Element *head) {
    Element *el = addEl("__htab", &text, 1);
    head->frere = el;
    *curr += 1;

    return (text == HTAB);
}
*/
bool isRequestLine(char *text, size_t *curr, Element *head){
    bool res = true;

    Element *tmp = malloc(sizeof(Element)); //contruction du sous-arbre pour ensuite le relier a "request-line" car on ne connait pas encore la taille pour créer l'élément "request-line"
    //head->fils = tmp;

    if(!isMethod(text, curr, tmp)) {res = false;}
    tmp = tmp->fils; //method devient la tete
    Element *save = tmp;
    

    if(!isSP(text[*curr], curr, tmp)) {res = false;}
    tmp = tmp->frere; //SP devient la tete


    if(!isRequestTarget(text+(*curr), curr, tmp)) {res = false;}
    tmp = tmp->frere; //request-target devient la tete
/*
    if(!isSP(text[*curr], curr, tmp)) {res = false;}
    tmp = tmp->frere; //SP devient la tete

    if(!isHTTPVersion(text, curr, tmp)) {res = false;}
    tmp = tmp->frere; //HTTP-version devient la tete
    */

    //if(!isCRLF(text, curr, tmp)) {res = false;}
    
    Element *el = addEl("request_line", text, *curr);
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
        tmp = addEl("tchar", text+icurr, 1);
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
        *curr += icurr;
        return true;
    }
}

bool isRequestTarget(char *text, size_t *curr, Element *head){ // request-target = origin-form      //head = SP
    bool res = false;
    size_t curr_mem = *curr;            //sauvegarde de curr pour pouvoir définir la taille de request-target ensuite
    Element *tmp = malloc(sizeof(Element));

    if(isOriginForm(text, curr, tmp)){res = true;}
    Element *el = addEl("request-target", text, (*curr)-curr_mem); 
    head->frere = el; //request target devient le frere de SP          // maintenant qu'on a la longueur de request-target on peut l'ajouter à l'arbre
    el->fils = tmp->fils; //Origin form devient le fils de request target
    free(tmp);
    return res;
}

bool isOriginForm(char *text, size_t *curr, Element *head){ // origin-form = absolute-path [ "?" query ]
    
    size_t curr_mem = *curr; 
    Element *tmp = malloc(sizeof(Element));

    if(!isAbsolutePath(text, curr, tmp)){return false;}

    /*

    if(strcmp(text+(*curr),"?")){
        *curr += 1;
        if (isQuery(text, curr, tmp)){
            curr
        }
    }

    */
    Element *el = addEl("origin-form", text, (*curr)-curr_mem);
    head->fils = el;
    el->fils = tmp->fils;
    
    return true;
}

bool isAbsolutePath(char *text, size_t *curr, Element *head){ // absolute-path = 1*( "/" segment )
    
    Element *tmp = malloc(sizeof(Element));
    Element *c = malloc(sizeof(Element));
    Element *save = c;    //head->fils = addEl("absolute-path", text, 0);
    
    size_t curr_mem = *curr;
    size_t icurr = 0;
    bool boucle = true;

    while(boucle){
        if(strcmp(text+(*curr),"/")){
            tmp = addEl("__icar", "/", 1);
            c->frere = tmp;
            c = c->frere;
            (*curr)++;
        }
        else{
            boucle = false;
        }
        if(isSegment(text+(*curr),curr,c)){ //appel avec head=c="/"
            (*curr)++;
            c = c->frere; //c devient segment
        }
        else{
            boucle = false;
        }
        icurr++;
    }

    if (icurr == 0){
        return false;
    }

    //*curr += icurr;
    Element *el = addEl("absolute-path", text, (*curr)-curr_mem);
    head->fils = el;
    el->fils = save->frere;
    return true;
}

bool isSegment(char *text, size_t *curr, Element *head){ // segment = *pchar         //head="/"
    bool boucle = true;

    Element *tmp = addEl("tmp","text",strlen("text"));      //malloc(sizeof(Element));
    Element *c = malloc(sizeof(Element));
    Element *save=c;
    
    size_t curr_mem = *curr;
    //printf("Dans isSegment text vaut : -%s-",text);
    while(boucle){
        if(isPchar(text,curr,tmp)){
            c->frere = tmp; //->fils;
            c = c->frere;
        }
        else{
            boucle = false;
        }
    }
    
    Element *el = addEl("segment", text,(*curr)-curr_mem);
    head->frere = el; // segment devient le frere de head="/"
    el->fils = save->frere;
    
    return true;
}


bool isPchar(char *text, size_t *curr, Element *head){  // pchar = unreserved / pct-encoded / sub-delims / ":" / "@"

    Element *tmp = malloc(sizeof(Element));
    //Element *el;
    if(isUnreserved(*(text+(*curr)),tmp)){
        head = addEl("pchar",text,1);
        //head->fils = el;
        head->fils = tmp->fils;
        *curr += 1;
        return true;
    }/*
    else if(isPctEncoded(text+(*curr),curr,tmp)){
        el = addEl("pchar",text,3);
        head->fils= el;
        el->fils = tmp->fils;
        return true;
    }
    else if(isSubDelims(*(text+(*curr)),tmp)){
        el = addEl("pchar",text,1);
        head->fils= el;
        el->fils = tmp->fils;
        return true;
    }
    else if(strcmp(text+(*curr),":")){
        el = addEl("pchar",text,1);
        head->fils= el;
        el->fils = addEl("colon", text+(*curr), 1);
        *curr += 1;
        return true;
    }
    else if(strcmp(text,"@")){
        el = addEl("pchar",text,1);
        head->fils= el;
        el->fils = addEl("at-sign", text+(*curr), 1);
        *curr += 1;
        return true;
    }*/
    return false;
}

bool isUnreserved(char text, Element *head){    //unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~"
    //printf("Dans isUreserverd text vaux : %c\n",text);
    head->fils = addEl("unreserved",&text,1);
    return (isAlpha(text)||isDigit(text)||text==DASH||text == DOT||text == UNDERSCORE||text == VAGUE);  
}
/*
bool isPctEncoded(char *text, size_t *curr, Element *head){ //pct-encoded = "%" HEXDIG HEXDIG
    head->fils = addEl("pct-encoded",text,3);
    head = head->fils;

    if(*text == '%'){
        head->fils = addEl("pourcent",text,1);
        *curr=+1;
        head = head->fils;
        
        if(isHEXDIG(*(text+1))){
            head->frere = addEl("hexdig",text+1,1);
            *curr+=1;
            head = head->frere;
        
            if(isHEXDIG(*(text+2))){
                head->frere = addEl("hexdig",text+2,1);
                *curr+=1;
                return true;
            }
        }
    } 
    return false;
}

bool isSubDelims(char text, Element *head){ // sub-delims = "!" / "$" / "&" / "'" / "(" / ")"/ "*" / "+" / "," / ";" / "="
    head->fils = addEl("sub-delims",&text,1);
    return(text == '!' || text == '*'|| text == '&'|| text == SQUOTE || text == '(' || text == ')' || text == '*' || text == '+' || text == ',' || text == ';' || text == '=');
}*/

/*
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

bool isMessageBody(char *text, size_t *curr, Element *head){
    
    Element *el = addEl("message-body",text,8);
    head -> fils = el;

    if(!isOCTET(text+(*curr))){
        return false;
    }

    *curr += 8;

    return true;
}


*/

/*
bool isConnectionHeader(char *text, size_t *curr, Element *head) {
}

bool isCookieHeader(char *text, size_t *curr, Element *head) {
    if (!strcmp(text+*curr, "Cookie:")) { return false; }
    *curr += 7;

    while (isOWS(text+(*curr)), curr, head)) {
        continue;
    }

    if (!isCookieString(text+(*curr), curr, head)) { return ;alse
    }
                        
}


bool isOWS(char *text, size_t *curr, Element *head) {
    while (isSP(*(text+*curr)) || isHTAB(*text+*curr)) {
        continue;
    }
    return false;
}
*/

bool isTchar(char text){ // tchar = ("!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA)
    return (text == EXCLAMATION || text == HASHTAG || text == DOLLAR || text == POURCENT || text == ESP || text == SQUOTE || text == STAR || text == PLUS || text == DASH || text == DOT || text == CIRCONFLEXE || text == UNDERSCORE || text == 96 || text == BARRE || text == VAGUE || isAlpha(text) || isDigit(text)) ;
}

bool isOCTET(char text){
    for(int i=0; i<8 ; i++){
        if(!(text+i == 1 || text+i == 0)){
            return false;
        }
    }
    return true;
}

bool isAlpha(char text){
    return (text >= AMAJ && text <= ZMAJ) || (text >= AMIN && text <= ZMIN);
}

bool isDigit(char text) {
    return (text >= ZERO && text <= NINE);
}

bool isHEXDIG(char text){ // HEXDIG  =  DIGIT / "A" / "B" / "C" / "D" / "E" / "F"
    return((text>=ZERO && text<=NINE) || (text>=AMAJ && text<=FMAJ));
}
