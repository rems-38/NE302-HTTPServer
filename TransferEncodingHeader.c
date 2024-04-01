#include "arbre.h"

#define AMAJ 65
#define FMAJ 70
#define ZMAJ 90
#define AMIN 97
#define ZMIN 122
#define ZERO 48
#define NINE 57
#define SP 32           // espace
#define HTAB 9          // \t
#define DASH 45         // -
#define UNDERSCORE 95   // _
#define COMMA 44        // ,
#define DOT 46          // .
#define EXCLAMATION 33  // !
#define QUESTION 63     // ?
#define EQUAL 61        // =
#define COLON 58        // :
#define SEMICOLON 59    // ;
#define HASHTAG 35      // #
#define DOLLAR 36       // $
#define POURCENT 37     // %
#define ESP 38          // &
#define SQUOTE 39       // '
#define DQUOTE 34       // "
#define FQUOTE 96       // `
#define STAR 42         // *
#define PLUS 43         // +
#define SLASH 47        // /
#define CIRCONFLEXE 94  // ^
#define BARRE 124       // |
#define VAGUE 126       // ~
#define LF 10           // \n
#define CR 13

bool isOWS(char *text, size_t *curr, Element *data, bool is_fils) { //import
    Element *el = addEl("OWS", text, strlen(text));
    if (is_fils) {
        data->fils = el;
        data = data->fils;
    } else {
        data->frere = el;
        data = data->frere;
    }

    size_t count = 0;
    Element *sub;
    while (text[count] == SP || text[count] == HTAB) {
        if (text[count] == SP) { sub = addEl("__sp", text+count, 1); }
        else if (text[count] == HTAB) { sub = addEl("__htab", text+count, 1); }

        if (count == 0) { // ou el->fils == NULL mais vu qu'on change el faudrait le save...
            data->fils = sub;
            data = data->fils;
        }
        else {
            data->frere = sub;
            data = data->frere;
        }
        count++;
    }
    
    updateLength(data, count);
    *curr += count;
    return true;
}





bool isTransferCoding(char *text, size_t *curr, Element *data, bool is_fils){
    Element *el = addEl("transfert-coding",text, strlen(text));
    if (is_fils) {
        data->fils = el;
        data = data->fils; //transfert-coding devient la tete
    } else {
        data->frere = el;
        data = data->frere; //idem
    }

    if(!strncmp(text,"chuncked",8)) {
        Element *sub = addEl("case_insensitive_string","chuncked",8);
        data->fils = sub;
        *curr+=8;
    }
    else if(!strncmp(text,"compress",8)) {
        Element *sub = addEl("case_insensitive_string","compress",8);
        data->fils = sub;
        *curr+=8;
    }
    else if(!strncmp(text,"deflate",7)) {
        Element *sub = addEl("case_insensitive_string","deflate",7);
        data->fils = sub;
        *curr+=7;
    }
    else if(!strncmp(text,"gzip",4)) {
        Element *sub = addEl("case_insensitive_string","gzip",4);
        data->fils = sub;
        *curr+=4;
    }
    else {return false;}

    return true;
}

int OWS(char *text){
    size_t i = 0;
    while(text[i] == SP || text[i] == HTAB){
        i++;
    }

    if(text[i] == COMMA) {
        return 1; //cas OWS","
    }
    else if(text[i] == 'c' && text[i+1] == 'l'){ //cas OWS CRLF : on sort de Transfert-Encoding
        return 3;
    }
    else if(!strncmp(text+i,"chuncked",8) || !strncmp(text+i,"compress",8) || !strncmp(text+i,"deflate",7) || !strncmp(text+i,"gzip",4)){
        //cas OWS transfert-coding
        printf("dans OWS text+i : -%s-\n",text+i);
        return 2;
    }
    else { return 4; } //cas erreur
}


