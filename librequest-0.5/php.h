#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "config.h"
#include <arpa/inet.h>
#include "fastcgi.h"

typedef struct {
    unsigned char nameLengthB0;  /* nameLengthB0  >> 7 == 0 */
    unsigned char valueLengthB0; /* valueLengthB0 >> 7 == 0 */
    unsigned char *nameData;
    unsigned char *valueData;
} FCGI_NameValuePair11;

int createConnexion();
void send_begin_request(int sock, unsigned short requestId);
char *generateFileName(const char *filename);
void encode_name_value_pair(FCGI_NameValuePair11 pair, unsigned char *buffer, int *len);
char *getScriptName(const char *filename);
char *getScriptFilename(const char *filename);
void send_params(int sock, unsigned short requestId, FCGI_NameValuePair11 *params);
void send_empty_params(int sock, unsigned short requestId);
void send_stdin(int sock, unsigned short requestId, const char *data) ;
char *receive_response(int sock);
