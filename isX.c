#include "isX.h"


bool isAlpha(char text){
    return (text >= AMAJ && text <= ZMAJ) || (text >= AMIN && text <= ZMIN);
}

bool isDigit(char text) {
    return (text >= ZERO && text <= NINE);
}

bool isTchar(char text){ // tchar = ("!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA)
    return (text == EXCLAMATION || text == HASHTAG || text == DOLLAR || text == POURCENT || text == ESP || text == SQUOTE || text == STAR || text == PLUS || text == DASH || text == DOT || text == CIRCONFLEXE || text == UNDERSCORE || text == FQUOTE || text == BARRE || text == VAGUE || isAlpha(text) || isDigit(text)) ;
}

bool isOCTET(char text){
    for(int i=0; i<8 ; i++){
        if(!(text+i == 1 || text+i == 0)){
            return false;
        }
    }
    return true;
}

bool isHEXDIG(char text){ // HEXDIG  =  DIGIT / "A" / "B" / "C" / "D" / "E" / "F"
    return((text>=ZERO && text<=NINE) || (text>=AMAJ && text<=FMAJ));
}

bool isSP(char text, size_t *curr, Element *head){ // /!\ attention à l'ajout de l'élément SP en tant que frère de la tete
    Element *el = addEl("__sp", &text, 1);
    head->frere = el;
    *curr += 1;

    return (text == ' ');
}

// OWS = *( SP / HTAB )
bool isOWS(char *text, size_t *curr, Element *data, bool is_fils) { //import
    Element *el = addEl("OWS", text, strlen(text));
    if (is_fils) {
        data->fils = el;
        data = data->fils; //OWS devient la tete
    } else {
        data->frere = el;
        data = data->frere; //idem
    }
    Element *save = data;

    size_t count = 0;
    Element *sub;
    while (text[count] == SP || text[count] == HTAB) {
        if (text[count] == SP) { sub = addEl("__sp", text+count, 1); }
        else if (text[count] == HTAB) { sub = addEl("__htab", text+count, 1); }

        if (count == 0) { // ou el->fils == NULL mais vu qu'on change el faudrait le save...
            data->fils = sub;
            data = data->fils;
        }
        else {
            data->frere = sub;
            data = data->frere;
        }
        count++;
    }

    updateLength(save, count);
    *curr += count;
    return true;
}

int OWSCH(char *text){ //OWS ConnectionHeader
    size_t i = 0;
    while(text[i] == SP || text[i] == HTAB){
        i++;
    }

    if(text[i] == COMMA) {
        return 1; //cas OWS","
    }
    else if(text[i] == CR && text[i+1] == LF){ //cas OWS CRLF : on sort de Transfert-Encoding
        return 3;
    }
    else if(text[i] == EXCLAMATION || text[i] == HASHTAG || text[i] == DOLLAR || text[i] == POURCENT || text[i] == ESP || text[i] == SQUOTE || text[i] == STAR || text[i] == PLUS || text[i] == DASH || text[i] == DOT || text[i] == CIRCONFLEXE || text[i] == UNDERSCORE || text[i] == 96 || text[i] == BARRE || text[i] == VAGUE || isAlpha(text[i]) || isDigit(text[i])){
        //cas OWS connection-option = token = t-char
        //printf("dans OWS text+i : -%s-\n",text+i);
        return 2;
    }
    else { return 4; } //cas erreur
}

// token = 1*tchar
bool isToken(char *text, size_t *curr, Element *data, bool is_fils) {
    size_t icurr = 0;

    Element *el = addEl("token", text, strlen(text));
    if (is_fils) {
        data->fils = el;
        data = data->fils;
    } else {
        data->frere = el;
        data = data->frere;
    }
    Element *save = data;

    Element *tmp; //pour l'ajout des tchar
    while(isTchar(*(text+icurr))){
        tmp = addEl("tchar", text+icurr, 1);
        if (icurr == 0) {
            data->fils = tmp;
            data = data->fils;
        } else {
            data->frere = tmp;
            data = data->frere;
        }
        icurr += 1;
    }

    if(icurr == 0){
        return false;
    }
    else{
        data = save;
        updateLength(data, icurr);
        *curr += icurr;
        return true;
    }
}

// obs-text = %x80-FF
bool isObsText(char text) {
    // d'après gcc la condition est toujours FALSE et TRUE en même temps
    // je comprends pas trop mais 
    return (text >= 128 && text <= 255);
}

// field-name = token
bool isFieldName(char *text, size_t *curr, Element *data) {
    Element *el = addEl("field-name", text, strlen(text));
    data->fils = el;
    data = data->fils;

    if (!isToken(text, curr, data, true)) { return false; }

    updateLength(data, *curr);
    return true;
}

// field-vchar = VCHAR / obs-text
bool isFieldVchar(char text, Element *data) {
    Element *el = addEl("field-vchar", &text, 1);
    data->frere = el;
    data = data->frere;

    Element *sub;
    if (text >= EXCLAMATION && text <= VAGUE) { sub = addEl("__vchar", &text, 1); }
    else if (isObsText(text)) { sub = addEl("obs-text", &text, 1); }
    else { return false; }

    data->fils = sub;
    data = data->fils;

    return true;
}

// field-content = field-vchar [ 1*( SP / HTAB ) field-vchar ]
bool isFieldContent(char *text, size_t *curr, Element *data) {
    Element *el = addEl("field-content", text, strlen(text));
    data->fils = el;
    data = data->fils;

    size_t count = 0;
    if (!isFieldVchar(*text, data)) { return false; }
    count++;

    if (*(text+count) == SP || *(text+count) == HTAB) {
        while(*(text+count) == SP || *(text+count) == HTAB) {
            Element *sub;
            if (*(text+count) == SP) { sub = addEl("__sp", text+count, 1); }
            else { sub = addEl("__htab", text+count, 1); }
            data->frere = sub;
            data = data->frere;
            count++;
        }
        if (!isFieldVchar(*(text+count), data)) { return false; }
        count++;
    }

    updateLength(data, count);
    *curr += count;
    return true;
}

// obs-fold = CRLF 1*( SP / HTAB )
bool isObsFold(char *text, size_t *curr, Element *data) {
    Element *el = addEl("obs-fold", text, strlen(text));
    data->fils = el;
    data = data->fils;

    size_t count = 0;
    if (*text == CR && (*text+1) == LF) {
        Element *el = addEl("__crlf", text, 2);
        data->fils = el;
        data = data->fils;
        count += 2;
    } else { return false; }

    if (*(text+count) != SP || *(text+count) != HTAB) { return false; }
    while (*(text+count) == SP || *(text+count) == HTAB) {
        Element *sub;
        if (*(text+count) == SP) { sub = addEl("__sp", text+count, 1); }
        else { sub = addEl("__htab", text+count, 1); }
        
        data->frere = sub;
        data = data->frere;
        count++;
    }

    updateLength(data, count);
    *curr += count;
    return true;
}

// field-value = *( field-content / obs-fold )
bool isFieldValue(char *text, size_t *curr, Element *data) {
    Element *el = addEl("field-value", text, strlen(text));
    data->frere = el;
    data = data->frere;

    size_t count = 0;
    // while (isFieldContent(*(text+count), &count, data) || isObsFold(*(text+count), &count, data)) {
    //     if 
    // }
    // à compléter : faire gaffe au pointeur vers count, pas incrémenter 2 fois lors de la vérif du if...

    updateLength(data, count);
    *curr += count;
    return true;
}


