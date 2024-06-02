#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* DotRemovalSegment(char* uri){
    int n = strlen(uri);
    
    char* out = malloc(n+1);
    int i = 0, j = 0;
    while(i < n){
        /*printf("\nout :");
        for(int k = 0; k < j; k++){
            printf("%c",out[k]);
        }
        printf("\turi :");
        for(int k = i; k < n; k++){
            printf("%c",uri[k]);
        }*/
        if(uri[i] == '.' && uri[i+1] == '.' && uri[i+2] == '/'){
            i += 3; //enlever prefixe
        }
        else if(uri[i] == '.' && uri[i+1] == '/'){
            i += 2; //enlever prefixe
        }
        else if(uri[i] == '/' && uri[i+1] == '.' && uri[i+2] == '.' && uri[i+3] == '/'){
            i += 3;
            //retirer le dernier /x de out:
            while(out[j-1] != '/'){
                //out[j] = '-';
                j--;
            }
            j--;

        }
        else if(uri[i] == '/' && uri[i+1] == '.' && uri[i+2] == '.'){
            i += 2;
            //retirer le dernier /x de out:
            while(out[j-1] != '/'){
                //out[j] = '-';
                j--;
            }
            j--;

        }
        else if(uri[i] == '/' && uri[i+1] == '.' && uri[i+2] == '/'){
            i += 2; //remplacer par '/'
        }
        else if(uri[i] == '/' && uri[i+1] == '.'){
            i += 1;
            uri[i] = '/'; //rempalcer par '/'
        }
        else if(uri[i] == '.'){
            i += 1; //retirer
        }
        else if(uri[i] == '.' && uri[i+1] == '.'){
            i += 2; //retirer
        }
        else{
            //placer /x dans out
            int debut = 1;
            
            while((uri[i] != '/' || debut == 1) && i != n){
                debut = 0;
                out[j] = uri[i];
                i++;
                j++; 
            }
        }
    }
    out[j] = '\0';
    return out;
}



int main(){
    char* uri = "/a/b/c/./../../g";
    uri = DotRemovalSegment(uri);

    printf("\nsortie : %s\n",uri);

    return 0;
}


