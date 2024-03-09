/**
* \file Expr.cpp
* \brief contains expression class  and child class implementations as well as testing
        Expression class where we decide the grammar and how the different child classes
        interact with each other. Child classes are Num, Add, Variable, Mult, let .
* \author Ben Baysinger
*/


#include "Expr.hpp"
#include "parse.hpp"



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
Num::Num( int val ) {
    this->val = val;
}

/**
* \brief compares number expression with this
* \param comp expression to compare against this
* \return true if param expression is equal to this
*/
bool Num::equals(Expr *comp) {

    Num* numPtr = dynamic_cast<Num*>(comp);
    if ( numPtr == nullptr ) {
        return false;
    }
    else {
        return numPtr->val == this->val;
    }
}

/**
* \brief returns integer value of number expression
* \return integer value of number expression
*/
int Num::interp() {
    return this->val;
}

/**
* \brief boolean that returns true if expression has variable
* \return recursive call comparing lhs and rhs. True if expressions are equal false if not.
*/
bool Num::has_variable() {
    return false;
}

/**
* \brief returns this expression
* \param valToSub unused
* \param expr unused
* \return this
*/
Expr* Num::subst(std::string valToSub, Expr *expr) {
    return this;
}

/**
* \brief converts num expression to string
* \param ostream used to convert integer to string
*/
void Num::print( std::ostream &ostream){
    ostream<<std::to_string(val);
}

/**
* \brief converts num expression to string
* \param ostream used to convert integer to string
*/
void Num::pretty_print( std::ostream  &ostream){
    ostream<<std::to_string(val);
}

/**
* \brief converts num expression to string
* \param ostream used to convert integer to string
* \param  precedence unused
* \param parentHasParen unused
* \param spaceCounter unused
*/
void Num::pretty_print_at(std::ostream  &ostream, precedence_t precedence, bool parentHasParen, std::streampos spaceCounter) {
    ostream<<std::to_string(val);
}


//**********************ADD CLASS IMPLEMENTATIONS **************************************

/**
* \brief constructor to make an add expression
* \param lhs expression of add expression
* \param rhs expression of add expression
*/
Add::Add(Expr *lhs, Expr *rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
}

/**
* \brief compares both sides of an add expression to this using recursion
* \param comp expression to compare against this
* \return recursive call comparing lhs and rhs. True if expressions are equal false if not.
*/
bool Add::equals ( Expr *comp ) {

    Add* addPtr = dynamic_cast<Add*>(comp);
    if ( addPtr == nullptr ) {
        return false;
    }
    else {
        return addPtr->lhs->equals(this->lhs) && addPtr->rhs->equals(this->rhs);
    }
}

/**
* \brief gives add operation result of add expression
* \return int result of add expression by recursive call comparing lhs and rhs to navigate down to a num expression
*/
int Add::interp() {
    return this->lhs->interp() + this->rhs->interp();
}

/**
* \brief bool returns true if add expression contains a variable
* \return recursive call to lhs and rhs. True if either side of expression contains a variable
*/
bool Add::has_variable() {
    return this->lhs->has_variable() || this->rhs->has_variable();
}

/**
* \brief function returns new version of this with an expression in place of targeted variable using recursion
* \param valToSub variable value to substitute expression for
* \param expr expression to place in this expression valToSub
* \return recursive call to lhs and rhs to navigate to position of valToSub
*/
Expr* Add::subst(std::string valToSub, Expr *expr) {

    return new Add(this->lhs->subst(valToSub,expr), this->rhs->subst(valToSub,expr)) ;
}

/**
* \brief converts both sides of an add expression to string using recursion to narrow down until var or num encountered
 * surrounds whole expression in parenthesis, no space between expressions and prefix
* \param ostream used to convert add expression to string
*/
void Add::print( std::ostream &ostream){

    ostream << "(";
    this->lhs->print(ostream);
    ostream<< "+";
    this->rhs->print(ostream);
    ostream<< ")";
}

/**
* \brief driver function for recursion call that takes in a enum precidence of caller
* \param ostream used to convert add expression  to string
*/
void Add::pretty_print( std::ostream  &ostream){
    std::streampos pos = ostream.tellp();

    pretty_print_at(ostream, prec_add, false, pos);
}

