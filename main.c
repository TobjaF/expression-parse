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
#include "main.h"

//globale Variablen initialisieren
int tokens_size=0;
int tokens_position=0;
struct token tokens[256];
int indent_stack=0;
int indent_amount=4;
char indent_char=' ';

void push_indent_stack(){
++indent_stack;
}

void pop_indent_stack(){
--indent_stack;
}

int get_indent_stack(){
return indent_stack;
}

void print_indent_stack(){
    for(int i=0; i<indent_amount*indent_stack;i++) printf("%c", indent_char);
}


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
            case '+': {push_token(token_type_plus, c);  c = fgetc(stdin); break;}
            case '-': {push_token(token_type_minus, c); c = fgetc(stdin); break;}
            case '/': {push_token(token_type_durch, c); c = fgetc(stdin); break;}
            case '*': {push_token(token_type_mal, c); c = fgetc(stdin); break;}
            case '(': {push_token(token_type_klammer_auf, c); c = fgetc(stdin); break;}
            case ')': {push_token(token_type_klammer_zu, c); c = fgetc(stdin); break;}
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
                push_token(token_type_zahl, zahl_wert);
                break;
            }
            default: {c = fgetc(stdin); break;}; // Fehler ausgeben
        }
    }
    push_token(token_type_ende, 0);
}

void print_tokens(){
    for (int i = 0; i<tokens_size-1; i++){
        if (tokens[i].type == token_type_zahl) printf("%d", tokens[i].value);
        else printf("%c", tokens[i].value);
    }
}

void print_function_style_ast_ausdruck(struct ausdruck* ausdruck){
    print_function_style_ast_summe(ausdruck->summe);
    printf("\n\n");
}

void print_function_style_ast_sum_element(struct sum_element* sum_element){
    print_function_style_ast_produkt(sum_element->value);
}

void print_function_style_ast_summe(struct sum* summe){
    struct sum_element* current_sum_element = summe->first_child;
    if (!(current_sum_element->next)){
        print_function_style_ast_sum_element(current_sum_element);
        return;
    }
    printf("Summe( ");
    while (current_sum_element){
        if (current_sum_element->operation == sum_operation_minus){
            printf("%c", current_sum_element->operation);
        }
        print_function_style_ast_sum_element(current_sum_element);
        if (current_sum_element->next) printf(", ");
        current_sum_element = current_sum_element->next;
    }
    printf(" )");
}

void print_function_style_ast_product_element(struct product_element* product_element){
  if (!(product_element->value->value_sum)) {
            int value = product_element->value->value_int;
            printf("%d", value);
        } else {
            print_function_style_ast_summe(product_element->value->value_sum);
        }
}

void print_function_style_ast_produkt(struct product* product){
    struct product_element* current_product_element = product->first_child;
    if (!(current_product_element->next)){
        print_function_style_ast_product_element(current_product_element);
        return;
    }
    printf("Produkt( ");
    while (current_product_element){
        if(current_product_element->operation == product_operation_divide){
            printf("%c", current_product_element->operation);
        }
        print_function_style_ast_product_element(current_product_element);
        if (current_product_element->next) printf(", ");
        current_product_element = current_product_element->next;
    }
    printf(" )");
}


void print_html_style_ast_ausdruck(struct ausdruck* ausdruck){
    print_indent_stack(); printf("<div class=\"ausdruck\">\n");
    push_indent_stack();
    print_html_style_ast_summe(ausdruck->summe);
    pop_indent_stack();
    print_indent_stack(); printf("</div>");
}

void print_html_style_ast_summe(struct sum* sum){
    struct sum_element* current_sum_element = sum->first_child;
    if (!(current_sum_element->next)){
        print_html_style_ast_sum_element(current_sum_element);
        return;
    }
    print_indent_stack(); printf("<div class=\"summe\">\n");
    push_indent_stack();
    while (current_sum_element){
        if (current_sum_element->operation == sum_operation_minus){
            print_indent_stack(); printf("<div class=\"summand-minus\">\n");
        } else {print_indent_stack(); printf("<div class=\"summand-plus\">\n");}
        push_indent_stack();
        print_html_style_ast_sum_element(current_sum_element);
        pop_indent_stack();
        print_indent_stack(); printf("</div>\n");
        current_sum_element = current_sum_element->next;
    }
    pop_indent_stack();
    print_indent_stack(); printf("</div>\n");
}