// cookie-name = token
bool isCookieName(char *text, size_t *curr, Element *data) {
    Element *el = addEl("cookie_name", text, strlen(text));
    data->fils = el;
    data = data->fils;

    if (!isToken(text, curr, data, true)) { return false; }

    updateLength(data, *curr);
    return true;
}

bool isDQUOTE(char text, Element *head, bool is_fils) {
    Element *el = addEl("__dquote", &text, 1);
    if (is_fils) {
        head->fils = el;
        head = head->fils;
    } else {
        head->frere = el;
        head = head->frere;
    }

    return (text == DQUOTE);
}

// cookie-octet = *(%x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E)
bool isCookieOctet(char *text, size_t *curr, Element *data, bool is_fils) {
    Element *el = addEl("cookie-octet", text, 1);
    if (is_fils) {
        data->fils = el;
        data = data->fils;
    } else {
        data->frere = el;
        data = data->frere;
    }

    size_t count = 0;
    while (*(text+count) == EXCLAMATION || (*(text+count) >= HASHTAG && *(text+count) <= PLUS) || (*(text+count) >= DASH && *(text+count) <= COLON) || (*(text+count) >= 60 && *(text+count) <= 91) || (*(text+count) >= 93 && *(text+count) <= VAGUE)) {
        Element *el = addEl("__icar", text+count, 1);
        if (count == 0) {
            data->fils = el;
            data = data->fils;
        } else {
            data->frere = el;
            data = data->frere;
        }
        count += 1;
    }
    *curr += count;
    updateLength(data, count);
    return true;
}

// cookie-value = ( DQUOTE *cookie-octet DQUOTE ) / *cookie-octet
bool isCookieValue(char *text, size_t *curr, Element *data) {
    size_t count = 0;

    Element *el = addEl("cookie-value", text, strlen(text));
    data->frere = el;
    data = data->frere;
    Element *save = data;

    if (isDQUOTE(text[count], data, true)) {
        count++;
        data = data->fils;
        while (*(text+count) != DQUOTE) {
            isCookieOctet(text+count, &count, data, false);
            data = data->frere;
        }

        isDQUOTE(text[count], data, false);
        count++;
    } else {
        isCookieOctet(text+count, &count, data, true);
    }

    data = save;
    updateLength(data, count);

    *curr += count;
    return true;
}

// cookie-pair = cookie-name "=" cookie-value
bool isCookiePair(char *text, size_t *curr, Element *data, bool is_fils) {
    size_t count = 0;

    Element *el = addEl("cookie-pair", text, strlen(text));
    if (is_fils) { // c'est pas un __sp
        data->fils = el;
        data = data->fils;
    } else {
        data->frere = el;
        data = data->frere;
    }
    Element *save = data;

    if (!isCookieName(text, &count, data)) { return false; }
    data = data->fils;

    if (*(text+count) == EQUAL) {
        Element *eq = addEl("__equal", text+count, 1);
        data->frere = eq;
        data = data->frere;
        count += 1;
    }

    if (!isCookieValue(text+count, &count, data)) { return false; }

    data = save;
    updateLength(data, count);

    *curr += count;
    return true;
}

// cookie-string = cookie-pair *( ";" SP cookie-pair )
bool isCookieString(char *text, size_t *curr, Element *data) {
    size_t count = 0;

    Element *el = addEl("cookie-string", text, strlen(text));
    data->frere = el;
    data = data->frere;
    Element *save = data;

    if (!isCookiePair(text, &count, data, true)) { return false; }
    data = data->fils;
    if (*(text+count) == SEMICOLON) {
        while (text[count] == SEMICOLON && text[count+1] == SP) {
            Element *el1 = addEl("__colon", text+count, 1);
            data->frere = el1;
            data = data->frere;

            Element *el2 = addEl("__sp", text+count+1, 1);
            data->frere = el2;
            data = data->frere;

            count += 2;

            if (!isCookiePair(text+count, &count, data, false)) { return false; }
        }
    }

    data = save;
    updateLength(data, count);

    *curr += count;
    return true;
}

// qdtext = HTAB / SP / "!" / %x23-5B ; '#'-'[' / %x5D-7E ; ']'-'~' / obs-text
bool isQdText(char *text, size_t *curr, Element *data) {
    Element *el = addEl("qdtext", text, 1);
    data->frere = el;
    data = data->frere;
    
    *curr += 1;
    return (*text == HTAB || *text == SP || *text == EXCLAMATION || (*text >= HASHTAG && *text <= OBRACKET));
}

// quoted-pair = "\" ( HTAB / SP / VCHAR / obs-text )
bool isQuotedPair(char *text, size_t *curr, Element *data) {
    Element *el = addEl("quoted-pair", text, strlen(text));
    data->frere = el;
    data = data->frere;

    size_t count = 0;
    if (*(text+count) == BACKSLASH) {
        Element *el = addEl("__backslash", text+count, 1);
        data->fils = el;
        data = data->fils;
        count += 1;
    } else { return false; }

    if (*(text+count) == HTAB || *(text+count) == SP || (*(text+count) >= EXCLAMATION && *(text+count) <= VAGUE) || isObsText(*(text+count))) {
        Element *sub;
        if (*(text+count) == HTAB) { sub = addEl("__htab", text+count, 1); }
        else if (*(text+count) == SP) { sub = addEl("__sp", text+count, 1); }
        else if (*(text+count) >= EXCLAMATION && *(text+count) <= VAGUE) { sub = addEl("__vchar", text+count, 1); }
        else { sub = addEl("obs-text", text+count, 1); }
        data->frere = sub;
        data = data->frere;
        count += 1;
    } else { return false; }

    updateLength(data, count);
    *curr += count;
    return true;
}

// quoted-string = DQUOTE *( qdtext / quoted-pair ) DQUOTE
bool isQuotedString(char *text, size_t *curr, Element *data) {
    Element *el = addEl("quoted-string", text, strlen(text));
    data->frere = el;
    data = data->frere;

    size_t count = 0;
    if (!isDQUOTE(*text, data, true)) { return false; }
    count += 1;
    data = data->fils;

    while(isQdText(text+count, &count, data) || isQuotedPair(text+count, &count, data)) {
        data = data->frere;
    }

    if (!isDQUOTE(*(text+count), data, false)) { return false; }
    count += 1;

    updateLength(data, count);
    *curr += count;
    return true;
}

// parameter = token "=" ( token / quoted-string )
bool isParameter(char *text, size_t *curr, Element *data) {
    Element *el = addEl("parameter", text, strlen(text));
    data->frere = el;
    data = data->frere;
    Element *save = data;


    size_t count = 0;
    if (!isToken(text, &count, data, true)) { return false; }
    data = data->fils;
    if (*(text+count) == EQUAL) {
        Element *el = addEl("__equal", text+count, 1);
        data->frere = el;
        data = data->frere;
        count += 1;
    } else { return false; }
    int ok = 0;
    if (isToken(text+count, &count, data, false)) { ok = 1; }
    else if (!isQuotedString(text+count, &count, data)) { ok = 1;}
    else { ok = 0; }

    if (ok) {
        data = save;
        updateLength(data, count);
        *curr += count;
        return true;
    } else { return false; }
}

