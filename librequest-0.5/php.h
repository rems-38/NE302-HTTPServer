#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "config.h"
#include <arpa/inet.h>
#include "fastcgi.h"

int createConnexion();
void send_begin_request(int sock, unsigned short requestId);
char *generateFileName(const char *addr, const char *port, const char *filename);
void encode_name_value_pair(const char *name, const char *value, unsigned char *buffer, int *len);
void send_params(int sock, unsigned short requestId, const char *name, const char *value);
void send_empty_params(int sock, unsigned short requestId);
void send_stdin(int sock, unsigned short requestId, const char *data) ;
FCGI_Header *receive_response(int sock);
