#include "request.h"

typedef struct {
    int code;
    char *info;
    char **headers;
    int headersCount;
} reponse;

typedef struct listReponse listReponse;
struct listReponse {
    reponse current;
    listReponse *next;
};

message *createMsgFromReponse(reponse rep, unsigned int clientId);
listReponse *initReps();
int getRepCode(message req);
reponse generateReponse(message req);