// type = token
bool isSubType(char *text, size_t *curr, Element *data) {
    Element *el = addEl("subtype", text, strlen(text));
    data->frere = el;
    data = data->frere;

    bool res =  isToken(text, curr, data, true);
    updateLength(data,*curr);
    return res;
}

// type = token
bool isType(char *text, size_t *curr, Element *data) {
    Element *el = addEl("type", text, strlen(text));
    data->fils = el;
    data = data->fils;

    bool res = isToken(text, curr, data, true);
    updateLength(data,*curr);
    return res;
}

// media-type = type "/" subtype *( OWS ";" OWS parameter )
bool isMediaType(char *text, size_t *curr, Element *data) {
    Element *el = addEl("media-type", text, strlen(text));
    data->fils = el;
    data = data->fils;
    Element *save = data;

    size_t count = 0;
    if (!isType(text, &count, data)) { return false; }
    data = data->fils;
    if (*(text+count) == SLASH) {
        Element *el = addEl("__slash", text+count, 1);
        data->frere = el;
        data = data->frere;
        count += 1;
    } else { return false; }
    if (!isSubType(text+count, &count, data)) { return false; }
    data = data->frere;

    while (*(text+count) == SP || *(text+count) == HTAB) {
        if (!isOWS(text+count, &count, data, false)) { return false; }
        data = data->frere;
        if (*(text+count) == SEMICOLON) {
            Element *el = addEl("__semicolon", text+count, 1);
            data->frere = el;
            data = data->frere;
            count += 1;
        } else { return false; }
        if (!isOWS(text+count, &count, data, false)) { return false; }
        data = data->frere;
        if (!isParameter(text+count, &count, data)) { return false; }
        data = data->frere;
    }

    updateLength(save, count);
    *curr += count;
    return true;
}

//  Content-Type = media-type
bool isContentType(char *text, size_t *curr, Element *data) {
    Element *el = addEl("Content-Type", text, strlen(text));
    data->frere = el;
    data = data->frere;

    bool res = isMediaType(text, curr, data);
    updateLength(data,*curr);
    return res;
}

// Content-Length = 1*DIGIT
bool isContentLength(char *text, size_t *curr, Element *data) {
    size_t count = 0;

    Element *el = addEl("Content-Length", text, strlen(text));
    data->frere = el;
    data = data->frere;
    Element *save = data;

    if (!isDigit(*(text+count))) { return false; }
    while (isDigit(*(text+count))) {
        Element *el = addEl("__digit", text+count, 1);
        if (count == 0) {
            data->fils = el;
            data = data->fils;
        } else {
            data->frere = el;
            data = data->frere;
        }
        count += 1;
    }

    updateLength(save, count);
    *curr += count;
    return true;
}

// connection-option = token
bool isConnectionOption(char *text, size_t *curr, Element *data, bool is_fils) {
    Element *el = addEl("connection-option", text, strlen(text));
    if (is_fils) {
        data->fils = el;
        data = data->fils;
    } else {
        data->frere = el;
        data = data->frere;
    }

    return isToken(text, curr, data, true);
}

// Connection = *( "," OWS ) connection-option *( OWS "," [ OWS connection-option ] )
bool isConnection(char *text, size_t *curr, Element *data) {
    size_t count = 0;

    Element *el = addEl("Connection", text, strlen(text));
    data->frere = el;
    data = data->frere;
    Element *save = data;

    bool fst = false;
    if (*(text+count) == COMMA) {
        while (*(text+count) == COMMA) {
            Element *el = addEl("__comma", text+count, 1);
            data->fils = el;
            data = data->fils;
            count += 1;
            if (!isOWS(text+count, &count, data, false)) { return false; }
            data = data->frere;
            fst = true;
        }
    }
    
    if (!isConnectionOption(text+count, &count, data, !fst)) { return false; }
    data = data->frere;

    int boucle = OWSCH(text+count);
    if(boucle == 2 || boucle == 4){ //si on n'a pas OWS "," ou OWS CRLF
        return false;
    }
   
    while(boucle == 1){ //on entre si : OWS","
        isOWS(text+count, &count, data, false); //ajout de OWS (on sait qu'il est la grace a OWS())
        data = data->frere; //OWS devient la tete
        
        Element *el = addEl("__comma", text+count, 1);  //ajout de ","
        data->frere = el;
        data = data->frere;
        count += 1;
         
        boucle = OWSCH(text+count); //si 1 on reboucle, si 3 on sort de la boucle
        if(boucle == 2){ //OWS connection-option
            isOWS(text+count, &count, data, false); //ajout de OWS (on sait qu'il est la grace a OWS())
            data = data->frere; //OWS devient la tete

            isConnectionOption(text+count, &count, data,false); //ajout de connection-option
            data = data->frere; //connection-option devient la tete

            boucle = OWSCH(text+count); //si 1 on reboucle, si 3 on sort sinon :
            if(boucle == 4 || boucle == 2){ //OWS connection-option ne peut etre suivi que de OWS',' ou de OWS CRLF
                return false;
            }
        }
        else if(boucle == 4){ //cas d'erreur
            return false;
        }
    }

    updateLength(save, count);
    *curr += count;
    return true;
}

// unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~"
bool isUnreserved(char text) {
    return isAlpha(text) || isDigit(text) || text == DASH || text == DOT || text == UNDERSCORE || text == VAGUE;
}

// sub-delims = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="
bool isSubDelims(char text) {
    return (text == EXCLAMATION || text == DOLLAR || text == ESP || text == SQUOTE || text == OPAREN || text == CPAREN || text == STAR || text == PLUS || text == COMMA || text == SEMICOLON || text == EQUAL);
}

// HEXDIG = DIGIT / "A" / "B" / "C" / "D" / "E" / "F"
bool isHexdig(char text) {
    return isDigit(text) || (text >= AMAJ && text <= FMAJ);
}

// IPvFuture = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
bool isIPvFuture(char *text, size_t *curr, Element *data) {
    Element *el = addEl("IPvFuture", text, strlen(text));
    data->frere = el;
    data = data->frere;

    size_t count = 0;
    if (*(text+count) == 'v') {
        Element *el = addEl("__v", text+count, 1);
        data->fils = el;
        data = data->fils;
        count += 1;
    } else { return false; }
    
    if (!isHexdig(*(text+count))) { return false; }
    while (isHexdig(*(text+count))) {
        Element *el = addEl("__hexdig", text+count, 1);
        data->frere = el;
        data = data->frere;
        count += 1;
    }
    
    if (*(text+count) == DOT) {
        Element *el = addEl("__dot", text+count, 1);
        data->frere = el;
        data = data->frere;
        count += 1;
    } else { return false; }

    if (!isUnreserved(*(text+count)) && !isSubDelims(*(text+count)) && *(text+count) != COLON) { return false; }
    while (isUnreserved(*(text+count)) || isSubDelims(*(text+count)) || *(text+count) == COLON) {
        Element *sub;
        if (isUnreserved(*(text+count))) { sub = addEl("__unreserved", text+count, 1); }
        else if (isSubDelims(*(text+count))) { sub = addEl("__subdelims", text+count, 1); }
        else { sub = addEl("__colon", text+count, 1); }
        
        data->frere = sub;
        data = data->frere;
        count += 1;
    }

    updateLength(data, count);
    *curr += count;
    return true;
}

