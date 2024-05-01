#include "request.h"

typedef struct {
    int code;
    char *info;
    char **headers; 
} reponse;

message *createMsgFromReponse(reponse rep, unsigned int clientId);