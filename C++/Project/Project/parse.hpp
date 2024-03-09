//
//  parse.hpp
//  Project
//
//  Created by Ben Baysinger  on 2/13/24.
//

#ifndef parse_hpp
#define parse_hpp

#include <stdio.h>
#include "Expr.hpp"

static void consume(std::istream &in, int expect);
static void skip_whitespace(std::istream &in);
Expr *parse_num(std::istream &inn);
Expr *parse_expr(std::istream &in);
Expr *parse_multicand(std::istream &inn);
Expr *parse_addend(std::istream &inn);
Expr *parse(std::istream &in);
Expr *parse_input();
void executeInterp();
void executePrint();
void executePrettyPrint();
Expr *parse_let(std::istream &in);
Expr *parse_var(std::istream &in);
Expr *parse_str(std::string input);
bool isInvalidInput (std::istream &in);
void parse_keyword(std::istream &in);




#endif /* parse_hpp */
