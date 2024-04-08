/**
* \file parse.cpp
* \brief contains parse class implementations
        parses user input into Expr objects to perform interp(), print(), or pretty_print() methods on 
* \author Ben Baysinger
*/
#include "parse.hpp"


/**
* \brief fifth and most dynamic level of chain. Handles number, variable, let, if, bool, and function expresssions. As well      as well as parenthesis surrounding expressions by starting chain over again. Throws runtime_error if invalid input          is encountered
* \param inn input stream
* \return expression object
*/
PTR(Expr) parse_inner (std::istream & inn) {

    skip_whitespace (inn);
    int c = inn.peek();
    if ((c == '-') || isdigit(c)){
        return parse_num(inn);
    }
    else if (c == '(') {
        consume(inn, '(' );
        PTR(Expr) e = parse_expr(inn);
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
        consume(inn, '_');
        if ( inn.peek() == 'l') {
            return parse_let(inn);
        }
        else if (inn.peek() == 'i') {
            return parse_if(inn);
        }
        else if (inn.peek() == 't') {
            parse_keyword(inn);
            return NEW(BoolExpr)(true);
        }
        else { //BoolExpr(false) or FunExpr
            consume(inn, 'f');
            if ( inn.peek() == 'a') {
                parse_keyword(inn);
                return NEW(BoolExpr)(false);
            }
            else {
                return parse_fun(inn);
            }

        }
    }
    else{
        consume(inn, c);
        throw std::runtime_error("invalid input");
    }
}

/**
* \brief fourth level of chain will handle call expressions if not passes on to next level
* \param in input stream
* \return expression object
*/
PTR(Expr) parse_multicand(std::istream &in) {

    PTR(Expr) expr = parse_inner(in);
    PTR(Expr) actual_arg;
    while ( in.peek() == '(') {
        consume (in, '(');
        actual_arg = parse_expr(in);
        consume(in, ')');
        expr = NEW(CallExpr)(expr, actual_arg);
    }
    return expr;
}

/**
* \brief parses number expressions. Throws runtime_error if invalid input is encountered
* \param inn input stream
* \return NumExpr object
*/
PTR(Expr) parse_num(std::istream &inn) {
    int n = 0;
    bool negative = false;


    if (inn.peek() == '-' && !inn.eof()) {
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
    if (isInvalidInput(inn)) {
        throw std::runtime_error("invalid input");
    }
    return NEW(NumExpr)(n);
}

/**
* \brief second level of recursive chain. Will handle add expressions.  Passes everything else to next level
* \param in input stream
* \return expression object
*/
PTR(Expr) parse_comparg(std::istream &in) {

    PTR(Expr) e;
    e = parse_addend(in);

    skip_whitespace(in);

    int c = in.peek();
    if (c == '+') {
        consume(in, '+');
        skip_whitespace(in);
        PTR(Expr) rhs = parse_comparg(in);
        return NEW(AddExpr)(e, rhs);
    }
    else{
        return  e;
    }
}

/**
* \brief third level of recursive chain will handle multiplication expressions
    if not passes on to next level
* \param in input stream
* \return expression object
*/
PTR(Expr) parse_addend(std::istream &in) {

    PTR(Expr) e;
    e = parse_multicand(in);

    skip_whitespace(in);

    int c = in.peek();
    if (c == '*') {
        consume(in, '*');
        skip_whitespace(in) ;
        PTR(Expr) rhs = parse_addend(in);
        return NEW(MultExpr)(e, rhs);
    }
    else{
        return e ;
    }
}


/**
* \brief top of recursive chain will handle equality expressions  but passes everything else to
    next recursive function. Throws runtime_error if invalid input encountered
* \param in input stream
* \return expression object
*/
PTR(Expr) parse_expr(std::istream &in) {

    PTR(Expr) e;

    e = parse_comparg(in);

    skip_whitespace(in);
    int firstEquals = in.peek();
    if (firstEquals == '=') {
        consume(in, '=');
        int secondEquals = in.peek();
        if ( secondEquals == '=') {
            consume(in, '=');
            return NEW(EqExpr)(e, parse_expr(in));
        }
        else {
            throw std::runtime_error("invalid input");
        }
    }
    return  e;

}

/**
* \brief driver to take in input stream and go into recursive chain
* \param in input stream std::cin
* \return expression object
*/
PTR(Expr) parse(std::istream &in) {
    PTR(Expr) e;
    e = parse_expr(in);
    skip_whitespace(in);
    if ( !in.eof() ) {
        throw std::runtime_error("Invalid input");
    }

    return e;
}

/**
* \brief parses variable expressions. Throws runtime_error if invalid input is encountered
* \param in input stream
* \return VarExpr object
*/
PTR(Expr) parse_var(std::istream &in) {


    std::string var = "";
    char c = in.peek();
    while ( isalpha(c)) {
        var += c;
        consume(in, c);
        c = in.peek();
    }
    if (isInvalidInput(in)){
        throw std::runtime_error("invalid input");
    }

    return NEW(VarExpr)(var);
}

/**
* \brief boolean to check for input stream for valid characters
* \param in input stream
* \return true invalid input is encountered
*/
bool isInvalidInput (std::istream &in){
    int k = in.peek();
    return !isspace(k) 
           && k != ')'
           && k != '('
           && k != '*' && k != '+'
           && k != '=' && !in.eof();
}

/**
* \brief parses function expressions.
* \param inn input stream
* \return FunExpr object
*/
PTR(Expr) parse_fun (std::istream &inn) {

    //_fun(x) body
    std::string formal_arg = "";
    PTR(Expr) body;
    parse_keyword(inn); //consumes fun

    int c = inn.peek();
    //Consume parenthesis
    if ( c == '(') {
        consume(inn, '(');
    }

    //Extract formal_arg
    while ( isalpha(inn.peek())) {
        formal_arg += inn.peek();
        consume(inn, inn.peek());
    }
    skip_whitespace(inn);
    
    //Consume ')'
    c = inn.peek();
    if ( c == ')') {
        consume(inn, ')');
    }
    
    body = parse_expr(inn);

    return NEW(FunExpr)(formal_arg, body);
}


/**
* \brief parses if expressions.
* \param in input stream
* \return IfExpr object
*/
PTR(Expr) parse_if(std::istream &in) {

    PTR(Expr) test_part;
    PTR(Expr) then_part;
    PTR(Expr) else_part;

    parse_keyword(in);//consumes _if

    //Extract test_part
    test_part = parse_expr(in);
    skip_whitespace(in);

    //Extract then_part
    consume(in, '_');
    parse_keyword(in);//consumes _then
    then_part = parse_expr(in);
    skip_whitespace(in);

    //Extract else_part
    consume(in, '_');
    parse_keyword(in);//consumes _else
    else_part = parse_expr(in);

    return NEW(IfExpr)(test_part, then_part, else_part);
}

/**
* \brief parses let expressions. Throws runtime_error if invalid input is encountered
* \param in input stream
* \return LetExpr object
*/
PTR(Expr) parse_let(std::istream &in) {
    PTR(Expr) rhs;
    PTR(Expr) body;

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
    }
    else {
        throw std::runtime_error("consume mismatch for = \n");
    }
    skip_whitespace(in);

    //Extract rhs
    rhs = parse_expr(in);
    skip_whitespace(in);

    //consume underscore
    if ( in.peek() == '_') {
        consume(in, '_');
    }
    parse_keyword(in);//Consume _in
    //Extract body
    body = parse_expr(in);

    //Make sure valid let expression
    std::string bodyString = body->to_string();
    if (bodyString.find(lhs) == std::string::npos) {
        throw std::runtime_error("invalid let expression");
    }

    return NEW(LetExpr)(lhs, rhs, body);
}

