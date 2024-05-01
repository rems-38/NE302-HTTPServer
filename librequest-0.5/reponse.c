#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "reponse.h"

message *createMsgFromReponse(reponse rep, unsigned int clientId) {
    message *msg = malloc(sizeof(message));
    
    char *code = malloc(sizeof(int));
    sprintf(code, "%d", rep.code);
    
    int bufSize = 0;
    char *buf = malloc(1000); // à voir si on peut changer ça
    strcpy(buf, "HTTP/1.0 ");
    strcat(buf, code);
    strcat(buf, " ");
    strcat(buf, rep.info);
    strcat(buf, "\r\n");
    for (int i = 0; i < rep.headersCount; i++) {
        strcat(buf, rep.headers[i]);
        strcat(buf, "\r\n");

        bufSize += strlen(rep.headers[i]) + strlen("\r\n");
    }
    strcat(buf, "\r\n");
    
    bufSize += strlen("HTTP/1.0 ") + strlen(code) + strlen(" ") + strlen(rep.info) + strlen("\r\n") + strlen("\r\n");

    printf("bufSize: %d\n", bufSize);

    msg->buf = malloc(bufSize);
    strcpy(msg->buf, buf);
    msg->len = bufSize;
    msg->clientId = clientId;
    
    return msg;
}