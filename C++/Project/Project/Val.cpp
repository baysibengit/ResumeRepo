/**
* \file Val.cpp
* \brief contains Val class  and child class implementations
        Val class where results of interping an exprsssion are stored 
* \author Ben Baysinger
*/

#include "Val.hpp"
#include <utility>
#include "Expr.hpp"
#include "Env.hpp"

/**
* \brief converts Val objects to string and prints
* \return Val object as string
*/
std::string Val::to_string() {
    std::stringstream st("");
    this->print(st);
    return st.str();
}

//*****************************NUMVAL CLASS *********************************

/**
* \brief constructor to make a number val
* \param val  number value to represented within number val
*/
NumVal::NumVal(int val) {
    this->val_ = val;
}

/**
* \brief compares number val with 'this'
* \param v val to compare against this
* \return true if param val is equal to this
*/
bool NumVal::equals(PTR(Val) v){
    PTR(NumVal) valPtr = CAST(NumVal)(v);
    if ( valPtr == nullptr ) {
        return false;
    }
    return valPtr->val_ == this->val_;
}

/**
* \brief adds together two NumVals
 *throws exception if other Val object is not type NumVal
* \param v Val object to add to
* \return new Val object result of adding two NumVals.
*/
PTR(Val) NumVal::add_to(PTR(Val) v){
    PTR(NumVal) valPtr = CAST(NumVal)(v);
    if ( valPtr == nullptr ) {
        throw std::runtime_error("Trying to add a non-number!");
    }
    return NEW(NumVal)((unsigned)this->val_ + (unsigned )valPtr->val_);
}

/**
* \brief multiplies together two NumVals
 *throws exception if other Val object is not type NumVal
* \param v Val object to multiply with
* \return new Val object result of mulitplying two NumVals.
*/
PTR(Val) NumVal::mult_with(PTR(Val) v){
    PTR(NumVal) valPtr = CAST(NumVal)(v);
    if ( valPtr == nullptr ) {
        throw std::runtime_error("Trying to perform multiplication with a non-number!");
    }
    return NEW(NumVal)((unsigned)this->val_ * (unsigned)valPtr->val_);
}

/**
* \brief prints out NumVal by converting to NumExpr and calling Expr::print
* \param ostream used to print out
*/
void NumVal::print(std::ostream& ostream){

    this->to_expr()->print(ostream);
}

/**
* \brief converts NumVal to NumExpr
* \return new NumExpr with same member field as 'this'
*/
PTR(Expr) NumVal::to_expr(){
    return NEW(NumExpr)(this->val_);
}

/**
* \brief throws runtime_error because is_true can only be performed on BoolVal
*/
bool NumVal::is_true(){
    throw std::runtime_error("NumVal is not of type boolean");
}

/**
* \brief throws runtime_error because call can only be performed on FunVal object
*/
PTR(Val) NumVal::call(PTR(Val) actual_arg) {
    throw std::runtime_error("NumVal cannot call");
}

//*****************************BOOLVAL CLASS ********************************

/**
* \brief constructor to make a BoolVal object
* \param boolean val to represented in BoolVal object
*/
BoolVal::BoolVal(bool boolean) {
    this->boolean = boolean;
}

/**
* \brief converts BoolVal to BoolExpr
* \return new BoolExpr with same member field as 'this'
*/
PTR(Expr) BoolVal::to_expr() {
    return NEW(BoolExpr)(this->boolean);
}

/**
* \brief compares BoolVal expression with 'this'
* \param v Val to compare against this
* \return true if param Val  is equal to this
*/
bool BoolVal::equals(PTR(Val) v) {

    PTR(BoolVal) valPtr = CAST(BoolVal)(v);
    if ( valPtr == nullptr ) {
        return false;
    }
    return valPtr->boolean == this->boolean;

}

/**
* \brief throws runtime_error because add_to can only be performed on a NumVal
*/
PTR(Val) BoolVal::add_to(PTR(Val) v) {
    throw std::runtime_error("Cannot perform add operation on BoolVal!");
}

/**
* \brief throws runtime_error because mult_with can only be performed on a NumVal
*/
PTR(Val) BoolVal::mult_with(PTR(Val) v) {
    throw std::runtime_error("Cannot perform multiplication operation on BoolVal!");
}

/**
* \brief prints out BoolVal by converting to BoolExpr and calling Expr::print
* \param ostream used to print out
*/
void BoolVal::print(std::ostream& ostream) {
    this->to_expr()->print(ostream);
}

/**
* \brief returns condition of BoolVal's member boolean value
* \returns member boolean of BoolVal object
*/
bool BoolVal::is_true() {
    return this->boolean;
}

/**
* \brief throws runtime_error because call can only be performed on FunVal object
*/
PTR(Val) BoolVal::call(PTR(Val) actual_arg) {
    throw std::runtime_error("BoolVal cannot call");
}

//*****************************FUNVAL CLASS ********************************

/**
* \brief constructor to make a FunVal object
* \param formal_arg string value to be substituted in body
* \param body expression containing formall_arg
* \param env dictionary containing valid replacement for formal_arg
*/
FunVal::FunVal(std::string formal_arg, PTR(Expr) body, PTR(Env) env){

    this->formal_arg = std::move(formal_arg);
    this->body = body;
    this->env = env;

}

/**
* \brief converts FunVal to FunExpr
* \return new FunExpr with same member field as 'this'
*/
PTR(Expr) FunVal::to_expr(){
    return NEW(FunExpr)(this->formal_arg, this->body);
}

/**
* \brief compares FunVal object with 'this'
* \param v Val to compare against this
* \return true if param Val is equal to this
*/
bool FunVal::equals(PTR(Val) v){

    PTR(FunVal) funPtr = CAST(FunVal)(v);
    if ( funPtr == nullptr ) {
        return false;
    }
    return funPtr->formal_arg == this->formal_arg && funPtr->body->equals(this->body);

}

/**
* \brief throws runtime_error because add_to can only be performed on a NumVal
*/
PTR(Val) FunVal::add_to(PTR(Val) v){
    throw std::runtime_error("Cannot perform add operation on FunVal!");
}

/**
* \brief throws runtime_error because mult_with can only be performed on a NumVal
*/
PTR(Val) FunVal::mult_with(PTR(Val) v){
    throw std::runtime_error("Cannot perform multiplication operation on FunVal!");
}

/**
* \brief prints members of FunVal object with prefixes
* \param ostream used to print out FunVal object
*/
void FunVal::print(std::ostream& ostream){
    
    ostream << "[_fun (" << this->formal_arg << ") ";
    this->body->print(ostream);
    ostream << "]";
}

/**
* \brief throws runtime_error because is_true can only be performed on BoolVal
*/
bool FunVal::is_true() {
    throw std::runtime_error("FunVal is not of type boolean");
}

/**
* \brief subtitutes formal_arg with actual_arg by constructed an dictionary(Extended environment)
 *containing actual_arg to substitute formal_arg with 
* \param actual_arg Val to substitute in body of FunVal object
* \return interp result of the body after substitution of actual_arg
*/
PTR(Val) FunVal::call(PTR(Val) actual_arg) {
    return body->interp(NEW(ExtendedEnv)(formal_arg, actual_arg, env));
}