// dec-octet     = "25" %x30-35          ; 250-255
//               / "2" %x30-34 DIGIT     ; 200-249
//               / "1" 2DIGIT            ; 100-199
//              / %x31-39 DIGIT         ; 10-99
//              / DIGIT                 ; 0-9
bool isDecOctet(char *text, size_t *curr, Element *data, bool is_fils) {
    Element *el = addEl("dec-octet", text, strlen(text));
    if (is_fils) {
        data->fils = el;
        data = data->fils;
    } else {
        data->frere = el;
        data = data->frere;
    }

    size_t count = 0;
    if (isDigit(*text)) {
        Element *el = addEl("__digit", text, 1);
        data->frere = el;
        data = data->frere;
        count = 1;
    }
    else if (*text >= ZERO+1 && *text <= NINE) {
        Element *el = addEl("__digit", text, 1);
        data->frere = el;
        data = data->frere;
        if (isDigit(*(text+1))) {
            Element *el = addEl("__digit", text+1, 1);
            data->frere = el;
            data = data->frere;
            count = 2;
        } else { return false; }
    }
    else if (*text == '1' && isDigit(*(text+1))) {
        Element *el = addEl("__digit", text, 1);
        data->frere = el;
        data = data->frere;
        for (int i = 0; i < 2; i++) {
            if (!isDigit(*(text+i+1))) { return false; }
            Element *el = addEl("__digit", text+i+1, 1);
            data->frere = el;
            data = data->frere;
        }
        count = 3;
    }
    else if (*text == '2' && *(text+1) >= ZERO && *(text+1) <= ZERO+4 && isDigit(*(text+2))) {
        Element *el = addEl("__digit", text, 1);
        data->frere = el;
        data = data->frere;
        if (*(text+1) >= ZERO && *(text+1) <= ZERO+4) {
            Element *el = addEl("__digit", text+1, 1);
            data->frere = el;
            data = data->frere;
            if (isDigit(*(text+2))) {
                Element *el = addEl("__digit", text+2, 1);
                data->frere = el;
                data = data->frere;
                count = 3;
            } else { return false; }
        } else { return false; }
    }
    else if (*text == '2' && *(text+1) == '5' && *(text+2) >= ZERO && *(text+2) <= ZERO+5) {
        Element *el = addEl("__digit", text, 1);
        data->frere = el;
        data = data->frere;
        if (*(text+1) == '5') {
            Element *el = addEl("__digit", text+1, 1);
            data->frere = el;
            data = data->frere;
            if (*(text+2) >= ZERO && *(text+2) <= ZERO+5) {
                Element *el = addEl("__digit", text+2, 1);
                data->frere = el;
                data = data->frere;
                count = 3;
            } else { return false; }
        } else { return false; }
    }    
    else { return false; }

    updateLength(data, count);
    *curr += count;
    return true;
}

// IPv4address = dec-octet "." dec-octet "." dec-octet "." dec-octet
bool isIPv4address(char *text, size_t *curr, Element *data) {
    Element *el = addEl("IPv4address", text, strlen(text));
    data->fils = el;
    data = data->fils;  

    size_t count = 0;
    for (size_t i = 0; i < 4; i++) {
        if (!isDecOctet(text+count, &count, data, i == 0)) { return false; }
        
        if (i ==0) { data = data->fils; }
        else { data = data->frere; }
        
        if (*(text+count) == DOT) {
            Element *el = addEl("__dot", text+count, 1);
            data->frere = el;
            data = data->frere;
            count += 1;
        } else { return false; }
    }

    updateLength(data, count);
    *curr += count;
    return true;
}

// h16 = 1*4HEXDIG
bool isH16(char *text, size_t *curr, Element *data, bool is_fils) {
    Element *el = addEl("h16", text, strlen(text));
    if (is_fils) {
        data->fils = el;
        data = data->fils;
    } else {
        data->frere = el;
        data = data->frere;
    }
        
    size_t count = 0;
    while (isHexdig(*(text+count))) {
        Element *el = addEl("__hexdig", text+count, 1);
        if (count == 0) {
            data->fils = el;
            data = data->fils;
        } else {
            data->frere = el;
            data = data->frere;
        }
        count += 1;
    }

    *curr += count;
    return (count >= 1 && count <= 4);
}

// ls32 = ( h16 ":" h16 ) / IPv4address
bool isLS32(char *text, size_t *curr, Element *data) {
    Element *el = addEl("ls32", text, strlen(text));
    data->frere = el;
    data = data->frere;

    size_t count = 0;
    if (isH16(text, &count, data, true)) {
        if (*(text+count) == COLON) {
            Element *el = addEl("__colon", text+count, 1);
            data->frere = el;
            data = data->frere;
            count += 1;
            if (!isH16(text+count, &count, data, false)) { return false; }
        } else { return false; }
    }
    else if (!isIPv4address(text, &count, data)) { return false; }
    else { return false; }

    updateLength(data, count);
    *curr += count;
    return true;
}

// IPv6address =                             6( h16 ":" ) ls32
//              /                       "::" 5( h16 ":" ) ls32
//              / [ h16               ] "::" 4( h16 ":" ) ls32
//              / [ h16 *1( ":" h16 ) ] "::" 3( h16 ":" ) ls32
//              / [ h16 *2( ":" h16 ) ] "::" 2( h16 ":" ) ls32
//              / [ h16 *3( ":" h16 ) ] "::"    h16 ":"   ls32
//              / [ h16 *4( ":" h16 ) ] "::"              ls32
//              / [ h16 *5( ":" h16 ) ] "::"              h16
//              / [ h16 *6( ":" h16 ) ] "::"
bool isIPv6Address(char *text, size_t *curr, Element *data) {
    Element *el = addEl("IPv6address", text, strlen(text));
    data->frere = el;
    data = data->frere;

    size_t count = 0;
    
    // à coder

    updateLength(data, count);
    *curr += count;
    return true;
}

// IP-literal = "[" ( IPv6address / IPvFuture  ) "]"
bool isIPLiteral(char *text, size_t *curr, Element *data) {
    Element *el = addEl("IP-literal", text, strlen(text));
    data->frere = el;
    data = data->frere;

    if(*(text) != OBRACKET) { return false; }
    Element *el1 = addEl("__obracket", text, 1);
    data->fils = el1;
    data = data->fils;

    size_t count = 1;
    if (!isIPv6Address(text+count, &count, data)) { return false; }
    else if (!isIPvFuture(text+count, &count, data)) { return false; }
    else { return false; }

    if(*(text+count) != CBRACKET) { return false; }
    Element *el2 = addEl("__cbracket", text+count, 1);
    data->frere = el2;
    data = data->frere;
    
    updateLength(data, count);
    *curr += count+1;
    return true;
}

// pct-encoded = "%" HEXDIG HEXDIG
bool isPctEncoded(char *text, size_t *curr, Element *data) {
    Element *el = addEl("pct-encoded", text, strlen(text));
    data->fils = el;
    data = data->fils;

    size_t count = 0;
    if (*(text+count) == POURCENT) {
        Element *el = addEl("__percent", text+count, 1);
        data->fils = el;
        data = data->fils;
        count += 1;
    } else { return false; }

    for (size_t i = 0; i < 2; i++) {
        if (!isHexdig(*(text+count))) { return false; }
        Element *el = addEl("__hexdig", text+count, 1);
        data->frere = el;
        data = data->frere;
        count += 1;
    }

    updateLength(data, count);
    *curr += count;
    return true;
}

