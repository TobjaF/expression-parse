/*
2020-09-12 TOBJA FRANZ FÜR MODUL 403
2021-11-06 UPDATE FÜR MODUL 411
PARSER FÜR MATHEMATISCHE AUSDRÜCKE
ES KÖNNEN AUSDRÜCKE DER FOLGENDEN EBNF BERECHNET WERDEN
--------- EBNF --------------
ziffer    = 0|1|2|3|4|5|6|7|8|9
zahl      = ziffer {ziffer}
faktor    = zahl | '(' summe ')'
produkt   = faktor {*faktor}
summe     = produkt {+produkt}
ausdruck  = summe
-----------------------------
BEISPIEL: "34*(2+1)" IST EIN AUSDRUCK:

                                |Einstieg in Regel "ausdruck"
                                |ausdruck
                                |34*(2+1)
                                |
                                |summe
                               34*(2+1)
                                |
                                |produkt
                              34*(2+1)
                             /        \
                            |faktor    |faktor
                            34        (2+1)
                            |           |
                            |zahl       |( summe )
                           34         (2+1)
                        /     \         |
                    ziffer   ziffer     |summe
                       3       4        2+1
                                       /       \
                                      |produkt  |produkt
                                      2         1
                                      |faktor   |faktor
                                      2         1
                                      |         |
                                      |zahl     |zahl
                                      2         1
                                      |         |
                                    ziffer    ziffer
                                      2         1

*/

#include <stdio.h>
#include <stdbool.h>


enum token_type {
    zahl,
    klammer_auf,
    klammer_zu,
    plus,
    minus,
    mal,
    durch,
    ende
};

struct token {
    enum token_type type;
    int value;
};

//Funktionen deklarieren
bool ist_zahl(char);
bool ist_leerschlag(char);
struct token token_vorausschauen();
struct token konsumiere_token();
bool konsumiere_token_falls(enum token_type);
bool konsumiere_token_falls_eines(enum token_type, enum token_type);
int berechne_faktor(int*);
int berechne_zahl(int*);
int berechne_produkt(int*);
int berechne_summe(int*);
int berechne_ausdruck(int*);
void tokenize_stdin();
void print_error_message(int,int);
void clear_tokens();
void print_tokens();

//globale Variablen initialisieren
int tokens_size=0;
int tokens_position=0;
struct token tokens[256];

void push_token(enum token_type type, int value){
    struct token new_token;
    new_token.type = type;
    new_token.value = value;
    tokens[tokens_size++]=new_token;
};

void tokenize_stdin () {
    int c = fgetc(stdin);
    while (c!=EOF && c !='\n') {
        switch(c) {
            case ' ': {c = fgetc(stdin); break;} //Leerschlag überspringen
            case '\t': {c = fgetc(stdin); break;} // Tab überspringen
            case '+': {push_token(plus, c);  c = fgetc(stdin); break;}
            case '-': {push_token(minus, c); c = fgetc(stdin); break;}
            case '/': {push_token(durch, c); c = fgetc(stdin); break;}
            case '*': {push_token(mal, c); c = fgetc(stdin); break;}
            case '(': {push_token(klammer_auf, c); c = fgetc(stdin); break;}
            case ')': {push_token(klammer_zu, c); c = fgetc(stdin); break;}
            case '0'...'9': {
                //Zahlwert ermitteln
                int zahl_position=0;
                int zahl_zeichen[10];
                int zahl_wert=0;
                int positionswert=1;
                while (ist_zahl(c)){
                    zahl_zeichen[zahl_position++] = c - '0';
                    c = fgetc(stdin);
                }
                for (int i=zahl_position-1; i>=0; i--){
                    zahl_wert += positionswert*zahl_zeichen[i];
                    positionswert *= 10;
                }
                push_token(zahl, zahl_wert);
                break;
            }
            default: {c = fgetc(stdin); break;}; // Fehler ausgeben
        }
    }
    push_token(ende, 0);
}