/**
* \brief converts both sides of an add expression to string using recursion to narrow down until var or num encountered
 * places parenthesis to adhere to association to the right and our precedence scale
* \param ostream used to convert add expression  to string
* \param precedence precedence of the caller, will be prec_add
* \param parentHasParen boolean signifying if parent caller is surrounded by parenthesis
* \param spaceCounter streampos tracking spacing preceding a _let variable printout
*/
void Add::pretty_print_at(std::ostream &ostream, precedence_t precedence, bool parentHasParen, std::streampos spaceCounter) {

    if(precedence > prec_add){
        ostream << "(";
    }
    this->lhs->pretty_print_at(ostream, static_cast<precedence_t>(prec_add + 1), parentHasParen, spaceCounter);
    ostream<< " + ";
    this->rhs->pretty_print_at(ostream, prec_none, parentHasParen, spaceCounter);

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
Mult::Mult( Expr *lhs, Expr *rhs ) {
    this->lhs = lhs;
    this->rhs = rhs;
}

/**
* \brief compares both sides of an multiplication expression to this using recursion
* \param comp expression to compare against this
* \return recursive call comparing lhs and rhs. True if expressions are equal false if not.
*/
bool Mult::equals(Expr *comp) {
    Mult* multPtr = dynamic_cast<Mult*>(comp);
    if ( multPtr == nullptr ) {
        return false;
    }
    else {
        return multPtr->lhs->equals(this->lhs) && multPtr->rhs->equals(this->rhs);
    }
}

/**
* \brief gives multiplication operation result of multiplication expression
* \return int result of multiplication expression by recursive call comparing lhs and rhs to navigate down to a num expression
*/
int Mult::interp() {
    return this->lhs->interp() * this->rhs->interp();
}

/**
* \brief bool returns true if multiplication expression contains a variable
* \return recursive call to lhs and rhs. True if either side of expression contains a variable
*/
bool Mult::has_variable() {
    return this->lhs->has_variable() || this->rhs->has_variable();
}

/**
* \brief function returns new version of this with an expression in place of targeted variable using recursion
* \param valToSub variable value to substitute expression for
* \param expr expression to place in this expression valToSub
* \return recursive call to lhs and rhs to navigate to position of valToSub
*/
Expr* Mult::subst(std::string valToSub, Expr *expr) {

    return new Mult(this->lhs->subst(valToSub,expr), this->rhs->subst(valToSub,expr)) ;
}

/**
* \brief converts both sides of an multiplication expression to string using recursion to narrow down until var or num encountered
 * surrounds whole expression in parenthesis, no space between expressions and prefix
* \param ostream used to convert multiplication expression to string
*/
void Mult::print( std::ostream &ostream){
    ostream << "(";
    this->lhs->print(ostream);
    ostream<< "*";
    this->rhs->print(ostream);
    ostream<< ")";
}

/**
* \brief driver function for recursion call that takes in a enum precedence of caller
* \param ostream used to convert add expression  to string
*/
void Mult::pretty_print( std::ostream  &ostream){
    std::streampos pos = ostream.tellp();

    pretty_print_at(ostream, prec_mult, false, pos);
}

/**
* \brief converts both sides of an multiplication expression to string using recursion to narrow down until var or num encountered
 * places parenthesis to adhere to association to the right and our precedence scale
* \param ostream used to convert multiplication expression  to string
* \param precedence precedence of the caller, will be prec_mult
* \param parentHasParen boolean signifying if parent caller is surrounded by parenthesis
* \param spaceCounter streampos tracking spacing preceding a _let variable printout
*/
void Mult::pretty_print_at(std::ostream &ostream, precedence_t precedence, bool parentHasParen, std::streampos spaceCounter) {


    if(precedence > prec_mult){
        parentHasParen = true;
        ostream << "(";
    }
    this->lhs->pretty_print_at(ostream, static_cast<precedence_t>(prec_mult + 1), parentHasParen, spaceCounter);
    ostream<< " * ";
    
    
    this->rhs->pretty_print_at(ostream, prec_mult, parentHasParen, spaceCounter);

    if(precedence > prec_mult){
        ostream << ")";
    }
}


//**********************VARIABLE CLASS IMPLEMENTATIONS *******************************

/**
* \brief constructor to make a variable expression
* \param value  string value to represented within variable expression
*/
Variable::Variable(std::string value) {
    this->value = value;
}

/**
* \brief compares variable's values for equality
* \param comp expression to compare against this.
* \return recursive call comparing lhs and rhs. True if variable's value are equal false if not.
*/
bool Variable::equals(Expr *comp) {
    Variable* varPtr = dynamic_cast<Variable*>(comp);
    if ( varPtr == nullptr ) {
        return false;
    }
    else {
        return varPtr->value == this->value;
    }
}

/**
* \brief throws error because variable expression do not have an integer result
* \return runtime error because variable has no value
*/
int Variable::interp() {
    throw std::runtime_error("Variable has no value");
}

/**
* \brief boolean that returns true if variable expression contains a variable
* \return true always because variable expression always contains variable
*/
bool Variable::has_variable() {
    return true;
}

/**
* \brief function returns expression parameter substituted in place of parameter valToSub
* \param valToSub variable value to substitute expression for
* \param expr expression to place in this expression valToSub
* \return this if valToSub is not value withing expression, if it is return new expression with expr in place of valToSub
*/
Expr* Variable::subst(std::string valToSub, Expr *expr) {

    if ( this->value == valToSub ){
        return expr;
    }
    return this;
}

/**
* \brief variable expression to string
* \param ostream used to send out value which is already a string
*/
void Variable::print( std::ostream &ostream){

    ostream<< this->value;

}

/**
* \brief variable expression to string
* \param ostream used to send out value which is already a string
*/
void Variable::pretty_print( std::ostream  &ostream){
    ostream<< this->value;
}

/**
* \brief variable expression to string
* \param ostream used to send out value which is already a string
* \param  precedence unused
* \param parentHasParen unused
* \param spaceCounter unused
*/
void Variable::pretty_print_at(std::ostream &ostream, precedence_t precedence, bool parentHasParen, std::streampos spaceCounter) {
    ostream<< this->value;
}

//**********************LET CLASS IMPLEMENTATIONS ***********************************

/**
* \brief constructor to make an let expression
* \param var string variable to be replaced in the body
* \param replacement expression to be swapped for var in the exprToSub
* \param exprToSub expression containing var to be substituted by replacement
*/
LetExpr::LetExpr(std::string var, Expr *replacement, Expr *exprToSub) {

    this->lhs = var;
    this->rhs = replacement;
    this->body = exprToSub;
}

/**
* \brief compares all fields of a let expression to this using recursion
* \param comp expression to compare against this
* \return recursive call comparing rhs and body. True if expressions are equal false if not.
*/
bool LetExpr::equals(Expr *comp) {
    LetExpr* letPtr = dynamic_cast<LetExpr*>(comp);
    if ( letPtr == nullptr ) {
        return false;
    }
    else {
        return letPtr->lhs == this->lhs  && letPtr->rhs->equals(this->rhs) && letPtr->body->equals(this->body);
    }

}

/**
* \brief gives let expression result of body expression after substitution of rhs
* \return int result of let expression by recursive call comparing rhs and body to navigate down to a num expression
*/
int LetExpr::interp() {

    Expr* newExpr = this->body->subst(this->lhs, this->rhs);
    return newExpr->interp();
}

/**
* \brief bool returns true if let expression contains a variable
* \return recursive call to rhs and body. True if either field of expression contains a variable
*/
bool LetExpr::has_variable() {
    return this->rhs->has_variable() || this->body->has_variable();
}

/**
* \brief function returns new version of this with an expression in place of targeted variable using recursion
* \param valToSub variable value to substitute expression for
* \param expr expression to place in this expression valToSub
* \return new expression from body after substitution.  If lhs equals valToSub recursion call only on rhs, else call on rhs and body
*/
Expr* LetExpr::subst(std::string valToSub, Expr *expr) {

    if ( valToSub == this->lhs ){
        return new LetExpr(this->lhs, this->rhs->subst(valToSub, expr), this->body);
    }
    return new LetExpr(this->lhs, this->rhs->subst(valToSub, expr), this->body->subst(valToSub, expr));
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
* \brief driver function for recursion call that takes in a enum precedence of caller
* \param ostream used to convert add expression  to string
*/
void LetExpr::pretty_print( std::ostream  &ostream){

    std::streampos pos = ostream.tellp();
    pretty_print_at(ostream, prec_none, false, pos);
}

/**
* \brief converts let expression to string using recursion to narrow down until var or num encountered
 * places parenthesis to adhere to association to the right and our precedence scale
* \param ostream used to convert let expression  to string
* \param precedence precedence of the caller, will be prec_none
* \param parentHasParen boolean signifying if parent caller is surrounded by parenthesis
* \param spaceCounter streampos tracking spacing preceding a _let variable printout
*/
void LetExpr::pretty_print_at(std::ostream &ostream, precedence_t precedence, bool parentHasParen, std::streampos spaceCounter ) {

    if(!parentHasParen && precedence != prec_none){
        ostream << "(";
    }

    std::streampos letPosition  = ostream.tellp();

    ostream << "_let "<< this->lhs<< " = ";
    this->rhs->pretty_print_at(ostream, prec_none, parentHasParen, spaceCounter);
    ostream<< "\n";
    std::streampos newLinePos = ostream.tellp();

    //Print spaces to match up _in with _let on new line
    for ( int i = 0; i < letPosition - spaceCounter; i++ ){
        ostream << ' ';
    }

    ostream << "_in  ";

    this->body->pretty_print_at(ostream, prec_none, parentHasParen, newLinePos);
    if(!parentHasParen && precedence != prec_none){
        ostream << ")";
    }
}


//**********************CATCH TESTING **************************************

TEST_CASE("Num Class")
{
    SECTION("Equals") {

        CHECK((new Num(1))->equals(new Variable("x")) == false);
        CHECK((new Num(1))->equals(new Num(2)) == false);
        //Zero check
        CHECK((new Num(0))->equals(new Num(0)) == true);
        //Negative check
        CHECK((new Num(-2))->equals(new Num(-2)) == true);
        //Max and min check
        CHECK((new Num(INT16_MAX))->equals(new Num(INT16_MAX)) == true);
        CHECK((new Num(INT16_MIN))->equals(new Num(INT16_MIN)) == true);
        //Repeats
        CHECK((new Num(777))->equals(new Num(777)) == true);
    }
    SECTION("Interp"){

        CHECK((new Num(4))->interp() == 4);
        CHECK((new Num(8))->interp() != 6);
        //Checking interp after a sub change
        CHECK((new Variable("y"))->subst("y", new Num(8))->interp() == 8);
        CHECK_THROWS_WITH((new Variable("x"))->subst("yx", new Num(3))->interp(),  "Variable has no value");
        CHECK_THROWS_WITH((new Add(new Num(2), new Variable("4")))->interp(), "Variable has no value");

    }
    SECTION("HasVariable") {

        CHECK((new Num(7))->has_variable() == false);
        CHECK((new Num(5))->has_variable() != true );

    }
    SECTION("Substitution") {

        CHECK((new Num(7))->subst("x", new Variable("y"))->equals(new Num(7)));
        //Sub change variable to num
        CHECK((new Variable("x"))->subst("x", new Num(8))->equals(new Num(8)));
    }
    SECTION("Print ") {
        CHECK( (new Num(10))->to_string() == "10" );
        CHECK( (new Num (-11))->to_string() == "-11");
        CHECK( (new Num (777))->to_string() == "777");
        CHECK( (new Num (0))->to_string() == "0");
        CHECK( (new Num (83))->to_string() != "72");
    }
    SECTION("Pretty Print") {
        CHECK( (new Num(10))->to_stringPP() == "10" );
        CHECK( (new Num (-11))->to_stringPP() == "-11");
        CHECK( (new Num (777))->to_stringPP() == "777");
        CHECK( (new Num (0))->to_stringPP() == "0");
        CHECK( (new Num (83))->to_stringPP() != "72");
    }
}

TEST_CASE("Add Class")
{
    SECTION("Equals") {

        CHECK((new Add(new Num(2), new Num(3)))->equals(new Add(new Num(2), new Num(3))) == true);
        CHECK((new Add(new Num(2), new Num(3)))->equals(new Add(new Num(3), new Num(2))) == false);
        //Add vs Multiplication check
        CHECK((new Add(new Num(2), new Num(3)))->equals(new Mult(new Num(2), new Num(3))) == false);
        //Add using Num and Var expression
        CHECK((new Add(new Num(2), new Num(3)))->equals(new Add(new Variable("2"), new Variable("3"))) == false);
        CHECK((new Add(new Variable("2"), new Variable("3")))->equals(new Add(new Variable("2"), new Variable("3"))) == true);
    }
    SECTION("Interp") {

        CHECK((new Add(new Add(new Num(10), new Num(15)), new Add(new Num(20), new Num(20))))->interp() == 65);
        //Exception thrown if variable contained
        CHECK_THROWS_WITH((new Add(new Num(2), new Variable("4")))->interp(), "Variable has no value");
        //Add with multiplication
        CHECK((new Add(new Mult(new Num(2), new Num(4)), new Add(new Num(5), new Num(10))))->interp() == 23);
        //Add negative numbers
        CHECK((new Add(new Add(new Num(-5), new Num(15)), new Add(new Num(-15), new Num(23))))->interp() == 18);
        //Add with substitute
        CHECK((new Add((new Add(new Variable("x"), new Num(15)))->subst("x", new Num(8)), new Add(new Num(13), new Num(30))))->interp() == 66);
    }
    SECTION("HasVariable") {

        CHECK((new Add(new Num(2), new Num(4)))->has_variable() == false);
        //Has variable then substitutes
        CHECK((new Add((new Add(new Variable("x"), new Num(15)))->subst("x", new Num(8)), new Add(new Num(13), new Num(30))))->has_variable() == false);
        //Has variable then substitutes another variable
        CHECK((new Add((new Add(new Variable("x"), new Num(15)))->subst("x", new Variable("y")), new Add(new Num(13), new Num(30))))->has_variable() == true);
        CHECK((new Add((new Add(new Variable("x"), new Num(15)))->subst("x", new Add(new Num(7), new Variable("y"))), new Add(new Num(13), new Num(30))))->has_variable() == true);
        //Has variable then substitutes it out
        CHECK((new Add((new Add(new Variable("x"), new Num(15)))->subst("x", new Add(new Num(7), new Num(8))), new Add(new Num(13), new Num(30))))->has_variable() == false);
    }
    SECTION("Substitution") {

        //Var with Num
        CHECK((new Add(new Variable("x"), new Num(7)))->subst("x", new Num(9))->equals(new Add(new Num(9), new Num(7))) );
        //Var with another var
        CHECK((new Add(new Variable("x"), new Num(7)))->subst("x", new Variable("y"))->equals(new Add(new Variable("y"), new Num(7))) );
        //Var with add expression
        CHECK((new Add((new Add(new Variable("x"), new Num(15)))->subst("x", new Add(new Num(2), new Num(3))),new Add(new Num(13), new Num(30))))
        ->equals(new Add(new Add(new Add(new Num(2), new Num(3)),new Num(15)),new Add(new Num(13), new Num(30)))));
        //Var with multiplication expression within add
        CHECK((new Add((new Mult(new Variable("x"), new Num(26)))->subst("x", new Add(new Num(7), new Num(8))),new Add(new Num(14), new Num(25))))
        ->equals(new Add(new Mult(new Add(new Num(7), new Num(8)),new Num(26)),new Add(new Num(14), new Num(25)))));
    }
    SECTION("Print") {
        CHECK( (new Add(new Num(1), new Add(new Num(2), new Num(3))))->to_string() == "(1+(2+3))" );
        //Add with multiplication on right
        CHECK( (new Add(new Num(2), new Mult(new Num(5), new Num(9))))->to_string() == "(2+(5*9))" );
        //Add with multiplication on left
        CHECK( (new Add(new Mult(new Num(3), new Num(9)), new Add(new Num(7), new Num(8))))->to_string() == "((3*9)+(7+8))" );
        //Add with two adds
        CHECK( (new Add(new Add(new Num(6), new Num(3)), new Add(new Num(1), new Num(4))))->to_string() == "((6+3)+(1+4))" );
        //Add with two multiplications
        CHECK( (new Add(new Mult(new Num(9), new Num(2)), new Mult(new Num(3), new Num(5))))->to_string() == "((9*2)+(3*5))" );
        //Add with var
        CHECK( (new Add(new Variable("x"), new Add(new Num(2), new Num(3))))->to_string() == "(x+(2+3))" );
        //Add with num and var
        CHECK( (new Add(new Num(1), new Variable("y")))->to_string() == "(1+y)" );
        //Add with var and multiplication
        CHECK( (new Add(new Variable("z"), new Mult(new Num(4), new Num(9))))->to_string() == "(z+(4*9))" );
    }
    SECTION("Pretty Print") {
        //Add with num on left and add expression on right
        CHECK( (new Add(new Num(1), new Add(new Num(2), new Num(3))))->to_stringPP() == "1 + 2 + 3" );
        //Add with num on right and add expression on left
        CHECK( (new Add( new Add(new Num(7), new Num(7)), new Num(6)))->to_stringPP() == "(7 + 7) + 6");
        //Add with add expression on left and multiplication on right. Association to the right example
        CHECK( (new Add( new Add(new Num(7), new Num(7)), new Mult(new Num(9), new Num(2))))->to_stringPP() == "(7 + 7) + 9 * 2");
        //Add with multiplication expression on left and add on right. Grouped by precedence example
        CHECK( (new Add( new Mult(new Num(3), new Num(5)), new Add(new Num(6), new Num(1))))->to_stringPP() == "3 * 5 + 6 + 1");
        //Add with var in left and add expression on right.
        CHECK( (new Add( new Variable("x"), new Add(new Num(4), new Num(4))))->to_stringPP() == "x + 4 + 4");
        //Add with add expression on left and var on right
        CHECK( (new Add( new Add(new Num(8), new Num(1)), new Variable("y")))->to_stringPP() == "(8 + 1) + y");
        //Association to right example
        CHECK( (new Add( new Num(1), new Mult(new Num(2), new Num(3))))->to_stringPP() == "1 + 2 * 3");
        //Lakshay test
        CHECK( (new Add(new Add(new Variable("x"), new Variable("y")), new Num(1)))->to_stringPP() == "(x + y) + 1" );
        CHECK( (new Add(new Add(new Num(-4), new Num(3)), new Add(new Num(2), new Num(-1))) )->to_stringPP() == "(-4 + 3) + 2 + -1" );
    }

}

TEST_CASE("Multiplication Class")
{
    SECTION("Equals") {

        CHECK((new Mult(new Num(2), new Num(3)))->equals(new Mult(new Num(2), new Num(3))) == true);
        CHECK((new Mult(new Num(2), new Num(3)))->equals(new Mult(new Num(3), new Num(2))) == false);
        //Multiplication expression vs Add check
        CHECK((new Mult(new Num(2), new Num(3)))->equals(new Add(new Num(2), new Num(3))) == false);
        //Multiplication expression using Num and Var expression
        CHECK((new Mult(new Num(2), new Num(3)))->equals(new Mult(new Variable("2"), new Variable("3"))) == false);
        CHECK((new Mult(new Variable("2"), new Variable("3")))->equals(new Mult(new Variable("2"), new Variable("3"))) == true);
    }
    SECTION("Interp") {

        CHECK((new Mult(new Num(3), new Num(2)))->interp() == 6);
        //Exception thrown if variable contained
        CHECK_THROWS_WITH((new Mult(new Num(2), new Variable("4")))->interp(), "Variable has no value");
        //Multiplication with add
        CHECK((new Mult(new Add(new Num(2), new Num(4)), new Mult(new Num(5), new Num(10))))->interp() == 300);
        //Multiplication negative numbers
        CHECK((new Mult(new Mult(new Num(-5), new Num(15)), new Add(new Num(-15), new Num(23))))->interp() == -600);
        //Multiplication with substitute
        CHECK((new Mult((new Add(new Variable("x"), new Num(15)))->subst("x", new Num(8)), new Add(new Num(13), new Num(30))))->interp() == 989);
    }
    SECTION("HasVariable") {

        CHECK((new Mult(new Num(2), new Num(4)))->has_variable() == false);
        //Has variable then substitutes
        CHECK((new Mult((new Add(new Variable("x"), new Num(15)))->subst("x", new Num(8)), new Add(new Num(13), new Num(30))))->has_variable() == false);
        //Has variable then substitutes another variable
        CHECK((new Mult((new Add(new Variable("x"), new Num(15)))->subst("x", new Variable("y")), new Add(new Num(13), new Num(30))))->has_variable() == true);
        CHECK((new Mult((new Add(new Variable("x"), new Num(15)))->subst("x", new Add(new Num(7), new Variable("y"))), new Add(new Num(13), new Num(30))))->has_variable() == true);
        //Has variable then substitutes it out
        CHECK((new Mult((new Add(new Variable("x"), new Num(15)))->subst("x", new Add(new Num(7), new Num(8))), new Add(new Num(13), new Num(30))))->has_variable() == false);
    }
    SECTION("Substitution") {

        //Var with Num
        CHECK((new Mult(new Variable("x"), new Num(7)))->subst("x", new Num(9))->equals(new Mult(new Num(9), new Num(7))) );
        //Var with another var
        CHECK((new Mult(new Variable("x"), new Num(7)))->subst("x", new Variable("y"))->equals(new Mult(new Variable("y"), new Num(7))) );
        //Var with add expression
        CHECK((new Mult((new Add(new Variable("x"), new Num(15)))->subst("x", new Add(new Num(2), new Num(3))),new Add(new Num(13), new Num(30))))
        ->equals(new Mult(new Add(new Add(new Num(2), new Num(3)),new Num(15)),new Add(new Num(13), new Num(30)))));
        //Var with add expression within multiplication
        CHECK((new Mult((new Add(new Variable("x"), new Num(26)))->subst("x", new Add(new Num(7), new Num(8))),new Add(new Num(14), new Num(25))))
        ->equals(new Mult(new Add(new Add(new Num(7), new Num(8)),new Num(26)),new Add(new Num(14), new Num(25)))));
    }
    SECTION("Print") {

        CHECK( (new Mult(new Num(1), new Mult(new Num(2), new Num(3))))->to_string() == "(1*(2*3))" );
        //Multiplication with add expression on right
        CHECK( (new Mult(new Num(2), new Add(new Num(5), new Num(9))))->to_string() == "(2*(5+9))" );
        //Multiplication with add expression on left
        CHECK( (new Mult(new Add(new Num(3), new Num(9)), new Mult(new Num(7), new Num(8))))->to_string() == "((3+9)*(7*8))" );
        //Multiplication with two multiplication expressions
        CHECK( (new Mult(new Mult(new Num(6), new Num(3)), new Mult(new Num(1), new Num(4))))->to_string() == "((6*3)*(1*4))" );
        //Multiplication with two add expressions
        CHECK( (new Mult(new Add(new Num(9), new Num(2)), new Add(new Num(3), new Num(5))))->to_string() == "((9+2)*(3+5))" );
        //Multiplication with var
        CHECK( (new Mult(new Variable("x"), new Mult(new Num(2), new Num(3))))->to_string() == "(x*(2*3))" );
        //Multiplication with num and var
        CHECK( (new Mult(new Num(1), new Variable("y")))->to_string() == "(1*y)" );
        //Multiplication with var and add expression
        CHECK( (new Mult(new Variable("z"), new Add(new Num(4), new Num(9))))->to_string() == "(z*(4+9))" );
    }
    SECTION("Pretty Print") {

        //Associating to right examples
        CHECK((new Mult (new Num(2), new Mult (new Num(3), new Num(4))))->to_stringPP() == "2 * 3 * 4");
        CHECK((new Mult (new Mult (new Num(2), new Num(3)), new Num(4)))->to_stringPP() == "(2 * 3) * 4");
        //Multiplication with multiplication expression on left and add expression on right.
        CHECK( (new Mult( new Mult(new Num(7), new Num(7)), new Add(new Num(9), new Num(2))))->to_stringPP() == "(7 * 7) * (9 + 2)");
        //Multiplication with add expression on left and multiplication on right. Grouped by precedence example
        CHECK( (new Mult( new Add(new Num(3), new Num(5)), new Mult(new Num(6), new Num(1))))->to_stringPP() == "(3 + 5) * 6 * 1");
        //Multiplication with var in left and multiplication expression on right.
        CHECK( (new Mult( new Variable("x"), new Mult(new Num(4), new Num(4))))->to_stringPP() == "x * 4 * 4");
        //Multiplication with multiplication expression on left and var on right. Association to the right example
        CHECK( (new Mult( new Mult(new Num(8), new Num(1)), new Variable("y")))->to_stringPP() == "(8 * 1) * y");
        //Multiplication with a num on left and an add expression on the right
        CHECK( (new Mult( new Num(1), new Add(new Num(2), new Num(3))))->to_stringPP() == "1 * (2 + 3)");
    }
}

TEST_CASE("Variable Class")
{
    SECTION("Equals") {

        CHECK((new Variable("x"))->equals(new Variable("x")) == true);
        CHECK((new Variable("x"))->equals(new Variable("y")) == false);
        //Empty string check
        CHECK((new Variable(""))->equals(new Variable("")) == true);
        //Palindrome
        CHECK((new Variable("racecar"))->equals(new Variable("racecar")) == true);
        CHECK((new Variable("Racecar"))->equals(new Variable("racecar")) == false);
        //Uppercase
        CHECK((new Variable("CLANG"))->equals(new Variable("CLANG")) == true);
        //Mixed case
        CHECK((new Variable("HeLlO"))->equals(new Variable("HeLlO")) == true);
        CHECK((new Variable("HeLlO"))->equals(new Variable("HeLLO")) == false);
        }
        SECTION("Interp") {

        CHECK_THROWS_WITH((new Variable("x"))->interp(), "Variable has no value");
        //Checking interp after a sub
        CHECK((new Variable("z"))->subst("z", new Num(56))->interp() == 56);
    }
    SECTION("HasVariable") {

        CHECK((new Variable("x"))->has_variable() == true);
        //Doesn't have within number
        CHECK((new Num(4))->has_variable() == false);
        //Has within add
        CHECK((new Add(new Num(5), new Variable("6")))->has_variable() == true);
        //Doesn't have withing add
        CHECK((new Add(new Num(5), new Num(6)))->has_variable() == false);
        //Has within multiplication
        CHECK((new Mult(new Num(25), new Variable("6")))->has_variable() == true);
        //Doesn't have within multiplication
        CHECK((new Mult(new Num(36), new Num(8)))->has_variable() == false);
    }
    SECTION("Substitution") {

        CHECK( (new Variable("x"))->subst("x", new Add(new Variable("y"),new Num(7)))->equals(new Add(new Variable("y"),new Num(7))) );
        //Sub within num
        CHECK((new Num(4))->subst("x", new Num(8))->equals(new Num(4)));
        //Sub within add
        CHECK((new Add(new Num(5), new Variable("6")))->subst("6", new Add(new Num(6), new Num(0)))
        ->equals(new Add(new Num(5), new Add(new Num(6), new Num(0)))));
        //Sub within multiplication
        CHECK((new Mult(new Num(4), new Variable("3")))->subst("3", new Mult(new Num(7), new Num(2)))
        ->equals(new Mult(new Num(4), new Mult(new Num(7), new Num(2)))));
    }
    SECTION("Print") {

        CHECK( (new Variable("x"))->to_string() == "x" );
        CHECK( (new Variable("-y"))->to_string() == "-y" );
        CHECK( (new Variable("777"))->to_string() == "777" );
        CHECK( (new Variable("HeLlO"))->to_string() == "HeLlO" );
        CHECK( (new Variable("RACECAR"))->to_string() == "RACECAR");
    }
    SECTION("Pretty Print") {

        CHECK( (new Variable("x"))->to_stringPP() == "x" );
        CHECK( (new Variable("-y"))->to_stringPP() == "-y" );
        CHECK( (new Variable("777"))->to_stringPP() == "777" );
        CHECK( (new Variable("HeLlO"))->to_stringPP() == "HeLlO" );
        CHECK( (new Variable("RACECAR"))->to_stringPP() == "RACECAR");
    }
}

TEST_CASE("LET CLASS")
{
    SECTION("Equals") {

        //True check
        CHECK((new LetExpr("x", new Num(5), new Add(new Variable("x"), new Num(6))))->equals
        (new LetExpr("x", new Num(5), new Add(new Variable("x"), new Num(6)))) == true);
        //False check
        CHECK((new LetExpr("x", new Num(5), new Add(new Variable("x"), new Num(6))))->equals
        (new LetExpr("x", new Num(5), new Add(new Variable("y"), new Num(6)))) == false);
    }

    SECTION("Interp") {

        //Add expression
        CHECK((new Mult(new Num(5), new LetExpr("x", new Num(5), new Add(new Variable("x"), new Num(1)))))->interp() == 30);
        //Multiplication expression
        CHECK((new Add( new Mult(new Num(5), new LetExpr("x", new Num(5), new Variable("x"))), new Num(1)))->interp() == 26);
        //Nested in right argument of multiplication expression
        CHECK ( (new Mult(new Mult(new Num (2), new LetExpr("x", new Num(5), new Add(new Variable("x") , new Num(1)) )), new Num(3)))->interp() == 36);
        //Variable unchanged exception check
        CHECK_THROWS_WITH ((new Add(new LetExpr("x", new Num(3), new LetExpr("y", new Num(3), new Add(new Variable("y"), new Num(2))) ), new Variable("x")))->interp(), "Variable has no value");
        //Let in lhs of add
        CHECK ( (new Add(new LetExpr("x", new Num(2), new Add(new Variable("x"), new Num(9))), new Num(4)))->interp() == 15);
    }

    SECTION("Has Variable ") {

        //Variable contained in rhs
        CHECK((new LetExpr("x", new Variable("x"), new Add(new Num(8), new Num(9))))->has_variable() == true);
        //Variable contained in body
        CHECK((new LetExpr("x", new Num(3), new Add(new Variable("x"), new Num(2))))->has_variable() == true);
        //Not contained in rhs
        CHECK((new LetExpr("y", new Num(6), new Add(new Variable("y"), new Num(4))))->has_variable() == true);
        //Not contained in body
        CHECK((new LetExpr("z", new Variable("z"), new Add(new Num(7), new Num(7))))->has_variable() == true);
        //Not contained except for lhs false check
        CHECK((new LetExpr("x", new Num(2), new Add(new Num(8), new Num(9))))->has_variable() == false);
    }

    SECTION("Substitution") {

        //Dont sub x because lhs = valToSub
        CHECK( (new LetExpr("x", new Num(5), new Add(new Variable("x"), new Num(5))))->subst("x", new Num(4))
        ->equals( new LetExpr("x", new Num(5), new Add(new Variable("x"), new Num(5)))));
        //Don't sub x because valToSub is not contained
        CHECK( (new LetExpr("x", new Num(5), new Add(new Variable("x"), new Num(5))))->subst("y", new Num(4))
        ->equals( new LetExpr("x", new Num(5), new Add(new Variable("x"), new Num(5)))));
        //Sub if lhs == valToSub in rhs
        CHECK( (new LetExpr("x", new Add(new Variable("x"), new Num(8)), new Add(new Variable("x"), new Num(3))))->subst("x", new Num(4))
        ->equals(new LetExpr("x", new Add(new Num(4), new Num(8)), new Add(new Variable("x"), new Num(3)))) );
        //Sub if lhs != valToSub when valToSub is contained in rhs
        CHECK( (new LetExpr("x", new Add(new Variable("y"), new Num(8)), new Add(new Variable("x"), new Num(3))))->subst("y", new Num(4))
        ->equals(new LetExpr("x", new Add(new Num(4), new Num(8)), new Add(new Variable("x"), new Num(3)))) );
        //Sub if lhs != valToSub when valToSub is contained in body
        CHECK( (new LetExpr("x", new Num(6), new Add(new Variable("x"), new Add(new Variable("y"), new Num(7)))))->subst("y", new Num(4))
        ->equals(new LetExpr("x", new Num(6), new Add(new Variable("x"), new Add(new Num(4), new Num(7))))) );

    }

    SECTION("Print") {

        //Single let
        CHECK((new LetExpr("x", new Num(5), new Add(new Variable("x"), new Num(7))))
        ->to_string() == "(_let x=5 _in (x+7))");
        //Let with multiplication expression
        CHECK((new LetExpr("x", new Num(9), new Mult(new Variable("x"), new Num(3))))
        ->to_string() == "(_let x=9 _in (x*3))");
        //Nested let
        CHECK((new LetExpr("x", new Num(5), new Add(new LetExpr("y", new Num(3), new Add(new Variable("y"), new Num(2))), new Variable("x"))))
        ->to_string() == "(_let x=5 _in ((_let y=3 _in (y+2))+x))");
        //Triple nested let
        CHECK((new LetExpr("x", new Num(5), new Add(new LetExpr("y", new Num(3), new LetExpr("y", new Num(2), new Add(new Variable("y"), new Num(5)))), new Variable("x"))))
        ->to_string() == "(_let x=5 _in ((_let y=3 _in (_let y=2 _in (y+5)))+x))");
        //Let nested with let in right hand side
        CHECK( (new LetExpr("x", new LetExpr("y", new Num(7), new Add(new Variable("y"), new Num(6))), new Add(new Variable("x"), new Num(3))))
        ->to_string() == "(_let x=(_let y=7 _in (y+6)) _in (x+3))");
        
        CHECK((new LetExpr("gfw", new Variable("iju"), new Add(new Num(-1811580510), new Variable("gfw"))))->to_string() == "(_let gfw=iju _in (-1811580510+gfw))");
    }
    SECTION("Pretty Print") {

        //Let nested as right argument of parenthesized multiplication expression
        CHECK ( (new Mult(new Mult(new Num (2), new LetExpr("x", new Num(5), new Add(new Variable("x") , new Num(1)) )), new Num(3)))->to_stringPP() == "(2 * _let x = 5\n"
        "     _in  x + 1) * 3");
        //Let nested to the left in add expression which is nested to the right within a multiplication expression
        CHECK((new Mult(new Num(5), new Add(new LetExpr("x", new Num(5), new Variable("x")), new Num(1))))->to_stringPP() == "5 * ((_let x = 5\n"
        "      _in  x) + 1)");
        //Let in lhs of add
        CHECK ( (new Add(new LetExpr("x", new Num(2), new Add(new Variable("x"), new Num(9))), new Num(4)))->to_stringPP() == "(_let x = 2\n"
        " _in  x + 9) + 4");
        //Let in lhs of multiplication expression
        CHECK((new Mult(new LetExpr("x", new Num(5), new Add(new Variable("x"), new Num(8))), new Num(3)))->to_stringPP() == "(_let x = 5\n"
        " _in  x + 8) * 3");
        //Let nest as right argument of un-parenthesized multiplication expression
        CHECK((new Add (new Mult(new Num(4), new LetExpr("x", new Num(5), new Add(new Variable("x"), new Num(1)))), new Num(9)))->to_stringPP() == "4 * (_let x = 5\n"
        "     _in  x + 1) + 9");
        //Let nested to the left within let that is nested to the left within add
        CHECK ((new Add(new LetExpr("x", new Num(3), new LetExpr("y", new Num(3), new Add(new Variable("y"), new Num(2))) ), new Variable("x")))->to_stringPP() == "(_let x = 3\n"
                                                                                                                                                                   " _in  _let y = 3\n"
                                                                                                                                                                   "      _in  y + 2) + x");
        //Let nested in lhs of Add expression nested within body of let expression
        CHECK((new LetExpr("x", new Num(5), new Add(new LetExpr("y" , new Num(3), new Add(new Variable("y"), new Num(2))), new Variable("x"))))
        ->to_stringPP() == "_let x = 5\n"
                           "_in  (_let y = 3\n"
                           "      _in  y + 2) + x");
        //Triple nested let
        CHECK( ( new LetExpr( "x", new Num(5),
                              new Add( new LetExpr( "y", new Num(3),
                                                    new Add( new Variable("y"), new LetExpr("z",new Num(6),
                                                                                            new Add(new Variable("a"), new Num(8))) ) ), new Variable("x") ) ) )
        ->to_stringPP()== "_let x = 5\n"
                          "_in  (_let y = 3\n"
                          "      _in  y + _let z = 6\n"
                          "               _in  a + 8) + x" );
    }
}

TEST_CASE("Parse") {
    
    SECTION("parse_str") {
        
        CHECK_THROWS_WITH( parse_str("()"), "invalid input" );
        CHECK( parse_str("(1)")->equals(new Num(1)) );
        CHECK( parse_str("(((1)))")->equals(new Num(1)) );
        CHECK_THROWS_WITH( parse_str("(1"), "missing close parenthesis" );
        CHECK( parse_str("1")->equals(new Num(1)) );
        CHECK( parse_str("10")->equals(new Num(10)) );
        CHECK( parse_str("-3")->equals(new Num(-3)) );
        CHECK( parse_str(" \n 5 ")->equals(new Num(5)) );
        CHECK_THROWS_WITH( parse_str("-"), "invalid input" );
        CHECK_THROWS_WITH( parse_str(" - 5 "), "invalid input" );
        CHECK( parse_str("x")->equals(new Variable("x")) );
        CHECK( parse_str("xyz")->equals(new Variable("xyz")) );
        CHECK( parse_str("xYz")->equals(new Variable("xYz")) );
        CHECK_THROWS_WITH( parse_str("x_z"), "invalid input" );
        CHECK_THROWS_WITH( parse_str("x-z"), "invalid input" );
        CHECK( parse_str("x + y")->equals(new Add(new Variable("x"), new Variable("y"))) );
        CHECK( parse_str("x * y")->equals(new Mult(new Variable("x"), new Variable("y"))) );
        CHECK( parse_str("z * x + y")
                       ->equals(new Add(new Mult(new Variable("z"), new Variable("x")),
                                        new Variable("y"))) );
        CHECK( parse_str("z * (x + y)")
                       ->equals(new Mult(new Variable("z"),
                                         new Add(new Variable("x"), new Variable("y"))) ));
        CHECK( parse_str("_let x = 5 _in x + 5")->equals(new LetExpr("x", new Num(5), new Add(new Variable("x"), new Num(5)))));
        CHECK( parse_str("_let x = 5 _in ((_let x = 5 _in x + 1) + x)")->equals(new LetExpr("x", new Num(5), new Add(new LetExpr("x", new Num(5), new Add(new Variable("x"), new Num(1))), new Variable("x")))));
        CHECK( parse_str("_let x = 5 _in ((_let x = 5 _in x * 1) * x)")->equals(new LetExpr("x", new Num(5), new Mult(new LetExpr("x", new Num(5), new Mult(new Variable("x"), new Num(1))), new Variable("x")))));
            }
}





