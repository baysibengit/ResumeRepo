/**
* \file Val.hpp
* \brief contains Val parent class and child class declarations and member variables
*/

#ifndef HOMEWORK1SMSDSCRIPT_VAL_H
#define HOMEWORK1SMSDSCRIPT_VAL_H

#include <string>
#include "pointer.hpp"


class Expr;
class Env ;


CLASS(Val) {

public:
    virtual PTR(Expr) to_expr() = 0;
    virtual bool equals(PTR(Val) v) = 0;
    virtual PTR(Val) add_to(PTR(Val) v) = 0;
    virtual PTR(Val) mult_with(PTR(Val) v) = 0;
    virtual void print(std::ostream& ostream) = 0;
    virtual bool is_true() = 0;
    virtual PTR(Val) call(PTR(Val) actual_arg) = 0;
    std::string to_string();
    virtual ~Val() { }

};


class NumVal : public Val {
private:
    int val_;///< integer value of num val

public:
    NumVal(int val);
    PTR(Expr) to_expr();
    bool equals(PTR(Val) v);
    PTR(Val) add_to(PTR(Val) v);
    PTR(Val) mult_with(PTR(Val) v);
    void print(std::ostream& ostream);
    bool is_true();
    PTR(Val) call(PTR(Val) actual_arg);
};

class BoolVal : public Val {
private:
    bool boolean;///< boolean value of bool val

public:
    BoolVal(bool boolean);
    PTR(Expr) to_expr();
    bool equals(PTR(Val) v);
    PTR(Val) add_to(PTR(Val) v);
    PTR(Val) mult_with(PTR(Val) v);
    void print(std::ostream& ostream);
    bool is_true();
    PTR(Val) call(PTR(Val) actual_arg);
};

class FunVal : public Val {
private:
    std::string formal_arg;///< variable to be substituted in body
    PTR(Expr) body;///< expression containing formal_arg
    PTR(Env) env;///< dictionary containing expression to be subtituted for formal_arg 

public:
    FunVal(std::string formal_arg, PTR(Expr) body, PTR(Env) env = nullptr);
    PTR(Expr) to_expr();
    bool equals(PTR(Val) v);
    PTR(Val) add_to(PTR(Val) v);
    PTR(Val) mult_with(PTR(Val) v);
    void print(std::ostream& ostream);
    bool is_true();
    PTR(Val) call(PTR(Val) actual_arg);
};



#endif //HOMEWORK1SMSDSCRIPT_VAL_H


