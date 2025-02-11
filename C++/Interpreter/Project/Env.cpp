/**
* \file Env.cpp
* \brief contains Env class  and child class implementations
        Env object acts as dictionary to store binded varibles used in LetExpr and FunExpr
* \author Ben Baysinger
*/

#include "Env.hpp"

/**
* \brief static empty environment object
*/
PTR(Env) Env::empty = NEW(EmptyEnv)();

/**
* \brief empty env(dictionary) contain no variable so no lookup can be performed
* \throws std::runtime_error and prints out inteded lookup string
*/
PTR(Val) EmptyEnv::lookup(std::string find_name) {
  throw std::runtime_error("free variable: " + find_name);
}

/**
* \brief constructor to create a ExtendedEnv object
* \param name is the variable name
* \param val is the val to be substituted for the variable
* \param rest is the old environment
*/
ExtendedEnv::ExtendedEnv(std::string name, PTR(Val) val, PTR(Env) rest) {
    this->name = name;
    this->val = val;
    this->rest = rest; 
}

/**
* \brief returns the value associated with the string find_name
* \param find_name varible to find associated Val
* \return val associated with find_name if in current dictionary if not performs lookup on old environment 'rest'
*/
PTR(Val) ExtendedEnv::lookup(std::string find_name) {
  if ( find_name == name) {
    return val;
  }
  else {
    return rest->lookup(find_name);
  }
}
