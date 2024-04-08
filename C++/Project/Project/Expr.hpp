/**
* \file Expr.hpp
* \brief contains expression parent class and child class declarations and member variables
*/



#ifndef HOMEWORK1SMSDSCRIPT_EXPR_H
#define HOMEWORK1SMSDSCRIPT_EXPR_H

#include <iostream>
#include <stdexcept>
#include <sstream>
#include "pointer.hpp"

class Val;
class Env;


/*! \brief custom enum to set precendence withing operations
* multiplication above add
* num and variable will be none
*/
typedef enum {
    prec_none = 0,
    prec_add = 1,
    prec_mult = 2
} precedence_t;


CLASS(Expr) {
public:
    virtual bool equals(PTR(Expr) e) = 0;
    virtual PTR(Val) interp(PTR(Env) env = nullptr) = 0; 
    virtual PTR(Expr) subst( std::string valToSub, PTR(Expr) expr ) = 0;
    virtual void print( std::ostream &ostream) = 0;
    virtual void pretty_print( std::ostream  &ostream) = 0;
    std::string to_string();
    std::string to_stringPP();
    virtual void pretty_print_at(std::ostream  &ostream, precedence_t precedence, bool parentHasParen, std::streampos &caller_pos) = 0;
    virtual ~Expr() { }
};

class NumExpr : public Expr {
public:
    int val;///< integer value of num expression

    NumExpr(int val);
    bool equals( PTR(Expr) comp );
    PTR(Val) interp(PTR(Env) env = nullptr);
    PTR(Expr) subst( std::string valToSub, PTR(Expr) expr );
    void print( std::ostream &ostream);
    void pretty_print( std::ostream  &ostream);
    void pretty_print_at(std::ostream  &ostream, precedence_t precedence, bool parentHasParen, std::streampos &caller_pos);

};

class AddExpr : public Expr {
public:
    PTR(Expr) lhs;///< expression left hand side of add expression
    PTR(Expr) rhs;///< expression right hand side of add expression

    AddExpr(PTR(Expr) lhs, PTR(Expr) rhs);
    bool equals( PTR(Expr) comp );
    PTR(Val) interp(PTR(Env) env = nullptr);
    PTR(Expr) subst( std::string valToSub, PTR(Expr) expr );
    void print( std::ostream &ostream);
    void pretty_print( std::ostream  &ostream);
    void pretty_print_at(std::ostream  &ostream, precedence_t precedence, bool parentHasParen, std::streampos &caller_pos);

};


class MultExpr : public Expr {
public:
    PTR(Expr) lhs;///< expression left hand side of multiplication expression
    PTR(Expr) rhs;///< expression left hand side of multiplication expression

    MultExpr( PTR(Expr) lhs, PTR(Expr) rhs );
    bool equals( PTR(Expr) comp );
    PTR(Val) interp(PTR(Env) env = nullptr);
    PTR(Expr) subst( std::string valToSub, PTR(Expr) expr );
    void print( std::ostream &ostream);
    void pretty_print( std::ostream  &ostream);
    void pretty_print_at(std::ostream  &ostream, precedence_t precedence, bool parentHasParen, std::streampos &caller_pos);

};

class VarExpr : public Expr {
public:
    std::string value;///< String value contained in Variable expression

    VarExpr( std::string value);
    bool equals(PTR(Expr) comp);
    PTR(Val) interp(PTR(Env) env = nullptr);
    PTR(Expr) subst( std::string valToSub, PTR(Expr) expr );
    void print( std::ostream &ostream);
    void pretty_print( std::ostream  &ostream);
    void pretty_print_at(std::ostream  &ostream, precedence_t precedence, bool parentHasParen, std::streampos &caller_pos);
};

