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
            free(codes->table[i]->headers);
            free(codes->table[i]);
        }
    }
}

void addTable(HTTPTable *codes, int code, char *info, Header *headers, int headersCount) {
    HttpCode *el = malloc(sizeof(HttpCode));
    
    el->code = code;
    
    el->info = malloc(strlen(info) + 1);
    strcpy(el->info, info);

    el->headers = malloc(headersCount * sizeof(Header));
    for (int i = 0; i < headersCount; i++) {
        el->headers[i].label = malloc(strlen(headers[i].label) + 1);
        strcpy(el->headers[i].label, headers[i].label);
        el->headers[i].value = malloc(strlen(headers[i].value) + 1);
        strcpy(el->headers[i].value, headers[i].value);
        el->headers[i].len = headers[i].len;
    }
    el->headersCount = headersCount;

    int nbTry = 0;
    while (codes->table[hash(code, nbTry)] != NULL) { nbTry++; }
    codes->table[hash(code, nbTry)] = el;
}

HTTPTable *loadTable() {
    HTTPTable *codes = malloc(sizeof(HTTPTable));
    initTable(codes);
    
    Header headers[] = {
        {"Content-Type:", "text/html", 23},
        {"Content-Length:", "0", 18},
        {"Connection:", "keep-alive", 22}
    }; // 3e valeur correspond à taille du bloc
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
    addTable(codes, 505, "HTTP Version Not Supported", headers, headersCount);

    return codes;
}

HttpCode *getTable(HTTPTable *codes, int code) {
    int nbTry = 0;
    while (codes->table[hash(code, nbTry)]->code != code) { nbTry++; }

    return codes->table[hash(code, nbTry)];
}


message *createMsgFromReponse(HttpCode rep, FILE *fout, unsigned int clientId) {
    message *msg = malloc(sizeof(message));

    // Taille du fichier si existe
    size_t fileSize = 0;
    if (fout != NULL) {
        fseek(fout, 0, SEEK_END);
        fileSize = ftell(fout);
        fseek(fout, 0, SEEK_SET);
    }
    
    // Calcul de la taille nécessaire pour buf
    int bufSize = strlen("HTTP/1.0 ") + 3 + strlen(" ") + strlen(rep.info) + strlen("\r\n"); // 3 : taille d'une code (200, 404, ...)
    for (int i = 0; i < rep.headersCount; i++) {
        //if (fileSize != 0 && strcmp(rep.headers[i].label, "Content-Length:") == 0) {
            // snprintf(rep.headers[i].value, 21, "%zu", fileSize);
            // rep.headers[i].len = strlen(rep.headers[i].value) + strlen(rep.headers[i].label) + 1;
        //}
        bufSize += rep.headers[i].len + strlen("\r\n");
    }
    bufSize += fileSize;
    bufSize += 2 * strlen("\r\n");
    msg->buf = malloc(bufSize + 1);

    // Transfert de la data
    sprintf(msg->buf, "HTTP/1.0 %d %s\n", rep.code, rep.info);
    int len = strlen(msg->buf);
    for (int i = 0; i < rep.headersCount; i++) {
        sprintf(msg->buf+len, "%s %s", rep.headers[i].label, rep.headers[i].value);
        len += rep.headers[i].len + strlen("\r\n");
    }
    sprintf(msg->buf+len, "\r\n");
    len += strlen("\r\n");

    if (fout != NULL) {
        fread(msg->buf+len, fileSize, 1, fout);
        len += fileSize;
    }
    sprintf(msg->buf+len, "\r\n");
    
    msg->len = bufSize;
    msg->clientId = clientId;

    return msg;
}

int hexa(char c){
    if(c >= 48 && c <= 57){ //chiffre   
        return c-48;
    }
    else{ //lettre
        return c-55;
    }
}


char* percentEncoding(char* uri){
    int len = strlen(uri);
    int k = 0;
    int j = 0;
    char* dest = malloc(len + 1);

    while(k < len){
        if(uri[k] == '%'){
            int HEX1 = hexa(uri[k+1]);
            int HEX2 = hexa(uri[k+2]);
            dest[j] = 16*HEX1 + HEX2;
            k += 3;
        }

        else{
            dest[j] = uri[k];
            k++;
        }
        j++;
    }
    dest[j] = '\0';
    return dest;
}