bool isTransferEncoding(char *text, size_t *curr, Element *data){
    size_t count = 0;

    Element *el = addEl("Transfer-Encoding",text,strlen(text));
    data->frere = el;
    data = data->frere; //transfer-encoding devient la tete

    bool fst = false; //pour savoir si protocol va etre fils direct de Transfert-Encoding
    if (*(text+count) == COMMA) {
        while (*(text+count) == COMMA) {
            Element *el = addEl("__comma", text+count, 1);
            data->fils = el;
            data = data->fils;
            count += 1;
            if (!isOWS(text+count, &count, data, false)) { return false; }
            data = data->frere;
            fst = true;
        }
    }

    if (!isTransferCoding(text+count, &count, data, !fst)){return false;}
    data = data->frere ; //transfert-coding devient la tete

    int boucle = OWS(text+count);
    printf("boucle : %d\n",boucle);
    if(boucle == 2 || boucle == 4){ //si on n'a pas OWS "," ou OWS CRLF
        return false;
    }
    printf("ici\n");
    while(boucle == 1){ //on entre si : OWS","
        isOWS(text+count, &count, data, false); //ajout de OWS (on sait qu'il est la grace a OWS())
        data = data->frere; //OWS devient la tete
        printf("Ajout de OWS\n");
        
        Element *el = addEl("__comma", text+count, 1);  //ajout de ","
        data->frere = el;
        data = data->frere;
        printf("Ajout de ','\n");
        count += 1;
         
        boucle = OWS(text+count); //si 1 on reboucle, si 3 on sort de la boucle
        printf("recalcul de boucle : %d\n",boucle);
        if(boucle == 2){ //OWS transfert-coding
            isOWS(text+count, &count, data, false); //ajout de OWS (on sait qu'il est la grace a OWS())
            data = data->frere; //OWS devient la tete

            isTransferCoding(text+count, &count, data,false); //ajout de transfert-coding
            data = data->frere; //trasfert-coding devient la tete

            boucle = OWS(text+count); //si 1 on reboucle, si 3 on sort sinon :
            printf("rerecalcul de boucle qui commence à -%c-: %d\n",*(text+count),boucle);
            if(boucle == 4 || boucle == 2){ //OWS transfert-conding ne peut etre suivi que de OWS',' ou de OWS CRLF
                return false;
            }
        }
        else if(boucle == 4){ //cas d'erreur
            return false;
        }
    }
    *curr += count;
    return true;    
}



bool isTransferEncodingHeader(char *text, Element *data){

    if (!strcmp(text, "Transfer-Encoding")) {return false;}  //ok 

    Element *el = addEl("Transfer-Encoding-header", text, 25); //nombre a changer
    data->fils = el;
    data = data->fils; //la tete devient el

    Element *el2 = addEl("case_insensitive_string","Transfer-Encoding",17);
    data->fils = el2;
    data = data->fils; //la tete devient el2
    
    size_t count = 17;
    //printf("text : %c\n",*(text+count)); 

    if (*(text+count) == COLON){
        Element *el = addEl("__colon", text+count, 1);
        data->frere = el;
        data = data->frere; //la tete devient ":"
        count += 1;
    } 
    else {return false;}

    if(!isOWS(text+count, &count, data, false)) {return false;} 
    data = data->frere;   //la tete devient OWS
    //tab semble etre compté comme 2 sp, ou les tabulation son mal faites
    if(!isTransferEncoding(text+count, &count, data)) {return false;}
    data = data->frere;

    if(!isOWS(text+count, &count, data, false)) {return false;} 

    
    return true;
}



bool verifHeaderField(Element *data){
    bool res = false;
    Element *el = malloc(sizeof(Element));
    el->key = "header_field";
    el->word = data->word; //le mot ne s'affiche pas avec le printarbre
    data->fils = el;
    data = data->fils;

    if (isTransferEncodingHeader(data->word, data)) {
        res = true;
    }


    return res;
}



int main(void) {
    
    FILE *ftest = fopen("testTEH.txt", "r");
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    Element *message = malloc(sizeof(Element));
    message->key = "HTTP_message";

    if (ftest == NULL) {
        printf("Impossible d'ouvrir le fichier %s\n", "testTEH.txt");
        return -1;
    }

    if ((read = getline(&line, &len, ftest)) != -1) {
        message->word = line;
        message->length = read;
        int output = verifHeaderField(message);
        printf("%d\n", output);

        printArbre(message, 0);
    }

    fclose(ftest);
    if (line) free(line);
    return 0;
}