/**
* \file Expr.cpp
* \brief contains expression class  and child class implementations
        Expression class where we decide the grammar and how the different child classes
        interact with each other. Child classes are Num, Add, Variable, Mult, let .
* \author Ben Baysinger
*/


#include "Expr.hpp"
#include "Val.hpp"
#include "Env.hpp"



//**********************EXP CLASS IMPLEMENTATIONS *************************************

/**
* \brief prints out string results of print method
* \returns string result of print method
*/
std::string Expr:: to_string(){
    std::stringstream st("");
    this->print(st);
    return st.str();
}

/**
* \brief prints out string results of pretty print method
* \returns string result of pretty print method
*/
std::string Expr:: to_stringPP(){
    std::stringstream st("");
    this->pretty_print(st);
    return st.str();
}
//**********************NUM CLASS IMPLEMENTATIONS **************************************

/**
* \brief constructor to make a number expression
* \param val  number value to represented within number expression
*/
NumExpr::NumExpr( int val ) {
    this->val = val;
}

/**
* \brief compares number expression with this
* \param comp expression to compare against this
* \return true if param expression is equal to this
*/
bool NumExpr::equals(PTR(Expr) comp) {

    PTR(NumExpr) numPtr = CAST(NumExpr)(comp);
    if ( numPtr == nullptr ) {
        return false;
    }
    else {
        return numPtr->val == this->val;
    }
}

/**
* \brief returns integer value of number expression
* \param env environment object unused
* \return Val  object with integer this->val  of number expression
*/
PTR(Val) NumExpr::interp(PTR(Env) env) {
    return NEW(NumVal)(this->val);
}

/**
* \brief returns this expression
* \param valToSub unused
* \param expr unused
* \return this
*/
PTR(Expr) NumExpr::subst(std::string valToSub, PTR(Expr) expr) {
    return THIS;
}

/**
* \brief converts num expression to string
* \param ostream used to convert integer to string
*/
void NumExpr::print( std::ostream &ostream){
    ostream<<std::to_string(val);
}

/**
* \brief converts num expression to string
* \param ostream used to convert integer to string
*/
void NumExpr::pretty_print( std::ostream  &ostream){
    ostream<<std::to_string(val);
}

/**
* \brief converts num expression to string
* \param ostream used to convert integer to string
* \param  precedence unused
* \param parentHasParen unused
* \param caller_pos unused
*/
void NumExpr::pretty_print_at(std::ostream  &ostream, precedence_t precedence, bool parentHasParen, std::streampos &caller_pos) {
    ostream<<std::to_string(val);
}


//**********************ADD CLASS IMPLEMENTATIONS **************************************

/**
* \brief constructor to make an add expression
* \param lhs expression of add expression
* \param rhs expression of add expression
*/
AddExpr::AddExpr(PTR(Expr) lhs, PTR(Expr) rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
}

/**
* \brief compares both sides of an add expression to this using recursion
* \param comp expression to compare against this
* \return recursive call comparing lhs and rhs. True if expressions are equal false if not.
*/
bool AddExpr::equals ( PTR(Expr) comp ) {

    PTR(AddExpr) addPtr = CAST(AddExpr)(comp);
    if ( addPtr == nullptr ) {
        return false;
    }
    else {
        return addPtr->lhs->equals(this->lhs) && addPtr->rhs->equals(this->rhs);
    }
}

/**
* \brief gives add operation result of add expression
* \param env check if environment is null, if so create an empty environment object 
* \return Val object by recursive call on lhs and rhs to navigate down to a num expression and adding those two values together
*/
PTR(Val) AddExpr::interp(PTR(Env) env) {
    
    if(env == nullptr){
        env = Env::empty;
    }
    return lhs->interp(env)->add_to(rhs->interp(env));
}

/**
* \brief function returns new version of this with an expression in place of targeted variable using recursion
* \param valToSub variable value to substitute expression for
* \param expr expression to place in this expression valToSub
* \return recursive call to lhs and rhs to navigate to position of valToSub
*/
PTR(Expr) AddExpr::subst(std::string valToSub, PTR(Expr) expr) {

    return NEW(AddExpr)(this->lhs->subst(valToSub,expr), this->rhs->subst(valToSub,expr)) ;
}

