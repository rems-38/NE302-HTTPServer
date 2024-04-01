#include "arbre.h"


/* Arbre du header-field

header-field =  Connection-header / Content-Length-header / Content-Type-header / Cookie-header / Transfer-Encoding-header / Expect-header / Host-header / ( field-name ":" OWS field-value OWS )

OWS = *( SP / HTAB )

Connection-header = "Connection" ":" OWS Connection OWS
Connection-header = "Connection" ":" OWS *( "," OWS ) connection-option *( OWS "," [ OWS connection-option ] ) OWS
Connection-header = "Connection" ":" OWS *( "," OWS ) token *( OWS "," [ OWS token ] ) OWS
Connection-header = "Connection" ":" OWS *( "," OWS ) 1*("!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA) *( OWS "," [ OWS 1*("!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA) ] ) OWS

Content-Length-header = "Content-Length" ":" OWS Content-Length OWS

Content-Type-header = "Content-Type" ":" OWS Content-Type OWS

Cookie-header = "Cookie:" OWS cookie-string OWS

HTAB :  	

*/
#define AMAJ 65
#define FMAJ 70
#define ZMAJ 90
#define AMIN 97
#define ZMIN 122
#define ZERO 48
#define NINE 57
#define SP 32           // espace
#define HTAB 9          // \t
#define DASH 45         // -
#define UNDERSCORE 95   // _
#define COMMA 44        // ,
#define DOT 46          // .
#define EXCLAMATION 33  // !
#define QUESTION 63     // ?
#define EQUAL 61        // =
#define COLON 58        // :
#define SEMICOLON 59    // ;
#define HASHTAG 35      // #
#define DOLLAR 36       // $
#define POURCENT 37     // %
#define ESP 38          // &
#define SQUOTE 39       // '
#define DQUOTE 34       // "
#define FQUOTE 96       // `
#define STAR 42         // *
#define PLUS 43         // +
#define SLASH 47        // /
#define CIRCONFLEXE 94  // ^
#define BARRE 124       // |
#define VAGUE 126       // ~
#define OPAREN 40       // (
#define CPAREN 41       // )
#define OBRACKET 91     // [
#define CBRACKET 93     // ]
#define LF 10           // \n
#define CR 13


// OWS = *( SP / HTAB )
bool isOWS(char *text, size_t *curr, Element *data, bool is_fils) {
    Element *el = addEl("OWS", text, strlen(text));
    if (is_fils) {
        data->fils = el;
        data = data->fils;
    } else {
        data->frere = el;
        data = data->frere;
    }

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
    
    updateLength(data, count);
    *curr += count;
    return true;
}


bool isAlpha(char text){
    return (text >= AMAJ && text <= ZMAJ) || (text >= AMIN && text <= ZMIN);
}
bool isDigit(char text) {
    return (text >= ZERO && text <= NINE);
}
// tchar = ("!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA)
bool isTchar(char text){ 
    return (text == EXCLAMATION || text == HASHTAG || text == DOLLAR || text == POURCENT || text == ESP || text == SQUOTE || text == STAR || text == PLUS || text == DASH || text == DOT || text == CIRCONFLEXE || text == UNDERSCORE || text == 96 || text == BARRE || text == VAGUE || isAlpha(text) || isDigit(text)) ;
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
    // ça veut dire quoi les 2 ; au milieu ?!
    // return (text == HTAB || text == SP || text == EXCLAMATION || (text >= HASHTAG && text <= 91) || (text >= 93 && text <= 126));
    return true;
}

// quoted-pair = "\" ( HTAB / SP / VCHAR / obs-text )
bool isQuotedPair(char *text, size_t *curr, Element *data) {
    return true;
}

// quoted-string = DQUOTE *( qdtext / quoted-pair ) DQUOTE
bool isQuotedString(char *text, size_t *curr, Element *data) {
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

    return isToken(text, curr, data, true);
}

// type = token
bool isType(char *text, size_t *curr, Element *data) {
    Element *el = addEl("type", text, strlen(text));
    data->fils = el;
    data = data->fils;

    return isToken(text, curr, data, true);
}

// media-type = type "/" subtype *( OWS ";" OWS parameter )
bool isMediaType(char *text, size_t *curr, Element *data) {
    Element *el = addEl("media-type", text, strlen(text));
    data->fils = el;
    data = data->fils;

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

    updateLength(data, count);
    *curr += count;
    return true;
}

