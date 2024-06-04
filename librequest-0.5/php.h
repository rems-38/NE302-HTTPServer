#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "config.h"
#include <arpa/inet.h>
#include "fastcgi.h"

int createConnexion();
void send_begin_request(int sock, int requestId);
void send_params(int sock, int requestId, const char *name, const char *value);
void send_stdin(int sock, int requestId, const char *data) ;
void receive_response(int sock);