void print_html_style_ast_produkt(struct product* product){
    struct product_element* current_product_element = product->first_child;
    if (!(current_product_element->next)){
        print_html_style_ast_product_element(current_product_element);
        return;
    }
    print_indent_stack(); printf("<div class=\"produkt\">\n");
    push_indent_stack();
    while (current_product_element){
        if (current_product_element->operation == product_operation_multiply){
            print_indent_stack(); printf("<div class=\"faktor-multiplikation\">\n");
        } else {print_indent_stack(); printf("<div class=\"faktor-division\">\n");}
        push_indent_stack();
        print_html_style_ast_product_element(current_product_element);
        pop_indent_stack();
        print_indent_stack(); printf("</div>\n");
        current_product_element = current_product_element->next;
    }
    pop_indent_stack();
    print_indent_stack(); printf("</div>\n");
}

void print_html_style_ast_sum_element(struct sum_element* sum_element){
    print_html_style_ast_produkt(sum_element->value);
}

void print_html_style_ast_product_element(struct product_element* product_element){
     if (!(product_element->value->value_sum)) {
        int value = product_element->value->value_int;
        print_indent_stack();printf("%d\n", value);
    } else {
        print_html_style_ast_summe(product_element->value->value_sum);
    }
}


int eval_ast_faktor(struct product_element_value* product_element_value){
    int result;
    if (!(product_element_value->value_sum)) result = product_element_value->value_int;
    else result = eval_ast_summe(product_element_value->value_sum);
    return result;
}


int eval_ast_produkt(struct product* product){
    int result = 1;
    struct product_element* product_element_current = product->first_child;
    while (product_element_current){
        int current_element_value = eval_ast_faktor(product_element_current->value);
        if (product_element_current->operation == product_operation_multiply){
            result *= current_element_value;
        } else result /= current_element_value;
        product_element_current = product_element_current->next;
    }
    return result;
}

int eval_ast_summe(struct sum* summe){
    int result = 0;
    struct sum_element* sum_element_current = summe->first_child;
    while (sum_element_current){
        int current_element_value = eval_ast_produkt(sum_element_current->value);
        if (sum_element_current->operation == sum_operation_plus){
             result += current_element_value;
        } else result -= current_element_value;
        sum_element_current = sum_element_current->next;
    }
    return result;
}

int eval_ast_ausdruck(struct ausdruck* ausdruck){
    int result = eval_ast_summe(ausdruck->summe);
    return result;
}

