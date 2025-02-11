/**
* \file Env.hpp
* \brief contains Env parent class and child class declarations and member variables
*/

#ifndef Env_hpp
#define Env_hpp
#include "pointer.hpp"
#include <stdio.h>
#include "string"
class Val;

CLASS(Env) {
    
public:
    
virtual PTR(Val) lookup(std::string find_name) = 0;
static PTR(Env) empty;///< static environment object
};//End of Base class

class EmptyEnv : public Env {
    
public:
    EmptyEnv() = default ;
    PTR(Val) lookup(std::string find_name);
}; //End of EmptyEnv class



class ExtendedEnv : public Env {
    
private:
    
    std::string name;///< name of variable
    PTR(Val) val;///< value to be substituted
    PTR(Env) rest;///< old environment
    
public:
    ExtendedEnv(std::string name, PTR(Val) val, PTR(Env) rest);
    PTR(Val) lookup(std::string find_name);
};//End of ExtendedEnv class


#endif /* Env_hpp */
