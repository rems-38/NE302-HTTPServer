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
// cookie-name = token
bool isCookieName(char *text, size_t *curr, Element *data) {
    Element *el = addEl("cookie_name", text, 6); // changer la length
    data->fils = el;
    data = data->fils;

    return isToken(text, curr, data, false);
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
        Element *pre = data;
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

// Cookie-header = "Cookie:" OWS cookie-string OWS
bool isCookieHeader(char *text, Element *data) {
    if (!strcmp(text, "Cookie:")) { return false; }

    Element *cookie = addEl("Cookie", text, 7);
    data->fils = cookie;
    data = data->fils;

    int i_curr = 7;

    if(!isOWS(text+i_curr, &i_curr, data, false)) {return false; }
    data = data->frere;
    if (!isCookieString(text+i_curr, &i_curr, data)) { return false; }
    data = data->frere;
    if(!isOWS(text+i_curr, &i_curr, data, false)) {return false; }

    return true;
}


// obs-text = %x80-FF
bool isObsText(char text) {
    return (text >= 128 && text <= 255);
}


// qdtext = HTAB / SP / "!" / %x23-5B ; '#'-'[' / %x5D-7E ; ']'-'~' / obs-text
bool isQdText(char *text, size_t *curr, Element *data) {
    // ça veut dire quoi les 2 ; au milieu ?!
    // return (text == HTAB || text == SP || text == EXCLAMATION || (text >= HASHTAG && text <= 91) || (text >= 93 && text <= 126));
}

// quoted-pair = "\" ( HTAB / SP / VCHAR / obs-text )
bool isQuotedPair(char *text, size_t *curr, Element *data) {
    return false;
}
// quoted-string = DQUOTE *( qdtext / quoted-pair ) DQUOTE
bool isQuotedString(char *text, size_t *curr, Element *data) {
    return false;
}

// parameter = token "=" ( token / quoted-string )
bool isParameter(char *text, size_t *curr, Element *data) {
    Element *el = addEl("parameter", text, strlen(text));
    data->frere = el;
    data = data->frere;

    size_t count = 0;
    if (!isToken(text, &count, data, false)) { return false; }
    data = data->fils;
    if (*(text+count) == EQUAL) {
        Element *el = addEl("__equal", text+count, 1);
        data->frere = el;
        data = data->frere;
        count += 1;
    } else { return false; }
    int ok = 0;
    if (isToken(text+count, &count, data, true)) { ok = 1; }
    else if (!isQuotedString(text+count, &count, data)) { ok = 1;}
    else { ok = 0; }

    if (ok) {
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

    return isToken(text, curr, data, false);
}

// type = token
bool isType(char *text, size_t *curr, Element *data) {
    Element *el = addEl("type", text, strlen(text));
    data->fils = el;
    data = data->fils;

    return isToken(text, curr, data, false);
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

// Content-Length-header = "Content-Length" ":" OWS Content-Length OWS
bool isContentLengthHeader(char *text, Element *data) {
    if (!strcmp(text, "Content-Length")) { return false; }

    Element *el = addEl("Content-Length-header", text, 14);
    data->fils = el;
    data = data->fils;

    size_t count = 14;
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
    if (!strcmp(text, "Content-Type")) { return false; }

    Element *el = addEl("Content-Type-header", text, 12);
    data->fils = el;
    data = data->fils;

    size_t count = 12;
    if (*(text+count) == COLON) {
        Element *el = addEl("__colon", text+count, 1);
        data->frere = el;
        data = data->frere;
        count += 1;
    } else { return false; }
    if (!isOWS(text+count, &count, data, false)) { return false; }
    data = data->frere;
    if (!isContentType(text+count, &count, data)) { return false; }
    if (!isOWS(text+count, &count, data, false)) { return false; }

    updateLength(data, count);
    return true;
}

// Connection-header = "Connection" ":" OWS Connection OWS
bool isConnectionHeader(char *text, Element *data) {
    if (!strcmp(text, "Connection")) { return false; }

    Element *el = addEl("Connection-header", text, 10);
    data->fils = el;
    data = data->fils;

    size_t count = 10;
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