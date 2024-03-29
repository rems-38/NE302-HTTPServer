#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>

#include "verifMessage.h"
#include "arbre.h"

typedef struct Element {
    char *key;
    char *word;
    size_t length;
    struct Element *fils;
    struct Element *frere;
} Element;

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
#define HASHTAG 35
#define DOLLAR 36
#define POURCENT 37
#define ESP 38          // &
#define SQUOTE 39       // '
#define STAR 42         // *
#define PLUS 43
#define CIRCONFLEXE 94  // ^
#define BARRE 124       // |
#define VAGUE 126       // ~
#define FQUOTE 96       // `
#define LF 10           // \n
#define CR 13


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

bool verifStartLine(char *text, size_t *curr, Element *head);

bool verifHeaderField(char *text, size_t *curr);

int verifMessage(Element *data);