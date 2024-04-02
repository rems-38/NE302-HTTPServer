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

bool isOCTET(char text);

bool isHEXDIG(char text);

bool isSP(char text, size_t *curr, Element *head);

// OWS = *( SP / HTAB )
bool isOWS(char *text, size_t *curr, Element *data, bool is_fils);

int OWSCH(char *text);

// token = 1*tchar
bool isToken(char *text, size_t *curr, Element *data, bool is_fils);

// obs-text = %x80-FF
bool isObsText(char text);

// field-name = token
bool isFieldName(char *text, size_t *curr, Element *data);

// field-vchar = VCHAR / obs-text
bool isFieldVchar(char text, Element *data);

// field-content = field-vchar [ 1*( SP / HTAB ) field-vchar ]
bool isFieldContent(char *text, size_t *curr, Element *data);

// obs-fold = CRLF 1*( SP / HTAB )
bool isObsFold(char *text, size_t *curr, Element *data);

// field-value = *( field-content / obs-fold )
bool isFieldValue(char *text, size_t *curr, Element *data);

// cookie-name = token
bool isCookieName(char *text, size_t *curr, Element *data);

bool isDQUOTE(char text, Element *head, bool is_fils);

// cookie-octet = *(%x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E)
bool isCookieOctet(char *text, size_t *curr, Element *data, bool is_fils);

// cookie-value = ( DQUOTE *cookie-octet DQUOTE ) / *cookie-octet
bool isCookieValue(char *text, size_t *curr, Element *data);

// cookie-pair = cookie-name "=" cookie-value
bool isCookiePair(char *text, size_t *curr, Element *data, bool is_fils);

// cookie-string = cookie-pair *( ";" SP cookie-pair )
bool isCookieString(char *text, size_t *curr, Element *data);

// qdtext = HTAB / SP / "!" / %x23-5B ; '#'-'[' / %x5D-7E ; ']'-'~' / obs-text
bool isQdText(char *text, size_t *curr, Element *data);

// quoted-pair = "\" ( HTAB / SP / VCHAR / obs-text )
bool isQuotedPair(char *text, size_t *curr, Element *data);

// quoted-string = DQUOTE *( qdtext / quoted-pair ) DQUOTE
bool isQuotedString(char *text, size_t *curr, Element *data);

// parameter = token "=" ( token / quoted-string )
bool isParameter(char *text, size_t *curr, Element *data);

// type = token
bool isSubType(char *text, size_t *curr, Element *data);

// type = token
bool isType(char *text, size_t *curr, Element *data);

// media-type = type "/" subtype *( OWS ";" OWS parameter )
bool isMediaType(char *text, size_t *curr, Element *data);

//  Content-Type = media-type
bool isContentType(char *text, size_t *curr, Element *data);

// Content-Length = 1*DIGIT
bool isContentLength(char *text, size_t *curr, Element *data);

// connection-option = token
bool isConnectionOption(char *text, size_t *curr, Element *data, bool is_fils);

// Connection = *( "," OWS ) connection-option *( OWS "," [ OWS connection-option ] )
bool isConnection(char *text, size_t *curr, Element *data);

// unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~"
bool isUnreserved(char text);

// sub-delims = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="
bool isSubDelims(char text);

// HEXDIG = DIGIT / "A" / "B" / "C" / "D" / "E" / "F"
bool isHexdig(char text);

// IPvFuture = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
bool isIPvFuture(char *text, size_t *curr, Element *data);

// dec-octet     = "25" %x30-35          ; 250-255
//               / "2" %x30-34 DIGIT     ; 200-249
//               / "1" 2DIGIT            ; 100-199
//              / %x31-39 DIGIT         ; 10-99
//              / DIGIT                 ; 0-9
bool isDecOctet(char *text, size_t *curr, Element *data, bool is_fils);

// IPv4address = dec-octet "." dec-octet "." dec-octet "." dec-octet
bool isIPv4address(char *text, size_t *curr, Element *data);
// h16 = 1*4HEXDIG
bool isH16(char *text, size_t *curr, Element *data, bool is_fils);

// ls32 = ( h16 ":" h16 ) / IPv4address
bool isLS32(char *text, size_t *curr, Element *data);

// IPv6address =                             6( h16 ":" ) ls32
//              /                       "::" 5( h16 ":" ) ls32
//              / [ h16               ] "::" 4( h16 ":" ) ls32
//              / [ h16 *1( ":" h16 ) ] "::" 3( h16 ":" ) ls32
//              / [ h16 *2( ":" h16 ) ] "::" 2( h16 ":" ) ls32
//              / [ h16 *3( ":" h16 ) ] "::"    h16 ":"   ls32
//              / [ h16 *4( ":" h16 ) ] "::"              ls32
//              / [ h16 *5( ":" h16 ) ] "::"              h16
//              / [ h16 *6( ":" h16 ) ] "::"
bool isIPv6Address(char *text, size_t *curr, Element *data);

// IP-literal = "[" ( IPv6address / IPvFuture  ) "]"
bool isIPLiteral(char *text, size_t *curr, Element *data);

// pct-encoded = "%" HEXDIG HEXDIG
bool isPctEncoded(char *text, size_t *curr, Element *data);

// reg-name = *( unreserved / pct-encoded / sub-delims )
bool isRegName(char *text, size_t *curr, Element *data);

// host = IP-literal / IPv4address / reg-name
bool ishost(char *text, size_t *curr, Element *data);

// uri-host = host
bool isUriHost(char *text, size_t *curr, Element *data);

// port = *DIGIT
bool isPort(char *text, size_t *curr, Element *data);

// Host = uri-host [ ":" port ]
bool isHost(char *text, size_t *curr, Element *data);

// Expect = "100-continue"
bool isExpect(char *text, size_t *curr, Element *data);

bool isTransferCoding(char *text, size_t *curr, Element *data, bool is_fils);

int OWSTEH(char *text);

bool isTransferEncoding(char *text, size_t *curr, Element *data);

bool isTransferEncodingHeader(char *text, size_t *curr, Element *data);

// Cookie-header = "Cookie:" OWS cookie-string OWS
bool isCookieHeader(char *text, size_t *curr, Element *data);

// Content-Length-header = "Content-Length" ":" OWS Content-Length OWS
bool isContentLengthHeader(char *text, size_t *curr, Element *data);

// Content-Type-header = "Content-Type" ":" OWS Content-Type OWS
bool isContentTypeHeader(char *text, size_t *curr, Element *data);
// Connection-header = "Connection" ":" OWS Connection OWS

bool isConnectionHeader(char *text, size_t *curr, Element *data);

// Expect-header = "Expect" ":" OWS Expect OWS
bool isExpectHeader(char *text, size_t *curr, Element *data);

// Host-header = "Host" ":" OWS Host OWS
bool isHostHeader(char *text, size_t *curr, Element *data);

// header-field =  Connection-header / Content-Length-header / Content-Type-header / Cookie-header / Transfer-Encoding-header / Expect-header / Host-header / ( field-name ":" OWS field-value OWS )
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

bool isMessageBody(char *text, size_t *curr, Element *head);

bool isStartLine(char *text, size_t *curr, Element *head);

// HTTP-message = start-line *( header-field CRLF ) CRLF [ message-body ]
Element *isHTTPMessage(char *text, ssize_t len);