/**
* \brief converts both sides of an add expression to string using recursion to narrow down until var or num encountered
 * surrounds whole expression in parenthesis, no space between expressions and prefix
* \param ostream used to convert add expression to string
*/
void AddExpr::print( std::ostream &ostream){

    ostream << "(";
    this->lhs->print(ostream);
    ostream<< "+";
    this->rhs->print(ostream);
    ostream<< ")";
}

/**
* \brief driver function for recursion call that takes in ostream
* \param ostream used to convert add expression  to string
*/
void AddExpr::pretty_print( std::ostream  &ostream){
    std::streampos pos = 0;

    pretty_print_at(ostream, prec_add, false, pos);
}

/**
* \brief converts both sides of an add expression to string using recursion to narrow down until var or num encountered
 * places parenthesis to adhere to association to the right and our precedence scale
* \param ostream used to convert add expression  to string
* \param precedence precedence of the caller, will be prec_add
* \param parentHasParen boolean signifying if parent caller is surrounded by parenthesis
* \param caller_pos streampos reference to position of caller
*/
void AddExpr::pretty_print_at(std::ostream &ostream, precedence_t precedence, bool parentHasParen, std::streampos &caller_pos) {

    if(precedence > prec_add){
        ostream << "(";
    }
    this->lhs->pretty_print_at(ostream, static_cast<precedence_t>(prec_add + 1), parentHasParen, caller_pos);
    ostream<< " + ";
    this->rhs->pretty_print_at(ostream, prec_none, parentHasParen, caller_pos);

    if(precedence > prec_add){
        ostream << ")";
    }
}

//**********************MULT CLASS IMPLEMENTATIONS *************************************

/**
* \brief constructor to make an multiplication expression
* \param lhs expression of multiplication expression
* \param rhs expression of multiplication expression
*/
MultExpr::MultExpr( PTR(Expr) lhs, PTR(Expr) rhs ) {
    this->lhs = lhs;
    this->rhs = rhs;
}

/**
* \brief compares both sides of an multiplication expression to this using recursion
* \param comp expression to compare against this
* \return recursive call comparing lhs and rhs. True if expressions are equal false if not.
*/
bool MultExpr::equals(PTR(Expr) comp) {
    PTR(MultExpr) multPtr = CAST(MultExpr)(comp);
    if ( multPtr == nullptr ) {
        return false;
    }
    else {
        return multPtr->lhs->equals(this->lhs) && multPtr->rhs->equals(this->rhs);
    }
}

/**
* \brief gives multiplication operation result of multiplication expression
* \param env check if environment is null, if so create an empty environment object
* \return Val object by recursive call on lhs and rhs to navigate down to a num expression and mutliplying those two values together
*/
PTR(Val) MultExpr::interp(PTR(Env) env) {
    
    if(env == nullptr){
        env = Env::empty;
    }
    return lhs->interp(env)->mult_with(rhs->interp(env));
}

/**
* \brief function returns new version of this with an expression in place of targeted variable using recursion
* \param valToSub variable value to substitute expression for
* \param expr expression to place in this expression valToSub
* \return recursive call to lhs and rhs to navigate to position of valToSub
*/
PTR(Expr) MultExpr::subst(std::string valToSub, PTR(Expr) expr) {

    return NEW(MultExpr)(this->lhs->subst(valToSub,expr), this->rhs->subst(valToSub,expr)) ;
}

/**
* \brief converts both sides of an multiplication expression to string using recursion to narrow down until var or num encountered
 * surrounds whole expression in parenthesis, no space between expressions and prefix
* \param ostream used to convert multiplication expression to string
*/
void MultExpr::print( std::ostream &ostream){
    ostream << "(";
    this->lhs->print(ostream);
    ostream<< "*";
    this->rhs->print(ostream);
    ostream<< ")";
}