/**
* \brief performs interp() method on what expression is returned from recursive chain and prints result as string
*/
void executeInterp() {
    PTR(Expr) e = parse(std::cin);
    std::cout<< e->interp()->to_string() << std::endl;
}

/**
* \brief performs print() method on what expression is returned from recursive chain
*/
void executePrint() {
    PTR(Expr) e = parse(std::cin);
    std::cout<< e->to_string()<< std::endl;
}

/**
* \brief performs pretty_print() method on what expression is returned from recursive chain and prints result as string
*/
void executePrettyPrint() {
    PTR(Expr) e = parse(std::cin);
    std::cout<< e->to_stringPP()<< std::endl;
}

/**
* \brief moves forward input stream by consuming current location. Throws runtime_error if mismatch is encoutered
* \param in input stream
* \param expect expected value of current position of input stream
*/
static void consume(std::istream &in, int expect) {
    int c = in.get();
    if (c!=expect) {
        throw std::runtime_error("consume mismatch");
    }
}

/**
* \brief skips unused space of input stream
* \param in input stream
*/
static void skip_whitespace(std::istream &in) {
    while (1) {
        int c = in.peek();
        if (!isspace(c))
            break;
        consume(in, c);
    }
}

/**
* \brief Another driver to start recursive chain. Converts string to expression object
* \param input string value to be parsed into an expression object
* \return expression object corresponding with passed through string parameter
*/
PTR(Expr) parse_str(std::string input){
    std::stringstream ss(input);
    return parse_expr(ss);
}

/**
* \brief parses and consumes key words from input stream for let, if, bool, and function expression
    throws runtime_error if invalid input is encountered
* \param in input stream
*/
void parse_keyword(std::istream &in) {

    if (in.peek() == 'l' ) {
        consume(in, 'l');
        consume(in, 'e');
        consume(in, 't');
    }
    else if (in.peek() == 'i'){
        consume(in, 'i');
        if (in.peek() == 'f') {
            consume(in, 'f');
        }
        else {
            consume(in, 'n');
        }
    }
    else if (in.peek() == 't'){
        consume(in, 't');
        if (in.peek() == 'r') {
            consume(in, 'r');
            consume(in, 'u');
            consume(in, 'e');
        }
        else {
            consume(in, 'h');
            consume(in, 'e');
            consume(in, 'n');
        }

    }
    else if ( in.peek() == 'a') {
        consume(in, 'a');
        consume(in, 'l');
        consume(in, 's');
        consume(in, 'e');

    }
    else if ( in.peek() == 'u') {
        consume(in, 'u');
        consume(in, 'n');

    }
    else if ( in.peek() == 'e') {
        consume(in, 'e');
        consume(in, 'l');
        consume(in, 's');
        consume(in, 'e');
    }
    else{
        throw std::runtime_error("consume mismatch");
    }
    skip_whitespace(in);
}




