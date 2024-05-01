#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "reponse.h"

message *createMsgFromReponse(reponse rep, unsigned int clientId) {
    message *msg = malloc(sizeof(message));
    
    char *code = malloc(sizeof(int));
    sprintf(code, "%d", rep.code);
    
    // int bufSize = sizeof(int) + strlen(rep.info) + rep.headersCount;
    int bufSize = 150;
    char *buf = malloc(bufSize);
    strcpy(buf, "HTTP/1.0 ");
    strcat(buf, code);
    strcat(buf, " ");
    strcat(buf, rep.info);
    strcat(buf, "\r\n");
    for (int i = 0; i < rep.headersCount; i++) {
        strcat(buf, rep.headers[i]);
        strcat(buf, "\r\n");
    }
    strcat(buf, "\r\n");

    msg->buf = malloc(bufSize);
    strcpy(msg->buf, buf);
    msg->len = bufSize;
    msg->clientId = clientId;
    
    return msg;
}