/**
* \brief driver function for recursion call that takes in ostream
* \param ostream used to convert mult expression  to string
*/
void MultExpr::pretty_print( std::ostream  &ostream){
    std::streampos pos = 0;

    pretty_print_at(ostream, prec_mult, false, pos );
}

/**
* \brief converts both sides of an multiplication expression to string using recursion to narrow down until var or num encountered
 * places parenthesis to adhere to association to the right and our precedence scale
* \param ostream used to convert multiplication expression  to string
* \param precedence precedence of the caller, will be prec_mult
* \param parentHasParen boolean signifying if parent caller is surrounded by parenthesis
* \param caller_pos streampos reference to position of caller
*/
void MultExpr::pretty_print_at(std::ostream &ostream, precedence_t precedence, bool parentHasParen, std::streampos &caller_pos) {


    if(precedence > prec_mult){
        parentHasParen = true;
        ostream << "(";
    }
    this->lhs->pretty_print_at(ostream, static_cast<precedence_t>(prec_mult + 1), parentHasParen, caller_pos);
    ostream<< " * ";

    this->rhs->pretty_print_at(ostream, prec_mult, parentHasParen, caller_pos);

    if(precedence > prec_mult){
        ostream << ")";
    }
}


//**********************VARIABLE CLASS IMPLEMENTATIONS *******************************

/**
* \brief constructor to make a variable expression
* \param value  string value to represented within variable expression
*/
VarExpr::VarExpr(std::string value) {
    this->value = value;
}

/**
* \brief compares variable's values for equality
* \param comp expression to compare against this.
* \return recursive call comparing lhs and rhs. True if variable's value are equal false if not.
*/
bool VarExpr::equals(PTR(Expr) comp) {
    PTR(VarExpr) varPtr = CAST(VarExpr)(comp);
    if ( varPtr == nullptr ) {
        return false;
    }

    return varPtr->value == this->value;
}

/**
* \brief Val object resutl from lookup 
* \param env check if environment is null, if so create an empty environment object
* \return result of looking up variable from 'env' dictionary
*/
PTR(Val) VarExpr::interp(PTR(Env) env) {
    
    if(env == nullptr){
        env = Env::empty;
    }
    return env->lookup(this->value);
}

/**
* \brief function returns expression parameter substituted in place of parameter valToSub
* \param valToSub variable value to substitute expression for
* \param expr expression to place in this expression valToSub
* \return this if valToSub is not value withing expression, if it is return new expression with expr in place of valToSub
*/
PTR(Expr) VarExpr::subst(std::string valToSub, PTR(Expr) expr) {

    if ( this->value == valToSub ){
        return expr;
    }
    return THIS;
}

/**
* \brief variable expression to string
* \param ostream used to send out value which is already a string
*/
void VarExpr::print( std::ostream &ostream){

    ostream<< this->value;

}

/**
* \brief variable expression to string
* \param ostream used to send out value which is already a string
*/
void VarExpr::pretty_print( std::ostream  &ostream){
    ostream<< this->value;
}

/**
* \brief variable expression to string
* \param ostream used to send out value which is already a string
* \param  precedence unused
* \param parentHasParen unused
* \param caller_pos unused
*/
void VarExpr::pretty_print_at(std::ostream &ostream, precedence_t precedence, bool parentHasParen, std::streampos &caller_pos) {
    ostream<< this->value;
}

//**********************LET CLASS IMPLEMENTATIONS ***********************************

/**
* \brief constructor to make a let expression
* \param var string variable to be replaced in the body
* \param replacement expression to be swapped for var in the exprToSub
* \param exprToSub expression containing var to be substituted by replacement
*/
LetExpr::LetExpr(std::string var, PTR(Expr) replacement, PTR(Expr) exprToSub) {

    this->lhs = var;
    this->rhs = replacement;
    this->body = exprToSub;
}

