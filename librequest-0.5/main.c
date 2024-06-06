#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "reponse.h"
#include "../api/api.h"
#include "config.h"

int main(void)
{
	message *requete;
	
	while (1) {
		// on attend la reception d'une requete HTTP requete pointera vers une ressource allouée par librequest. 
		if ((requete=getRequest(8080)) == NULL ) return -1; 

		// Affichage de debug 
		printf("#########################################\nDemande recue depuis le client %d\n",requete->clientId); 
		printf("Client [%d] [%s:%d]\n",requete->clientId,inet_ntoa(requete->clientAddress->sin_addr),htons(requete->clientAddress->sin_port));
		printf("%.*s\n\n",requete->len,requete->buf);  

		message *msg;
		if (parseur(requete->buf, requete->len) != 1) { printf("Parser failed\n"); msg = generateReponse(*requete, 400); }
		else { msg = generateReponse(*requete, -1); }
		printf("#########################################\nReponse envoyee au client %d\n",requete->clientId);
		printf("Client [%d] [%s:%d]\n",requete->clientId,inet_ntoa(requete->clientAddress->sin_addr),htons(requete->clientAddress->sin_port));
		printf("%.*s\n\n",msg->len,msg->buf);
		sendReponse(msg); 
		controlConnection(msg);
		//requestShutdownSocket(requete->clientId); 

		// on ne se sert plus de requete a partir de maintenant, on peut donc liberer... 
		freeRequest(requete); 
	}
	return (1);
}