int getRepCode(message req, FILE **fout) {
    void *tree = getRootTree();
    int len;

    _Token *methodNode = searchTree(tree, "method");
    char *methodL = getElementValue(methodNode->node, &len);
    char *method = malloc(len + 1);
    strncpy(method, methodL, len);
    method[len] = '\0';
    if (!(strcmp(method, "GET") == 0 || strcmp(method, "POST") == 0 || strcmp(method, "HEAD") == 0)) { return 405; }
    else if (len > LEN_METHOD) { return 501; }

    if ((strcmp(method, "GET") == 0 || strcmp(method, "HEAD") == 0) && (searchTree(tree,"message_body") != NULL)){ return 400; }

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
    //percent-Encoding
    uri = percentEncoding(uri);

    //Voir algo dans les rfc :
    for (int i = 0; i < len-1; i++) {
        if (uri[i] == '.' && uri[i+1] == '.') { return 403; } // tentative de remonter à la racine du serveur
    }
    char *path = malloc(strlen(SERVER_ROOT) + strlen(uri) + 1);
    strcpy(path, SERVER_ROOT);
    strcat(path, uri);
    free(uri);
    *fout = fopen(path, "r"); // pas sur que ça marche, fout est peut etre pas gardé
    free(path);
    if (fout == NULL) { return 404; }

    _Token *versionNode = searchTree(tree, "HTTP_version");
    char *versionL = getElementValue(versionNode->node, &len);
    char majeur = versionL[5];
    char mineur = versionL[7];
    printf("%c, %c\n", majeur, mineur);
    if(!(majeur == '1' && (mineur == '1' || mineur == '0'))){return 505;}

    _Token *HostNode = searchTree(tree, "Host");
    if (majeur == 1 && mineur == 1 && HostNode == NULL) { return 400; } // HTTP/1.1 sans Host
    if ((HostNode != NULL) && (HostNode->next != NULL)) { return 400; } // plusieurs Host
    if (HostNode != NULL) {
        char *hostL = getElementValue(HostNode->node, &len);
        char *host = malloc(len + 1);
        strncpy(host, hostL, len);
        host[len] = '\0';

        if(len > LEN_HOST){return 400;}

        // déterminer la nature du host (dns ou ip)
        int i=0;
        int point = 0;
        int d_point = 0;
        while(host[i]!='\0'){
            if(host[i]=='.'){point++;}
            else if(host[i]==':'){d_point++;}
            i++;
        }

        if(point<2 | point>3){return 400;}

        if(point == 2){// nom de domaine
            char *dns[3];
            printf("nom de domaine\n");
            sscanf(host, "%s.%s.%s", dns[0], dns[1], dns[2]);
            if (strcmp(dns[0],"www")!=0){return 400;}
            if (strcmp(dns[2],"com")!=0 && strcmp(dns[2],"net")!=0 && strcmp(dns[2],"fr")!=0){return 400;}

            int j=0;
            while (dns[1][j] != '\0'){
                if(!((dns[1][j]>=65 && dns[1][j]<=90) | (dns[1][j]>=97 && dns[1][j]<=122) | dns[1][j] == '-')){return 400;}
            }
            if (j>63){return 400;}
        }

        if (point == 3 && d_point == 0){// ip sans port
            int ip[4];
            printf("ip sans port\n");
            sscanf(host, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
            if((ip[0]>256|ip[0]<0) | (ip[1]>256|ip[1]<0) | (ip[2]>256|ip[2]<0) | (ip[3]>256|ip[3]<0)){return 400;} //ip invalide
        }

        if (point == 3 && d_point == 1){// ip avec port
            int ip_port[5];
            printf("ip avec port\n");
            sscanf(host, "%d.%d.%d.%d:%d", &ip_port[0], &ip_port[1], &ip_port[2], &ip_port[3], &ip_port[4]);
            if((ip_port[0]>256|ip_port[0]<0) | (ip_port[1]>256|ip_port[1]<0) | (ip_port[2]>256|ip_port[2]<0) | (ip_port[3]>256|ip_port[3]<0) | ip_port[4] != 8080){return 400;} //ip invalide
        }
        
        
        free(host);
    }

    _Token *C_LengthNode = searchTree(tree, "Content_Length_header");
    _Token *T_EncodingNode = searchTree(tree, "Transfer_Encoding_header");
    if(T_EncodingNode != NULL && C_LengthNode != NULL){return 400;}

/*
    _Token *ConnectionNode = searchTree(tree, "Connection");
    char *ConnectionL = getElementValue(ConnectionNode->node, &len);
    if(strcmp(ConnectionL,"close") == 0){
        //renvoyer close
    }
    else if(majeur == 1 && mineur == 1){
        //garder connection ouverte
    }
    else if (majeur == 1 && mineur == 0 && (strcmp(ConnectionL,"keep-alive") == 0 || strcmp(ConnectionL,"Keep-Alive") == 0)){
        //garder la connection ouverte
    }
    else {
        //fermeture de la connection
    }
*/
    return 200;
}

message *generateReponse(message req, int opt_code) {
    HTTPTable *codes = loadTable();

    int code;
    FILE *fout = NULL;
    if (opt_code == -1) { code = getRepCode(req, &fout); }
    else { code = opt_code; }

    printf("%d\n", code);
    HttpCode *rep = getTable(codes, code);
    message *msg = createMsgFromReponse(*rep, fout, req.clientId);

    freeTable(codes);

    return msg;
}