class LetExpr : public Expr {
public:
    std::string lhs;///< String value containing the variable to be replaced
    PTR(Expr) rhs;///< replacement expression to swap lhs for in the body
    PTR(Expr) body;///< expression containing variable to be swapped by rhs
    LetExpr(std::string var, PTR(Expr) replacement, PTR(Expr) exprToSub);
    bool equals(PTR(Expr) comp);
    PTR(Val) interp(PTR(Env) env = nullptr);
    PTR(Expr) subst( std::string valToSub, PTR(Expr) expr );
    void print( std::ostream &ostream);
    void pretty_print( std::ostream  &ostream);
    void pretty_print_at(std::ostream  &ostream, precedence_t precedence, bool parentHasParen, std::streampos &caller_pos);
};

class BoolExpr : public Expr {
private:

    bool boolean;///< boolean value of bool expression

public:

    BoolExpr(bool boolean);
    bool equals(PTR(Expr) comp);
    PTR(Val) interp(PTR(Env) env = nullptr);
    PTR(Expr) subst( std::string valToSub, PTR(Expr) expr );
    void print( std::ostream &ostream);
    void pretty_print( std::ostream  &ostream);
    void pretty_print_at(std::ostream  &ostream, precedence_t precedence, bool parentHasParen, std::streampos &caller_pos);
};

class IfExpr : public Expr {
private:

    PTR(Expr) test_part;///< conditional expression equating to true or false
    PTR(Expr) then_part;///< resulting expression if test_part is true
    PTR(Expr) else_part;///< resulting expression if test_part is false

public:

    IfExpr( PTR(Expr) test_part, PTR(Expr) then_part, PTR(Expr) else_part );
    bool equals(PTR(Expr) comp);
    PTR(Val) interp(PTR(Env) env = nullptr);
    PTR(Expr) subst( std::string valToSub, PTR(Expr) expr );
    void print( std::ostream &ostream);
    void pretty_print( std::ostream  &ostream);
    void pretty_print_at(std::ostream  &ostream, precedence_t precedence, bool parentHasParen, std::streampos &caller_pos);
};

class EqExpr : public Expr {
private:

    PTR(Expr) lhs;///< left hand side of equality expression
    PTR(Expr) rhs;///< right hand side of equality expression

public:

    EqExpr( PTR(Expr) lhs, PTR(Expr) rhs );
    bool equals(PTR(Expr) comp);
    PTR(Val) interp(PTR(Env) env = nullptr);
    PTR(Expr) subst( std::string valToSub, PTR(Expr) expr );
    void print( std::ostream &ostream);
    void pretty_print( std::ostream  &ostream);
    void pretty_print_at(std::ostream  &ostream, precedence_t precedence, bool parentHasParen, std::streampos &caller_pos);
};

class FunExpr : public Expr {
private:
    
    std::string formal_arg;///< variable contained in body to be substituted
    PTR(Expr) body;///< expression containing formal_arg
    
public:
    
    FunExpr( std::string formal_arg, PTR(Expr) body );
    bool equals(PTR(Expr) comp);
    PTR(Val) interp(PTR(Env) env = nullptr);
    PTR(Expr) subst( std::string valToSub, PTR(Expr) expr );
    void print( std::ostream &ostream);
    void pretty_print( std::ostream  &ostream);
    void pretty_print_at(std::ostream  &ostream, precedence_t precedence, bool parentHasParen, std::streampos &caller_pos);
};

class CallExpr : public Expr {
private:
    
    PTR(Expr) to_be_called;///< exprssion containing expression to be subbed out. Will be a function expression
    PTR(Expr) actual_arg;///< expression to substitute with for variable expression in to_be_called
    
public:
    CallExpr( PTR(Expr) to_be_called, PTR(Expr) actual_arg );
    bool equals(PTR(Expr) comp);
    PTR(Val) interp(PTR(Env) env = nullptr);
    PTR(Expr) subst( std::string valToSub, PTR(Expr) expr );
    void print( std::ostream &ostream);
    void pretty_print( std::ostream  &ostream);
    void pretty_print_at(std::ostream  &ostream, precedence_t precedence, bool parentHasParen, std::streampos &caller_pos);
};

#endif //HOMEWORK1SMSDSCRIPT_EXPR_H