//  Content-Type = media-type
bool isContentType(char *text, size_t *curr, Element *data) {
    Element *el = addEl("Content-Type", text, strlen(text));
    data->frere = el;
    data = data->frere;

    return isMediaType(text, curr, data);
}

// Content-Length = 1*DIGIT
bool isContentLength(char *text, size_t *curr, Element *data) {
    size_t count = 0;

    Element *el = addEl("Content-Length", text, strlen(text));
    data->frere = el;
    data = data->frere;

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

    updateLength(data, count);
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

    while (*(text+count) == SP || *(text+count) == HTAB) {
        if (!isOWS(text+count, &count, data, false)) { return false; }
        data = data->frere;
        if (*(text+count) == COMMA) {
            Element *el = addEl("__comma", text+count, 1);
            data->frere = el;
            data = data->frere;
            count += 1;
            if (*(text+count) == SP || *(text+count) == HTAB) {
                if (!isOWS(text+count, &count, data, false)) { return false; }
                data = data->frere;
                if (!isConnectionOption(text+count, &count, data, false)) { return false; }
            }
        }
    }

    updateLength(data, count);
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

// Cookie-header = "Cookie:" OWS cookie-string OWS
bool isCookieHeader(char *text, Element *data) {
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

    return true;
}

// Content-Length-header = "Content-Length" ":" OWS Content-Length OWS
bool isContentLengthHeader(char *text, Element *data) {
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
    if (!isOWS(text+count, &count, data, false)) { return false; }


    updateLength(data, count);
    return true;
}

// Content-Type-header = "Content-Type" ":" OWS Content-Type OWS
bool isContentTypeHeader(char *text, Element *data) {
    Element *el = addEl("Content-Type-header", text, strlen(text));
    data->fils = el;
    data = data->fils;

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

    updateLength(data, count);
    return true;
}

// Connection-header = "Connection" ":" OWS Connection OWS
bool isConnectionHeader(char *text, Element *data) {
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
    if (!isOWS(text+count, &count, data, false)) { return false; }

    updateLength(data, count);
    return true;
}

// Expect-header = "Expect" ":" OWS Expect OWS
bool isExpectHeader(char *text, Element *data) {
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
    return true;
}

// Host-header = "Host" ":" OWS Host OWS
bool isHostHeader(char *text, Element *data) {
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
    return true;
}

bool verifHeaderField(Element *data){
    bool res = false;

    Element *el = malloc(sizeof(Element));
    el->key = "header_field";
    el->word = data->word;
    data->fils = el;
    data = data->fils;

    if (isCookieHeader(data->word, data)) {
        res = true;
    }

    // if (isConnectionHeader(text, curr, head)) {
    //     res = true;
    // } else if (isContentLengthHeader(text, curr, head)) {
    //     res = true;
    // } else if (isContentTypeHeader(text, curr, head)) {
    //     res = true;
    // } else if (isCookieHeader(text, curr, head)) {
    //     res = true;
    // } else if (isTransferEncodingHeader(text, curr, head)) {
    //     res = true;
    // } else if (isExpectHeader(text, curr, head)) {
    //     res = true;
    // } else if (isHostHeader(text, curr, head)) {
    //     res = true;
    // } else {
    //     if (!isFieldName(text, curr, head)) { return false; }
    //     if (!isColon(text, curr, head)) { return false; }
    //     if (!isOWS(text, curr, head)) { return false; }
    //     if (!isFieldValue(text, curr, head)) { return false; }
    //     if (!isOWS(text, curr, head)) { return false; }
    //     res = true;
    // }

    return res;
}


int main(void) {
    
    FILE *ftest = fopen("header.txt", "r");
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    Element *message = malloc(sizeof(Element));
    message->key = "HTTP_message";

    if (ftest == NULL) {
        printf("Impossible d'ouvrir le fichier %s\n", "header.txt");
        return -1;
    }

    if ((read = getline(&line, &len, ftest)) != -1) {
        message->word = line;
        message->length = read;
        int output = verifHeaderField(message);
        printf("%d\n", output);

        printArbre(message, 0);
    }

    fclose(ftest);
    if (line) free(line);
    return 0;
}