/**
* \brief compares all fields of a let expression to this using recursion
* \param comp expression to compare against this
* \return recursive call comparing rhs and body. True if expressions are equal false if not.
*/
bool LetExpr::equals(PTR(Expr) comp) {
    PTR(LetExpr) letPtr = CAST(LetExpr)(comp);
    if ( letPtr == nullptr ) {
        return false;
    }
    else {
        return letPtr->lhs == this->lhs  && letPtr->rhs->equals(this->rhs) && letPtr->body->equals(this->body);
    }

}

/**
* \brief gives let expression result of body expression after substitution of rhs
* \param env check if environment is null, if so create an empty environment object
* \return Val object result of let expression by recursive call on  rhs and body to navigate down to a num expression
 *Checks env dictionary for binded lhs and substitutes that variable in the body *
*/
PTR(Val) LetExpr::interp(PTR(Env) env) {
    
    if(env == nullptr){
        env = Env::empty;
    }

        PTR(Val) rhs_val = rhs->interp(env);
        PTR(Env) new_env = NEW(ExtendedEnv)(lhs, rhs_val, env);
        return body->interp(new_env);
}

/**
* \brief function returns new version of this with an expression in place of targeted variable using recursion
* \param valToSub variable value to substitute expression for
* \param expr expression to place in this expression valToSub
* \return new expression from body after substitution.  If lhs equals valToSub recursion call only on rhs, else call on rhs and body
*/
PTR(Expr) LetExpr::subst(std::string valToSub, PTR(Expr) expr) {

    if ( valToSub == this->lhs ){
        return NEW(LetExpr)(this->lhs, this->rhs->subst(valToSub, expr), this->body);
    }
    return NEW(LetExpr)(this->lhs, this->rhs->subst(valToSub, expr), this->body->subst(valToSub, expr));
}

/**
* \brief converts let expression to string using recursion to narrow down until var or num encountered
 * surrounds whole expression in parenthesis, no space between expressions and prefixes
* \param ostream used to convert let expression to string
*/
void LetExpr::print( std::ostream &ostream){

    ostream << "(_let ";
    ostream << this->lhs;
    ostream << "=";
    this->rhs->print(ostream);
    ostream<< " _in ";
    this->body->print(ostream);
    ostream<< ")";
}

/**
* \brief driver function for recursion call that takes in ostream
* \param ostream used to convert let expression  to string
*/
void LetExpr::pretty_print( std::ostream  &ostream){

    std::streampos pos = 0;
    pretty_print_at(ostream, prec_none, false, pos);
}

/**
* \brief converts let expression to string using recursion to narrow down until var or num encountered
 * places parenthesis to adhere to association to the right and our precedence scale
* \param ostream used to convert let expression  to string
* \param precedence precedence of the caller, will be prec_none
* \param parentHasParen boolean signifying if parent caller is surrounded by parenthesis
* \param caller_pos streampos reference to position of caller
*/
void LetExpr::pretty_print_at(std::ostream & ostream, precedence_t precedence, bool parentHasParen, std::streampos & caller_pos ) {

    if(!parentHasParen && precedence != prec_none){
        ostream << "(";
    }

    std::streampos initial_pos = ostream.tellp();
    std::string kw_pos = std::string( initial_pos - caller_pos, ' ' );

    ostream << "_let "<< this->lhs<< " = ";
    this->rhs->pretty_print_at(ostream, prec_none, parentHasParen, caller_pos);
    ostream<< "\n";
    caller_pos = ostream.tellp();

    ostream << kw_pos << "_in  ";

    this->body->pretty_print_at(ostream, prec_none, parentHasParen, caller_pos);
    if(!parentHasParen && precedence != prec_none){
        ostream << ")";
    }
}


//********************** IFEXPR CLASS IMPLEMENTATIONS ***********************************

/**
* \brief constructor to make an if expression
* \param test_part conditional expression to determine which of the following expression to use
* \param then_part if test_part condition is true use this expression
* \param else_part if test_part condition is false use this expression
*/
IfExpr::IfExpr( PTR(Expr) test_part, PTR(Expr) then_part, PTR(Expr) else_part ){

    this->test_part = test_part;
    this->then_part = then_part;
    this->else_part = else_part;
}

