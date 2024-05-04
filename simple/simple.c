#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../librequest-0.5/api/request.h"   
#include "../api/api.h"   

#define REPONSEGOOD "HTTP/1.1 200 OK\r\nContent-Length: 0\r\nConnection: KeepAlive\r\n\r\n"
#define REPONSEBAD "HTTP/1.0 400 Bad Request\r\n\r\n"

// ceci decrit la methode experimentale de renvoi de la reponse Cf API


typedef struct Element {
    char *key;
    char *word;
    size_t length;
    struct Element *fils;
    struct Element *frere;
} Element;



int main(int argc, char *argv[])
{
	message *requete; 

	while ( 1 ) {
		// on attend la reception d'une requete HTTP requete pointera vers une ressource allouée par librequest. 
		if ((requete=getRequest(8080)) == NULL ) return -1; 

		// Affichage de debug 
		printf("#########################################\nDemande recue depuis le client %d\n",requete->clientId); 
		printf("Client [%d] [%s:%d]\n",requete->clientId,inet_ntoa(requete->clientAddress->sin_addr),htons(requete->clientAddress->sin_port));
		printf("Contenu de la demande ''%.*s''\n\n",requete->len,requete->buf);  
			
		if ( parseur(requete->buf,requete->len) !=1 ) { 
			
			writeDirectClient(requete->clientId,REPONSEBAD,strlen(REPONSEBAD)); 
		} 
		else {
			_Token* tok = searchTree(NULL,"request_target");
			int len;
			char* request = getElementTag(tok->node,&len);
			printf("taille1 : %d\n",len);
			printf("RT : '%s'\n",request);
			
			char* r1 = getElementValue(tok->node,&len);
			printf("RT : '%s'\n",r1);
			printf("taille2 : %d\n",len);
			char* r2 = malloc(len);
			strncpy(r2,r1,len); //gdb -> bonne valeur dans r2
			//printf("RT : '%s'\n",r2); //mais printf ne fonctionne pas

			

			//Element* el = getRootTree();
			//printf("fils word : %s\n",el->fils->word);
			writeDirectClient(requete->clientId,REPONSEGOOD,strlen(REPONSEGOOD)); 
		}
		endWriteDirectClient(requete->clientId); 
		//requestShutdownSocket(requete->clientId); 
	// on ne se sert plus de requete a partir de maintenant, on peut donc liberer... 
	freeRequest(requete); 
	}
	return (1);
}

