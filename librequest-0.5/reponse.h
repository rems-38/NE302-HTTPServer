#include "request.h"
#include "config.h"
#include "php.h"

#include <stdbool.h>


typedef struct {
    char *label;
    char *value;
} Header;

typedef struct {
    int code;
    char *info;
} HttpCode;

typedef struct {
    HttpCode *code;
    int httpminor;
    char *filename;
    bool is_head;
    Header *headers;
    int headersCount;
} HttpReponse;

typedef struct {
    HttpCode *table[HTTP_CODE_MAX+1];
    int httpminor;
    char *filename;
    bool is_head;
    bool is_php;
    Header *headers;
    int headersCount;
} HTTPTable;


void initTable(HTTPTable *codes);
void freeTable(HTTPTable *codes);
void addTable(HTTPTable *codes, int code, char *info);
HTTPTable *loadTable();
HttpReponse *getTable(HTTPTable *codes, int code);

void updateHeaderHttpReponse(HttpReponse rep, char *label, char *value);
void updateHeader(HTTPTable *codes, char *label, char *value);
int configFileMsgBody(char *name, HTTPTable *codes);
char *message_body_from_STD_OUT(char* STD_OUT_txt);
char *HexaToChar(char *content);
message *createMsgFromReponse(HttpReponse rep, unsigned int clientId);
message* createMsgFromReponsePHP(HttpReponse rep, unsigned int clientId, char* txtData);
int hexa(char c);
char *message_body_from_STD_OUT(char* STD_OUT_txt);
void headers_from_STDOUT(char* STD_OUT_txt,HttpReponse rep);
int ErrorInSTD_OUT(char* STD_OUT_txt);
char *HexaToChar(char *content);
char* percentEncoding(char* uri);
char* DotRemovalSegment(char* uri);
void controlConnection(message *msg);
int getRepCode(HTTPTable *codes);
//HttpReponse *convertFCGI_HTTP(FCGI_Header *reponse, HTTPTable *codes, int code);
message *generateReponse(message req, int opt_code);