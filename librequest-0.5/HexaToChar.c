#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *HexaToChar(char *content){
    int content_length = strlen(content);
    char *result = malloc(content_length/2);
    char *hexa = malloc(3);

    for(int i=0; i<content_length/2 ; i++){
        hexa[0] = content[2*i];
        hexa[1] = content[2*i+1];
        hexa[2] = '\0';
        char valeur = strtol(hexa,NULL,16);
        result[i] = valeur;
    }
    return result;
}

int main(){

    char *test = "436f6e74656e742d747970653a20746578742f68746d6c3b20636861727365743d5554462d380d0a0d0a";

    // text = Content-type: text/html; charset=UTF-8
    
    printf("conversion : %s", HexaToChar(test));

    return 0;
}
