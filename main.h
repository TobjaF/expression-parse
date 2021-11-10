#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

enum token_type {
    token_type_zahl = '8',
    token_type_klammer_auf = '(',
    token_type_klammer_zu = ')',
    token_type_plus = '+',
    token_type_minus = '-',
    token_type_mal = '*',
    token_type_durch = '/',
    token_type_ende = 'x'
};

struct token {
    enum token_type type;
    int value;
};

//AST-ELEMENTS
enum sum_operation{
    sum_operation_plus = '+',
    sum_operation_minus = '-'
};

enum product_operation{
    product_operation_multiply = '*',
    product_operation_divide = '/',
};

struct sum_element {
    enum sum_operation operation;
    struct product* value;
    struct sum_element* next;
};

struct product_element_value{
    struct sum* value_sum;
    int value_int;
};

struct product_element{
    enum product_operation operation;
    struct product_element* next;
    struct product_element_value* value;
};

struct sum {
    struct sum_element* first_child;
};


struct product{
    struct product_element* first_child;
};

struct ausdruck{
    struct sum* summe;
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
int lese_summe(struct sum*);
int lese_produkt(struct product*);
int lese_faktor(struct product_element_value*);
int berechne_ausdruck(int*);
void tokenize_stdin();
void print_error_message(int,int);
void clear_tokens();
void print_tokens();
void print_ast_ausdruck(struct ausdruck*);
void print_ast_summe(struct sum*);
void print_ast_produkt(struct product*);

#endif // MAIN_H_INCLUDED