// reg-name = *( unreserved / pct-encoded / sub-delims )
bool isRegName(char *text, size_t *curr, Element *data) {
    Element *el = addEl("reg-name", text, strlen(text));
    data->fils = el;
    data = data->fils;

    size_t count = 0;
    bool fst = true;
    while (!(isUnreserved(*(text+count)) || isSubDelims(*(text+count)) || isPctEncoded(text+count, &count, data))) {
        Element *sub;
        if (isUnreserved(*(text+count))) { sub = addEl("__unreserved", text+count, 1); count++; }
        else if (isSubDelims(*(text+count))) { sub = addEl("__subdelims", text+count, 1); count++; }
        else { sub = addEl("__pctencoded", text-count, 3); } // - car déjà augmenter lors de l'appel dans le while si true

        if (fst) {
            data->fils = sub;
            data = data->fils;
        } else {
            data->frere = sub;
            data = data->frere;
        }
        fst = false;
    }

    updateLength(data, count);
    *curr += count;
    return true;
}

// host = IP-literal / IPv4address / reg-name
bool ishost(char *text, size_t *curr, Element *data) {
    Element *el = addEl("host", text, strlen(text));
    data->fils = el;
    data = data->fils;

    size_t count = 0;
    bool rtn = false;
    if (isIPLiteral(text, &count, data)) { rtn = true; }
    else if (isIPv4address(text, &count, data)) { rtn = true; }
    else if (isRegName(text, &count, data)) { rtn = true; }

    updateLength(data, count);
    *curr += count;
    return rtn;
}

// uri-host = host
bool isUriHost(char *text, size_t *curr, Element *data) {
    size_t count = 0;

    Element *el = addEl("Uri-host", text, strlen(text));
    data->fils = el;
    data = data->fils;

    if (!ishost(text, &count, data)) { return false; }

    updateLength(data, count);
    *curr += count;
    return true;
}

// port = *DIGIT
bool isPort(char *text, size_t *curr, Element *data) {
    size_t count = 0;

    Element *el = addEl("Port", text, strlen(text));
    data->frere = el;
    data = data->frere;

    while (isDigit(*(text+count))) {
        Element *el = addEl("__digit", text+count, 1);
        if (count == 0) {
            data->fils = el;
            data = data->fils;
        } else {
            data->frere = el;
            data = data->frere;
        }
        count += 1;
    }

    updateLength(data, count);
    *curr += count;
    return true;
}

// Host = uri-host [ ":" port ]
bool isHost(char *text, size_t *curr, Element *data) {
    size_t count = 0;

    Element *el = addEl("Host", text, strlen(text));
    data->frere = el;
    data = data->frere;

    if (!isUriHost(text, &count, data)) { return false; }
    data = data->fils;
    if (*(text+count) == COLON) {
        Element *el = addEl("__colon", text+count, 1);
        data->frere = el;
        data = data->frere;
        count += 1;
        if (!isPort(text+count, &count, data)) { return false; }
    }

    // updateLength(data, count);
    *curr += count;
    return true;
}

// Expect = "100-continue"
bool isExpect(char *text, size_t *curr, Element *data) {
    Element *el = addEl("Expect", text, strlen(text));
    data->frere = el;
    data = data->frere;

    if (strcmp(text, "100-continue")) { return false; }

    updateLength(data, 14);
    *curr += 14;
    return true;
}

bool isTransferCoding(char *text, size_t *curr, Element *data, bool is_fils){
    Element *el = addEl("transfert-coding",text, strlen(text));
    if (is_fils) {
        data->fils = el;
        data = data->fils; //transfert-coding devient la tete
    } else {
        data->frere = el;
        data = data->frere; //idem
    }

    size_t n = 0;
    if(!strncmp(text,"chuncked",8)) {
        n = 8;
        Element *sub = addEl("case_insensitive_string","chuncked",8);
        data->fils = sub;
        *curr+=8;
    }
    else if(!strncmp(text,"compress",8)) {
        n = 8;
        Element *sub = addEl("case_insensitive_string","compress",8);
        data->fils = sub;
        *curr+=8;
    }
    else if(!strncmp(text,"deflate",7)) {
        n = 7;
        Element *sub = addEl("case_insensitive_string","deflate",7);
        data->fils = sub;
        *curr+=7;
    }
    else if(!strncmp(text,"gzip",4)) {
        n = 4;
        Element *sub = addEl("case_insensitive_string","gzip",4);
        data->fils = sub;
        *curr+=4;
    }
    else {return false;}

    updateLength(data,n);

    return true;
}

int OWSTEH(char *text){ //OWS TransferEncodingHeader
    size_t i = 0;
    while(text[i] == SP || text[i] == HTAB){
        i++;
    }

    if(text[i] == COMMA) {
        return 1; //cas OWS","
    }
    else if(text[i] == CR && text[i+1] == LF){ //cas OWS CRLF : on sort de Transfert-Encoding
        return 3;
    }
    else if(!strncmp(text+i,"chuncked",8) || !strncmp(text+i,"compress",8) || !strncmp(text+i,"deflate",7) || !strncmp(text+i,"gzip",4)){
        //cas OWS transfert-coding
        //printf("dans OWS text+i : -%s-\n",text+i);
        return 2;
    }
    else { return 4; } //cas erreur
}

bool isTransferEncoding(char *text, size_t *curr, Element *data){
    size_t count = 0;

    Element *el = addEl("Transfer-Encoding",text,strlen(text));
    data->frere = el;
    data = data->frere; //transfer-encoding devient la tete
    Element *save = data;

    bool fst = false; //pour savoir si protocol va etre fils direct de Transfert-Encoding
    if (*(text+count) == COMMA) {
        while (*(text+count) == COMMA) {
            Element *el = addEl("__comma", text+count, 1);
            data->fils = el;
            data = data->fils;
            count += 1;
            if (!isOWS(text+count, &count, data, false)) { return false; }
            data = data->frere;
            fst = true;
        }
    }

    if (!isTransferCoding(text+count, &count, data, !fst)){return false;}
    data = data->frere ; //transfert-coding devient la tete

    int boucle = OWSTEH(text+count);
    //printf("boucle : %d\n",boucle);
    if(boucle == 2 || boucle == 4){ //si on n'a pas OWS "," ou OWS CRLF
        return false;
    }
    //printf("ici\n");
    while(boucle == 1){ //on entre si : OWS","
        isOWS(text+count, &count, data, false); //ajout de OWS (on sait qu'il est la grace a OWS())
        data = data->frere; //OWS devient la tete
        //printf("Ajout de OWS\n");
        
        Element *el = addEl("__comma", text+count, 1);  //ajout de ","
        data->frere = el;
        data = data->frere;
        //printf("Ajout de ','\n");
        count += 1;
         
        boucle = OWSTEH(text+count); //si 1 on reboucle, si 3 on sort de la boucle
        //printf("recalcul de boucle : %d\n",boucle);
        if(boucle == 2){ //OWS transfert-coding
            isOWS(text+count, &count, data, false); //ajout de OWS (on sait qu'il est la grace a OWS())
            data = data->frere; //OWS devient la tete

            isTransferCoding(text+count, &count, data,false); //ajout de transfert-coding
            data = data->frere; //trasfert-coding devient la tete

            boucle = OWSTEH(text+count); //si 1 on reboucle, si 3 on sort sinon :
            //printf("rerecalcul de boucle qui commence à -%c-: %d\n",*(text+count),boucle);
            if(boucle == 4 || boucle == 2){ //OWS transfert-conding ne peut etre suivi que de OWS',' ou de OWS CRLF
                return false;
            }
        }
        else if(boucle == 4){ //cas d'erreur
            return false;
        }
    }

    updateLength(save,count);
    *curr += count;
    return true;    
}

