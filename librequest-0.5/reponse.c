#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "reponse.h"
#include "../api/api.h"
#include "config.h"


uint32_t h1(uint32_t code) { return code % NB_HTTP_CODES; }
uint32_t h2(uint32_t code) { return 1 + (code % (NB_HTTP_CODES - 1)); }
uint32_t hash(uint32_t code, uint32_t nbTry) {
    return (h1(code) + nbTry * h2(code)) % NB_HTTP_CODES;
}

void initTable(HTTPTable *codes) {
    for (int i = 0; i < NB_HTTP_CODES; i++) { codes->table[i] = NULL; }
}

void freeTable(HTTPTable *codes) {
    for (int i = 0; i < NB_HTTP_CODES; i++) {
        if (codes->table[i] != NULL) {
            free(codes->table[i]->info);
            for (int j = 0; j < codes->table[i]->headersCount; j++) {
                free(codes->table[i]->headers[j]);
            }
            free(codes->table[i]->headers);
            free(codes->table[i]);
        }
    }
}

void addTable(HTTPTable *codes, int code, char *info, char **headers, int headersCount) {
    HttpCode *el = malloc(sizeof(HttpCode));
    el->code = code;
    el->info = malloc(strlen(info) + 1);
    strcpy(el->info, info);
    el->headers = malloc(headersCount * sizeof(char *));
    for (int i = 0; i < headersCount; i++) {
        el->headers[i] = malloc(strlen(headers[i]) + 1);
        strcpy(el->headers[i], headers[i]);
    }
    el->headersCount = headersCount;

    int nbTry = 0;
    while (codes->table[hash(code, nbTry)] != NULL) { nbTry++; }
    codes->table[hash(code, nbTry)] = el;
}

HTTPTable *loadTable() {
    HTTPTable *codes = malloc(sizeof(HTTPTable));
    initTable(codes);
    
    char *headers[] = {"Content-Type: text/html", "Content-Length: 0", "Connection: keep-alive"};
    int headersCount = sizeof(headers) / sizeof(headers[0]);

    addTable(codes, 200, "OK", headers, headersCount);
    addTable(codes, 201, "Created", headers, headersCount);
    addTable(codes, 202, "Accepted", headers, headersCount);
    addTable(codes, 204, "No Content", headers, headersCount);
    addTable(codes, 400, "Bad Request", headers, headersCount);
    addTable(codes, 401, "Unauthorized", headers, headersCount);
    addTable(codes, 403, "Forbidden", headers, headersCount);
    addTable(codes, 404, "Not Found", headers, headersCount);
    addTable(codes, 405, "Method Not Allowed", headers, headersCount);
    addTable(codes, 414, "URI Too Long", headers, headersCount);
    addTable(codes, 500, "Internal Server Error", headers, headersCount);
    addTable(codes, 501, "Not Implemented", headers, headersCount);
    addTable(codes, 503, "Service Unavailable", headers, headersCount);

    return codes;
}

HttpCode *getTable(HTTPTable *codes, int code) {
    int nbTry = 0;
    while (codes->table[hash(code, nbTry)]->code != code) { nbTry++; }

    return codes->table[hash(code, nbTry)];
}


message *createMsgFromReponse(HttpCode rep, unsigned int clientId) {
    message *msg = malloc(sizeof(message));

    // Calcul de la taille nécessaire pour buf
    int bufSize = strlen("HTTP/1.0 ") + 3 + strlen(" ") + strlen(rep.info) + strlen("\r\n"); // 3 : taille d'une code (200, 404, ...)
    for (int i = 0; i < rep.headersCount; i++) {
        bufSize += strlen(rep.headers[i]) + strlen("\r\n");
    }
    bufSize += strlen("\r\n");

    msg->buf = malloc(bufSize + 1);
    sprintf(msg->buf, "HTTP/1.0 %d %s\n", rep.code, rep.info);
    int len = strlen(msg->buf);
    for (int i = 0; i < rep.headersCount; i++) {
        sprintf(msg->buf+len, "%s\n", rep.headers[i]);
        len += strlen(rep.headers[i]) + strlen("\n");
    }
    
    msg->len = bufSize;
    msg->clientId = clientId;

    return msg;
}


int getRepCode(message req) {
    void *tree = getRootTree();
    int len;

    _Token *methodNode = searchTree(tree, "method");
    char *methodL = getElementValue(methodNode->node, &len);
    char *method = malloc(len + 1);
    strncpy(method, methodL, len);
    method[len] = '\0';
    if (!(strcmp(method, "GET") == 0 || strcmp(method, "POST") == 0 || strcmp(method, "HEAD") == 0)) { return 405; }
    else if (len > LEN_METHOD) { return 501; }


    _Token *uriNode = searchTree(tree, "request_target");
    char *uriL = getElementValue(uriNode->node, &len);
    char *uri = malloc(len + 1);
    strncpy(uri, uriL, len);
    uri[len] = '\0';
    if (len > LEN_URI) { return 414; }
    if (strcmp(uri, "/") == 0) {
        uri = realloc(uri, strlen("/index.html") + 1);
        strcpy(uri, "/index.html");
    }
    for (int i = 0; i < len-1; i++) {
        if (uri[i] == '.' && uri[i+1] == '.') { return 403; } // tentative de remonter à la racine du serveur
    }
    char *path = malloc(strlen(SERVER_ROOT) + strlen(uri) + 1);
    strcpy(path, SERVER_ROOT);
    strcat(path, uri);
    FILE *file = fopen(path, "r");
    if (file == NULL) { return 404; }

    _Token *versionNode = searchTree(tree, "HTTP_version");
    char *versionL = getElementValue(versionNode->node, &len);
    char majeur = versionL[5];
    char mineur = versionL[7];
    if(!(majeur == '1' && (mineur == '1' || mineur == '0'))){return 505;}

    _Token *HostNode = searchTree(tree, "Host");
    if (HostNode->next != NULL) { return 400; } // plusieurs Host
    if (majeur == 1 && mineur == 1 && HostNode == NULL) { return 400; } // HTTP/1.1 sans Host
    char *hostL = getElementValue(HostNode->node, &len);
    char *host = malloc(len + 1);


    return 200;
}

message *generateReponse(message req, int opt_code) {
    HTTPTable *codes = loadTable();

    int code;
    if (opt_code == -1) { code = getRepCode(req); }
    else { code = opt_code; }

    HttpCode *rep = getTable(codes, code);
    message *msg = createMsgFromReponse(*rep, req.clientId);

    freeTable(codes);

    return msg;
}