/**
* \brief compares all fields of an if expression to 'this' using recursion
* \param comp expression to compare against this
* \return recursive call comparing each field of if expression. True if expressions are equal false if not.
*/
bool IfExpr::equals(PTR(Expr)comp){

    PTR(IfExpr) ifPtr = CAST(IfExpr)(comp);
    if ( ifPtr == nullptr ) {
        return false;
    }

    return ifPtr->test_part->equals(this->test_part) && ifPtr->then_part->equals(this->then_part) && ifPtr-> else_part->equals(this->else_part);
}

/**
* \brief gives result of expression determined by test_part conditional expression
* \param env check if environment is null, if so create an empty environment object
* \return Val object result of expression by recursive call interping expression until navigating to VarExpr or NumExpr
*/
PTR(Val) IfExpr::interp(PTR(Env) env) {

    if (test_part->interp(env)->is_true())
        return then_part->interp(env);

    else
    {
        return else_part->interp(env);
    }
}

/**
* \brief function returns expression parameter substituted in place of parameter valToSub
* \param valToSub variable value to substitute expression for
* \param expr expression to place in this expression valToSub
* \return this if valToSub is not value withing expression, if it is return new expression with expr in place of valToSub
*/
PTR(Expr) IfExpr::subst( std::string valToSub, PTR(Expr) expr ) {

    return NEW(IfExpr)(this->test_part->subst(valToSub, expr), this->then_part->subst(valToSub,expr), this->else_part->subst(valToSub, expr)) ;
}

/**
* \brief converts if expression to string using recursion to narrow down until var or num encountered
 * surrounds whole expression in parenthesis, no space between expressions and prefixes
* \param ostream used to convert if expression to string
*/
void IfExpr::print( std::ostream &ostream) {

    ostream << "(_if ";
    this->test_part->print(ostream);
    ostream << " _then ";
    this->then_part->print(ostream);
    ostream << " _else ";
    this->else_part->print(ostream);
    ostream << ")";
}

/**
* \brief driver function for recursion call that takes in ostream
* \param ostream used to convert if expression  to string
*/
void IfExpr::pretty_print( std::ostream  &ostream) {
    std::streampos pos = 0;
    pretty_print_at(ostream, prec_none, false, pos);
}

/**
* \brief converts let expression to string using recursion to narrow down until var or num encountered
 * places parenthesis to adhere to association to the right and our precedence scale
* \param ostream used to convert let expression  to string
* \param precedence precedence of the caller, will be prec_none
* \param parentHasParen boolean signifying if parent caller is surrounded by parenthesis
* \param caller_pos streampos reference to position of caller
*/
void IfExpr::pretty_print_at(std::ostream  &ostream, precedence_t precedence, bool parentHasParen, std::streampos &caller_pos) {

    if(precedence > prec_none) {
        ostream << "(";
    }

    std::streampos initial_pos = ostream.tellp();
    std::string kw_pos = std::string( initial_pos - caller_pos, ' ' );

    ostream << "_if   ";
    this->test_part->pretty_print_at(ostream, prec_none, parentHasParen, caller_pos);
    ostream << "\n";

    caller_pos = ostream.tellp();

    ostream << kw_pos << "_then ";
    this->then_part->pretty_print_at(ostream, prec_none, parentHasParen, caller_pos);
    ostream << "\n";

    caller_pos = ostream.tellp();

    ostream << kw_pos << "_else ";
    this->else_part->pretty_print_at(ostream, prec_none, parentHasParen, caller_pos);

    if(precedence > prec_none){
        ostream << ")";
    }
}

//********************** BOOLEXPR CLASS IMPLEMENTATIONS *********************************

/**
* \brief constructor to make an if expression
* \param boolean bool variable to determine type of bool expression
*/
BoolExpr::BoolExpr(bool boolean) {
    this->boolean = boolean;
}

