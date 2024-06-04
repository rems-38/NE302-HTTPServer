#include "php.h"

int createConnexion() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); return -1; }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    server_addr.sin_port = htons(SERVER_PORT_PHP);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sock);
        exit(1);
    }

    return sock;
}

void send_begin_request(int sock, unsigned short requestId) {
    FCGI_Header header;
    FCGI_BeginRequestBody body;

    header.version = FCGI_VERSION_1;
    header.type = FCGI_BEGIN_REQUEST;
    header.requestId = htons(requestId);
    header.contentLength = htons(sizeof(FCGI_BeginRequestBody));
    header.paddingLength = 0;
    header.reserved = 0;

    body.role = htons(FCGI_RESPONDER);
    body.flags = 0;
    memset(body.unused, 0, sizeof(body.unused));

    printf("-> Sending begin request\n");

    write(sock, &header, sizeof(header));
    write(sock, &body, sizeof(body));
}

char *generateFileName(const char *filename) {
    
    char *addr = "127.0.0.1";
    char *port = "9000";
    // char *final = malloc(64);
    // char *res = realpath(filename, final);
    char *final = "/home/userir/NE302-HTTPServer/html/www/test.php";
    char *res = "pas null";

    if (res != NULL) {
        char *script_filename = malloc(strlen("proxy:fcgi://") + strlen(addr) + strlen(port) + strlen(final) + 3);
        sprintf(script_filename, "proxy:fcgi://%s:%s/%s", addr, port, final);
        printf("script_filename: %s\n", script_filename);
        //free(final);
        
        return script_filename;
    } else { free(final); return " "; }
}

void encode_name_value_pair(const char *name, const char *value, unsigned char *buffer, int *len) {
    int name_len = strlen(name);
    int value_len = strlen(value);
    unsigned char *start = buffer;

    if (name_len < 128) {
        *buffer++ = (unsigned char)name_len;
    } else {
        *buffer++ = (unsigned char)((name_len >> 24) | 0x80);
        *buffer++ = (unsigned char)(name_len >> 16);
        *buffer++ = (unsigned char)(name_len >> 8);
        *buffer++ = (unsigned char)name_len;
    }

    if (value_len < 128) {
        *buffer++ = (unsigned char)value_len;
    } else {
        *buffer++ = (unsigned char)((value_len >> 24) | 0x80);
        *buffer++ = (unsigned char)(value_len >> 16);
        *buffer++ = (unsigned char)(value_len >> 8);
        *buffer++ = (unsigned char)value_len;
    }

    memcpy(buffer, name, name_len);
    buffer += name_len;
    memcpy(buffer, value, value_len);
    buffer += value_len;

    *len = buffer - start;
}

void send_params(int sock, unsigned short requestId, const char *name, const char *value) {
    FCGI_Header header;
    unsigned char buffer[FASTCGILENGTH];
    int content_len;

    encode_name_value_pair(name, value, buffer, &content_len);

    header.version = FCGI_VERSION_1;
    header.type = FCGI_PARAMS;
    header.requestId = htons(requestId);
    header.contentLength = htons(content_len);
    header.paddingLength = 0;
    header.reserved = 0;

    write(sock, &header, sizeof(header));
    write(sock, buffer, content_len);
}

void send_empty_params(int sock, unsigned short requestId) {
    FCGI_Header header;

    header.version = FCGI_VERSION_1;
    header.type = FCGI_PARAMS;
    header.requestId = htons(requestId);
    header.contentLength = 0;
    header.paddingLength = 0;
    header.reserved = 0;

    printf("-> Sending empty params\n");

    write(sock, &header, sizeof(header));
}

void send_stdin(int sock, unsigned short requestId, const char *data) {
    FCGI_Header header;
    int data_len = strlen(data);

    header.version = FCGI_VERSION_1;
    header.type = FCGI_STDIN;
    header.requestId = htons(requestId);
    header.contentLength = data_len;
    header.paddingLength = 0;
    header.reserved = 0;

    printf("-> Sending stdin\n");

    write(sock, &header, sizeof(header));
    write(sock, data, data_len);
}

FCGI_Header *receive_response(int sock) {
    FCGI_Header *header = malloc(sizeof(FCGI_Header) * 32);
    int i = 0;
    while (read(sock, &header[i], sizeof(header[i])) > 0) {
        header[i].requestId = ntohs(header[i].requestId);
        header[i].contentLength = ntohs(header[i].contentLength);

        char *content = malloc(header[i].contentLength + 1);
        read(sock, content, header[i].contentLength);
        content[header[i].contentLength] = '\0';

        if (header[i].type == FCGI_STDOUT) {
            printf("Output: %s\n", content);
        } else if (header[i].type == FCGI_STDERR) {
            fprintf(stderr, "Error: %s\n", content);
        }

        free(content);

        if (header[i].paddingLength > 0) {
            char padding[header[i].paddingLength];
            read(sock, padding, header[i].paddingLength);
        }

        if (header[i].type == FCGI_END_REQUEST) {
            break;
        }

        i++; if (i >= 32) { break; }
    }

    return header;
}

