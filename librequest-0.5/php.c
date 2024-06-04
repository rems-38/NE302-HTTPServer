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

void send_begin_request(int sock, int requestId) {
    FCGI_Header header;
    FCGI_BeginRequestBody body;

    header.version = FCGI_VERSION_1;
    header.type = FCGI_BEGIN_REQUEST;
    header.requestId = htons(requestId);
    header.contentLength = htons(sizeof(body));
    header.paddingLength = 0;
    header.reserved = 0;

    body.role = htons(FCGI_RESPONDER);
    body.flags = 0;
    memset(body.unused, 0, sizeof(body.unused));

    write(sock, &header, sizeof(header));
    write(sock, &body, sizeof(body));
}

void send_params(int sock, int requestId, const char *name, const char *value) {
    FCGI_Header header;
    int name_len = strlen(name);
    int value_len = strlen(value);
    int content_len = name_len + value_len + 2;

    header.version = FCGI_VERSION_1;
    header.type = FCGI_PARAMS;
    header.requestId = htons(requestId);
    header.contentLength = htons(content_len);
    header.paddingLength = 0;
    header.reserved = 0;

    unsigned char *content = malloc(content_len);
    content[0] = name_len;
    content[1] = value_len;
    memcpy(content + 2, name, name_len);
    memcpy(content + 2 + name_len, value, value_len);

    write(sock, &header, sizeof(header));
    write(sock, content, content_len);

    free(content);
}

void send_stdin(int sock, int requestId, const char *data) {
    FCGI_Header header;
    int data_len = strlen(data);

    header.version = FCGI_VERSION_1;
    header.type = FCGI_STDIN;
    header.requestId = htons(requestId);
    header.contentLength = htons(data_len);
    header.paddingLength = 0;
    header.reserved = 0;

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