void clear_tokens () {
    tokens_position = 0;
    tokens_size = 0;
    for (int i=0; i<256; i++){
        tokens[i].type = token_type_zahl;
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
    if (naechstes.type == token_type_zahl) {
        *faktor = naechstes.value;
        return 0;
    }
    else if (naechstes.type == token_type_klammer_auf){
        int errorcode = berechne_summe(faktor);
        if (errorcode < 0) return errorcode;
        if (!konsumiere_token_falls(token_type_klammer_zu)) return -1;
        return 0;
    }
    else if (naechstes.type == token_type_ende) return -2; //Fehler "Hier muss ein Zeichen stehen"
    else return -3; //Fehler "Zeichen nicht erlaubt"
}

int lese_faktor(struct product_element_value* value){
    struct token naechstes = konsumiere_token();
    if (naechstes.type == token_type_zahl) {
        value->value_int = naechstes.value;
        return 0;
    }

    else if (naechstes.type == token_type_klammer_auf){
        struct sum* summe = (struct sum*) calloc(1,sizeof(struct sum)); //new_sum();
        int errorcode = lese_summe(summe);
        if (errorcode < 0) return errorcode;
        if (!konsumiere_token_falls(token_type_klammer_zu)) return -1;
        value->value_sum = summe;
        return 0;
    }
    else return -4; //Fehler "Zahl oder öffnende Klammer erwartet
    //else if (naechstes.type == token_type_ende) return -2; //Fehler "Hier muss ein Zeichen stehen"
    //else return -3; //Fehler "Zeichen nicht erlaubt"
}

int berechne_produkt(int* produkt){
    struct token naechstes;
    naechstes.type = token_type_mal;
    *produkt = 1;
    do {
        int faktor;
        int errorcode = berechne_faktor(&faktor);
        if (errorcode < 0 ) return errorcode;
        if (naechstes.type == token_type_mal )*produkt *= faktor;
        else *produkt /= faktor;
        naechstes = token_vorausschauen();
    } while (konsumiere_token_falls_eines(token_type_mal,token_type_durch));
    return 0;
}

int lese_produkt(struct product* produkt){
    struct product_element* product_element_current = NULL;
    struct token naechstes;
    naechstes.type = token_type_mal;
    do {
        struct product_element* new_product_elem = calloc(1,sizeof(struct product_element));
        if (naechstes.type == token_type_mal) new_product_elem->operation = product_operation_multiply;
        else new_product_elem->operation = product_operation_divide;
        new_product_elem->value = (struct product_element_value*) calloc(1,sizeof(struct product_element_value));
        int errorcode = lese_faktor(new_product_elem->value);
        if (errorcode < 0 ) return errorcode;
        if (!product_element_current) produkt->first_child = new_product_elem;
        else product_element_current->next = new_product_elem;
        product_element_current = new_product_elem;
        naechstes = token_vorausschauen();
    } while (konsumiere_token_falls_eines(token_type_mal,token_type_durch));
    return 0;
}

int berechne_summe(int* summe){
    struct token naechstes;
    naechstes.type = token_type_plus;
    *summe = 0;
    do {
        int produkt;
        int errorcode = berechne_produkt(&produkt);
        if (errorcode < 0) return errorcode;
        if (naechstes.type == token_type_plus) *summe += produkt;
        else *summe -= produkt;
        naechstes = token_vorausschauen();
    } while (konsumiere_token_falls_eines(token_type_plus,token_type_minus));
    return 0;
}

int lese_summe(struct sum* summe){
    struct sum_element* sum_element_current = NULL;
    struct token naechstes;
    naechstes.type = token_type_plus;
    do {
        struct sum_element* new_sum_elem = (struct sum_element*) calloc(1,sizeof(struct sum_element));
        if (naechstes.type == token_type_plus) new_sum_elem->operation = sum_operation_plus;
        else new_sum_elem->operation = sum_operation_minus;
        new_sum_elem->value = (struct product*) calloc(1,sizeof(struct product));
        int errorcode = lese_produkt(new_sum_elem->value);
        if (errorcode < 0) return errorcode;
        if (!sum_element_current) summe->first_child = new_sum_elem;
        else sum_element_current->next = new_sum_elem;
        sum_element_current = new_sum_elem;
        naechstes = token_vorausschauen();
    } while (konsumiere_token_falls_eines(token_type_plus,token_type_minus));
    return 0;
};

int berechne_ausdruck(int* resultat){
    int errorcode = berechne_summe(resultat);
    if (errorcode < 0) return errorcode;
    if (!konsumiere_token_falls(token_type_ende)) return -3; //Fehler "Zeichen nicht erlaubt"
    return 0;
}

int lese_ausdruck(struct ausdruck* ausdruck){
    ausdruck->summe = calloc(1,sizeof(struct sum));
    int errorcode = lese_summe(ausdruck->summe);
    if (errorcode < 0) return errorcode;
    if (!konsumiere_token_falls(token_type_ende)) return -3; //Fehler "Zeichen nicht erlaubt"
    return 0;
}

void print_error_message (int errorcode, int position) {
    for (int i= 0; i<position+3; i++) printf(" "); //Die Position des Fehlers mit einem '^' markieren
    printf("^\n");
    switch(errorcode){
        case -1: printf("Fehler an Position %d: Abschliessende Klammer ')' erwartet\n\n", position); break;
        case -2: printf("Fehler an Position %d: Hier muss ein Zeichen stehen \n\n", position); break;
        case -3: printf("Fehler an Position %d: Zeichen hier nicht erlaubt\n\n", position); break;
        case -4: printf("Fehler an Position %d: Zahl oder oeffnende Klammer '(' erwartet\n\n", position); break;
        default: printf("Fehler an Position %d: \n\n", position); break;
    }
}

int main(int argc, char *argv[])
{
    //system("\"C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe\"");
    while(true){
        printf("Mathematischen Ausdruck eingeben, z.B. \'3+(2-10)*4\' \n");
        tokenize_stdin();
        printf("\n\nEingabe \n--> ");
        print_tokens();
        printf("\n");
        //int resultat;
        struct ausdruck* ausdruck = calloc(1,sizeof(struct ausdruck));//NULL;//new_ausdruck();
        int errorcode = lese_ausdruck(ausdruck);
        if (errorcode < 0) print_error_message(errorcode, tokens_position);
        else {
            printf("\n\n");
            int result = eval_ast_ausdruck(ausdruck);
            printf("Eingabe in Function-Style \n--> ");
            print_function_style_ast_ausdruck(ausdruck);
            printf("\n\nEingabe als HTML-Baum \n-->\n");
            print_html_style_ast_ausdruck(ausdruck);
            printf("\n\nResultat \n--> %d \n\n", result);
        }
        printf("\n\n\n");
        clear_tokens();
    }
    return 0;
}
