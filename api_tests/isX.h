#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>

#include "arbre.h"


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
#define BACKSLASH 92    // '\'
#define CIRCONFLEXE 94  // ^
#define BARRE 124       // |
#define VAGUE 126       // ~
#define OPAREN 40       // (
#define CPAREN 41       // )
#define OBRACKET 91     // [
#define CBRACKET 93     // ]
#define LF 10           // \n
#define CR 13

bool isAlpha(char text);
bool isDigit(char text);
bool isTchar(char text);
bool isHEXDIG(char text);
bool isSP(char text, size_t *curr, Element *head);
bool isOWS(char *text, size_t *curr, Element *data, bool is_fils);
int OWSCH(char *text);
bool isToken(char *text, size_t *curr, Element *data, bool is_fils);
bool isObsText(unsigned char text);
bool isFieldName(char *text, size_t *curr, Element *data);
bool isFieldVchar(char text, Element *data);
bool isFieldContent(char *text, size_t *curr, Element *data, bool is_fils);
bool isObsFold(char *text, size_t *curr, Element *data, bool is_fils);
bool isFieldValue(char *text, size_t *curr, Element *data);
bool isCookieName(char *text, size_t *curr, Element *data);
bool isDQUOTE(char text, Element *head, bool is_fils);
bool isCookieOctet(char *text, size_t *curr, Element *data, bool is_fils);
bool isCookieValue(char *text, size_t *curr, Element *data);
bool isCookiePair(char *text, size_t *curr, Element *data, bool is_fils);
bool isCookieString(char *text, size_t *curr, Element *data);
bool isQdText(char *text, size_t *curr, Element *data);
bool isQuotedPair(char *text, size_t *curr, Element *data);
bool isQuotedString(char *text, size_t *curr, Element *data);
bool isParameter(char *text, size_t *curr, Element *data);
bool isSubType(char *text, size_t *curr, Element *data);
bool isType(char *text, size_t *curr, Element *data);
bool isMediaType(char *text, size_t *curr, Element *data);
bool isContentType(char *text, size_t *curr, Element *data);
bool isContentLength(char *text, size_t *curr, Element *data);
bool isConnectionOption(char *text, size_t *curr, Element *data, bool is_fils);
bool isConnection(char *text, size_t *curr, Element *data);
bool isUnreserved(char text);
bool isSubDelims(char text);
bool isHexdig(char text);
bool isIPvFuture(char *text, size_t *curr, Element *data);
bool isDecOctet(char *text, size_t *curr, Element *data, bool is_fils);
bool isIPv4address(char *text, size_t *curr, Element *data);
bool isH16(char *text, size_t *curr, Element *data, bool is_fils);
bool isLS32(char *text, size_t *curr, Element *data);
bool isIPv6Address(char *text, size_t *curr, Element *data);
bool isIPLiteral(char *text, size_t *curr, Element *data);
bool isPctEncoded(char *text, size_t *curr, Element *data);
bool isRegName(char *text, size_t *curr, Element *data);
bool ishost(char *text, size_t *curr, Element *data);
bool isUriHost(char *text, size_t *curr, Element *data);
bool isPort(char *text, size_t *curr, Element *data);
bool isHost(char *text, size_t *curr, Element *data);
bool isExpect(char *text, size_t *curr, Element *data);
bool isTransferCoding(char *text, size_t *curr, Element *data, bool is_fils);
int OWSTEH(char *text);
bool isTransferEncoding(char *text, size_t *curr, Element *data);
bool isTransferEncodingHeader(char *text, size_t *curr, Element *data);
bool isCookieHeader(char *text, size_t *curr, Element *data);
bool isContentLengthHeader(char *text, size_t *curr, Element *data);
bool isContentTypeHeader(char *text, size_t *curr, Element *data);
bool isConnectionHeader(char *text, size_t *curr, Element *data);
bool isExpectHeader(char *text, size_t *curr, Element *data);
bool isHostHeader(char *text, size_t *curr, Element *data);
bool isHeaderField(char *text, size_t *curr, Element *data);
bool isRequestLine(char *text, size_t *curr, Element *head);
bool isMethod(char *text, size_t *curr, Element *head);
bool isRequestTarget(char *text, size_t *curr, Element *head);
bool isOriginForm(char *text, size_t *curr, Element *head);
bool isQuery(char *text, size_t *curr, Element *head);
bool isAbsolutePath(char *text, size_t *curr, Element *head);
bool isSegment(char *text, size_t *curr, Element *head);
bool isPchar(char *text, size_t *curr, Element *head);
bool isUnreservedBis(char text, Element *head);
bool isPctEncodedBis(char *text, size_t *curr, Element *head);
bool isSubDelimsBis(char text, Element *head);
bool isHTTPVersion(char *text, size_t *curr, Element *head);
bool isHTTPname(char *text);
bool isMessageBody(char *text, Element *head);
bool isStartLine(char *text, size_t *curr, Element *head);
struct Element *isHTTPMessage(char *text, ssize_t len);