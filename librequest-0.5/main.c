#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "reponse.h"
#include "../api/api.h"
#include "config.h"


int main(int argc, char *argv[])
{
	message *requete;
	
	while (1) {
		// on attend la reception d'une requete HTTP requete pointera vers une ressource allouée par librequest. 
		if ((requete=getRequest(8080)) == NULL ) return -1; 

		// Affichage de debug 
		printf("#########################################\nDemande recue depuis le client %d\n",requete->clientId); 
		printf("Client [%d] [%s:%d]\n",requete->clientId,inet_ntoa(requete->clientAddress->sin_addr),htons(requete->clientAddress->sin_port));
		printf("Contenu de la demande %.*s\n\n",requete->len,requete->buf);  

		if (parseur(requete->buf, requete->len) != 1) {
			char *headers[] = {"Content-Type: text/html", "Content-Length: 0", "Connection: keep-alive"};
			reponse repBadRequest = {.code = 400, .info = "Bad Request", .headers = headers, .headersCount = sizeof(headers) / sizeof(headers[0])};

			message *msg = createMsgFromReponse(repBadRequest, requete->clientId);
			printf("Reponse: %s\n", msg->buf);
			sendReponse(msg); 
		} else {
			printf("HTTP Code : %d\n", getRepCode(*requete));
			// message *msg = createMsgFromReponse(generateReponse(*requete), requete->clientId);
			// printf("Reponse: %s\n", msg->buf);
			// sendReponse(msg); 
		}

		requestShutdownSocket(requete->clientId); 

	// on ne se sert plus de requete a partir de maintenant, on peut donc liberer... 
	freeRequest(requete); 
	}
	return (1);
}