bool isTransferEncodingHeader(char *text, size_t *curr, Element *data){

    if (!strcmp(text, "Transfer-Encoding")) {return false;}  //ok 

    Element *el = addEl("Transfer-Encoding-header", text, strlen(text)); //nombre a changer
    data->fils = el;
    data = data->fils; //la tete devient el
    Element *save = data;

    Element *el2 = addEl("case_insensitive_string","Transfer-Encoding",17);
    data->fils = el2;
    data = data->fils; //la tete devient el2
    
    size_t count = 17;
    //printf("text : %c\n",*(text+count)); 

    if (*(text+count) == COLON){
        Element *el = addEl("__colon", text+count, 1);
        data->frere = el;
        data = data->frere; //la tete devient ":"
        count += 1;
    } 
    else {return false;}

    if(!isOWS(text+count, &count, data, false)) {return false;} 
    data = data->frere;   //la tete devient OWS
    //tab semble etre compté comme 2 sp, ou les tabulation son mal faites
    if(!isTransferEncoding(text+count, &count, data)) {return false;}
    data = data->frere;

    if(!isOWS(text+count, &count, data, false)) {return false;} 

    updateLength(save,count);
    *curr += count;
    return true;
}

// Cookie-header = "Cookie:" OWS cookie-string OWS
bool isCookieHeader(char *text, size_t *curr, Element *data) {
    Element *el = addEl("Cookie", text, strlen(text));
    data->fils = el;
    data = data->fils;

    size_t count = 0;
    if (!strcmp(text, "Cookie:")) { return false; }
    el = addEl("case_insensitive_string", text, 7);
    data->fils = el;
    data = data->fils;
    count += 7;

    if(!isOWS(text+count, &count, data, false)) {return false; }
    data = data->frere;
    if (!isCookieString(text+count, &count, data)) { return false; }
    data = data->frere;
    if(!isOWS(text+count, &count, data, false)) {return false; }

    updateLength(data, count);
    *curr += count;
    return true;
}

// Content-Length-header = "Content-Length" ":" OWS Content-Length OWS
bool isContentLengthHeader(char *text, size_t *curr, Element *data) {
    Element *el = addEl("Content-Length-header", text, strlen(text));
    data->fils = el;
    data = data->fils;

    size_t count = 0;
    if (!strcmp(text, "Content-Length")) { return false; }
    el = addEl("case_insensitive_string", text, 14);
    data->fils = el;
    data = data->fils;
    count += 14;

    if (*(text+count) == COLON) {
        Element *el = addEl("__colon", text+count, 1);
        data->frere = el;
        data = data->frere;
        count += 1;
    } else { return false; }
    if (!isOWS(text+count, &count, data, false)) { return false; }
    data = data->frere;
    if (!isContentLength(text+count, &count, data)) { return false; }
    data = data->frere;
    if (!isOWS(text+count, &count, data, false)) { return false; }

    updateLength(data, count);
    *curr += count;
    return true;
}

// Content-Type-header = "Content-Type" ":" OWS Content-Type OWS
bool isContentTypeHeader(char *text, size_t *curr, Element *data) {
    Element *el = addEl("Content-Type-header", text, strlen(text));
    data->fils = el;
    data = data->fils;
    Element *save = data;

    size_t count = 0;
    if (!strcmp(text, "Content-Type")) { return false; }
    el = addEl("case_insensitive_string", text, 12);
    data->fils = el;
    data = data->fils;
    count += 12;

    if (*(text+count) == COLON) {
        Element *el = addEl("__colon", text+count, 1);
        data->frere = el;
        data = data->frere;
        count += 1;
    } else { return false; }
    if (!isOWS(text+count, &count, data, false)) { return false; }
    data = data->frere;
    if (!isContentType(text+count, &count, data)) { return false; }
    data = data->frere;
    if (!isOWS(text+count, &count, data, false)) { return false; }

    updateLength(save, count);
    *curr += count;
    return true;
}

// Connection-header = "Connection" ":" OWS Connection OWS
bool isConnectionHeader(char *text, size_t *curr, Element *data) {
    Element *el = addEl("Connection-header", text, strlen(text));
    data->fils = el;
    data = data->fils;

    size_t count = 0;
    if (!strcmp(text, "Connection")) { return false; }
    el = addEl("case_insensitive_string", text, 10);
    data->fils = el;
    data = data->fils;
    count += 10;

    if (*(text+count) == COLON) {
        Element *el = addEl("__colon", text+count, 1);
        data->frere = el;
        data = data->frere;
        count += 1;
    } else { return false; }
    if (!isOWS(text+count, &count, data, false)) { return false; }
    data = data->frere;
    if (!isConnection(text+count, &count, data)) { return false; }
    data = data->frere; //a rajouter non
    if (!isOWS(text+count, &count, data, false)) { return false; }

    updateLength(data, count);
    *curr += count;
    return true;
}

// Expect-header = "Expect" ":" OWS Expect OWS
bool isExpectHeader(char *text, size_t *curr, Element *data) {
    Element *el = addEl("Expect-header", text, strlen(text));
    data->fils = el;
    data = data->fils;

    size_t count = 0;
    if (!strcmp(text, "Expect")) { return false; }
    el = addEl("case_insensitive_string", text, 6);
    data->fils = el;
    data = data->fils;
    count += 6;

    if (*(text+count) == COLON) {
        Element *el = addEl("__colon", text+count, 1);
        data->frere = el;
        data = data->frere;
        count += 1;
    } else { return false; }

    if (!isOWS(text+count, &count, data, false)) { return false; }
    data = data->frere;
    if (!isExpect(text+count, &count, data)) { return false; }
    data = data->frere;
    if (!isOWS(text+count, &count, data, false)) { return false; }

    updateLength(data, count);
    *curr += count;
    return true;
}

// Host-header = "Host" ":" OWS Host OWS
bool isHostHeader(char *text, size_t *curr, Element *data) {
    Element *el = addEl("Host-header", text, strlen(text));
    data->fils = el;
    data = data->fils;

    size_t count = 0;
    if (!strcmp(text, "Host")) { return false; }
    el = addEl("case_insensitive_string", text, 4);
    data->fils = el;
    data = data->fils;
    count += 4;

    if (*(text+count) == COLON) {
        Element *el = addEl("__colon", text+count, 1);
        data->frere = el;
        data = data->frere;
        count += 1;
    } else { return false; }
    if (!isOWS(text+count, &count, data, false)) { return false; }
    data = data->frere;
    if (!isHost(text+count, &count, data)) { return false; }
    data = data->frere;
    if (!isOWS(text+count, &count, data, false)) { return false; }

    updateLength(data, count); 
    *curr += count;
    return true;
}

