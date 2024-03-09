//
//  parse.cpp
//  Project
//
//  Created by Ben Baysinger  on 2/13/24.
//
#include "parse.hpp"



Expr *parse_multicand (std::istream & inn) {

    
    skip_whitespace (inn);
    int c = inn.peek();

    
    
    if ((c == '-') || isdigit(c)){
        return parse_num(inn);
    }
    else if (c == '(') {
        consume(inn, '(' );
        Expr *e = parse_expr(inn);
        skip_whitespace(inn);
        c = inn.get();
        if (c != ')'){
            throw std::runtime_error("missing close parenthesis");
        }
        return e;
    }
    else if ( isalpha(c)) {
        return parse_var(inn);
    }
    else if ( c == '_' ) {
        return parse_let(inn);
    }
    else{
        consume(inn, c);
        throw std::runtime_error("invalid input");
    }
}

Expr *parse_num(std::istream &inn) {
    int n = 0;
    bool negative = false;

    if (inn.peek() == '-') {
        negative = true;
        consume (inn, '-');
        if(!isdigit(inn.peek())){
            throw std::runtime_error("invalid input") ;
        }
    }


    while (1) {
        int c = inn.peek();
        if (isdigit(c))
        {
            consume(inn, c);
            n = n*10 + (c - '0');
        }
        else
            break;
    }

    if (negative)
        n = n * -1;

    return new Num(n);
}

Expr *parse_expr(std::istream &in) {
    
    Expr *e;

    e = parse_addend(in);

    skip_whitespace(in);
    int c = in.peek();
    if (c == '+') {
        consume(in, '+');
        Expr *rhs = parse_expr(in);
        return new Add(e, rhs);
    }
    else{
        return  e;
    }
}

Expr *parse_addend(std::istream &in) {
    
    Expr *e;
    e = parse_multicand(in);

    skip_whitespace(in);

    int c = in.peek();
    if (c == '*') {
        consume(in, '*');
        skip_whitespace(in) ;
        Expr *rhs = parse_addend(in);
        return new Mult(e, rhs);
    }
    else{
        return e ;
    }
}

Expr *parse(std::istream &in) {
    Expr* e;
    e = parse_expr(in);
    skip_whitespace(in);
    if ( !in.eof() ) {
        throw std::runtime_error("Invalid input");
    }
    
    return e;
}

Expr *parse_input(){
    std::string input;
    getline( std::cin, input);
    std::stringstream ss(input);
    return parse_expr(ss);
    
}

Expr *parse_var(std::istream &in) {

    std::string var = "";
    char c = in.peek();
    while ( isalpha(c)) {
        var += c;
        consume(in, c);
        c = in.peek();
    }
    if (isInvalidInput(in)) {
        throw std::runtime_error("invalid input");
    }

    return new Variable(var);
}

bool isInvalidInput (std::istream &in){
    
    char c = in.peek();
    int newLine = 10;
    int space = 32;
    int leftParen = 40;
    int rightParen = 41;
    int multSign = 42;
    int plusSign = 43;
    return c!= newLine && c != space && c != EOF  && c != leftParen && c != rightParen &&
    c != multSign && c != plusSign && !isalpha(c);
}

Expr *parse_let(std::istream &in) {
    Expr* rhs;
    Expr* body;

    parse_keyword(in);//consumes _let
    
    int c = in.peek();
    
    //Extract lhs
    std::string lhs = "";
    while ( isalpha(c) ) {
        lhs += c;
        consume(in, c);
        c = in.peek();
    }
    skip_whitespace(in);
    c = in.peek();
    if ( c == '=' ) {
        consume(in, '=');
        c = in.peek();
    }
    else {
        throw std::runtime_error("consume mismatch for = \n");
    }
    skip_whitespace(in);
    
    //Extract rhs
    rhs = parse_expr(in);
    skip_whitespace(in);
    
    parse_keyword(in);//Consume _in
    
    //Extract body
    body = parse_expr(in);
    
    return new LetExpr(lhs, rhs, body);
}

void executeInterp() {
    Expr *e = parse(std::cin);
    std::cout<< e->interp()<< std::endl;
}

void executePrint() {
    Expr *e = parse(std::cin);
    std::cout<< e->to_string()<< std::endl;
}
void executePrettyPrint() {
    Expr *e = parse(std::cin);
    std::cout<< e->to_stringPP()<< std::endl;
}

static void consume(std::istream &in, int expect) {
    int c = in.get();
    if (c!=expect) {
        throw std::runtime_error("consume mismatch");
    }
}

static void skip_whitespace(std::istream &in) {
    while (1) {
        int c = in.peek();
        if (!isspace(c))
            break;
        consume(in, c);
    }
}

Expr *parse_str(std::string input){
    std::stringstream ss(input);
    return parse_expr(ss);
}

void parse_keyword(std::istream &in) {
    
    consume(in, '_');
    
    if (in.peek() == 'l' ) {
        consume(in, 'l');
        consume(in, 'e');
        consume(in, 't');
    }
    else {
        consume(in, 'i');
        consume(in, 'n');
    }
    skip_whitespace(in);
}


