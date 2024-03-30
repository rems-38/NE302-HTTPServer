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
Cookie-header = "Cookie:" OWS cookie-pair *( ";" SP cookie-pair ) OWS
Cookie-header = "Cookie:" OWS cookie-name "=" cookie-value *( ";" SP cookie-name "=" cookie-value ) OWS
Cookie-header = "Cookie:" OWS token "=" ( DQUOTE *cookie-octet DQUOTE ) / *cookie-octet *( ";" SP token "=" ( DQUOTE *cookie-octet DQUOTE ) / *cookie-octet ) OWS
Cookie-header = "Cookie:" OWS 1*tchar "=" ( DQUOTE *cookie-octet DQUOTE ) / *cookie-octet *( ";" SP 1*tchar "=" ( DQUOTE *cookie-octet DQUOTE ) / *cookie-octet ) OWS
Cookie-header = "Cookie:" OWS 1*tchar "=" ( DQUOTE *(%x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E) DQUOTE ) / *(%x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E) *( ";" SP 1*tchar "=" ( DQUOTE *(%x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E) DQUOTE ) / *(%x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E) ) OWS

HTAB :  	

*/

#define SP 32
#define HTAB 9

bool isConnectionHeader(char *text, size_t *curr, Element *head) {
}

bool isOWS(char *text, size_t *curr, Element *data) {
    Element *el = addEl("OWS", text, strlen(text));
    Element *head = el;

    size_t count = 0;
    Element *sub;
    while (text[count] == SP || text[count] == HTAB) {
        if (text[count] == SP) { sub = addEl("__sp", text+count, 1); }
        else if (text[count] == HTAB) { sub = addEl("__htab", text+count, 1); }

        if (count == 0) { // ou el->fils == NULL mais vu qu'on change el faudrait le save...
            el->fils = sub;
            el = el->fils;
        }
        else {
            el->frere = sub;
            el = el->frere;
        }
        count++;
    }

    if (data->fils == NULL) {
        data->fils = head;
        data = data->fils;
    } else {
        Element *top = data;
        while (data->frere != NULL) {
            data = data->frere;
        }
        data->frere = head;
        data = top;
    }

    *curr += count;
    return true;
}

bool isCookiePair(char *text, size_t *curr, Element *data) {
    *curr += 4;
    return strcmp(text, "pair");
}

bool isCookieString(char *text, size_t *curr, Element *data) {
    size_t count = 0;

    Element *el = addEl("cookie-string", text, strlen(text));
    data->frere = el;
    data = data->frere;
    Element *save = data;

    if (!isCookiePair(text, &count, data)) { return false; }

    if (*(text+count) == ';') {
        while (text[count] == ';' && text[count+1] == SP) {
            Element *el1 = addEl("__colon", text+count, 1);
            data->frere = el1;
            data = data->frere;

            Element *el2 = addEl("__sp", text+count+1, 1);
            data->frere = el2;
            data = data->frere;

            count += 2;
            if (!isCookiePair(text+count, &count, data)) { return false; }
        }
    }

    data = save;

    *curr += count;
    return true;
}

bool isCookieHeader(char *text, Element *data) {
    if (!strcmp(text, "Cookie:")) { return false; }

    Element *cookie = addEl("Cookie", text, 7);
    data->fils = cookie;
    data = data->fils;

    int i_curr = 7;

    isOWS(text+i_curr, &i_curr, data);
    data = data->fils;
    if (!isCookieString(text+i_curr, &i_curr, data)) { return false; }
    isOWS(text+i_curr, &i_curr, data);
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