/**
* \brief compares all fields of an if expression to 'this' using recursion
* \param comp expression to compare against this
* \return true if boolean values equate to same condition on both expression objects
*/
bool BoolExpr::equals(PTR(Expr)comp) {
    PTR(BoolExpr) boolPtr = CAST(BoolExpr)(comp);
    if ( boolPtr == nullptr ) {
        return false;
    }
    return boolPtr->boolean == this->boolean;

}

/**
* \brief gives back a BoolVal object with member bool
* \param env check if environment is null, if so create an empty environment object
* \return BoolVal object result of expression
*/
PTR(Val) BoolExpr::interp(PTR(Env) env) {
    return NEW(BoolVal)(this->boolean);
}

/**
* \brief function returns expression parameter substituted in place of parameter valToSub
* \param valToSub variable value to substitute expression for
* \param expr expression to place in this expression valToSub
* \return this if valToSub is not value withing expression, if it is return new expression with expr in place of valToSub
*/
PTR(Expr) BoolExpr::subst( std::string valToSub, PTR(Expr) expr ) {
    return THIS;
}

/**
* \brief converts bool expression to string, prints out '_true' or 'false' depending upon boolean condition
* \param ostream used to convert bool expression to string
*/
void BoolExpr::print( std::ostream &ostream) {

    if (boolean) {
        ostream<< "_true";
    }
    else {
        ostream<< "_false";
    }
}

/**
* \brief same as print function above
* \param ostream used to convert bool expression  to string
*/
void BoolExpr::pretty_print( std::ostream  &ostream) {


    if (boolean) {
        ostream<< "_true"; //turnary operator
    }
    else {
        ostream<< "_false";
    }
}

/**
* \brief same as print function above
* \param ostream unused
* \param precedence unused
* \param parentHasParen unused
* \param caller_pos unused
*/
void BoolExpr::pretty_print_at(std::ostream  &ostream, precedence_t precedence, bool parentHasParen, std::streampos &caller_pos) {

    if (boolean) {
        ostream<< "_true";
    }
    else {
        ostream<< "_false";
    }
}

//********************** EQEXPR CLASS IMPLEMENTATIONS ***********************************

/**
* \brief constructor to make an equality expression
* \param lhs left hand side of equality expression
* \param rhs right hand side of equality expression
*/
EqExpr::EqExpr( PTR(Expr) lhs, PTR(Expr) rhs ) {
    this->lhs = lhs;
    this->rhs = rhs;
}

/**
* \brief compares all fields of an equality expression to 'this' using recursion
* \param comp expression to compare against this
* \return recursive call comparing each field of equality expression. True if expressions are equal false if not.
*/
bool EqExpr::equals(PTR(Expr) comp) {

    PTR(EqExpr) eqPtr = CAST(EqExpr)(comp);
    if ( eqPtr == nullptr ) {
        return false;
    }
    return eqPtr->lhs->equals(this->lhs) && eqPtr->rhs->equals(this->rhs);
}

/**
* \brief gives result of expression determined by test_part conditional expression
* \param env check if environment is null, if so create an empty environment object
* \return Val object result of expression by recursive call interping expression until navigating to VarExpr or NumExpr
*/
PTR(Val) EqExpr::interp(PTR(Env) env) {
    return NEW(BoolVal)(this->lhs->interp(env)->equals(this->rhs->interp(env)));
}


/**
* \brief function returns expression parameter substituted in place of parameter valToSub
* \param valToSub variable value to substitute expression for
* \param expr expression to place in this expression valToSub
* \return this if valToSub is not value withing expression, if it is return new expression with expr in place of valToSub
*/
PTR(Expr) EqExpr::subst( std::string valToSub, PTR(Expr) expr ) {
    return NEW(EqExpr)(this->lhs->subst(valToSub, expr), this->rhs->subst(valToSub, expr));
}

/**
* \brief converts equality expression to string using recursion to narrow down until var or num encountered
 * surrounds whole expression in parenthesis, no space between expressions and prefixes
* \param ostream used to convert equality expression to string
*/
void EqExpr::print( std::ostream &ostream) {
    ostream << "(";
    this->lhs->print(ostream);
    ostream << "==";
    this->rhs->print(ostream);
    ostream << ")";

}