// header-field =  Connection-header / Content-Length-header / Content-Type-header / Cookie-header / Transfer-Encoding-header / Expect-header / Host-header / ( field-name ":" OWS field-value OWS )
bool isHeaderField(char *text, size_t *curr, Element *data){
    Element *el = addEl("header-field", text, strlen(text));
    data->fils = el;
    data = data->fils;

    size_t count = 0;
    if (strncmp(text, "Connection", 10) == 0) {
        if (!isConnectionHeader(text, &count, data)) { return false; }
    }
    else if (strncmp(text, "Content-Length", 14) == 0) {
        if (!isContentLengthHeader(text, &count, data)) { return false; }
    }
    else if (strncmp(text, "Content-Type", 12) == 0) {
        if (!isContentTypeHeader(text, &count, data)) { return false; }
    }
    else if (strncmp(text, "Cookie", 6) == 0) {
        if (!isCookieHeader(text, &count, data)) { return false; }
    }
    else if (strncmp(text, "Transfer-Encoding", 17) == 0) {
        if (!isTransferEncodingHeader(text, &count, data)) { return false; }
    }
    else if (strncmp(text, "Expect", 6) == 0) {
        if (!isExpectHeader(text, &count, data)) { return false; }
    }
    else if (strncmp(text, "Host", 4) == 0) {
        if (!isHostHeader(text, &count, data)) { return false; }
    }
    else { if(!isFieldName(text, &count, data)) { return false; } }

    updateLength(data, count);
    *curr += count;
    return true;
}

bool isRequestLine(char *text, size_t *curr, Element *head)
{
    bool res = true;

    Element *tmp = malloc(sizeof(Element)); // contruction du sous-arbre pour ensuite le relier a "request-line" car on ne connait pas encore la taille pour créer l'élément "request-line"
    // head->fils = tmp;

    if (!isMethod(text, curr, tmp))
    {
        res = false;
    }
    tmp = tmp->fils; // method devient la tete
    Element *save = tmp;

    //printf("valeur curr sortie de méthode : %ld\n", *curr);

    if (!isSP(text[*curr], curr, tmp))
    {
        res = false;
    }
    tmp = tmp->frere; // SP devient la tete

    //printf("valeur curr après SP : %ld\n", *curr);

    if (!isRequestTarget(text, curr, tmp))
    {
        res = false;
    }                 // ancien premier argument : text+(*curr)
    tmp = tmp->frere; // request-target devient la tete

    if(!isSP(text[*curr], curr, tmp)) {res = false;}
    tmp = tmp->frere; //SP devient la tete

    if(!isHTTPVersion(text, curr, tmp)) {res = false;}
    tmp = tmp->frere; //HTTP-version devient la tete

    if (*(text +(*curr)) == CR && *(text +(*curr)+ 1) == LF)
    {
        Element *el = addEl("__crlf", text +(*curr), 2);
        tmp->frere = el;
        *curr += 2;
    }
    else
    {
        return false;
    }

    Element *el = addEl("request_line", text, *curr);
    head->fils = el;
    head->fils->fils = save;
    return res;
}

bool isMethod(char *text, size_t *curr, Element *head){
    bool res = false;

    Element *tmp = malloc(sizeof(Element));
    
    if(isToken(text, curr, tmp, true)) {res = true;}
        
    Element *el = addEl("method", text, *curr);
    head->fils = el;
    el->fils = tmp->fils;
    return res;
}

bool isRequestTarget(char *text, size_t *curr, Element *head){ // request-target = origin-form      //head = SP
    bool res = false;
    size_t curr_mem = *curr;            //sauvegarde de curr pour pouvoir définir la taille de request-target ensuite
    Element *tmp = malloc(sizeof(Element));

    printf("valeur curr début RequestTarget : %ld\n",*curr);

    if(isOriginForm(text, curr, tmp)){res = true;}
    Element *el = addEl("request-target", text+curr_mem, (*curr)-curr_mem); 
    head->frere = el; //request target devient le frere de SP          // maintenant qu'on a la longueur de request-target on peut l'ajouter à l'arbre
    el->fils = tmp->fils; //Origin form devient le fils de request target
    free(tmp);
    return res;
}

bool isOriginForm(char *text, size_t *curr, Element *head)
{ // origin-form = absolute-path [ "?" query ]
    //printf("valeur curr début OriginForm : %ld\n", *curr);
    size_t curr_mem = *curr;
    Element *tmp = malloc(sizeof(Element));
    Element *c = malloc(sizeof(Element));
    //Element *save = c;
    if (!isAbsolutePath(text, curr, tmp)){
        return false;
    }
    c = tmp->fils;
    // [ "?" query ]
    //printf("debut optionnel originForm\n");

    if(*(text+(*curr)) == '?'){
        Element *pont = addEl("__ponct",text + (*curr), 1);
        c->frere = pont;
        c = c->frere;
        *curr += 1;
        //printf("avant query, text :%s, curr: %ld\n", text + (*curr), *curr);
        if (!isQuery(text, curr, c)){
            return false;
        }
    }

    Element *el = addEl("origin-form", text + curr_mem, (*curr) - curr_mem);
    head->fils = el;
    el->fils = tmp->fils;

    return true;
}

bool isQuery(char *text, size_t *curr, Element *head)
{ // query = *( pchar / "/" / "?" )
    bool boucle = true;
    size_t curr_comp = *curr;
    //printf("curr mem : %ld", curr_comp);
    //printf("curr: %ld\n", *curr);
    Element *tmp = malloc(sizeof(Element));
    Element *c = malloc(sizeof(Element));
    Element *p = malloc(sizeof(Element));
    Element *save = c;

    //printf("avant boucle query, text : %s, curr : %ld, char: %c\n", text, *curr, *(text+(*curr)-1));
    while (boucle)
    {
        if (*(text + (*curr)) == '/')
        {
            tmp = addEl("__ponct", "/", 1);
            c->frere = tmp;
            c = c->frere;
            *curr += 1;
        }
        else if (*(text+(*curr)) == '?')
        {
            //printf("query ? , curr : %ld\n", *curr);
            tmp = addEl("__ponct", "?", 1);
            c->frere = tmp;
            c = c->frere;
            *curr += 1;
            //printf("curr: %ld\n", *curr);
        }
        else if (isPchar(text, curr, p))
        {
            c->frere = p->fils;
            c = c->frere;
        }
        else
        {
            boucle = false;
        }
    }
    //printf("curr: %ld\n", *curr);
    //printf("text: %s, curr-curr: %ld\n", text+(curr_comp), (*curr) - curr_comp);
    Element *el = addEl("query", text+(curr_comp), (*curr) - curr_comp);
    head->frere = el;
    el->fils = save->frere;

    return true;
}

