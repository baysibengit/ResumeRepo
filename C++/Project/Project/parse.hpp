/**
* \file parse.hpp
* \brief contains parse class declarations
*/

#ifndef parse_hpp
#define parse_hpp

#include <stdio.h>
#include "Expr.hpp"
#include "pointer.hpp"
#include "Val.hpp"

static void consume(std::istream &in, int expect);
static void skip_whitespace(std::istream &in);
PTR(Expr) parse_num(std::istream &inn);
PTR(Expr) parse_expr(std::istream &in);
PTR(Expr) parse_multicand(std::istream &in);
PTR(Expr) parse_addend(std::istream &inn);
PTR(Expr) parse(std::istream &in);
void executeInterp();
void executePrint();
void executePrettyPrint();
PTR(Expr) parse_let(std::istream &in);
PTR(Expr) parse_var(std::istream &in);
PTR(Expr) parse_if(std::istream &in);
PTR(Expr) parse_str(std::string input);
bool isInvalidInput (std::istream &in);
void parse_keyword(std::istream &in);
PTR(Expr) parse_comparg(std::istream &in);
PTR(Expr) parse_inner (std::istream &inn);
PTR(Expr) parse_fun (std::istream &inn);





#endif /* parse_hpp */

