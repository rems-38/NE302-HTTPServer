#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "reponse.h"

message *createMsgFromReponse(reponse rep, unsigned int clientId) {
    message *msg = malloc(sizeof(message));
    
    char *code = malloc(sizeof(int));
    sprintf(code, "%d", rep.code);
    
    // Calcul de la taille nécessaire pour buf
    int bufSize = strlen("HTTP/1.0 ") + strlen(code) + strlen(" ") + strlen(rep.info) + strlen("\r\n");
    for (int i = 0; i < rep.headersCount; i++) {
        bufSize += strlen(rep.headers[i]) + strlen("\r\n");
    }
    bufSize += strlen("\r\n");

    char *buf = malloc(bufSize + 1);
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

    msg->buf = malloc(bufSize + 1);
    strcpy(msg->buf, buf);
    msg->len = bufSize;
    msg->clientId = clientId;

    free(code);
    free(buf);
    
    return msg;
}

listReponse *initReps() {
    listReponse *reps = malloc(sizeof(listReponse));
    listReponse *head = reps;

    char *headers[] = {"Content-Type: text/html", "Content-Length: 0", "Connection: keep-alive"};
    
	reponse repOk = {.code = 200, .info = "OK", .headers = headers, .headersCount = sizeof(headers) / sizeof(headers[0])};
    reps->current = repOk;
    reps->next = malloc(sizeof(listReponse));
    reps = reps->next;
    
    reponse repCreated = {.code = 201, .info = "Created", .headers = headers, .headersCount = sizeof(headers) / sizeof(headers[0])};
    reps->current = repCreated;
    reps->next = malloc(sizeof(listReponse));
    reps = reps->next;
    
    reponse repAccepted = {.code = 202, .info = "Accepted", .headers = headers, .headersCount = sizeof(headers) / sizeof(headers[0])};
    reps->current = repAccepted;
    reps->next = malloc(sizeof(listReponse));
    reps = reps->next;
    
    reponse repNoContent = {.code = 204, .info = "No Content", .headers = headers, .headersCount = sizeof(headers) / sizeof(headers[0])};
    reps->current = repNoContent;
    reps->next = malloc(sizeof(listReponse));
    reps = reps->next;
    
    reponse repBadRequest = {.code = 400, .info = "Bad Request", .headers = headers, .headersCount = sizeof(headers) / sizeof(headers[0])};
    reps->current = repBadRequest;
    reps->next = malloc(sizeof(listReponse));
    reps = reps->next;
    
    reponse repUnauthorized = {.code = 401, .info = "Unauthorized", .headers = headers, .headersCount = sizeof(headers) / sizeof(headers[0])};
    reps->current = repUnauthorized;
    reps->next = malloc(sizeof(listReponse));
    reps = reps->next;
    
    reponse repForbidden = {.code = 403, .info = "Forbidden", .headers = headers, .headersCount = sizeof(headers) / sizeof(headers[0])};
    reps->current = repForbidden;
    reps->next = malloc(sizeof(listReponse));
    reps = reps->next;
    
    reponse repNotFound = {.code = 404, .info = "Not Found", .headers = headers, .headersCount = sizeof(headers) / sizeof(headers[0])};
    reps->current = repNotFound;
    reps->next = malloc(sizeof(listReponse));
    reps = reps->next;
    
    reponse repMethodNotAllowed = {.code = 405, .info = "Method Not Allowed", .headers = headers, .headersCount = sizeof(headers) / sizeof(headers[0])};
    reps->current = repMethodNotAllowed;
    reps->next = malloc(sizeof(listReponse));
    reps = reps->next;
    
    reponse repInternalServerError = {.code = 500, .info = "Internal Server Error", .headers = headers, .headersCount = sizeof(headers) / sizeof(headers[0])};
    reps->current = repInternalServerError;
    reps->next = malloc(sizeof(listReponse));
    reps = reps->next;
    
    reponse repNotImplemented = {.code = 501, .info = "Not Implemented", .headers = headers, .headersCount = sizeof(headers) / sizeof(headers[0])};
    reps->current = repNotImplemented;
    reps->next = malloc(sizeof(listReponse));
    reps = reps->next;
    
    reponse repServiceUnavailable = {.code = 503, .info = "Service Unavailable", .headers = headers, .headersCount = sizeof(headers) / sizeof(headers[0])};
    reps->current = repServiceUnavailable;
    reps->next = malloc(sizeof(listReponse));
    reps = reps->next;
    
    return head;
}


int getRepCode(message req) {
    


}

reponse generateReponse(message req) {

    int code = getRepCode(req);

}