bool isAbsolutePath(char *text, size_t *curr, Element *head)
{ // absolute-path = 1*( "/" segment )
    //printf("valeur curr début absolute-path : %ld\n", *curr);
    Element *tmp = malloc(sizeof(Element));
    Element *c = malloc(sizeof(Element));
    Element *save = c; // head->fils = addEl("absolute-path", text, 0);

    size_t curr_mem = *curr;
    size_t icurr = 0;
    bool boucle = true;
    // printf("txt avant boucle  : %s, curseur : %ld\n", text, (*curr));
    while (boucle)
    {

        if (*(text + (*curr)) == '/')
        {
            tmp = addEl("__icar", "/", 1);
            c->frere = tmp;
            c = c->frere;
            *curr += 1;

            if (isSegment(text, curr, c))
            { // appel avec head=c="/"
                //*curr += 1; // a ne pas mettre car pchar incrémente deja le curseur car pchar soit +1 soit +3
                c = c->frere; // c devient segment
            }
            else
            {
                boucle = false;
            }
        }
        else
        {
            boucle = false;
        }

        icurr++;
    }
    //printf("sortie absolute-path\n");

    if (icurr == 0)
    {
        return false;
    }

    //*curr += icurr;
    Element *el = addEl("absolute-path", text + curr_mem, (*curr) - curr_mem);
    head->fils = el;
    el->fils = save->frere;
    return true;
}

bool isSegment(char *text, size_t *curr, Element *head){ // segment = *pchar         //head="/"
    bool boucle = true;

    Element *tmp = malloc(sizeof(Element)); //addEl("tmp","text",strlen("text"));
    Element *c = malloc(sizeof(Element));
    Element *save=c;
    
    size_t curr_mem = *curr;
    //printf("Dans isSegment text vaut : -%s-",text);
    while(boucle){
        //printf("caractère : %s\n curseur : %ld\n",text,*curr);
        if(isPchar(text,curr,tmp)){
            c->frere = tmp; //->fils;
            c = c->frere;
        }
        else{
            boucle = false;
        }
    }
    //printf("sortie de la boucle segment\n");
    Element *el = addEl("segment", text+curr_mem,(*curr)-curr_mem);
    head->frere = el; // segment devient le frere de head="/"
    el->fils = save->frere->fils;
    
    return true;
}


bool isPchar(char *text, size_t *curr, Element *head)
{ // pchar = unreserved / pct-encoded / sub-delims / ":" / "@"

    Element *tmp = malloc(sizeof(Element));
    Element *el;
    //printf("text dans pchar : %s, curr :%ld\n", text+(*curr), *curr);
    if (isUnreservedBis(*(text+(*curr)), tmp))
    {
        //printf("valeur text pchar/unreserved : %s\n", text);
        el = addEl("pchar", text+(*curr), 1);
        head->fils = el;
        el->fils = tmp->fils;
        *curr += 1;
        return true;
    }
    else if (isSubDelimsBis(*(text + (*curr)), tmp))
    {
        //printf("subDelims\n");
        el = addEl("pchar", text+(*curr), 1);
        head->fils = el;
        el->fils = tmp->fils;
        *curr +=1;
        return true;
    }
    else if (isPctEncodedBis(text + (*curr), curr, tmp))
    {
        //printf("valeur text pchar/pct : %s, curr :%ld\n", text + (*curr), *curr);
        el = addEl("pchar", text + (*curr), 3);
        head->fils = el;
        el->fils = tmp->fils;
        *curr +=3;
        return true;
    }
    else if (*(text + (*curr)) == ':')
    {
        el = addEl("pchar",text+(*curr),1);
        head->fils= el;
        el->fils = addEl("colon", text+(*curr), 1);
        *curr += 1;
        return true;
    }
    else if (*(text + (*curr)) == '@')
    {
        el = addEl("pchar",text+(*curr),1);
        head->fils= el;
        el->fils = addEl("at-sign", text+(*curr), 1);
        *curr += 1;
        return true;
    }
    return false;
}

bool isUnreservedBis(char text, Element *head){    //unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~"
    //printf("Dans isUreserverd text vaux : %c\n",text);
    head->fils = addEl("unreserved",&text,1);
    return (isAlpha(text)||isDigit(text)||text==DASH||text == DOT||text == UNDERSCORE||text == VAGUE);  
}


bool isPctEncodedBis(char *text, size_t *curr, Element *head){ //pct-encoded = "%" HEXDIG HEXDIG
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

bool isSubDelimsBis(char text, Element *head){ // sub-delims = "!" / "$" / "&" / "'" / "(" / ")"/ "*" / "+" / "," / ";" / "="
    head->fils = addEl("sub-delims",&text,1);
    return(text == '!' || text == '*'|| text == '&'|| text == SQUOTE || text == '(' || text == ')' || text == '*' || text == '+' || text == ',' || text == ';' || text == '=');
}

bool isHTTPVersion(char *text, size_t *curr, Element *head){ //HTTP-version = HTTP-name "/" DIGIT "." DIGIT
    head->frere = addEl("HTTP-version", text, 8); //HTTP-version devient la tete
    head = head->frere; ///je fais des fils  a SP

    if(!isHTTPname(text+(*curr))){
        return false;
    }

    head->fils = addEl("HTTP-name",text+(*curr), 4);
    head = head->fils; // head deviens HTTP-name
    *curr += 4;

    if (!(*(text+(*curr)) == '/')){
        return false;
    }

    head->frere = addEl("ponct", text+(*curr), 1);
    head = head->frere; // head deviens ponct 
    *curr += 1;

    if(!isDigit(*(text+(*curr)))){
        return false;
    }

    head->frere = addEl("DIGIT",text+(*curr), 1);
    head = head->frere; //head devient DIGIT
    *curr += 1;

    if (!(*(text+(*curr)) == '.')){
        return false;
    }

    head->frere = addEl("ponct",text+(*curr), 1);
    head = head->frere;
    *curr += 1;

    if(!isDigit(*(text+(*curr)))){
        return false;
    }

    head->frere = addEl("DIGIT",text+(*curr), 1);
    *curr += 1;

    return true;
}

bool isHTTPname(char *text){ //HTTP-name = HTTP
    return strcmp(text,"HTTP");
}

bool isMessageBody(char *text, size_t *curr, Element *head){
    
    Element *el = addEl("message-body",text,8);
    head -> fils = el;

    if(!isOCTET(*(text+(*curr)))){
        return false;
    }

    *curr += 8;

    return true;
}

bool isStartLine(char *text, size_t *curr, Element *head){ //start-line = request-line
    bool res = false;

    Element *tmp = malloc(sizeof(Element));
    //head->fils = tmp;                                 // on créer maintenant l'élément qu'on donne a isRequestLine 
    if(isRequestLine(text, curr, tmp)){res = true;}
    // mettre le if avant pour avoir curr pour la taille dans la start-line ??
    Element *el = addEl("start-line", text, *curr); //quelle valeure mettre pour length??
    head->fils = el;
    el->fils = tmp->fils;
    return res;
}


// HTTP-message = start-line *( header-field CRLF ) CRLF [ message-body ]
Element *isHTTPMessage(char *text, ssize_t len) {
    Element *data = addEl("HTTP-message", text, len);

    size_t count = 0;
    if (!isStartLine(text, &count, data)) { return NULL; }
    data = data->fils;

    bool loop = true;
    while (loop) {
        if (!isHeaderField(text+count, &count, data)) { loop = false; }
        data = data->frere;
        if (*(text+count) == CR && *(text+count+1) == LF) {
            Element *el = addEl("__crlf", text+count, 2);
            data->frere = el;
            data = data->frere;
            count += 2;
        } else { return NULL; }
    }

    if (*(text+count) == CR && *(text+count+1) == LF) {
        Element *el = addEl("__crlf", text+count, 2);
        data->frere = el;
        data = data->frere;
        count += 2;
    } else { return NULL; }

    if (isMessageBody(text+count, &count, data)) { ; }

    return data;
}