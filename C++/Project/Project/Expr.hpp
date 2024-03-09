/**
* \file Expr.hpp
* \brief contains expression parent class and child class delarations and member variables
*/


#ifndef HOMEWORK1SMSDSCRIPT_EXPR_H
#define HOMEWORK1SMSDSCRIPT_EXPR_H

#include <iostream>
#include "catch.hpp"
#include <stdexcept>
#include <sstream>


/*! \brief custom enum to set precendence withing operations
* multiplication above add
* num and variable will be none
*/
typedef enum {
    prec_none = 0,
    prec_add = 1,
    prec_mult = 2
} precedence_t;


class Expr {
public:
    virtual bool equals(Expr *e) = 0;
    virtual int interp() = 0;
    virtual bool has_variable() = 0;
    virtual Expr* subst( std::string valToSub, Expr* expr ) = 0;
    virtual void print( std::ostream &ostream) = 0;
    virtual void pretty_print( std::ostream  &ostream) = 0;
    std::string to_string();
    std::string to_stringPP();
    virtual void pretty_print_at(std::ostream  &ostream, precedence_t precedence, bool parentHasParen, std::streampos spaceCounter) = 0;
};

class Num : public Expr {
public:
    int val;///< integer value of num expression

    Num(int val);
    bool equals( Expr *comp );
    int interp();
    bool has_variable();
    Expr* subst( std::string valToSub, Expr* expr );
    void print( std::ostream &ostream);
    void pretty_print( std::ostream  &ostream);
    void pretty_print_at(std::ostream  &ostream, precedence_t precedence, bool parentHasParen, std::streampos spaceCounter);

};

class Add : public Expr {
public:
    Expr *lhs;///< expression left hand side of add expression
    Expr *rhs;///< expression right hand side of add expression

    Add(Expr *lhs, Expr *rhs);
    bool equals( Expr *comp );
    int interp();
    bool has_variable();
    Expr* subst( std::string valToSub, Expr* expr );
    void print( std::ostream &ostream);
    void pretty_print( std::ostream  &ostream);
    void pretty_print_at(std::ostream  &ostream, precedence_t precedence, bool parentHasParen, std::streampos spaceCounter);

};


class Mult : public Expr {
public:
    Expr *lhs;///< expression left hand side of multiplication expression
    Expr *rhs;///< expression left hand side of multiplication expression

    Mult( Expr *lhs, Expr *rhs );
    bool equals( Expr *comp );
    int interp();
    bool has_variable();
    Expr* subst( std::string valToSub, Expr* expr );
    void print( std::ostream &ostream);
    void pretty_print( std::ostream  &ostream);
    void pretty_print_at(std::ostream  &ostream, precedence_t precedence, bool parentHasParen, std::streampos spaceCounter);

};

class Variable : public Expr {
public:
    std::string value;///< String value contained in Variable expression

    Variable( std::string value);
    bool equals(Expr *comp);
    int interp();
    bool has_variable();
    Expr* subst( std::string valToSub, Expr* expr );
    void print( std::ostream &ostream);
    void pretty_print( std::ostream  &ostream);
    void pretty_print_at(std::ostream  &ostream, precedence_t precedence, bool parentHasParen, std::streampos spaceCounter);
};

class LetExpr : public Expr {
public:
    std::string lhs;///< String value containing the variable to be replaced
    Expr *rhs;///< replacement expression to swap lhs for in the body
    Expr *body;///< expression containing variable to be swapped by rhs
    LetExpr(std::string var, Expr *replacement, Expr *exprToSub);
    bool equals(Expr *comp);
    int interp();
    bool has_variable();
    Expr* subst( std::string valToSub, Expr* expr );
    void print( std::ostream &ostream);
    void pretty_print( std::ostream  &ostream);
    void pretty_print_at(std::ostream  &ostream, precedence_t precedence, bool parentHasParen, std::streampos spaceCounter);
};


#endif //HOMEWORK1SMSDSCRIPT_EXPR_H


