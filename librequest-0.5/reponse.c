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
    
    Header headers[] = {
        {"Content-Type", ""},
        {"Content-Length", ""},
        {"Connection", ""}
    };
    int headersCount = sizeof(headers) / sizeof(headers[0]);

    codes->headers = malloc(headersCount * sizeof(Header));
    for (int i = 0; i < headersCount; i++) {
        codes->headers[i].label = malloc(strlen(headers[i].label) + 1);
        strcpy(codes->headers[i].label, headers[i].label);
        codes->headers[i].value = malloc(strlen(headers[i].value) + 1);
        strcpy(codes->headers[i].value, headers[i].value);
    }
    codes->headersCount = headersCount;
}

void freeTable(HTTPTable *codes) {
    for (int i = 0; i < NB_HTTP_CODES; i++) {
        if (codes->table[i] != NULL) {
            free(codes->table[i]->info);
            free(codes->table[i]);
        }
    }
    free(codes->headers);
}

void addTable(HTTPTable *codes, int code, char *info) {
    HttpCode *el = malloc(sizeof(HttpCode));
    
    el->code = code;
    
    el->info = malloc(strlen(info) + 1);
    strcpy(el->info, info);

    int nbTry = 0;
    while (codes->table[hash(code, nbTry)] != NULL) { nbTry++; }
    codes->table[hash(code, nbTry)] = el;
}

HTTPTable *loadTable() {
    HTTPTable *codes = malloc(sizeof(HTTPTable));
    initTable(codes);

    addTable(codes, 200, "OK");
    addTable(codes, 201, "Created");
    addTable(codes, 202, "Accepted");
    addTable(codes, 204, "No Content");
    addTable(codes, 400, "Bad Request");
    addTable(codes, 401, "Unauthorized");
    addTable(codes, 403, "Forbidden");
    addTable(codes, 404, "Not Found");
    addTable(codes, 405, "Method Not Allowed");
    addTable(codes, 414, "URI Too Long");
    addTable(codes, 500, "Internal Server Error");
    addTable(codes, 501, "Not Implemented");
    addTable(codes, 503, "Service Unavailable");
    addTable(codes, 505, "HTTP Version Not Supported");

    return codes;
}

HttpReponse *getTable(HTTPTable *codes, int code) {
    HttpReponse *rep = malloc(sizeof(HttpReponse));

    int nbTry = 0;
    while (codes->table[hash(code, nbTry)]->code != code) { nbTry++; }

    rep->code = codes->table[hash(code, nbTry)];
    rep->httpminor = codes->httpminor;
    rep->headers = codes->headers;
    rep->headersCount = codes->headersCount;

    return rep;
}