void print_tokens(){
    printf("tokenized: [ ");
    for (int i = 0; i<tokens_size-1; i++){
        if (tokens[i].type == zahl) printf("%d", tokens[i].value);
        else printf("%c", tokens[i].value);
    }
    printf(" ]\n\n");
}

void clear_tokens () {
    tokens_position = 0;
    tokens_size = 0;
    for (int i=0; i<256; i++){
        tokens[i].type = zahl;
        tokens[i].value = 0;
    }
}

bool ist_zahl(char x){
    return x=='0' || x=='1' || x=='2' || x=='3' || x=='4' || x=='5' || x=='6' || x=='7' || x=='8' || x=='9';
}

bool ist_leerschlag(char x){
    return x==' ' || x=='\t';
}

struct token token_vorausschauen(){
    return tokens[tokens_position];
};

struct token konsumiere_token () {
    return tokens[tokens_position++];
}

bool konsumiere_token_falls (enum token_type type){
    if (tokens[tokens_position].type == type) {
        konsumiere_token();
        return true;
    }
    return false;
}


bool konsumiere_token_falls_eines (enum token_type type1, enum token_type type2){
    if (tokens[tokens_position].type == type1 || tokens[tokens_position].type == type2) {
        konsumiere_token();
        return true;
    }
    return false;
}
int berechne_faktor(int* faktor){
    struct token naechstes = konsumiere_token();
    if (naechstes.type == zahl) {
        *faktor = naechstes.value;
        return 0;
    }
    else if (naechstes.type == klammer_auf){
        int errorcode = berechne_summe(faktor);
        if (errorcode < 0) return errorcode;
        if (!konsumiere_token_falls(klammer_zu)) return -1;
        return 0;
    }
    else if (naechstes.type == ende) return -2; //Fehler "Hier muss ein Zeichen stehen"
    else return -3; //Fehler "Zeichen nicht erlaubt"
}

int berechne_produkt(int* produkt){
    struct token naechstes;
    naechstes.type = mal;
    *produkt = 1;
    do {
        int faktor;
        int errorcode = berechne_faktor(&faktor);
        if (errorcode < 0 ) return errorcode;
        if (naechstes.type == mal )*produkt *= faktor;
        else *produkt /= faktor;
        naechstes = token_vorausschauen();
    } while (konsumiere_token_falls_eines(mal,durch));
    return 0;
}

int berechne_summe(int* summe){
    struct token naechstes;
    naechstes.type = plus;
    *summe = 0;
    do {
        int produkt;
        int errorcode = berechne_produkt(&produkt);
        if (errorcode < 0) return errorcode;
        if (naechstes.type == plus) *summe += produkt;
        else *summe -= produkt;
        naechstes = token_vorausschauen();
    } while (konsumiere_token_falls_eines(plus,minus));
    return 0;
}

int berechne_ausdruck(int* resultat){
    int errorcode = berechne_summe(resultat);
    if (errorcode < 0) return errorcode;
    if (!konsumiere_token_falls(ende)) return -3; //Fehler "Zeichen nicht erlaubt"
    return 0;
}

void print_error_message (int errorcode, int position) {
    for (int i= 0; i<position+12; i++) printf(" "); //Die Position des Fehlers mit einem '^' markieren
    printf("^\n");
    switch(errorcode){
        case -1: printf("Fehler an Position %d: Abschliessende Klammer ')' erwartet\n\n", position); break;
        case -2: printf("Fehler an Position %d: Hier muss ein Zeichen stehen \n\n", position); break;
        case -3: printf("Fehler an Position %d: Zeichen hier nicht erlaubt\n\n", position); break;
        default: printf("Fehler an Position %d: \n\n", position); break;
    }
}

int main(int argc, char *argv[])
{
    while(true){
        printf("Mathematischen Ausdruck eingeben:\n");
        tokenize_stdin();
        print_tokens();
        int resultat;
        int errorcode = berechne_ausdruck(&resultat);
        if (errorcode < 0) print_error_message(errorcode, tokens_position);
        else printf("%d \n\n", resultat);
        clear_tokens();
    }
    return 0;
}