/**
* \brief driver function for recursion call that takes in ostream
* \param ostream used to convert equality expression  to string
*/
void EqExpr::pretty_print( std::ostream  &ostream) {

    std::streampos pos = 0;
    pretty_print_at(ostream, prec_none, false, pos);

}

/**
* \brief converts equality expression to string using recursion to narrow down until var or num encountered
 * places parenthesis to adhere to association to the right and our precedence scale
* \param ostream used to convert function expression  to string
* \param precedence precedence of the caller, will be prec_none
* \param parentHasParen boolean signifying if parent caller is surrounded by parenthesis
* \param caller_pos streampos reference to position of caller
*/
void EqExpr::pretty_print_at(std::ostream  &ostream, precedence_t precedence, bool parentHasParen, std::streampos &caller_pos) {

    if(precedence > prec_none){
        ostream << "(";
    }

    this->lhs->pretty_print_at(ostream, static_cast<precedence_t >(prec_none + 1), parentHasParen, caller_pos);
    ostream << " == ";
    this->rhs->pretty_print_at(ostream, prec_none, parentHasParen, caller_pos);

    if(precedence > prec_none){
        ostream << ")";
    }
}

//********************** FUNEXPR CLASS IMPLEMENTATIONS ***********************************

/**
* \brief constructor to make an Function expression
* \param formal_arg varible in body to be substituted
* \param body exprssion containing formal_arg
*/
FunExpr::FunExpr( std::string formal_arg, PTR(Expr) body ){
    this->formal_arg = std::move(formal_arg); //todo may need to change
    this->body = body;
}

/**
* \brief compares all fields of an function expression to 'this' using recursion
* \param comp expression to compare against this
* \return recursive call comparing each field of function expression. True if expressions are equal false if not.
*/
bool FunExpr::equals(PTR(Expr)comp){

    PTR(FunExpr) funPtr = CAST(FunExpr)(comp);
    if ( funPtr == nullptr ) {
        return false;
    }
    else {
        return funPtr->formal_arg == this->formal_arg  && funPtr->body->equals(this->body);
    }
}

/**
* \brief converts to a FunVal expresion with same fields including passed through environment
* \param env dictionary
* \return FunVal object
*/
PTR(Val) FunExpr::interp(PTR(Env) env){
    
    return NEW(FunVal)(formal_arg, body, env);
}


/**
* \brief function returns expression parameter substituted in place of parameter valToSub
* \param valToSub variable value to substitute expression for
* \param expr expression to place in this expression valToSub
* \return this if valToSub is not value withing expression, if it is return new expression with expr in place of valToSub
*/
PTR(Expr) FunExpr::subst( std::string valToSub, PTR(Expr) expr ){

    if ( valToSub == this->formal_arg) {
        return THIS;
    }
    return NEW(FunExpr)(this->formal_arg, this->body->subst(valToSub, expr));
}

/**
* \brief converts function expression to string using recursion to narrow down until var or num encountered
 * surrounds whole expression in parenthesis, no space between expressions and prefixes
* \param ostream used to convert function expression to string
*/
void FunExpr::print( std::ostream &ostream){

    ostream << "(_fun (" << this->formal_arg << ") ";
    this->body->print(ostream);
    ostream << ")";
}

/**
* \brief driver function for recursion call that takes in ostream
* \param ostream used to convert function expression  to string
*/
void FunExpr::pretty_print( std::ostream  &ostream){
    std::streampos initialPos = 0;
    pretty_print_at(ostream, prec_none, false, initialPos);
}