message *createMsgFromReponse(HttpReponse rep, FILE *fout, unsigned int clientId) {
    message *msg = malloc(sizeof(message));

    // Taille du fichier si existe
    size_t fileSize = 0;
    if (fout != NULL) {
        fseek(fout, 0, SEEK_END);
        fileSize = ftell(fout);
        fseek(fout, 0, SEEK_SET);
    }
    
    // Calcul de la taille nécessaire pour buf
    int bufSize = strlen("HTTP/1.0 ") + 3 + strlen(" ") + strlen(rep.code->info) + strlen("\r\n"); // 3 : taille d'une code (200, 404, ...)
    for (int i = 0; i < rep.headersCount; i++) {
        if (!(strcmp(rep.headers[i].value, "") == 0)) {
            bufSize += strlen(rep.headers[i].label) + 2 + strlen(rep.headers[i].value) + strlen("\r\n"); // +2 pour le ": "
        }
    }   
    bufSize += fileSize;
    bufSize += 2 * strlen("\r\n");
    msg->buf = malloc(bufSize + 1);

    // Transfert de la data
    sprintf(msg->buf, "HTTP/1.%d %d %s\n", rep.httpminor, rep.code->code, rep.code->info);
    int len = strlen(msg->buf);
    for (int i = 0; i < rep.headersCount; i++) {
        if (!(strcmp(rep.headers[i].value, "") == 0)) {
            sprintf(msg->buf+len, "%s: %s\r\n", rep.headers[i].label, rep.headers[i].value);
            len += strlen(rep.headers[i].label) + 2 + strlen(rep.headers[i].value) + strlen("\r\n");
        }
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


int getRepCode(message req, HTTPTable *codes, FILE **fout) {
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

    /*
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
    else {
        for (int i = 0; i < codes->headersCount; i++) {
            if (strcmp(codes->headers[i].label, "Content-Type")) {
                ; // récupérer le type du fichier
                // si accepter (comment?)
            }
            else if (strcmp(codes->headers[i].label, "Content-Length")) {
            char *fileSizeS = (char *)fileSize; // trouver comment mettre le int en char*
                codes->headers[i].value = malloc(strlen(fileSizeS));
                strcpy(codes->headers[i].value, fileSizeS);
            }
        }
    }
    */

    _Token *versionNode = searchTree(tree, "HTTP_version");
    char *versionL = getElementValue(versionNode->node, &len);
    char majeur = versionL[5];
    char mineur = versionL[7];
    printf("%c, %c\n", majeur, mineur);

    codes->httpminor = mineur - '0';
    
    if(!(majeur == '1' && (mineur == '1' || mineur == '0'))){return 505;}

    // Host

    _Token *HostNode = searchTree(tree, "Host");
    if (majeur == '1' && mineur == '1' && HostNode == NULL) { return 400; } // HTTP/1.1 sans Host
    if ((HostNode != NULL) && (HostNode->next != NULL)) { return 400; } // plusieurs Host
    if (HostNode != NULL) {
        char *hostL = getElementValue(HostNode->node, &len);
        char *host = malloc(len + 1);
        strncpy(host, hostL, len);
        host[len] = '\0';
        if(len > LEN_HOST){return 400;}

        // déterminer la nature du host (nom de domaine ou ip)
        int i=0;
        int point = 0;
        int d_point = 0;
        while(host[i]!='\0'){
            if(host[i]=='.'){point++;}
            else if(host[i]==':'){d_point++;}
            i++;
        }

        if(point<2 | point>3){return 400;}

        if(point == 2 && d_point == 0){// nom de domaine sans port
            char txt1[63],txt2[63],txt3[63];

            printf("nom de domaine sans port\n");
            sscanf(host, "%[^.].%[^.].%s", txt1, txt2, txt3);
            

            i=0;
            while (txt1[i] != '\0'){
                if(!((txt1[i]>=65 && txt1[i]<=90) | (txt1[i]>=97 && txt1[i]<=122) | txt1[i] == '-')){return 400;}
                i++;
            }
            if (i>LEN_HOST_TXT){return 400;}

            i=0;
            while (txt2[i] != '\0'){
                if(!((txt2[i]>=65 && txt2[i]<=90) | (txt2[i]>=97 && txt2[i]<=122) | txt2[i] == '-')){return 400;}
                i++;
            }
            if (i>LEN_HOST_TXT){return 400;}

            i=0;
            while (txt3[i] != '\0'){
                if(!((txt3[i]>=65 && txt3[i]<=90) | (txt3[i]>=97 && txt3[i]<=122) | txt3[i] == '-')){return 400;}
                i++;
            }
            if (i>LEN_HOST_TXT){return 400;}

        }

        if(point == 2 && d_point == 1){// nom de domaine avec port
            char txt1[63],txt2[63],txt3[63];
            int port;

            printf("nom de domaine sans port\n");
            sscanf(host, "%[^.].%[^.].%[^:]:%d", txt1, txt2, txt3,&port);

            i=0;
            while (txt1[i] != '\0'){
                if(!((txt1[i]>=65 && txt1[i]<=90) | (txt1[i]>=97 && txt1[i]<=122) | txt1[i] == '-')){return 400;}
                i++;
            }
            if (i>LEN_HOST_TXT){return 400;}

            i=0;
            while (txt2[i] != '\0'){
                if(!((txt2[i]>=65 && txt2[i]<=90) | (txt2[i]>=97 && txt2[i]<=122) | txt2[i] == '-')){return 400;}
                i++;
            }
            if (i>LEN_HOST_TXT){return 400;}

            i=0;
            while (txt3[i] != '\0'){
                if(!((txt3[i]>=65 && txt3[i]<=90) | (txt3[i]>=97 && txt3[i]<=122) | txt3[i] == '-')){return 400;}
                i++;
            }
            if (i>LEN_HOST_TXT){return 400;}

            if (port!=8080){return 400;}

        }

        if (point == 3 && d_point == 0){// ip sans port
            int ip[4];
            printf("ip sans port\n");
            sscanf(host, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
            if((ip[0]>256|ip[0]<0) | (ip[1]>256|ip[1]<0) | (ip[2]>256|ip[2]<0) | (ip[3]>256|ip[3]<0)){return 400;}
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
    _Token *Message_Body = searchTree(tree,"message_body");

    // Content-Length

    if(T_EncodingNode != NULL && C_LengthNode != NULL){return 400;} // Ne pas mettre s’il y a déjà Transfer-Encoding : 400

    if(C_LengthNode != NULL && C_LengthNode->next != NULL){return 400;} // plusieurs content-length

    if (C_LengthNode != NULL && C_LengthNode->next == NULL){ // si un seul Content-Length avec valeur invalide : 400
        char *c_lengthL = getElementValue(C_LengthNode->node, &len);
        char *c_length = malloc(len + 1);
        strncpy(c_length, c_lengthL, len);
        c_length[len] = '\0';
        if(c_length[0]== '0'){return 400;} // on ne veut pas de 0XXXX

        for (int i=0; i<len ; i++){
            if(!(c_length[i] >= '0' && c_length[i] <= '9')){return 400;} // valeur invalide (négative ou avec des caractères autres que des chiffres)
        }

        int content_l_value = atoi(c_length);
        char *message_bodyL = getElementValue(Message_Body->node, &len);
        if(content_l_value != len){return 400;}// vérifier que c'est la taille du message body
    }

    // Transfer-Encoding

    if (majeur == '1' && mineur == '1' && T_EncodingNode != NULL){ // Ne pas traiter si HTTP 1.0
        char *transfer_encodingL = getElementValue(T_EncodingNode->node, &len);
        char *transfer_encoding = malloc(len + 1);
        strncpy(transfer_encoding, transfer_encodingL, len);
        transfer_encoding[len] = '\0';

        if(!(strcmp(transfer_encoding,"chunked")==0 | strcmp(transfer_encoding,"gzip")==0 | strcmp(transfer_encoding,"deflate")==0 | strcmp(transfer_encoding,"compress")==0 |strcmp(transfer_encoding,"identity")==0 )) {return 400;} // vérifier que la valeur du champ est bien prise en charge
        if(!(transfer_encodingL[len]=='\r' && transfer_encodingL[len+1]=='\n' && transfer_encodingL[len+2]=='\r' && transfer_encodingL[len+3]=='\n')){return 400;} // vérifier \r\n\r\n après la valeur du champ
    }

    // Message Body

    if (Message_Body != NULL && C_LengthNode == NULL){return 411;} // Si Message Body mais pas Content-Length : 411 Length Required


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
    if (opt_code == -1) { code = getRepCode(req, codes, &fout); }
    else { code = opt_code; }

    printf("%d\n", code);
    HttpReponse *rep = getTable(codes, code);
    message *msg = createMsgFromReponse(*rep, fout, req.clientId);

    freeTable(codes);

    return msg;
}