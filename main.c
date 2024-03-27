#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>

#define INIT "message"

#define AMAJ 65
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
#define COLON 58        // :
#define LF 10           // \n

/*
Table ASCII :
A-Z = 65-90
a-z = 97-122
0-9 = 48-57
SP = 32
HTAB = 9
- = 45
_ = 95
, = 44
. = 46
! = 33
? = 63
: = 58
LF = 10
*/

typedef struct Element {
    char *key;
    char *word;
    size_t length;
    struct Element *fils;
    struct Element *frere;
} Element;




int main(int argc, char *argv[]) {
    
    FILE *ftest = fopen(argv[1], "r");
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    Element *message = malloc(sizeof(Element));
    message->key = INIT;

    if (ftest == NULL) {
        printf("Impossible d'ouvrir le fichier %s\n", argv[1]);
        return -1;
    }

    if ((read = getline(&line, &len, ftest)) != -1) {
        message->word = line;
        message->length = read;
        int output = verifMessage(message);
        printf("%d\n", output);

        printArbre(message, 0);
    }

    fclose(ftest);
    if (line) free(line);
    return 0;
}

// HTTP-message = start-line *( header-field CRLF ) CRLF [ message-body ]


/* Arbre de la start-line

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
bool verifStartLine(char *text, size_t curr){
    return true;
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


bool verifHeaderField(char *text, size_t curr){
    return true;
}


int verifMessage(Element *data){

    char *text = data->word;
    size_t curr = 0;

    if(!verifRequestLine(text,curr)){
        return -1;
    }

    bool boucle = true;

    while(boucle){
        if(!verifHeaderField(text,curr)){
            return -1;
        }
    }

    return 1;
}
