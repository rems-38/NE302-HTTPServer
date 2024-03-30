#include "verifMessage.h"


// HTTP-message = start-line *( header-field CRLF ) CRLF [ message-body ]

/* 
Arbre de la start-line :

start-line = request-line
request-line = method SP request-target SP HTTP-version CRLF
request-line = token SP origin-form SP HTTP-version CRLF
request-line = token SP origin-form SP HTTP-name "/" DIGIT "." DIGIT CRLF
request-line = token SP absolute-path [ "?" query ] SP HTTP-name "/" DIGIT "." DIGIT CRLF
request-line = token SP 1*( "/" segment ) [ "?" query ] SP HTTP-name "/" DIGIT "." DIGIT CRLF
request-line = 1*tchar SP 1*( "/" segment ) [ "?" query ] SP HTTP-name "/" DIGIT "." DIGIT CRLF
request-line = 1*("!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA) SP 1*( "/" segment ) [ "?" query ] SP HTTP-name "/" DIGIT "." DIGIT CRLF
request-line = 1*("!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA) SP 1*( "/" *pchar ) [ "?" *( pchar / "/" / "?" ) ] SP %x48.54.54.50 ; HTTP "/" DIGIT "." DIGIT CRLF
request-line = 1*("!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA) SP 1*( "/" *(unreserved / pct-encoded / sub-delims / ":" / "@") ) [ "?" *(unreserved / pct-encoded / sub-delims / ":" / "@" / "/" / "?" ) ] SP %x48.54.54.50 ; HTTP "/" DIGIT "." DIGIT CRLF
request-line = 1*("!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA) SP 1*( "/" *(ALPHA / DIGIT / "-" / "." / "_" / "~" / "%" HEXDIG HEXDIG / "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "=" / ":" / "@") ) [ "?" *(ALPHA / DIGIT / "-" / "." / "_" / "~" / "%" HEXDIG HEXDIG / "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "=" / ":" / "@" / "/" / "?" ) ] SP %x48.54.54.50 ; HTTP "/" DIGIT "." DIGIT CRLF
*/

bool verifStartLine(char *text, size_t *curr, Element *head){ //start-line = request-line
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


*/
/*
bool verifHeaderField(char *text, size_t *curr, Element *head){
    bool res = false;

    Element *tmp = malloc(sizeof(Element));


    return res;
}
*/

int verifMessage(Element *data){

    Element *head = data; //pointeur vers la tete de l'arbre
    char *text = data->word;
    size_t curr = 0;

    if(!verifStartLine(text,&curr,data)){ 
        return -1;
    }
    //data = data->fils;
    /*
    bool boucle = true;

    while(boucle){
        if(verifHeaderField(text,&curr, data)){
            data = data->frere;
            if(isCRLF(text, curr, data)){
                data= data->frere;
            }
            else{
                boucle = false;
            }
        }
        else {
            boucle = false;
        }
    }

    if(!isCRLF(text, curr, data)){return -1;}
    data= data->frere;

    while(isMessageBody(text, curr)){
        data=data->frere;
        curr++;
    }
    */
    data = head;
    return 1;
}