/**
* \brief converts function expression to string using recursion to narrow down until var or num encountered
 * places parenthesis to adhere to association to the right and our precedence scale
* \param ostream used to convert function expression  to string
* \param precedence precedence of the caller, will be prec_none
* \param parentHasParen boolean signifying if parent caller is surrounded by parenthesis
* \param caller_pos streampos reference to position of caller
*/
void FunExpr::pretty_print_at(std::ostream  &ostream, precedence_t precedence, bool parentHasParen, std::streampos &caller_pos){
    
    
    if ( precedence > prec_none ) {
        ostream << "(";
    }
    
    std::streampos initial_pos = ostream.tellp();
    std::string kw_pos = std::string( initial_pos - caller_pos, ' ' );
    
    
    ostream << "_fun (" << this->formal_arg << ")";
    ostream << "\n";
    
    caller_pos = ostream.tellp();
    ostream << kw_pos << "  ";
    
    this->body->pretty_print_at(ostream, prec_none, parentHasParen, caller_pos );

    if ( precedence > prec_none ) {
        ostream << ")";
    }
}

//********************** CALLEXPR CLASS IMPLEMENTATIONS ***********************************

/**
* \brief constructor to make a call expression
* \param to_be_called expression to containing value to be subbed will be a function expression
* \param actual_arg expression to substitute with in to_be_called expression
*/
CallExpr::CallExpr( PTR(Expr) to_be_called, PTR(Expr) actual_arg ) {

    this->to_be_called = to_be_called;
    this->actual_arg = actual_arg;
}

/**
* \brief compares all fields of an call expression to 'this' using recursion
* \param comp expression to compare against this
* \return recursive call comparing each field of call expression. True if expressions are equal false if not.
*/
bool CallExpr::equals(PTR(Expr) comp) {

    PTR(CallExpr) callPtr = CAST(CallExpr)(comp);
    if ( callPtr == nullptr ) {
        return false;
    }
    else {
        return callPtr->to_be_called->equals(this->to_be_called)  && callPtr->actual_arg->equals(this->actual_arg);
    }
}

/**
* \brief gives result of expression determined by test_part conditional expression
* \param env check if environment is null, if so create an empty environment object
* \return Val object result of to_be_called expression after substitution of actual_arg
*/
PTR(Val) CallExpr::interp(PTR(Env) env){

    return to_be_called->interp(env)->call(actual_arg->interp(env));
}


/**
* \brief function returns expression parameter substituted in place of parameter valToSub
* \param valToSub variable value to substitute expression for
* \param expr expression to place in this expression valToSub
* \return this if valToSub is not value withing expression, if it is return new expression with expr in place of valToSub
*/
PTR(Expr) CallExpr::subst( std::string valToSub, PTR(Expr) expr ){

    return NEW(CallExpr)(this->to_be_called->subst(valToSub,expr), this->actual_arg->subst(valToSub,expr)) ;
}

/**
* \brief converts call expression to string using recursion to narrow down until var or num encountered
 * surrounds whole expression in parenthesis, no space between expressions and prefixes
* \param ostream used to convert call expression to string
*/
void CallExpr::print( std::ostream &ostream){

  
    this->to_be_called->print(ostream);
    ostream<< " ";
    this->actual_arg->print(ostream);
}

/**
* \brief driver function for recursion call that takes in ostream
* \param ostream used to convert call expression  to string
*/
void CallExpr::pretty_print(std::ostream &ostream) {
    std::streampos initialPos = 0;
    pretty_print_at(ostream, prec_none, false, initialPos);

}

/**
* \brief converts call expression to string using recursion to narrow down until var or num encountered
 * places parenthesis to adhere to association to the right and our precedence scale
* \param ostream used to convert call expression  to string
* \param precedence precedence of the caller, will be prec_none
* \param parentHasParen boolean signifying if parent caller is surrounded by parenthesis
* \param caller_pos streampos reference to position of caller
*/
void CallExpr::pretty_print_at(std::ostream  &ostream, precedence_t precedence, bool parentHasParen, std::streampos &caller_pos){
    
       
    this->to_be_called->pretty_print_at(ostream, prec_none, parentHasParen, caller_pos);
    ostream<< "(";
    this->actual_arg->pretty_print_at(ostream, prec_none, parentHasParen, caller_pos);
    ostream<< ")";

}
