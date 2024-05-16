#include "request.h"
#include "config.h"


typedef struct {
    int code;
    char *info;
    char **headers;
    int headersCount;
} HttpCode;

typedef struct {
    HttpCode *table[NB_HTTP_CODES];
} HTTPTable;


uint32_t hash(uint32_t code, uint32_t nbTry);
void initTable(HTTPTable *codes);
void freeTable(HTTPTable *codes);
void addTable(HTTPTable *codes, int code, char *info, char **headers, int headersCount);
HTTPTable *loadTable();
HttpCode *getTable(HTTPTable *codes, int code);

message *createMsgFromReponse(HttpCode rep, FILE *fout, unsigned int clientId);
int getRepCode(message req, FILE **fout) ;
message *generateReponse(message req, int opt_code);