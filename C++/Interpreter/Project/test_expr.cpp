/**
* \file test_expr.cpp
* \brief contains catch2 testing of Expr, Val, Env, and parse classes
* \author Ben Baysinger
*/

#include "test_expr.hpp"
#include "catch.hpp"
#include "Expr.hpp"
#include "Val.hpp"
#include "parse.hpp"
#include "Env.hpp"



//**********************CATCH TESTING **************************************

TEST_CASE("NumExpr Class")
{
    SECTION("NumExpr::Equals") 
    {
        CHECK( (NEW(NumExpr)(1))->equals(NEW(VarExpr)("x")) == false );
        CHECK( (NEW(NumExpr)(1))->equals(NEW(NumExpr)(2)) == false );
        //Zero check
        CHECK( (NEW(NumExpr)(0))->equals(NEW(NumExpr)(0)) == true );
        //Negative check
        CHECK( (NEW(NumExpr)(-2))->equals(NEW(NumExpr)(-2)) == true );
        //Max and min check
        CHECK( (NEW(NumExpr)(INT16_MAX))->equals(NEW(NumExpr)(INT16_MAX)) == true );
        CHECK( (NEW(NumExpr)(INT16_MIN))->equals(NEW(NumExpr)(INT16_MIN)) == true );
        //Repeats
        CHECK( (NEW(NumExpr)(777))->equals(NEW(NumExpr)(777)) == true );
    }
    
    SECTION("NumExpr::Interp")
    {
        CHECK( (NEW(NumExpr)(4))->interp()->equals(NEW(NumVal)(4)) );
        CHECK_FALSE( (NEW(NumExpr)(8))->interp()->equals(NEW(NumVal)(6)) );
        //Checking interp after a sub change
        CHECK( (NEW(VarExpr)("y"))->subst("y", NEW(NumExpr)(8))->interp()->equals(NEW(NumVal)(8)) );
        CHECK_THROWS_WITH( (NEW(VarExpr)("x"))->subst("yx", NEW(NumExpr)(3))
                          ->interp(), "free variable: x" );
        CHECK_THROWS_WITH( (NEW(AddExpr)(NEW(NumExpr)(2), NEW(VarExpr)("4")))
                          ->interp(), "free variable: 4" );

    }
    
    SECTION("NumExpr::Substitution") 
    {
        CHECK( (NEW(NumExpr)(7))->subst("x", NEW(VarExpr)("y"))->equals(NEW(NumExpr)(7)) );
        //Sub change variable to num
        CHECK( (NEW(VarExpr)("x"))->subst("x", NEW(NumExpr)(8))->equals(NEW(NumExpr)(8)) );
    }
    
    SECTION("NumExpr::Print ") 
    {
        CHECK( (NEW(NumExpr)(10))->to_string() == "10"  );
        CHECK( (NEW(NumExpr)(-11))->to_string() == "-11" );
        CHECK( (NEW(NumExpr)(777))->to_string() == "777" );
        CHECK( (NEW(NumExpr)(0))->to_string() == "0" );
        CHECK( (NEW(NumExpr)(83))->to_string() != "72" );
    }
    
    SECTION("NumExpr::Pretty Print") 
    {
        CHECK( (NEW(NumExpr)(10))->to_stringPP() == "10"  );
        CHECK( (NEW(NumExpr)(-11))->to_stringPP() == "-11" );
        CHECK( (NEW(NumExpr)(777))->to_stringPP() == "777" );
        CHECK( (NEW(NumExpr)(0))->to_stringPP() == "0");
        CHECK( (NEW(NumExpr)(83))->to_stringPP() != "72" );
    }
}

TEST_CASE("AddExpr Class")
{
    SECTION("AddExpr::Equals") 
    {
        CHECK( (NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))->equals(NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3))) == true );
        CHECK( (NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))->equals(NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(2))) == false );
        //Add vs Multiplication check
        CHECK( (NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))->equals(NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3))) == false );
        //Add using Num and Var expression
        CHECK( (NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))->equals(NEW(AddExpr)(NEW(VarExpr)("2"), NEW(VarExpr)("3"))) == false );
        CHECK( (NEW(AddExpr)(NEW(VarExpr)("2"), NEW(VarExpr)("3")))->equals(NEW(AddExpr)(NEW(VarExpr)("2"), NEW(VarExpr)("3"))) == true );
    }
    
    SECTION("AddExpr::Interp") 
    {
        CHECK( (NEW(AddExpr)(NEW(AddExpr)(NEW(NumExpr)(10), NEW(NumExpr)(15)), NEW(AddExpr)(NEW(NumExpr)(20), NEW(NumExpr)(20))))->interp()
              ->equals(NEW(NumVal)(65)) );
        //Exception thrown if variable contained
        CHECK_THROWS_WITH( (NEW(AddExpr)(NEW(NumExpr)(2), NEW(VarExpr)("4")))
                          ->interp(), "free variable: 4");
        //AddExpr with multiplication
        CHECK( (NEW(AddExpr)(NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(4)), NEW(AddExpr)(NEW(NumExpr)(5), NEW(NumExpr)(10))))->interp()
              ->equals( NEW(NumVal)(23)) );
        //AddExpr negative numbers
        CHECK( (NEW(AddExpr)(NEW(AddExpr)(NEW(NumExpr)(-5), NEW(NumExpr)(15)), NEW(AddExpr)(NEW(NumExpr)(-15), NEW(NumExpr)(23))))->interp()->equals(NEW(NumVal)(18)) );
        //AddExpr with substitute
        CHECK( (NEW(AddExpr)((NEW(AddExpr)( NEW(VarExpr)("x"), NEW(NumExpr)(15)))->subst("x", NEW(NumExpr)(8)), NEW(AddExpr)(NEW(NumExpr)(13), NEW(NumExpr)(30))))->interp()->equals(NEW(NumVal)(66)) );
    }
    
    
    SECTION("AddExpr::Substitution") 
    {
        //Var with Num
        CHECK( (NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(7)))->subst("x", NEW(NumExpr)(9))
              ->equals(NEW(AddExpr)(NEW(NumExpr)(9), NEW(NumExpr)(7))) );
        //Var with another var
        CHECK( (NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(7)))->subst("x", NEW(VarExpr)("y"))
              ->equals(NEW(AddExpr)(NEW(VarExpr)("y"), NEW(NumExpr)(7))) );
        //Var with add expression
        CHECK( (NEW(AddExpr)((NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(15)))->subst("x", NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3))),NEW(AddExpr)(NEW(NumExpr)(13), NEW(NumExpr)(30))))
        ->equals(NEW(AddExpr)(NEW(AddExpr)(NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)),NEW(NumExpr)(15)),NEW(AddExpr)(NEW(NumExpr)(13), NEW(NumExpr)(30)))) );
        //Var with multiplication expression within add
        CHECK((NEW(AddExpr)((NEW(MultExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(26)))->subst("x", NEW(AddExpr)(NEW(NumExpr)(7), NEW(NumExpr)(8))),NEW(AddExpr)(NEW(NumExpr)(14), NEW(NumExpr)(25))))
        ->equals(NEW(AddExpr)(NEW(MultExpr)(NEW(AddExpr)(NEW(NumExpr)(7), NEW(NumExpr)(8)),NEW(NumExpr)(26)),NEW(AddExpr)(NEW(NumExpr)(14), NEW(NumExpr)(25)))) );
    }
    
    SECTION("AddExpr::Print") 
    {
        CHECK( (NEW(AddExpr) (NEW(NumExpr) (1), NEW(AddExpr) (NEW(NumExpr) (2), NEW(NumExpr) (3))))->to_string() == "(1+(2+3))" );
        //Add with multiplication on right
        CHECK( (NEW(AddExpr) (NEW(NumExpr) (2), NEW(MultExpr) (NEW(NumExpr) (5), NEW(NumExpr) (9))))->to_string() == "(2+(5*9))" );
        //Add with multiplication on left
        CHECK( (NEW(AddExpr) (NEW(MultExpr) (NEW(NumExpr) (3), NEW(NumExpr) (9)), NEW(AddExpr) (NEW(NumExpr) (7), NEW(NumExpr) (8))))->to_string() == "((3*9)+(7+8))" );
        //Add with two adds
        CHECK( (NEW(AddExpr) (NEW(AddExpr) (NEW(NumExpr) (6), NEW(NumExpr) (3)), NEW(AddExpr) (NEW(NumExpr) (1), NEW(NumExpr) (4))))->to_string() == "((6+3)+(1+4))" );
        //Add with two multiplications
        CHECK( (NEW(AddExpr) (NEW(MultExpr) (NEW(NumExpr) (9), NEW(NumExpr) (2)), NEW(MultExpr) (NEW(NumExpr) (3), NEW(NumExpr) (5))))->to_string() == "((9*2)+(3*5))" );
        //Add with var
        CHECK( (NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(AddExpr) (NEW(NumExpr) (2), NEW(NumExpr) (3))))->to_string() == "(x+(2+3))" );
        //Add with num and var
        CHECK( (NEW(AddExpr) (NEW(NumExpr) (1), NEW(VarExpr) ("y")))->to_string() == "(1+y)" );
        //Add with var and multiplication
        CHECK( (NEW(AddExpr) (NEW(VarExpr) ("z"), NEW(MultExpr) (NEW(NumExpr) (4), NEW(NumExpr) (9))))->to_string() == "(z+(4*9))" );
    }
    
    SECTION("AddExpr::Pretty Print") 
    {
        //Add with num on left and add expression on right
        CHECK( (NEW(AddExpr) (NEW(NumExpr) (1), NEW(AddExpr) (NEW(NumExpr) (2), NEW(NumExpr) (3))))->to_stringPP() == "1 + 2 + 3" );
        //Add with num on right and add expression on left
        CHECK( (NEW(AddExpr) ( NEW(AddExpr) (NEW(NumExpr) (7), NEW(NumExpr) (7)), NEW(NumExpr) (6)))->to_stringPP() == "(7 + 7) + 6");
        //Add with add expression on left and multiplication on right. Association to the right example
        CHECK( (NEW(AddExpr) ( NEW(AddExpr) (NEW(NumExpr) (7), NEW(NumExpr) (7)), NEW(MultExpr) (NEW(NumExpr) (9), NEW(NumExpr) (2))))->to_stringPP() == "(7 + 7) + 9 * 2");
        //Add with multiplication expression on left and add on right. Grouped by precedence example
        CHECK( (NEW(AddExpr) ( NEW(MultExpr) (NEW(NumExpr) (3), NEW(NumExpr) (5)), NEW(AddExpr) (NEW(NumExpr) (6), NEW(NumExpr) (1))))->to_stringPP() == "3 * 5 + 6 + 1");
        //Add with var in left and add expression on right.
        CHECK( (NEW(AddExpr) ( NEW(VarExpr) ("x"), NEW(AddExpr) (NEW(NumExpr) (4), NEW(NumExpr) (4))))->to_stringPP() == "x + 4 + 4");
        //Add with add expression on left and var on right
        CHECK( (NEW(AddExpr) ( NEW(AddExpr) (NEW(NumExpr) (8), NEW(NumExpr) (1)), NEW(VarExpr) ("y")))->to_stringPP() == "(8 + 1) + y");
        //Association to right example
        CHECK( (NEW(AddExpr) ( NEW(NumExpr) (1), NEW(MultExpr) (NEW(NumExpr) (2), NEW(NumExpr) (3))))->to_stringPP() == "1 + 2 * 3");
        //Lakshay test
        CHECK( (NEW(AddExpr) (NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(VarExpr) ("y")), NEW(NumExpr) (1)))->to_stringPP() == "(x + y) + 1" );
        CHECK( (NEW(AddExpr) (NEW(AddExpr) (NEW(NumExpr) (-4), NEW(NumExpr) (3)), NEW(AddExpr) (NEW(NumExpr) (2), NEW(NumExpr) (-1))) )->to_stringPP() == "(-4 + 3) + 2 + -1" );
    }

}

TEST_CASE("MultExpr Class")
{
    SECTION("MultExpr::Equals")
    {
        CHECK((NEW(MultExpr) (NEW(NumExpr) (2), NEW(NumExpr) (3)))->equals(NEW(MultExpr) (NEW(NumExpr) (2), NEW(NumExpr) (3))) == true);
        CHECK((NEW(MultExpr) (NEW(NumExpr) (2), NEW(NumExpr) (3)))->equals(NEW(MultExpr) (NEW(NumExpr) (3), NEW(NumExpr) (2))) == false);
        //Multiplication expression vs Add check
        CHECK((NEW(MultExpr) (NEW(NumExpr) (2), NEW(NumExpr) (3)))->equals(NEW(AddExpr) (NEW(NumExpr) (2), NEW(NumExpr) (3))) == false);
        //Multiplication expression using Num and Var expression
        CHECK((NEW(MultExpr) (NEW(NumExpr) (2), NEW(NumExpr) (3)))->equals(NEW(MultExpr) (NEW(VarExpr) ("2"), NEW(VarExpr) ("3"))) == false);
        CHECK((NEW(MultExpr) (NEW(VarExpr) ("2"), NEW(VarExpr) ("3")))->equals(NEW(MultExpr) (NEW(VarExpr) ("2"), NEW(VarExpr) ("3"))) == true);
    }
    
    SECTION("MultExpr::Interp") 
    {
        CHECK((NEW(MultExpr) (NEW(NumExpr) (3), NEW(NumExpr) (2)))->interp()->equals(NEW(NumVal) (6)));
        //Exception thrown if variable contained
        CHECK_THROWS_WITH((NEW(MultExpr) (NEW(NumExpr) (2), NEW(VarExpr) ("4")))->interp(), "free variable: 4");
        //Multiplication with add
        CHECK((NEW(MultExpr) (NEW(AddExpr) (NEW(NumExpr) (2), NEW(NumExpr) (4)), NEW(MultExpr) (NEW(NumExpr) (5), NEW(NumExpr) (10))))->interp()->equals(NEW(NumVal) (300)));
        //Multiplication negative numbers
        CHECK((NEW(MultExpr) (NEW(MultExpr) (NEW(NumExpr) (-5), NEW(NumExpr) (15)), NEW(AddExpr) (NEW(NumExpr) (-15), NEW(NumExpr) (23))))->interp()->equals(NEW(NumVal) (-600)));
        //Multiplication with substitute
        CHECK((NEW(MultExpr) ((NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (15)))->subst("x", NEW(NumExpr) (8)), NEW(AddExpr) (NEW(NumExpr) (13), NEW(NumExpr) (30))))->interp()->equals(NEW(NumVal) (989)));
    }
    
    SECTION("MultExpr::Substitution")
    {
        //Var with Num
        CHECK((NEW(MultExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (7)))->subst("x", NEW(NumExpr) (9))->equals(NEW(MultExpr) (NEW(NumExpr) (9), NEW(NumExpr) (7))) );
        //Var with another var
        CHECK((NEW(MultExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (7)))->subst("x", NEW(VarExpr) ("y"))->equals(NEW(MultExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (7))) );
        //Var with add expression
        CHECK((NEW(MultExpr) ((NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (15)))->subst("x", NEW(AddExpr) (NEW(NumExpr) (2), NEW(NumExpr) (3))),NEW(AddExpr) (NEW(NumExpr) (13), NEW(NumExpr) (30))))
        ->equals(NEW(MultExpr) (NEW(AddExpr) (NEW(AddExpr) (NEW(NumExpr) (2), NEW(NumExpr) (3)),NEW(NumExpr) (15)),NEW(AddExpr) (NEW(NumExpr) (13), NEW(NumExpr) (30)))));
        //Var with add expression within multiplication
        CHECK((NEW(MultExpr) ((NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (26)))->subst("x", NEW(AddExpr) (NEW(NumExpr) (7), NEW(NumExpr) (8))),NEW(AddExpr) (NEW(NumExpr) (14), NEW(NumExpr) (25))))
    ->equals(NEW(MultExpr) (NEW(AddExpr) (NEW(AddExpr) (NEW(NumExpr) (7), NEW(NumExpr) (8)),NEW(NumExpr) (26)),NEW(AddExpr) (NEW(NumExpr) (14), NEW(NumExpr) (25)))));
    }
    
    SECTION("MultExpr::Print") 
    {
        CHECK( (NEW(MultExpr) (NEW(NumExpr) (1), NEW(MultExpr) (NEW(NumExpr) (2), NEW(NumExpr) (3))))->to_string() == "(1*(2*3))" );
        //Multiplication with add expression on right
        CHECK( (NEW(MultExpr) (NEW(NumExpr) (2), NEW(AddExpr) (NEW(NumExpr) (5), NEW(NumExpr) (9))))->to_string() == "(2*(5+9))" );
        //Multiplication with add expression on left
        CHECK( (NEW(MultExpr) (NEW(AddExpr) (NEW(NumExpr) (3), NEW(NumExpr) (9)), NEW(MultExpr) (NEW(NumExpr) (7), NEW(NumExpr) (8))))->to_string() == "((3+9)*(7*8))" );
        //Multiplication with two multiplication expressions
        CHECK( (NEW(MultExpr) (NEW(MultExpr) (NEW(NumExpr) (6), NEW(NumExpr) (3)), NEW(MultExpr) (NEW(NumExpr) (1), NEW(NumExpr) (4))))->to_string() == "((6*3)*(1*4))" );
        //Multiplication with two add expressions
        CHECK( (NEW(MultExpr) (NEW(AddExpr) (NEW(NumExpr) (9), NEW(NumExpr) (2)), NEW(AddExpr) (NEW(NumExpr) (3), NEW(NumExpr) (5))))->to_string() == "((9+2)*(3+5))" );
        //Multiplication with var
        CHECK( (NEW(MultExpr) (NEW(VarExpr) ("x"), NEW(MultExpr) (NEW(NumExpr) (2), NEW(NumExpr) (3))))->to_string() == "(x*(2*3))" );
        //Multiplication with num and var
        CHECK( (NEW(MultExpr) (NEW(NumExpr) (1), NEW(VarExpr) ("y")))->to_string() == "(1*y)" );
        //Multiplication with var and add expression
        CHECK( (NEW(MultExpr) (NEW(VarExpr) ("z"), NEW(AddExpr) (NEW(NumExpr) (4), NEW(NumExpr) (9))))->to_string() == "(z*(4+9))" );
    }
    
    SECTION("MultExpr::Pretty Print") 
    {
        //Associating to right examples
        CHECK((NEW(MultExpr)  (NEW(NumExpr) (2), NEW(MultExpr)  (NEW(NumExpr) (3), NEW(NumExpr) (4))))->to_stringPP() == "2 * 3 * 4");
        CHECK((NEW(MultExpr)  (NEW(MultExpr)  (NEW(NumExpr) (2), NEW(NumExpr) (3)), NEW(NumExpr) (4)))->to_stringPP() == "(2 * 3) * 4");
        //Multiplication with multiplication expression on left and add expression on right.
        CHECK( (NEW(MultExpr) ( NEW(MultExpr) (NEW(NumExpr) (7), NEW(NumExpr) (7)), NEW(AddExpr) (NEW(NumExpr) (9), NEW(NumExpr) (2))))->to_stringPP() == "(7 * 7) * (9 + 2)");
        //Multiplication with add expression on left and multiplication on right. Grouped by precedence example
        CHECK( (NEW(MultExpr) ( NEW(AddExpr) (NEW(NumExpr) (3), NEW(NumExpr) (5)), NEW(MultExpr) (NEW(NumExpr) (6), NEW(NumExpr) (1))))->to_stringPP() == "(3 + 5) * 6 * 1");
        //Multiplication with var in left and multiplication expression on right.
        CHECK( (NEW(MultExpr) ( NEW(VarExpr) ("x"), NEW(MultExpr) (NEW(NumExpr) (4), NEW(NumExpr) (4))))->to_stringPP() == "x * 4 * 4");
        //Multiplication with multiplication expression on left and var on right. Association to the right example
        CHECK( (NEW(MultExpr) ( NEW(MultExpr) (NEW(NumExpr) (8), NEW(NumExpr) (1)), NEW(VarExpr) ("y")))->to_stringPP() == "(8 * 1) * y");
        //Multiplication with a num on left and an add expression on the right
        CHECK( (NEW(MultExpr) ( NEW(NumExpr) (1), NEW(AddExpr) (NEW(NumExpr) (2), NEW(NumExpr) (3))))->to_stringPP() == "1 * (2 + 3)");
    }
}

TEST_CASE("VarExpr Class")
{
    SECTION("VarExpr::Equals") 
    {
        CHECK((NEW(VarExpr) ("x"))->equals(NEW(VarExpr) ("x")) == true);
        CHECK((NEW(VarExpr) ("x"))->equals(NEW(VarExpr) ("y")) == false);
        //Empty string check
        CHECK((NEW(VarExpr) (""))->equals(NEW(VarExpr) ("")) == true);
        //Palindrome
        CHECK((NEW(VarExpr) ("racecar"))->equals(NEW(VarExpr) ("racecar")) == true);
        CHECK((NEW(VarExpr) ("Racecar"))->equals(NEW(VarExpr) ("racecar")) == false);
        //Uppercase
        CHECK((NEW(VarExpr) ("CLANG"))->equals(NEW(VarExpr) ("CLANG")) == true);
        //Mixed case
        CHECK((NEW(VarExpr) ("HeLlO"))->equals(NEW(VarExpr) ("HeLlO")) == true);
        CHECK((NEW(VarExpr) ("HeLlO"))->equals(NEW(VarExpr) ("HeLLO")) == false);
    }
    
    SECTION("VarExpr::Interp") 
    {
        CHECK_THROWS_WITH((NEW(VarExpr) ("x"))->interp(), "free variable: x");
        //Checking interp after a sub
        CHECK((NEW(VarExpr) ("z"))->subst("z", NEW(NumExpr) (56))->interp()->equals(NEW(NumVal) (56)));
    }
    
    
    SECTION("VarExpr::Substitution") 
    {
        CHECK( (NEW(VarExpr) ("x"))->subst("x", NEW(AddExpr) (NEW(VarExpr) ("y"),NEW(NumExpr) (7)))->equals(NEW(AddExpr) (NEW(VarExpr) ("y"),NEW(NumExpr) (7))) );
        //Sub within num
        CHECK((NEW(NumExpr) (4))->subst("x", NEW(NumExpr) (8))->equals(NEW(NumExpr) (4)));
        //Sub within add
        CHECK((NEW(AddExpr) (NEW(NumExpr) (5), NEW(VarExpr) ("6")))->subst("6", NEW(AddExpr) (NEW(NumExpr) (6), NEW(NumExpr) (0)))
        ->equals(NEW(AddExpr) (NEW(NumExpr) (5), NEW(AddExpr) (NEW(NumExpr) (6), NEW(NumExpr) (0)))));
        //Sub within multiplication
        CHECK((NEW(MultExpr) (NEW(NumExpr) (4), NEW(VarExpr) ("3")))->subst("3", NEW(MultExpr) (NEW(NumExpr) (7), NEW(NumExpr) (2)))
        ->equals(NEW(MultExpr) (NEW(NumExpr) (4), NEW(MultExpr) (NEW(NumExpr) (7), NEW(NumExpr) (2)))));
    }
    
    SECTION("VarExpr::Print")
    {
        CHECK( (NEW(VarExpr) ("x"))->to_string() == "x" );
        CHECK( (NEW(VarExpr) ("-y"))->to_string() == "-y" );
        CHECK( (NEW(VarExpr) ("777"))->to_string() == "777" );
        CHECK( (NEW(VarExpr) ("HeLlO"))->to_string() == "HeLlO" );
        CHECK( (NEW(VarExpr) ("RACECAR"))->to_string() == "RACECAR");
    }
    
    SECTION("VarExpr::Pretty Print") 
    {
        CHECK( (NEW(VarExpr) ("x"))->to_stringPP() == "x" );
        CHECK( (NEW(VarExpr) ("-y"))->to_stringPP() == "-y" );
        CHECK( (NEW(VarExpr) ("777"))->to_stringPP() == "777" );
        CHECK( (NEW(VarExpr) ("HeLlO"))->to_stringPP() == "HeLlO" );
        CHECK( (NEW(VarExpr) ("RACECAR"))->to_stringPP() == "RACECAR");
    }
}

TEST_CASE("LetExpr Class")
{
    SECTION("LetExpr::Equals") 
    {
        //True check
        CHECK((NEW(LetExpr) ("x", NEW(NumExpr) (5), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (6))))->equals
        (NEW(LetExpr) ("x", NEW(NumExpr) (5), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (6)))) == true);
        //False check
        CHECK((NEW(LetExpr) ("x", NEW(NumExpr) (5), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (6))))->equals
        (NEW(LetExpr) ("x", NEW(NumExpr) (5), NEW(AddExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (6)))) == false);
    }

    SECTION("LetExpr::Interp") 
    {
        //AddExpr expression
        CHECK((NEW(MultExpr) (NEW(NumExpr) (5), NEW(LetExpr) ("x", NEW(NumExpr) (5), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (1)))))->interp()->equals(NEW(NumVal) (30)));
        //Multiplication expression
        CHECK((NEW(AddExpr) (NEW(MultExpr) (NEW(NumExpr) (5), NEW(LetExpr) ("x", NEW(NumExpr) (5), NEW(VarExpr) ("x"))), NEW(NumExpr) (1)))->interp()->equals(NEW(NumVal) (26)));
        //Nested in right argument of multiplication expression
        CHECK ((NEW(MultExpr) (NEW(MultExpr) (NEW(NumExpr)  (2), NEW(LetExpr) ("x", NEW(NumExpr) (5), NEW(AddExpr) (NEW(VarExpr) ("x") , NEW(NumExpr) (1)) )), NEW(NumExpr) (3)))->interp()->equals(NEW(NumVal) (36)));
        //VarExpr unchanged exception check
        CHECK_THROWS_WITH ((NEW(AddExpr) (NEW(LetExpr) ("x", NEW(NumExpr) (3), NEW(LetExpr) ("y", NEW(NumExpr) (3), NEW(AddExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (2))) ), NEW(VarExpr) ("x")))->interp(), "free variable: x");
        //Let in lhs of add
        CHECK ((NEW(AddExpr) (NEW(LetExpr) ("x", NEW(NumExpr) (2), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (9))), NEW(NumExpr) (4)))->interp()->equals(NEW(NumVal) (15)));
    }

    SECTION("LetExpr::Substitution") 
    {
        //Dont sub x because lhs = valToSub
        CHECK( (NEW(LetExpr) ("x", NEW(NumExpr) (5), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (5))))->subst("x", NEW(NumExpr) (4))
        ->equals( NEW(LetExpr) ("x", NEW(NumExpr) (5), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (5)))));
        //Don't sub x because valToSub is not contained
        CHECK( (NEW(LetExpr) ("x", NEW(NumExpr) (5), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (5))))->subst("y", NEW(NumExpr) (4))
        ->equals( NEW(LetExpr) ("x", NEW(NumExpr) (5), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (5)))));
        //Sub if lhs == valToSub in rhs
        CHECK( (NEW(LetExpr) ("x", NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (8)), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (3))))->subst("x", NEW(NumExpr) (4))
        ->equals(NEW(LetExpr) ("x", NEW(AddExpr) (NEW(NumExpr) (4), NEW(NumExpr) (8)), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (3)))) );
        //Sub if lhs != valToSub when valToSub is contained in rhs
        CHECK( (NEW(LetExpr) ("x", NEW(AddExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (8)), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (3))))->subst("y", NEW(NumExpr) (4))
        ->equals(NEW(LetExpr) ("x", NEW(AddExpr) (NEW(NumExpr) (4), NEW(NumExpr) (8)), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (3)))) );
        //Sub if lhs != valToSub when valToSub is contained in body
        CHECK( (NEW(LetExpr) ("x", NEW(NumExpr) (6), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(AddExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (7)))))->subst("y", NEW(NumExpr) (4))
        ->equals(NEW(LetExpr) ("x", NEW(NumExpr) (6), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(AddExpr) (NEW(NumExpr) (4), NEW(NumExpr) (7))))) );

    }

    SECTION("LetExpr::Print") 
    {
        //Single let
        CHECK((NEW(LetExpr) ("x", NEW(NumExpr) (5), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (7))))
        ->to_string() == "(_let x=5 _in (x+7))");
        //Let with multiplication expression
        CHECK((NEW(LetExpr) ("x", NEW(NumExpr) (9), NEW(MultExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (3))))
        ->to_string() == "(_let x=9 _in (x*3))");
        //Nested let
        CHECK((NEW(LetExpr) ("x", NEW(NumExpr) (5), NEW(AddExpr) (NEW(LetExpr) ("y", NEW(NumExpr) (3), NEW(AddExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (2))), NEW(VarExpr) ("x"))))
        ->to_string() == "(_let x=5 _in ((_let y=3 _in (y+2))+x))");
        //Triple nested let
        CHECK((NEW(LetExpr) ("x", NEW(NumExpr) (5), NEW(AddExpr) (NEW(LetExpr) ("y", NEW(NumExpr) (3), NEW(LetExpr) ("y", NEW(NumExpr) (2), NEW(AddExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (5)))), NEW(VarExpr) ("x"))))
        ->to_string() == "(_let x=5 _in ((_let y=3 _in (_let y=2 _in (y+5)))+x))");
        //Let nested with let in right hand side
        CHECK( (NEW(LetExpr) ("x", NEW(LetExpr) ("y", NEW(NumExpr) (7), NEW(AddExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (6))), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (3))))
        ->to_string() == "(_let x=(_let y=7 _in (y+6)) _in (x+3))");

        CHECK((NEW(LetExpr) ("gfw", NEW(VarExpr) ("iju"), NEW(AddExpr) (NEW(NumExpr) (-1811580510), NEW(VarExpr) ("gfw"))))->to_string() == "(_let gfw=iju _in (-1811580510+gfw))");
    }
    SECTION("LetExpr::Pretty Print") 
    {
        //Let nested as right argument of parenthesized multiplication expression
        CHECK ( (NEW(MultExpr) (NEW(MultExpr) (NEW(NumExpr)  (2), NEW(LetExpr) ("x", NEW(NumExpr) (5), NEW(AddExpr) (NEW(VarExpr) ("x") , NEW(NumExpr) (1)) )), NEW(NumExpr) (3)))->to_stringPP() == "(2 * _let x = 5\n"
        "     _in  x + 1) * 3");
        //Let nested to the left in add expression which is nested to the right within a multiplication expression
        CHECK((NEW(MultExpr) (NEW(NumExpr) (5), NEW(AddExpr) (NEW(LetExpr) ("x", NEW(NumExpr) (5), NEW(VarExpr) ("x")), NEW(NumExpr) (1))))->to_stringPP() == "5 * ((_let x = 5\n"
        "      _in  x) + 1)");
        //Let in lhs of add
        CHECK ( (NEW(AddExpr) (NEW(LetExpr) ("x", NEW(NumExpr) (2), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (9))), NEW(NumExpr) (4)))->to_stringPP() == "(_let x = 2\n"
        " _in  x + 9) + 4");
        //Let in lhs of multiplication expression
        CHECK((NEW(MultExpr) (NEW(LetExpr) ("x", NEW(NumExpr) (5), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (8))), NEW(NumExpr) (3)))->to_stringPP() == "(_let x = 5\n"
        " _in  x + 8) * 3");
        //Let nest as right argument of un-parenthesized multiplication expression
        CHECK((NEW(AddExpr)  (NEW(MultExpr) (NEW(NumExpr) (4), NEW(LetExpr) ("x", NEW(NumExpr) (5), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (1)))), NEW(NumExpr) (9)))->to_stringPP() == "4 * (_let x = 5\n"
        "     _in  x + 1) + 9");
        //Let nested to the left within let that is nested to the left within add
        CHECK ((NEW(AddExpr) (NEW(LetExpr) ("x", NEW(NumExpr) (3), NEW(LetExpr) ("y", NEW(NumExpr) (3), NEW(AddExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (2))) ), NEW(VarExpr) ("x")))->to_stringPP() == "(_let x = 3\n"
        " _in  _let y = 3\n"
        "      _in  y + 2) + x");
        //Let nested in lhs of Add expression nested within body of let expression
        CHECK((NEW(LetExpr) ("x", NEW(NumExpr) (5), NEW(AddExpr) (NEW(LetExpr) ("y" , NEW(NumExpr) (3), NEW(AddExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (2))), NEW(VarExpr) ("x"))))
        ->to_stringPP() == "_let x = 5\n"
        "_in  (_let y = 3\n"
        "      _in  y + 2) + x");
        //Triple nested let
        CHECK( ( NEW(LetExpr) ( "x", NEW(NumExpr) (5),
        NEW(AddExpr) ( NEW(LetExpr) ( "y", NEW(NumExpr) (3),
        NEW(AddExpr) ( NEW(VarExpr) ("y"), NEW(LetExpr) ("z",NEW(NumExpr) (6),
        NEW(AddExpr) (NEW(VarExpr) ("a"), NEW(NumExpr) (8))) ) ), NEW(VarExpr) ("x") ) ) )
        ->to_stringPP()== "_let x = 5\n"
        "_in  (_let y = 3\n"
        "      _in  y + _let z = 6\n"
        "               _in  a + 8) + x" );
    }
}

TEST_CASE ( "EqExpr class" )
{
    SECTION ( "EqExpr::Equals" )
    {
        CHECK( ( NEW(EqExpr) ( NEW(NumExpr) ( 0 ), NEW(NumExpr) ( 0 ) ) )
        ->equals( NEW(EqExpr) ( NEW(NumExpr) ( 0 ), NEW(NumExpr) ( 0 ) ) ) );
        CHECK( ( NEW(EqExpr) ( NEW(NumExpr) ( 1 ), NEW(NumExpr) ( 1 ) ) )
        ->equals( NEW(EqExpr) ( NEW(NumExpr) ( 1 ), NEW(NumExpr) ( 1 ) ) ) );
        CHECK( ( NEW(EqExpr) ( NEW(NumExpr) ( -1 ), NEW(NumExpr) ( -1 ) ) )
        ->equals( NEW(EqExpr) ( NEW(NumExpr) ( -1 ), NEW(NumExpr) ( -1 ) ) ) );
        CHECK_FALSE( ( NEW(EqExpr) ( NEW(NumExpr) ( -1 ), NEW(NumExpr) ( -1 ) ) )
        ->equals( NEW(EqExpr) ( NEW(NumExpr) ( 0 ), NEW(NumExpr) ( 0 ) ) ) );

        CHECK( ( NEW(EqExpr) ( NEW(VarExpr) ( "" ), NEW(VarExpr) ( "" ) ) )
        ->equals( NEW(EqExpr) ( NEW(VarExpr) ( "" ), NEW(VarExpr) ( "" ) ) ) );
        CHECK( ( NEW(EqExpr) ( NEW(VarExpr) ( "x" ), NEW(VarExpr) ( "x" ) ) )
        ->equals( NEW(EqExpr) ( NEW(VarExpr) ( "x" ), NEW(VarExpr) ( "x" ) ) ) );
        CHECK_FALSE( ( NEW(EqExpr) ( NEW(VarExpr) ( "x" ), NEW(VarExpr) ( "x" ) ) )
        ->equals( NEW(EqExpr) ( NEW(VarExpr) ( "X" ), NEW(VarExpr) ( "X" ) ) ) );

        CHECK( ( NEW(EqExpr) ( NEW(AddExpr) ( NEW(NumExpr) ( 42 ), NEW(VarExpr) ( "x" ) ), NEW(MultExpr) ( NEW(BoolExpr) ( true ), NEW(BoolExpr) ( false ) ) ) )
        ->equals( NEW(EqExpr) ( NEW(AddExpr) ( NEW(NumExpr) ( 42 ), NEW(VarExpr) ( "x" ) ),
        NEW(MultExpr) ( NEW(BoolExpr) ( true ), NEW(BoolExpr) ( false ) ) ) ) );

        CHECK( ( NEW(EqExpr) ( NEW(LetExpr) ( "x", NEW(NumExpr) ( 42 ), NEW(AddExpr) ( NEW(VarExpr) ( "x" ), NEW(NumExpr) ( -42 ) ) ),
        NEW(IfExpr) ( NEW(EqExpr) ( NEW(VarExpr) ( "Z" ), NEW(VarExpr) ( "Z" ) ), NEW(NumExpr) ( 1 ), NEW(NumExpr) ( 2 ) ) ) )
        ->equals( NEW(EqExpr) ( NEW(LetExpr) ( "x", NEW(NumExpr) ( 42 ), NEW(AddExpr) ( NEW(VarExpr) ( "x" ), NEW(NumExpr) ( -42 ) ) ),
        NEW(IfExpr) ( NEW(EqExpr) ( NEW(VarExpr) ( "Z" ), NEW(VarExpr) ( "Z" ) ), NEW(NumExpr) ( 1 ),
        NEW(NumExpr) ( 2 ) ) ) ) );
    }


    SECTION ( "EqExpr::Interp" )
    {
        // TRUE Nums
        CHECK( ( NEW(EqExpr) ( NEW(NumExpr) ( 0 ), NEW(NumExpr) ( 0 ) ) )->interp()
        ->equals( NEW(BoolVal) ( true ) ) );
        CHECK( ( NEW(EqExpr) ( NEW(NumExpr) ( 1 ), NEW(NumExpr) ( 1 ) ) )->interp()
        ->equals( NEW(BoolVal) ( true ) ) );
        CHECK( ( NEW(EqExpr) ( NEW(NumExpr) ( -1 ), NEW(NumExpr) ( -1 ) ) )->interp()
        ->equals( NEW(BoolVal) ( true ) ) );
        CHECK( ( NEW(EqExpr) ( NEW(NumExpr) ( INT_MAX ), NEW(NumExpr) ( INT_MAX ) ) )->interp()
        ->equals( NEW(BoolVal) ( true ) ) );
        CHECK( ( NEW(EqExpr) ( NEW(NumExpr) ( INT_MIN ), NEW(NumExpr) ( INT_MIN ) ) )->interp()
        ->equals( NEW(BoolVal) ( true ) ) );
        // FALSE Nums
        CHECK( ( NEW(EqExpr) ( NEW(NumExpr) ( 1 ), NEW(NumExpr) ( 0 ) ) )->interp()
        ->equals( NEW(BoolVal) ( false ) ) );
        CHECK( ( NEW(EqExpr) ( NEW(NumExpr) ( -1 ), NEW(NumExpr) ( 0 ) ) )->interp()
        ->equals( NEW(BoolVal) ( false ) ) );
        CHECK( ( NEW(EqExpr) ( NEW(NumExpr) ( -1 ), NEW(NumExpr) ( 1 ) ) )->interp()
        ->equals( NEW(BoolVal) ( false ) ) );
        CHECK( ( NEW(EqExpr) ( NEW(NumExpr) ( INT_MAX ), NEW(NumExpr) ( INT_MIN ) ) )->interp()
        ->equals( NEW(BoolVal) ( false ) ) );
        CHECK( ( NEW(EqExpr) ( NEW(NumExpr) ( INT_MIN ), NEW(NumExpr) ( INT_MAX ) ) )->interp()
        ->equals( NEW(BoolVal) ( false ) ) );

        // TRUE Bools
        CHECK( ( NEW(EqExpr) ( NEW(BoolExpr) ( true ), NEW(BoolExpr) ( true ) ) )->interp()
        ->equals( NEW(BoolVal) ( true ) ) );
        CHECK( ( NEW(EqExpr) ( NEW(NumExpr) ( false ), NEW(NumExpr) ( false ) ) )->interp()
        ->equals( NEW(BoolVal) ( true ) ) );
        // FALSE Bools
        CHECK( ( NEW(EqExpr) ( NEW(BoolExpr) ( false ), NEW(BoolExpr) ( true ) ) )->interp()
        ->equals( NEW(BoolVal) ( false ) ) );
        CHECK( ( NEW(EqExpr) ( NEW(NumExpr) ( true ), NEW(NumExpr) ( false ) ) )->interp()
        ->equals( NEW(BoolVal) ( false ) ) );

        // FALSE Num and Bool with 0
        CHECK( ( NEW(EqExpr) ( NEW(NumExpr) ( 0 ), NEW(BoolExpr) ( false ) ) )->interp()
        ->equals( NEW(BoolVal) ( false ) ) );
        // Opposite for good measure
        CHECK_FALSE( ( NEW(EqExpr) ( NEW(NumExpr) ( 0 ), NEW(BoolExpr) ( false ) ) )->interp()
        ->equals( NEW(BoolVal) ( true ) ) );

        // FALSE Num and Bool with non-zero
        CHECK( ( NEW(EqExpr) ( NEW(NumExpr) ( 1 ), NEW(BoolExpr) ( true ) ) )->interp()
        ->equals( NEW(BoolVal) ( false ) ) );
        // Opposite for good measure
        CHECK_FALSE( ( NEW(EqExpr) ( NEW(NumExpr) ( 1 ), NEW(BoolExpr) ( true ) ) )->interp()
        ->equals( NEW(BoolVal) ( true ) ) );

        // TRUE Bool v Bool
        CHECK( ( NEW(EqExpr) ( NEW(BoolExpr) ( true ), NEW(BoolExpr) ( true ) ) )->interp()->equals( NEW(BoolVal) ( true ) ) );
        CHECK( ( NEW(EqExpr) ( NEW(BoolExpr) ( false ), NEW(BoolExpr) ( false ) ) )->interp()->equals( NEW(BoolVal) ( true ) ) );
        // FALSE Bool v Bool
        CHECK( ( NEW(EqExpr) ( NEW(BoolExpr) ( true ), NEW(BoolExpr) ( false ) ) )->interp()->equals( NEW(BoolVal) ( false ) ) );
        CHECK( ( NEW(EqExpr) ( NEW(BoolExpr) ( false ), NEW(BoolExpr) ( true ) ) )->interp()->equals( NEW(BoolVal) ( false ) ) );

        // TRUE Add, Mult
        CHECK( ( NEW(EqExpr) ( NEW(AddExpr) ( NEW(NumExpr) ( 42 ), NEW(NumExpr) ( 42 ) ),
        NEW(AddExpr) ( NEW(NumExpr) ( 42 ), NEW(NumExpr) ( 42 ) ) ) )->interp()->equals( NEW(BoolVal) ( true ) ) );
        CHECK( ( NEW(EqExpr) ( NEW(MultExpr) ( NEW(NumExpr) ( 42 ), NEW(NumExpr) ( 42 ) ),
        NEW(MultExpr) ( NEW(NumExpr) ( 42 ), NEW(NumExpr) ( 42 ) ) ) )->interp()->equals( NEW(BoolVal) ( true ) ) );
        // FALSE Add, Mult
        CHECK( ( NEW(EqExpr) ( NEW(AddExpr) ( NEW(NumExpr) ( 100000 ), NEW(NumExpr) ( 42 ) ),
        NEW(AddExpr) ( NEW(NumExpr) ( 42 ), NEW(NumExpr) ( 42 ) ) ) )->interp()->equals( NEW(BoolVal) ( false ) ) );
        CHECK( ( NEW(EqExpr) ( NEW(MultExpr) ( NEW(NumExpr) ( 100000 ), NEW(NumExpr) ( 42 ) ),
        NEW(MultExpr) ( NEW(NumExpr) ( 42 ), NEW(NumExpr) ( 42 ) ) ) )->interp()->equals( NEW(BoolVal) ( false ) ) );

        // Triple-nested Let
        CHECK( ( NEW(EqExpr) (

            NEW(LetExpr) ( "x", NEW(NumExpr) ( 5 ), NEW(AddExpr) (
            NEW(LetExpr) ( "y", NEW(NumExpr) ( 3 ), NEW(AddExpr) ( NEW(VarExpr) ( "y" ),
            NEW(LetExpr) ( "z", NEW(NumExpr) ( 6 ),
            NEW(AddExpr) ( NEW(VarExpr) ( "z" ), NEW(NumExpr) ( 8 ) ) ) ) ),
            NEW(VarExpr) ( "x" ) ) ),

            NEW(LetExpr) ( "x", NEW(NumExpr) ( 5 ), NEW(AddExpr) (
            NEW(LetExpr) ( "y", NEW(NumExpr) ( 3 ), NEW(AddExpr) ( NEW(VarExpr) ( "y" ),
            NEW(LetExpr) ( "z", NEW(NumExpr) ( 6 ),
            NEW(AddExpr) ( NEW(VarExpr) ( "z" ), NEW(NumExpr) ( 8 ) ) ) ) ),
            NEW(VarExpr) ( "x" ) ) ) ) )

            ->interp()->equals( NEW(BoolVal) ( true ) ) );
    }

    SECTION ( "EqExpr::Substitution" )
    {
        CHECK( ( NEW(EqExpr) ( NEW(VarExpr) ( "x" ), NEW(NumExpr) ( 42 ) ) )
        ->subst( "x", NEW(NumExpr) ( 42 ) )
        ->equals( NEW(EqExpr) ( NEW(NumExpr) ( 42 ), NEW(NumExpr) ( 42 ) ) ) );

        CHECK( ( NEW(EqExpr) ( NEW(VarExpr) ( "x" ), NEW(NumExpr) ( 42 ) ) )
        ->subst( "x", NEW(EqExpr) ( NEW(NumExpr) ( 42 ), NEW(NumExpr) ( 42 ) ) )
        ->equals( NEW(EqExpr) ( NEW(EqExpr) ( NEW(NumExpr) ( 42 ), NEW(NumExpr) ( 42 ) ), NEW(NumExpr) ( 42 ) ) ) );

        // replace both LHS and RHS when same value
        CHECK( ( NEW(EqExpr) ( NEW(VarExpr) ( "x" ), NEW(VarExpr) ( "x" ) ) )
        ->subst( "x", NEW(VarExpr) ( "y" ) )
        ->equals( NEW(EqExpr) ( NEW(VarExpr) ( "y" ), NEW(VarExpr) ( "y" ) ) ) );

        // replace selectively when both LHS and RHS are of type Var
        CHECK( ( NEW(EqExpr) ( NEW(VarExpr) ( "x" ), NEW(VarExpr) ( "y" ) ) )
        ->subst( "y", NEW(VarExpr) ( "z" ) )
        ->equals( NEW(EqExpr) ( NEW(VarExpr) ( "x" ), NEW(VarExpr) ( "z" ) ) ) );

        // replace selectively when !both LHS and RHS are of type Var
        CHECK( ( NEW(EqExpr) ( NEW(VarExpr) ( "x" ), NEW(NumExpr) ( 42 ) ) )
        ->subst( "x", NEW(VarExpr) ( "y" ) )
        ->equals( NEW(EqExpr) ( NEW(VarExpr) ( "y" ), NEW(NumExpr) ( 42 ) ) ) );

        CHECK_FALSE( ( NEW(EqExpr) ( NEW(VarExpr) ( "apple" ), NEW(NumExpr) ( 10 ) ) )
        ->subst( "x", NEW(EqExpr) ( NEW(NumExpr) ( 7 ), NEW(NumExpr) ( 3 ) ) )
        ->equals( NEW(EqExpr) ( NEW(NumExpr) ( 10 ), NEW(EqExpr) ( NEW(NumExpr) ( 7 ), NEW(NumExpr) ( 3 ) ) ) ) );
    }

    SECTION ( "EqExpr::Print" )
    {
        // Nums
        CHECK( ( NEW(EqExpr) ( NEW(NumExpr) ( 42 ), NEW(NumExpr) ( 42 ) ) )->to_string() == "(42==42)" );

        // Vars
        CHECK( ( NEW(EqExpr) ( NEW(VarExpr) ( "x" ), NEW(VarExpr) ( "x" ) ) )->to_string() == "(x==x)" );

        // Bools
        CHECK( ( NEW(EqExpr) ( NEW(BoolExpr) ( true ), NEW(BoolExpr) ( true ) ) )->to_string() == "(_true==_true)" );
        CHECK( ( NEW(EqExpr) ( NEW(BoolExpr) ( false ), NEW(BoolExpr) ( false ) ) )->to_string() == "(_false==_false)" );
        CHECK( ( NEW(EqExpr) ( NEW(BoolExpr) ( true ), NEW(BoolExpr) ( false ) ) )->to_string() == "(_true==_false)" );
        CHECK( ( NEW(EqExpr) ( NEW(BoolExpr) ( false ), NEW(BoolExpr) ( true ) ) )->to_string() == "(_false==_true)" );

        // Num and Bool
        CHECK( ( NEW(EqExpr) ( NEW(NumExpr) ( 1 ), NEW(BoolExpr) ( true ) ) )->to_string() == "(1==_true)" );

        // Num and Var
        CHECK( ( NEW(EqExpr) ( NEW(VarExpr) ( "x" ), NEW(NumExpr) ( 42 ) ) )->to_string() == "(x==42)" );

        // Add/Mult, Add/Mult with Bools
        CHECK( ( NEW(EqExpr) ( NEW(AddExpr) ( NEW(NumExpr) ( 42 ), NEW(NumExpr) ( 42 ) ),
        NEW(AddExpr) ( NEW(NumExpr) ( 42 ), NEW(NumExpr) ( 42 ) ) ) )->to_string() == "((42+42)==(42+42))" );
        CHECK( ( NEW(EqExpr) ( NEW(MultExpr) ( NEW(NumExpr) ( 42 ), NEW(NumExpr) ( 42 ) ),
        NEW(MultExpr) ( NEW(NumExpr) ( 42 ), NEW(NumExpr) ( 42 ) ) ) )->to_string() == "((42*42)==(42*42))" );
        CHECK( ( NEW(EqExpr) ( NEW(AddExpr) ( NEW(NumExpr) ( 42 ), NEW(NumExpr) ( 42 ) ), NEW(BoolExpr) ( true ) ) )->to_string() ==
        "((42+42)==_true)" );
        CHECK( ( NEW(EqExpr) ( NEW(MultExpr) ( NEW(NumExpr) ( 42 ), NEW(NumExpr) ( 42 ) ), NEW(BoolExpr) ( true ) ) )->to_string() ==
        "((42*42)==_true)" );

        // Let
        CHECK( ( NEW(EqExpr) ( NEW(LetExpr) ( "x", NEW(NumExpr) ( 42 ), NEW(VarExpr) ( "x" ) ),
        NEW(LetExpr) ( "x", NEW(NumExpr) ( 42 ), NEW(VarExpr) ( "x" ) ) ) )->to_string() ==
        "((_let x=42 _in x)==(_let x=42 _in x))" );
    }

    SECTION ( "EqExpr::Pretty Print" )
    {
        // Nums
        CHECK( ( NEW(EqExpr) ( NEW(NumExpr) ( 42 ), NEW(NumExpr) ( 42 ) ) )->to_stringPP() == "42 == 42" );

        // Vars
        CHECK( ( NEW(EqExpr) ( NEW(VarExpr) ( "x" ), NEW(VarExpr) ( "x" ) ) )->to_stringPP() == "x == x" );

        // Bools
        CHECK( ( NEW(EqExpr) ( NEW(BoolExpr) ( true ), NEW(BoolExpr) ( true ) ) )->to_stringPP() == "_true == _true" );
        CHECK( ( NEW(EqExpr) ( NEW(BoolExpr) ( false ), NEW(BoolExpr) ( false ) ) )->to_stringPP() == "_false == _false" );
        CHECK( ( NEW(EqExpr) ( NEW(BoolExpr) ( true ), NEW(BoolExpr) ( false ) ) )->to_stringPP() == "_true == _false" );
        CHECK( ( NEW(EqExpr) ( NEW(BoolExpr) ( false ), NEW(BoolExpr) ( true ) ) )->to_stringPP() == "_false == _true" );

        // Num and Bool
        CHECK( ( NEW(EqExpr) ( NEW(NumExpr) ( 1 ), NEW(BoolExpr) ( true ) ) )->to_stringPP() == "1 == _true" );

        // Num and Var
        CHECK( ( NEW(EqExpr) ( NEW(VarExpr) ( "x" ), NEW(NumExpr) ( 42 ) ) )->to_stringPP() == "x == 42" );

        // Add/Mult, Add/Mult with Bools
        CHECK( ( NEW(EqExpr) ( NEW(AddExpr) ( NEW(NumExpr) ( 42 ), NEW(NumExpr) ( 42 ) ),
        NEW(AddExpr) ( NEW(NumExpr) ( 42 ), NEW(NumExpr) ( 42 ) ) ) )->to_stringPP() == "42 + 42 == 42 + 42" );
        CHECK( ( NEW(EqExpr) ( NEW(MultExpr) ( NEW(NumExpr) ( 42 ), NEW(NumExpr) ( 42 ) ),
        NEW(MultExpr) ( NEW(NumExpr) ( 42 ), NEW(NumExpr) ( 42 ) ) ) )->to_stringPP() == "42 * 42 == 42 * 42" );
        CHECK( ( NEW(EqExpr) ( NEW(AddExpr) ( NEW(NumExpr) ( 42 ), NEW(NumExpr) ( 42 ) ), NEW(BoolExpr) ( true ) ) )->to_stringPP() ==
        "42 + 42 == _true" );
        CHECK( ( NEW(EqExpr) ( NEW(MultExpr) ( NEW(NumExpr) ( 42 ), NEW(NumExpr) ( 42 ) ), NEW(BoolExpr) ( true ) ) )->to_stringPP() ==
        "42 * 42 == _true" );
    }
}

TEST_CASE("IfExpr class") 
{
    
    SECTION ( "IfExpr::Equals" )
    {
        //True
        CHECK((NEW(IfExpr) (NEW(BoolExpr) (true), NEW(NumExpr) (4), NEW(NumExpr) (9)))->equals(NEW(IfExpr) (NEW(BoolExpr) (true), NEW(NumExpr) (4), NEW(NumExpr) (9))));
        //False
        CHECK_FALSE((NEW(IfExpr) (NEW(BoolExpr) (true), NEW(NumExpr) (4), NEW(NumExpr) (9)))->equals(NEW(IfExpr) (NEW(BoolExpr) (false), NEW(NumExpr) (4), NEW(NumExpr) (9))));
    }
    SECTION ( "IfExpr::Interp" )
    {
        //IfExpr with false equality
        CHECK((NEW(IfExpr) (NEW(EqExpr) (NEW(NumExpr) (1), NEW(NumExpr) (2)), NEW(NumExpr) (5), NEW(NumExpr) (6)))->interp()->equals(NEW(NumVal) (6)));
        //IfExpr with true equality
        CHECK((NEW(IfExpr) (NEW(EqExpr) (NEW(NumExpr) (4), NEW(NumExpr) (4)), NEW(NumExpr) (7), NEW(NumExpr) (6)))->interp()->equals(NEW(NumVal) (7)));
        //Nested IfExpr
        CHECK((NEW(IfExpr) (NEW(IfExpr) (NEW(EqExpr) (NEW(NumExpr) (1), NEW(NumExpr) (2)), NEW(BoolExpr) (false), NEW(BoolExpr) (true)), NEW(NumExpr) (5), NEW(NumExpr) (6)))->interp()->equals(NEW(NumVal) (5)));
        //Invalid Expression in test_part of IfExpr
        CHECK_THROWS_WITH((NEW(IfExpr) (NEW(AddExpr) (NEW(NumExpr) (1), NEW(NumExpr) (2)), NEW(BoolExpr) (false), NEW(BoolExpr) (true)))->interp(), "NumVal is not of type boolean");
        //Invalid AddExpr within IfExpr
        CHECK_THROWS_WITH((NEW(IfExpr) (NEW(BoolExpr) (false), NEW(NumExpr) (5), NEW(AddExpr) (NEW(BoolExpr) (true), NEW(NumExpr) (1))))->interp(), "Cannot perform add operation on BoolVal!");
    }
    
    SECTION ("IfExpr::Substitution")
    {
        //Successful substitution
        CHECK(((NEW(IfExpr) (NEW(BoolExpr) (true), NEW(NumExpr) (4), NEW(VarExpr) ("x")))->subst("x", NEW(NumExpr) (9)))->equals(NEW(IfExpr) (NEW(BoolExpr) (true), NEW(NumExpr) (4), NEW(NumExpr) (9))));
        //Unsuccessful substitution
        CHECK(((NEW(IfExpr) (NEW(BoolExpr) (true), NEW(NumExpr) (4), NEW(VarExpr) ("x")))->subst("y", NEW(NumExpr) (9)))->equals(NEW(IfExpr) (NEW(BoolExpr) (true), NEW(NumExpr) (4), NEW(VarExpr) ("x"))));
        //Successful substitution but false
        CHECK(!((NEW(IfExpr) (NEW(BoolExpr) (true), NEW(NumExpr) (8), NEW(VarExpr) ("x")))->subst("x", NEW(NumExpr) (1)))->equals(NEW(IfExpr) (NEW(BoolExpr) (false), NEW(NumExpr) (8), NEW(NumExpr) (1))));
        //Unsuccessful substituion but false
        CHECK(!((NEW(IfExpr) (NEW(BoolExpr) (true), NEW(NumExpr) (4), NEW(VarExpr) ("x")))->subst("y", NEW(NumExpr) (9)))->equals(NEW(IfExpr) (NEW(BoolExpr) (true), NEW(NumExpr) (4), NEW(VarExpr) ("y"))));
        //Successful substitution with expression substituted being IfExpr
        CHECK(((NEW(IfExpr) (NEW(BoolExpr) (true), NEW(NumExpr) (4), NEW(VarExpr) ("x")))->subst("x", NEW(IfExpr) (NEW(EqExpr) (NEW(NumExpr) (2), NEW(NumExpr) (2)), NEW(NumExpr) (3), NEW(NumExpr) (6))))->equals(NEW(IfExpr) (NEW(BoolExpr) (true), NEW(NumExpr) (4), NEW(IfExpr) (NEW(EqExpr) (NEW(NumExpr) (2), NEW(NumExpr) (2)), NEW(NumExpr) (3), NEW(NumExpr) (6)))));
    }

    SECTION ( "IfExpr::Print" )
    {
        //Regular IfExpr
        CHECK((NEW(IfExpr) (NEW(BoolExpr) (true), NEW(NumExpr) (6), NEW(NumExpr) (8)))->to_string() == "(_if _true _then 6 _else 8)");
        //IfExpr within AddExpr
        CHECK((NEW(AddExpr) (NEW(NumExpr) (1), NEW(IfExpr) (NEW(BoolExpr) (false), NEW(NumExpr) (5), NEW(NumExpr) (2))))->to_string() == "(1+(_if _false _then 5 _else 2))");
        //IfExpr within MultExpr
        CHECK((NEW(MultExpr) (NEW(NumExpr) (9), NEW(IfExpr) (NEW(BoolExpr) (true), NEW(NumExpr) (4), NEW(NumExpr) (2))))->to_string() == "(9*(_if _true _then 4 _else 2))");
        //IfExpr within LetExpr
        CHECK((NEW(LetExpr) (std::string("x"), NEW(NumExpr) (3), NEW(IfExpr) (NEW(BoolExpr) (true), NEW(VarExpr) ("x"), NEW(NumExpr) (7))))->to_string() == "(_let x=3 _in (_if _true _then x _else 7))");
        //IfExpr within EqExpr
        CHECK((NEW(EqExpr) (NEW(NumExpr) (5), NEW(IfExpr) (NEW(BoolExpr) (true), NEW(NumExpr) (4), NEW(NumExpr) (8))))->to_string() == "(5==(_if _true _then 4 _else 8))");
    }

    SECTION ( "IfExpr::Pretty Print" )
    {
        //Regular IfExpr
        CHECK((NEW(IfExpr) (NEW(BoolExpr) (true), NEW(NumExpr) (6), NEW(NumExpr) (8)))->to_stringPP() == 
              "_if   _true\n"
              "_then 6\n"
              "_else 8");
        //IfExpr within AddExpr
        CHECK((NEW(AddExpr) (NEW(NumExpr) (1), NEW(IfExpr) (NEW(BoolExpr) (false), NEW(NumExpr) (5), NEW(NumExpr) (2))))->to_stringPP() == 
              "1 + _if   _false\n"
              "    _then 5\n"
              "    _else 2");
        //IfExpr within MultExpr
        CHECK((NEW(MultExpr) (NEW(NumExpr) (9), NEW(IfExpr) (NEW(BoolExpr) (true), NEW(NumExpr) (4), NEW(NumExpr) (2))))->to_stringPP() == 
              "9 * (_if   _true\n"
              "     _then 4\n"
              "     _else 2)");
        //IfExpr within LetExpr
        CHECK((NEW(LetExpr) (std::string("x"), NEW(NumExpr) (3), NEW(IfExpr) (NEW(BoolExpr) (true), NEW(VarExpr) ("x"), NEW(NumExpr) (7))))->to_stringPP() == 
              "_let x = 3\n"
              "_in  _if   _true\n"
              "     _then x\n"
              "     _else 7");
        //IfExpr within EqExpr
        CHECK((NEW(EqExpr) (NEW(NumExpr) (5), NEW(IfExpr) (NEW(BoolExpr) (true), NEW(NumExpr) (4), NEW(NumExpr) (8))))->to_stringPP() ==
              "5 == _if   _true\n"
              "     _then 4\n"
              "     _else 8");
    }
}

TEST_CASE("BoolExpr class") 
{
    SECTION ( "BoolExpr::Equals" )
    {
        //True check
        CHECK((NEW(BoolExpr) (true))->equals(NEW(BoolExpr) (true)));
        //False check
        CHECK(!(NEW(BoolExpr) (true))->equals(NEW(BoolExpr) (false)));
    }
    
    SECTION ( "BoolExpr::Interp" )
    {
        //Interp checks for both member field possiblities for BoolExpr
        CHECK((NEW(BoolExpr) (true))->interp()->equals(NEW(BoolVal) (true)));
        CHECK((NEW(BoolExpr) (false))->interp()->equals(NEW(BoolVal) (false)));
        //False returns for incorrect interp
        CHECK(!(NEW(BoolExpr) (true))->interp()->equals(NEW(BoolVal) (false)));
        CHECK(!(NEW(BoolExpr) (false))->interp()->equals(NEW(BoolVal) (true)));
    }
    
    SECTION ( "BoolExpr::Substitution" )
    {
        //Check for returnign original object since BoolExpr do not contain a variable
        CHECK((NEW(BoolExpr) (true))->subst("X", NEW(NumExpr) (7))->equals(NEW(BoolExpr) (true)));
        CHECK((NEW(BoolExpr) (false))->subst("X", NEW(NumExpr) (7))->equals(NEW(BoolExpr) (false)));

    }

    SECTION ( "BoolExpr::Print" )
    {
        CHECK((NEW(BoolExpr) (true))->to_string() == "_true");
        CHECK((NEW(BoolExpr) (false))->to_string() == "_false");
    }
}

TEST_CASE("FunExpr") 
{
    
    SECTION("FunExpr::Equals")
    {
        //True check
        CHECK((NEW(FunExpr) ("x", NEW(AddExpr) (NEW(NumExpr) (5), NEW(NumExpr) (3))))->equals(NEW(FunExpr) ("x", NEW(AddExpr) (NEW(NumExpr) (5), NEW(NumExpr) (3)))));
        //False check
        CHECK_FALSE((NEW(FunExpr) ("y", NEW(MultExpr) (NEW(NumExpr) (9), NEW(NumExpr) (0))))->equals(NEW(FunExpr) ("x", NEW(AddExpr) (NEW(NumExpr) (5), NEW(NumExpr) (3)))));
    }
    
    SECTION("FunExpr::Interp")
    {
        //FunExpr with NumExpr body
        CHECK((NEW(FunExpr) ("x", NEW(NumExpr) (5)))->interp()->equals(NEW(FunVal) ("x", NEW(NumExpr) (5))));
        //FunExpr with AddExpr body
        CHECK((NEW(FunExpr) ("y", NEW(AddExpr) (NEW(NumExpr) (2), NEW(NumExpr) (3))))->interp()->equals(NEW(FunVal) ("y", NEW(AddExpr) (NEW(NumExpr) (2), NEW(NumExpr) (3)))));
        //FunExpr with MultExpr body
        CHECK((NEW(FunExpr) ("z", NEW(MultExpr) (NEW(NumExpr) (8), NEW(NumExpr) (12))))->interp()->equals(NEW(FunVal) ("z", NEW(MultExpr) (NEW(NumExpr) (8), NEW(NumExpr) (12)))));
        //FunExpr with LetExpr body
        CHECK((NEW(FunExpr) ("x", NEW(LetExpr) ("f", NEW(NumExpr) (4), NEW(AddExpr) (NEW(VarExpr) ("f"), NEW(NumExpr) (8)))))->interp()->equals(NEW(FunVal) ("x", NEW(LetExpr) ("f", NEW(NumExpr) (4), NEW(AddExpr) (NEW(VarExpr) ("f"), NEW(NumExpr) (8))))));
        //FunExpr with IfExpr body
        CHECK((NEW(FunExpr) ("x", NEW(IfExpr) (NEW(EqExpr) (NEW(NumExpr) (1), NEW(NumExpr) (2)), NEW(NumExpr) (5), NEW(NumExpr) (6))))->interp()->equals(NEW(FunVal) ("x", NEW(IfExpr) (NEW(EqExpr) (NEW(NumExpr) (1), NEW(NumExpr) (2)), NEW(NumExpr) (5), NEW(NumExpr) (6)))));
        
    }
    
    SECTION("FunExpr::Substituion")
    {
        //FunExpr can be substituted
        CHECK((NEW(FunExpr) ("x", NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(VarExpr) ("y"))))->subst("y", NEW(NumExpr) (4))->equals(NEW(FunExpr) ("x", NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (4)))));
        //FunExpr cannot be substituted
        CHECK((NEW(FunExpr) ("x", NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (7))))->subst("x", NEW(NumExpr) (4))->equals(NEW(FunExpr) ("x", NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (7)))));
        //Successful substitution when valToSub is nested in FunExpr->body
        CHECK((NEW(FunExpr) ("x", NEW(AddExpr) (NEW(NumExpr) (3), NEW(IfExpr) (NEW(BoolExpr) (true), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(VarExpr) ("y")), NEW(NumExpr) (4)))))
              ->subst("y", NEW(NumExpr) (3))->equals
              (NEW(FunExpr) ("x", NEW(AddExpr) (NEW(NumExpr) (3), NEW(IfExpr) (NEW(BoolExpr) (true), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (3)), NEW(NumExpr) (4))))));
        //Unsuccessful substitution when valToSub is nested in FunExpr->body
        CHECK((NEW(FunExpr) ("x", NEW(AddExpr) (NEW(NumExpr) (3), NEW(IfExpr) (NEW(BoolExpr) (true), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(VarExpr) ("y")), NEW(NumExpr) (4)))))
              ->subst("x", NEW(NumExpr) (3))->equals
              (NEW(FunExpr) ("x", NEW(AddExpr) (NEW(NumExpr) (3), NEW(IfExpr) (NEW(BoolExpr) (true), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(VarExpr) ("y")), NEW(NumExpr) (4))))));
        //Unsuccessfuls substitution when FunExpr has valToSub in multiple locations
        CHECK((NEW(FunExpr) ("x", NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(IfExpr) (NEW(BoolExpr) (true), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(VarExpr) ("y")), NEW(NumExpr) (4)))))->subst("x", NEW(NumExpr) (5))->equals(NEW(FunExpr) ("x", NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(IfExpr) (NEW(BoolExpr) (true), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(VarExpr) ("y")), NEW(NumExpr) (4))))));
    }
    
    SECTION("FunExpr::Print")
    {
        //Basic FunExpr
        CHECK((NEW(FunExpr) ("x", NEW(AddExpr) (NEW(NumExpr) (2), NEW(VarExpr) ("x"))))->to_string() ==
        "(_fun (x) (2+x))");
        //FunExpr with nested LetExpr
        CHECK((NEW(FunExpr) ("x", NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(LetExpr) ("y", NEW(NumExpr) (5), NEW(AddExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (6))))))->to_string() ==
              "(_fun (x) (x+(_let y=5 _in (y+6))))");
        //FunExpr nested within AddExpr
        CHECK((NEW(AddExpr) (NEW(NumExpr) (4), NEW(FunExpr) ("x", NEW(MultExpr) (NEW(NumExpr) (5), NEW(VarExpr) ("x")))))->to_string() ==
              "(4+(_fun (x) (5*x)))");
        //FunExpr nested within LetExpr
        CHECK((NEW(LetExpr) ("x", NEW(NumExpr) (3), NEW(FunExpr) ("y", NEW(AddExpr) (NEW(VarExpr) ("y"), NEW(VarExpr) ("x")))))->to_string() ==
              "(_let x=3 _in (_fun (y) (y+x)))");
        //FunExp nested within FunExpr
        CHECK((NEW(FunExpr) ("x", NEW(MultExpr) (NEW(VarExpr) ("x"), NEW(FunExpr) ("y", NEW(AddExpr) (NEW(NumExpr) (4), NEW(VarExpr) ("y"))))))->to_string() ==
              "(_fun (x) (x*(_fun (y) (4+y))))");
    }
    
    SECTION("FunExpr::Pretty-Print")
    {
        //Basic FunExpr
        CHECK((NEW(FunExpr) ("x", NEW(AddExpr) (NEW(NumExpr) (2), NEW(VarExpr) ("x"))))->to_stringPP() ==
        "_fun (x)\n"
        "  2 + x");
        //FunExpr with nested LetExpr
        CHECK((NEW(FunExpr) ("x", NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(LetExpr) ("y", NEW(NumExpr) (5), NEW(AddExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (6))))))->to_stringPP() ==
              "_fun (x)\n"
              "  x + _let y = 5\n"
              "      _in  y + 6");
        //FunExpr nested within AddExpr
        CHECK((NEW(AddExpr) (NEW(NumExpr) (4), NEW(FunExpr) ("x", NEW(MultExpr) (NEW(NumExpr) (5), NEW(VarExpr) ("x")))))->to_stringPP() ==
              "4 + _fun (x)\n"
              "      5 * x");
        //FunExpr nested within LetExpr
        CHECK((NEW(LetExpr) ("x", NEW(NumExpr) (3), NEW(FunExpr) ("y", NEW(AddExpr) (NEW(VarExpr) ("y"), NEW(VarExpr) ("x")))))->to_stringPP() ==
              "_let x = 3\n"
              "_in  _fun (y)\n"
              "       y + x");
        //FunExp nested within FunExpr
        CHECK((NEW(FunExpr) ("x", NEW(MultExpr) (NEW(VarExpr) ("x"), NEW(FunExpr) ("y", NEW(AddExpr) (NEW(NumExpr) (4), NEW(VarExpr) ("y"))))))->to_stringPP() ==
              "_fun (x)\n"
              "  x * (_fun (y)\n"
              "         4 + y)");
    }
}

TEST_CASE("CallExpr") 
{
    
    SECTION("CallExpr::Equals")
    {
        //True check
        CHECK((NEW(CallExpr) (NEW(NumExpr) (5), NEW(NumExpr) (7)))->equals(NEW(CallExpr) (NEW(NumExpr) (5), NEW(NumExpr) (7))));
        //False check
        CHECK(!(NEW(CallExpr) (NEW(VarExpr) ("t"), NEW(NumExpr) (7)))->equals(NEW(CallExpr) (NEW(NumExpr) (5), NEW(NumExpr) (7))));
    }
    
    SECTION("CallExpr::Interp")
    {
        //Error handle for NumVal
        CHECK_THROWS_WITH((NEW(CallExpr) (NEW(NumExpr) (5), NEW(NumExpr) (9)))->interp(), "NumVal cannot call");
        //Error handle for BoolVal
        CHECK_THROWS_WITH((NEW(CallExpr) (NEW(BoolExpr) (true), NEW(NumExpr) (9)))->interp(), "BoolVal cannot call");
        //Interp on FunExpr when substituting to_be_called with a NumExpr
        CHECK((NEW(CallExpr) (NEW(FunExpr) ("x", NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (1))), NEW(NumExpr) (4)))->interp()->equals(NEW(NumVal) (5)));
        //Interp on FunExpr when substituting to_be_called with a AddExpr
        CHECK((NEW(CallExpr) (NEW(FunExpr) ("x", NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (9))), NEW(AddExpr) (NEW(NumExpr) (3), NEW(NumExpr) (7))))->interp()->equals(NEW(NumVal) (19)));
        //Interp on FunExpr when substituting to_be_called with a MultExpr
        CHECK((NEW(CallExpr) (NEW(FunExpr) ("x", NEW(MultExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (3))), NEW(MultExpr) (NEW(NumExpr) (6), NEW(NumExpr) (2))))->interp()->equals(NEW(NumVal) (36)));
        //Interp on FunExpr when substituting to_be_called with a LetExpr
        CHECK((NEW(CallExpr) (NEW(FunExpr) ("x", NEW(MultExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (6))), NEW(LetExpr) ("y", NEW(NumExpr) (4), NEW(AddExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (8)))))->interp()->equals(NEW(NumVal) (72)));
    }
    
    SECTION("CallExpr::Substituion")
    {
        //Successful substitution on basic CallExpr
        CHECK((NEW(CallExpr) (NEW(NumExpr) (3), NEW(VarExpr) ("x")))->subst("x", NEW(NumExpr) (7))->equals(NEW(CallExpr) (NEW(NumExpr) (3), NEW(NumExpr) (7))));
        //Unsuccessful substitution on basic CallExpr
        CHECK((NEW(CallExpr) (NEW(NumExpr) (3), NEW(NumExpr) (4)))->subst("x", NEW(NumExpr) (7))->equals(NEW(CallExpr) (NEW(NumExpr) (3), NEW(NumExpr) (4))));
        //Successful substitution when valToSub is nested in to_be_called expression
        CHECK((NEW(CallExpr) (NEW(AddExpr) (NEW(NumExpr) (4), NEW(VarExpr) ("x")), NEW(NumExpr) (5)))->subst("x", NEW(NumExpr) (8))->equals(NEW(CallExpr) (NEW(AddExpr) (NEW(NumExpr) (4), NEW(NumExpr) (8)), NEW(NumExpr) (5))));
        //Successful substitution when valToSub nested in actual_arg expression
        CHECK((NEW(CallExpr) (NEW(NumExpr) (4), NEW(MultExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (0))))->subst("y", NEW(NumExpr) (9))->equals(NEW(CallExpr) (NEW(NumExpr) (4), NEW(MultExpr) (NEW(NumExpr) (9), NEW(NumExpr) (0)))));
        //Unsuccessful substitution when valToSub is not in ntested actual_arg expression
        CHECK((NEW(CallExpr) (NEW(NumExpr) (4), NEW(MultExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (0))))->subst("z", NEW(NumExpr) (9))->equals(NEW(CallExpr) (NEW(NumExpr) (4), NEW(MultExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (0)))));
    }
    
    SECTION("CallExpr::Print")
    {
        //FunExpr with VarExpr
        CHECK((NEW(CallExpr) (NEW(FunExpr) ("x", NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (4))), NEW(VarExpr) ("x")))->to_string() == 
              "(_fun (x) (x+4)) x");
        //FunExpr with AddExpr
        CHECK((NEW(CallExpr) (NEW(FunExpr) ("x", NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (4))), NEW(AddExpr) (NEW(NumExpr) (3), NEW(NumExpr) (8))))->to_string() ==
              "(_fun (x) (x+4)) (3+8)");
        //FunExpr with MultExpr
        CHECK((NEW(CallExpr) (NEW(FunExpr) ("y", NEW(AddExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (7))), NEW(MultExpr) (NEW(NumExpr) (1), NEW(NumExpr) (5))))->to_string() ==
              "(_fun (y) (y+7)) (1*5)");
        //FunExpr with LetExpr
        CHECK((NEW(CallExpr) (NEW(FunExpr) ("y", NEW(AddExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (7))), NEW(LetExpr) ("x", NEW(NumExpr) (9), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (4)))))->to_string() ==
              "(_fun (y) (y+7)) (_let x=9 _in (x+4))");
        //FunExpr with NumExpr
        CHECK((NEW(CallExpr) (NEW(FunExpr) ("y", NEW(AddExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (7))), NEW(NumExpr) (0)))->to_string() ==
              "(_fun (y) (y+7)) 0");
    }
    
    SECTION("Pretty-Print")
    {
        //FunExpr with VarExpr
        CHECK((NEW(CallExpr) (NEW(FunExpr) ("x", NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (4))), NEW(VarExpr) ("x")))->to_stringPP() ==
              "_fun (x)\n"
              "  x + 4(x)");
        //FunExpr with AddExpr
        CHECK((NEW(CallExpr) (NEW(FunExpr) ("x", NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (4))), NEW(AddExpr) (NEW(NumExpr) (3), NEW(NumExpr) (8))))->to_stringPP() ==
              "_fun (x)\n"
              "  x + 4(3 + 8)");
        //FunExpr with MultExpr
        CHECK((NEW(CallExpr) (NEW(FunExpr) ("y", NEW(AddExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (7))), NEW(MultExpr) (NEW(NumExpr) (1), NEW(NumExpr) (5))))->to_stringPP() ==
              "_fun (y)\n"
              "  y + 7(1 * 5)");
        //FunExpr with LetExpr
        CHECK((NEW(CallExpr) (NEW(FunExpr) ("y", NEW(AddExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (7))), NEW(LetExpr) ("x", NEW(NumExpr) (9), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (4)))))->to_stringPP() ==
              "_fun (y)\n"
              "  y + 7(_let x = 9\n"
              "        _in  x + 4)");
        //FunExpr with NumExpr
        CHECK((NEW(CallExpr) (NEW(FunExpr) ("y", NEW(AddExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (7))), NEW(NumExpr) (0)))->to_stringPP() ==
              "_fun (y)\n"
              "  y + 7(0)");
    }
}

TEST_CASE("NumVal") 
{

    SECTION("NumVal::Equals")
    {
        CHECK((NEW(NumVal) (1))->equals(NEW(NumVal) (5)) == false);
        CHECK((NEW(NumVal) (1))->equals(NEW(NumVal) (2)) == false);
        //Zero check
        CHECK((NEW(NumVal) (0))->equals(NEW(NumVal) (0)) == true);
        //Negative check
        CHECK((NEW(NumVal) (-2))->equals(NEW(NumVal) (-2)) == true);
        //Max and min check
        CHECK((NEW(NumVal) (INT16_MAX))->equals(NEW(NumVal) (INT16_MAX)) == true);
        CHECK((NEW(NumVal) (INT16_MIN))->equals(NEW(NumVal) (INT16_MIN)) == true);
        //Repeats
        CHECK((NEW(NumVal) (777))->equals(NEW(NumVal) (777)) == true);
    }

    SECTION("NumVal::Print")
    {
        CHECK((NEW(NumVal) (10))->to_string() == "10" );
        CHECK((NEW(NumVal) (-11))->to_string() == "-11");
        CHECK((NEW(NumVal) (777))->to_string() == "777");
        CHECK((NEW(NumVal) (0))->to_string() == "0");
        CHECK((NEW(NumVal) (83))->to_string() != "72");
    }

    SECTION("NumVal::to_expr")
    {
        CHECK((NEW(NumVal) (10))->to_expr()->equals(NEW(NumExpr) (10)));
        CHECK((NEW(NumVal)  (-11))->to_expr()->equals(NEW(NumExpr) (-11)));
        CHECK((NEW(NumVal)  (777))->to_expr()->equals(NEW(NumExpr) (777)));
        CHECK((NEW(NumVal)  (0))->to_expr()->equals(NEW(NumExpr) (0)));
        CHECK((NEW(NumVal)  (83))->to_expr()->equals(NEW(NumExpr) (83)));
    }

    SECTION("NumVal::add_expr")
    {
        CHECK(((NEW(NumVal) (1))->add_to(NEW(NumVal) (4)))->equals(NEW(NumVal) (5)));
        //Not equals check
        CHECK_FALSE(((NEW(NumVal) (1))->add_to(NEW(NumVal) (4)))->equals(NEW(NumVal) (10)));
        //Zero check
        CHECK(((NEW(NumVal) (0))->add_to(NEW(NumVal) (0)))->equals(NEW(NumVal) (0)));
        //Handle negatives
        CHECK(((NEW(NumVal) (-7))->add_to(NEW(NumVal) (-5)))->equals(NEW(NumVal) (-12)));
        //Handle large numbers and wraparound
        CHECK(((NEW(NumVal) (INT_MAX))->add_to(NEW(NumVal) (1)))->equals(NEW(NumVal) (INT_MIN)));
        CHECK( ( NEW(NumVal) ( INT_MAX ) )->add_to( NEW(NumVal) ( -INT_MAX ) )->equals( NEW(NumVal) ( 0 ) ) );
        CHECK( ( NEW(NumVal) ( INT_MIN ) )->add_to( NEW(NumVal) ( -INT_MIN ) )->equals( NEW(NumVal) ( 0 ) ) );
        //Mixed sign
        CHECK(((NEW(NumVal) (-20))->add_to(NEW(NumVal) (4)))->equals(NEW(NumVal) (-16)));
        //Repeats
        CHECK( ( ( NEW(NumVal) ( 42 ) )->add_to( NEW(NumVal) ( 42 ) )->add_to( NEW(NumVal) (42) ) )->equals( NEW(NumVal) ( 126 ) ) );
        //Exception handling
        CHECK_THROWS_WITH( ( NEW(NumVal) ( -1 ) )->add_to( NEW(BoolVal) ( true ) ), "Trying to add a non-number!" );
        CHECK_THROWS_WITH( ( NEW(NumVal) ( 0 ) )->add_to( NEW(BoolVal) ( true ) ), "Trying to add a non-number!" );
        CHECK_THROWS_WITH( ( NEW(NumVal) ( 1 ) )->add_to( NEW(BoolVal) ( true ) ), "Trying to add a non-number!" );
    }

    SECTION("NumVal::mult_with")
    {
        CHECK(((NEW(NumVal) (1))->mult_with(NEW(NumVal) (4)))->equals(NEW(NumVal) (4)));
        //Not equals check
        CHECK_FALSE(((NEW(NumVal) (7))->mult_with(NEW(NumVal) (8)))->equals(NEW(NumVal) (10)));
        //Multiply by 0
        CHECK(((NEW(NumVal) (76))->mult_with(NEW(NumVal) (0)))->equals(NEW(NumVal) (0)));
        //Multiply by 1
        CHECK(((NEW(NumVal) (23))->mult_with(NEW(NumVal) (1)))->equals(NEW(NumVal) (23)));
        //Handle negative
        CHECK(((NEW(NumVal) (-6))->mult_with(NEW(NumVal) (-3)))->equals(NEW(NumVal) (18)));
        //Mixed sign
        CHECK(((NEW(NumVal) (-20))->mult_with(NEW(NumVal) (4)))->equals(NEW(NumVal) (-80)));
        //Exception handling
        CHECK_THROWS_WITH( ( NEW(NumVal) ( -1 ) )->mult_with( NEW(BoolVal) ( true ) ), "Trying to perform multiplication with a non-number!" );
        CHECK_THROWS_WITH( ( NEW(NumVal) ( 0 ) )->mult_with( NEW(BoolVal) ( true ) ), "Trying to perform multiplication with a non-number!" );
        CHECK_THROWS_WITH( ( NEW(NumVal) ( 1 ) )->mult_with( NEW(BoolVal) ( true ) ), "Trying to perform multiplication with a non-number!" );
    }
    
    SECTION("NumVal::to_expression")
    {
        CHECK( ( NEW(NumVal) ( 0 ) )->to_expr()->equals( NEW(NumExpr) ( 0 ) ) );

        CHECK( ( NEW(NumVal) ( 1 ) )->to_expr()->equals( NEW(NumExpr) ( 1 ) ) );
        CHECK( ( NEW(NumVal) ( -1 ) )->to_expr()->equals( NEW(NumExpr) ( -1 ) ) );

        CHECK( ( NEW(NumVal) ( INT_MAX ) )->to_expr()->equals( NEW(NumExpr) ( INT_MAX ) ) );
        CHECK( ( NEW(NumVal) ( INT_MIN ) )->to_expr()->equals( NEW(NumExpr) ( INT_MIN ) ) );

        CHECK_FALSE( ( NEW(NumVal) ( 1 ) )->to_expr()->equals( NEW(NumExpr) ( -1 ) ) );
    }
    
    SECTION( "NumVal::is_true()" )
    {
        CHECK_THROWS_WITH( ( NEW(NumVal) ( 0 ) )->is_true(), "NumVal is not of type boolean" );

        CHECK_THROWS_WITH( ( NEW(NumVal) ( 1 ) )->is_true(), "NumVal is not of type boolean" );
        CHECK_THROWS_WITH( ( NEW(NumVal) ( -1 ) )->is_true(), "NumVal is not of type boolean" );

        CHECK_THROWS_WITH( ( NEW(NumVal) ( INT_MIN ) )->is_true(), "NumVal is not of type boolean" );
        CHECK_THROWS_WITH( ( NEW(NumVal) ( INT_MAX ) )->is_true(), "NumVal is not of type boolean" );
    }
    
    SECTION("NumVal::Call")
    {
        CHECK_THROWS_WITH((NEW(NumVal) (5))->call(NEW(NumVal) (5)), "NumVal cannot call");
    }
}


TEST_CASE ( "BoolVal" )
{
    SECTION( "BoolVal::to_expr()" )
    {
        CHECK( ( NEW(BoolVal) ( true ) )->to_expr()->equals( NEW(BoolExpr) ( true ) ) );
        CHECK( ( NEW(BoolVal) ( false ) )->to_expr()->equals( NEW(BoolExpr) ( false ) ) );

        CHECK_FALSE( ( NEW(BoolVal) ( true ) )->to_expr()->equals( NEW(BoolExpr) ( false ) ) );
        CHECK_FALSE( ( NEW(BoolVal) ( false ) )->to_expr()->equals( NEW(BoolExpr) ( true ) ) );

        CHECK_FALSE( ( NEW(BoolVal) ( true ) )->to_expr()->equals( nullptr ) );
        CHECK_FALSE( ( NEW(BoolVal) ( false ) )->to_expr()->equals( nullptr ) );
    }

    SECTION( "BoolVal::Equals" )
    {
        CHECK( ( NEW(BoolVal) ( true ) )->equals( NEW(BoolVal) ( true ) ) );
        CHECK( ( NEW(BoolVal) ( false ) )->equals( NEW(BoolVal) ( false ) ) );

        CHECK_FALSE( ( NEW(BoolVal) ( true ) )->equals( NEW(BoolVal) ( false ) ) );
        CHECK_FALSE( ( NEW(BoolVal) ( false ) )->equals( NEW(BoolVal) ( true ) ) );

        CHECK_FALSE( ( NEW(BoolVal) ( true ) )->equals( nullptr ) );
        CHECK_FALSE( ( NEW(BoolVal) ( false ) )->equals( nullptr ) );
    }

    SECTION( "BoolVal::add_to()" )
    {
        CHECK_THROWS_WITH( ( NEW(BoolVal) ( true ) )->add_to( NEW(BoolVal) ( true ) ), "Cannot perform add operation on BoolVal!" );
        CHECK_THROWS_WITH( ( NEW(BoolVal) ( false ) )->add_to( NEW(BoolVal) ( true ) ), "Cannot perform add operation on BoolVal!" );
        CHECK_THROWS_WITH( ( NEW(BoolVal) ( true ) )->add_to( NEW(NumVal) ( 42 ) ), "Cannot perform add operation on BoolVal!" );
        CHECK_THROWS_WITH( ( NEW(BoolVal) ( false ) )->add_to( NEW(NumVal) ( 42 ) ), "Cannot perform add operation on BoolVal!" );
    }

    SECTION( "BoolVal::mult_with()" )
    {
        CHECK_THROWS_WITH( ( NEW(BoolVal) ( true ) )->mult_with( NEW(BoolVal) ( true ) ),
        "Cannot perform multiplication operation on BoolVal!" );
        CHECK_THROWS_WITH( ( NEW(BoolVal) ( false ) )->mult_with( NEW(BoolVal) ( true ) ),
        "Cannot perform multiplication operation on BoolVal!" );
        CHECK_THROWS_WITH( ( NEW(BoolVal) ( true ) )->mult_with( NEW(NumVal) ( 42 ) ), "Cannot perform multiplication operation on BoolVal!" );
        CHECK_THROWS_WITH( ( NEW(BoolVal) ( false ) )->mult_with( NEW(NumVal) ( 42 ) ), "Cannot perform multiplication operation on BoolVal!" );
    }

    SECTION( "BoolVal::is_true()" )
    {
        CHECK( ( NEW(BoolVal) ( true ) )->is_true() );
        CHECK_FALSE( ( NEW(BoolVal) ( false ) )->is_true() );
    }

    SECTION( "BoolVal::Print" )
    {
        CHECK( ( NEW(BoolVal) ( true ) )->to_string() == "_true" );
        CHECK( ( NEW(BoolVal) ( false ) )->to_string() == "_false" );
    }
    
    SECTION("BoolVal::Call")
    {
        CHECK_THROWS_WITH((NEW(BoolVal) (true))->call(NEW(BoolVal) (false)), "BoolVal cannot call");
    }
}

TEST_CASE ( "FunVal" )
{
    SECTION( "FunVal::to_expr()" )
    {
        //FunVal with NumExpr
        CHECK((NEW(FunVal) ("x", NEW(NumExpr) (7)))->to_expr()->equals(NEW(FunExpr) ("x", NEW(NumExpr) (7))));
        //FunVal with AddExpr
        CHECK((NEW(FunVal) ("x", NEW(AddExpr) (NEW(NumExpr) (2), NEW(VarExpr) ("x"))))->to_expr()->equals(NEW(FunExpr) ("x", NEW(AddExpr) (NEW(NumExpr) (2), NEW(VarExpr) ("x")))));
        //FunVal with MultExpr
        CHECK((NEW(FunVal) ("x", NEW(MultExpr) (NEW(NumExpr) (2), NEW(VarExpr) ("x"))))->to_expr()->equals(NEW(FunExpr) ("x", NEW(MultExpr) (NEW(NumExpr) (2), NEW(VarExpr) ("x")))));
        //FunVal with nested LetExpr
        CHECK((NEW(FunVal) ("x", NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(LetExpr) ("y", NEW(NumExpr) (5), NEW(AddExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (6))))))->to_expr()->equals(NEW(FunExpr) ("x", NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(LetExpr) ("y", NEW(NumExpr) (5), NEW(AddExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (6)))))));
        //FunVal nested within FunExpr
        CHECK((NEW(FunVal) ("x", NEW(MultExpr) (NEW(VarExpr) ("x"), NEW(FunExpr) ("y", NEW(AddExpr) (NEW(NumExpr) (4), NEW(VarExpr) ("y"))))))->to_expr()->equals(NEW(FunExpr) ("x", NEW(MultExpr) (NEW(VarExpr) ("x"), NEW(FunExpr) ("y", NEW(AddExpr) (NEW(NumExpr) (4), NEW(VarExpr) ("y")))))));
    }

    SECTION( "FunVal::Equals" )
    {
        //True check
        CHECK((NEW(FunVal) ("x", NEW(AddExpr) (NEW(NumExpr) (5), NEW(NumExpr) (3))))->equals(NEW(FunVal) ("x", NEW(AddExpr) (NEW(NumExpr) (5), NEW(NumExpr) (3)))));
        //False check
        CHECK_FALSE((NEW(FunVal) ("y", NEW(MultExpr) (NEW(NumExpr) (9), NEW(NumExpr) (0))))->equals(NEW(FunVal) ("x", NEW(AddExpr) (NEW(NumExpr) (5), NEW(NumExpr) (3)))));
    }

    SECTION( "FunVal::add_to()" )
    {
        CHECK_THROWS_WITH((NEW(FunVal) ("x", NEW(AddExpr) (NEW(NumExpr) (5), NEW(NumExpr) (3))))->add_to(NEW(NumVal) (7)), "Cannot perform add operation on FunVal!");
        CHECK_THROWS_WITH((NEW(FunVal) ("x", NEW(AddExpr) (NEW(NumExpr) (5), NEW(NumExpr) (3))))->add_to(NEW(BoolVal) (true)), "Cannot perform add operation on FunVal!");
    }

    SECTION( "FunVal::mult_with()" )
    {
        CHECK_THROWS_WITH((NEW(FunVal) ("x", NEW(AddExpr) (NEW(NumExpr) (5), NEW(NumExpr) (3))))->mult_with(NEW(NumVal) (7)), "Cannot perform multiplication operation on FunVal!");
        CHECK_THROWS_WITH((NEW(FunVal) ("x", NEW(AddExpr) (NEW(NumExpr) (5), NEW(NumExpr) (3))))->mult_with(NEW(BoolVal) (7)), "Cannot perform multiplication operation on FunVal!");
    }

    SECTION( "FunVal::is_true()" )
    {
        CHECK_THROWS_WITH((NEW(FunVal) ("x", NEW(AddExpr) (NEW(NumExpr) (5), NEW(NumExpr) (3))))->is_true(), "FunVal is not of type boolean");
    }

    SECTION( "FunVal::Print" )
    {
        //FunVal with NumExpr
        CHECK((NEW(FunVal) ("x", NEW(NumExpr) (7)))->to_string() ==
        "[_fun (x) 7]");
        //FunVal with AddExpr
        CHECK((NEW(FunVal) ("x", NEW(AddExpr) (NEW(NumExpr) (2), NEW(VarExpr) ("x"))))->to_string() ==
        "[_fun (x) (2+x)]");
        //FunVal with MultExpr
        CHECK((NEW(FunVal) ("x", NEW(MultExpr) (NEW(NumExpr) (2), NEW(VarExpr) ("x"))))->to_string() ==
        "[_fun (x) (2*x)]");
        //FunVal with nested LetExpr
        CHECK((NEW(FunVal) ("x", NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(LetExpr) ("y", NEW(NumExpr) (5), NEW(AddExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (6))))))->to_string() ==
              "[_fun (x) (x+(_let y=5 _in (y+6)))]");
        //FunVal nested within FunExpr
        CHECK((NEW(FunVal) ("x", NEW(MultExpr) (NEW(VarExpr) ("x"), NEW(FunExpr) ("y", NEW(AddExpr) (NEW(NumExpr) (4), NEW(VarExpr) ("y"))))))->to_string() ==
              "[_fun (x) (x*(_fun (y) (4+y)))]");
    }
    
    SECTION( "FunVal::Call")
    {

        //FunVal with NumExpr with unsuccessful call substitution
        CHECK((NEW(FunVal) ("x", NEW(NumExpr) (7)))->call(NEW(NumVal) (6))->equals(NEW(NumVal) (7)));
        //FunVal with AddExpr calling an AddExpr
        CHECK((NEW(FunVal) ("x", NEW(AddExpr) (NEW(NumExpr) (2), NEW(VarExpr) ("x"))))->call((NEW(AddExpr) (NEW(NumExpr) (4), NEW(NumExpr) (9)))->interp())->equals(NEW(NumVal) (15)));
        //FunVal with MultExpr calling a MultExpr
        CHECK((NEW(FunVal) ("x", NEW(MultExpr) (NEW(NumExpr) (2), NEW(VarExpr) ("x"))))->call((NEW(MultExpr) (NEW(NumExpr) (4), NEW(NumExpr) (9)))->interp())->equals(NEW(NumVal) (72)));
        //FunVal with nested LetExpr calling a LetExpr
        CHECK((NEW(FunVal) ("x", NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(LetExpr) ("y", NEW(NumExpr) (5), NEW(AddExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (6))))))->call((NEW(LetExpr) ("y", NEW(NumExpr) (5), NEW(AddExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (6))))->interp())->equals(NEW(NumVal) (22)));
        //FunVal nested within Val that throws exception
        CHECK_THROWS_WITH((NEW(FunVal) ("x", NEW(MultExpr) (NEW(VarExpr) ("x"), NEW(FunExpr) ("y", NEW(AddExpr) (NEW(NumExpr) (4), NEW(VarExpr) ("y"))))))->call(NEW(NumVal) (4)), "Trying to perform multiplication with a non-number!");
        //Trying to call with BoolVal to throw exception
        CHECK_THROWS_WITH((NEW(FunVal) ("x", NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (6))))->call(NEW(BoolVal) (false)),"Cannot perform add operation on BoolVal!");
    }
}


TEST_CASE("Parse") 
{

    SECTION("Provided") 
    {
        CHECK_THROWS_WITH( parse_str("()"), "invalid input" );
        CHECK( parse_str("(1)")->equals(NEW(NumExpr) (1)) );
        CHECK( parse_str("(((1)))")->equals(NEW(NumExpr) (1)) );
        CHECK_THROWS_WITH( parse_str("(1"), "missing close parenthesis" );
        CHECK( parse_str("1")->equals(NEW(NumExpr) (1)) );
        CHECK( parse_str("10")->equals(NEW(NumExpr) (10)) );
        CHECK( parse_str("-3")->equals(NEW(NumExpr) (-3)) );
        CHECK( parse_str(" \n 5 ")->equals(NEW(NumExpr) (5)) );
        CHECK_THROWS_WITH( parse_str("-"), "invalid input" );
        CHECK_THROWS_WITH( parse_str(" - 5 "), "invalid input" );
        CHECK( parse_str("x")->equals(NEW(VarExpr) ("x")) );
        CHECK( parse_str("xyz")->equals(NEW(VarExpr) ("xyz")) );
        CHECK( parse_str("xYz")->equals(NEW(VarExpr) ("xYz")) );
        CHECK_THROWS_WITH( parse_str("x_z"), "invalid input" );
        CHECK_THROWS_WITH( parse_str("x-z"), "invalid input" );
        CHECK( parse_str("x + y")->equals(NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(VarExpr) ("y"))) );
        CHECK( parse_str("x * y")->equals(NEW(MultExpr) (NEW(VarExpr) ("x"), NEW(VarExpr) ("y"))) );
        CHECK( parse_str("z * x + y")
        ->equals(NEW(AddExpr) (NEW(MultExpr) (NEW(VarExpr) ("z"), NEW(VarExpr) ("x")),
        NEW(VarExpr) ("y"))) );
        CHECK( parse_str("z * (x + y)")
        ->equals(NEW(MultExpr) (NEW(VarExpr) ("z"),
        NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(VarExpr) ("y"))) ));
        CHECK( parse_str("_let x = 5 _in x + 5")->equals(NEW(LetExpr) ("x", NEW(NumExpr) (5), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (5)))));
        CHECK( parse_str("_let x = 5 _in ((_let x = 5 _in x + 1) + x)")->equals(NEW(LetExpr) ("x", NEW(NumExpr) (5), NEW(AddExpr) (NEW(LetExpr) ("x", NEW(NumExpr) (5), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (1))), NEW(VarExpr) ("x")))));
        CHECK( parse_str("_let x = 5 _in ((_let x = 5 _in x * 1) * x)")->equals(NEW(LetExpr) ("x", NEW(NumExpr) (5), NEW(MultExpr) (NEW(LetExpr) ("x", NEW(NumExpr) (5), NEW(MultExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (1))), NEW(VarExpr) ("x")))));
        }
    SECTION( "NumExpr" )
    {
        CHECK( parse_str( "56780" )->equals( NEW(NumExpr) ( 56780 ) ) );
        CHECK( parse_str( "-56780" )->equals( NEW(NumExpr) ( -56780 ) ) );

        CHECK( parse_str( "     56780" )->equals( NEW(NumExpr) ( 56780 ) ) );
        CHECK( parse_str( "     -56780" )->equals( NEW(NumExpr) ( -56780 ) ) );

        CHECK( parse_str( "\n\n\t     56780" )->equals( NEW(NumExpr) ( 56780 ) ) );
        CHECK( parse_str( "\n\n\t     -56780" )->equals( NEW(NumExpr) ( -56780 ) ) );

        CHECK( parse_str( "     56780     " )->equals( NEW(NumExpr) ( 56780 ) ) );
        CHECK( parse_str( "     -56780     " )->equals( NEW(NumExpr) ( -56780 ) ) );

        CHECK( parse_str( "\n\n\t     56780\n\n\t     " )->equals( NEW(NumExpr) ( 56780 ) ) );
        CHECK( parse_str( "\n\n\t     -56780\n\n\t     " )->equals( NEW(NumExpr) ( -56780 ) ) );

        CHECK_THROWS_WITH( parse_str( "56780-" ), "invalid input" );
        CHECK_THROWS_WITH( parse_str( "-56780x" ), "invalid input" );
        CHECK_THROWS_WITH( parse_str( "-56780-" ), "invalid input" );
        CHECK_THROWS_WITH( parse_str( "-" ), "invalid input" );
        CHECK_THROWS_WITH( parse_str( "- 5" ), "invalid input" );
    }

    SECTION( "VarExpr" )
    {
        CHECK( parse_str( "x" )->equals( NEW(VarExpr) ( "x" ) ) );
        CHECK( parse_str( "variable" )->equals( NEW(VarExpr) ( "variable" ) ) );

        CHECK( parse_str( "X" )->equals( NEW(VarExpr) ( "X" ) ) );
        CHECK( parse_str( "mRkrAbS" )->equals( NEW(VarExpr) ( "mRkrAbS" ) ) );

        CHECK( parse_str( "     var" )->equals( NEW(VarExpr) ( "var" ) ) );
        CHECK( parse_str( "     VAR" )->equals( NEW(VarExpr) ( "VAR" ) ) );

        CHECK( parse_str( "\n\n\t     var" )->equals( NEW(VarExpr) ( "var" ) ) );
        CHECK( parse_str( "\n\n\t     VAR" )->equals( NEW(VarExpr) ( "VAR" ) ) );

        CHECK( parse_str( "     var     " )->equals( NEW(VarExpr) ( "var" ) ) );
        CHECK( parse_str( "     VAR     " )->equals( NEW(VarExpr) ( "VAR" ) ) );

        CHECK( parse_str( "\n\n\t     var\n\n\t     " )->equals( NEW(VarExpr) ( "var" ) ) );
        CHECK( parse_str( "\n\n\t     VAR\n\n\t     " )->equals( NEW(VarExpr) ( "VAR" ) ) );

        CHECK_THROWS_WITH( parse_str( "x1" ), "invalid input" );
        CHECK_THROWS_WITH( parse_str( "x_" ), "invalid input" );
        CHECK_THROWS_WITH( parse_str( "x-" ), "invalid input" );
        CHECK_THROWS_WITH( parse_str( "-x" ), "invalid input" );
        CHECK_THROWS_WITH( parse_str( "2x" ), "invalid input" );
    }

    SECTION( "AddExpr" )
    {
        CHECK( parse_str("_if _true _then 5 _else 6")->equals(NEW(IfExpr) (NEW(BoolExpr) (true), NEW(NumExpr) (5), NEW(NumExpr) (6))) );
        CHECK( parse_str( "42+42" )->equals( NEW(AddExpr) ( NEW(NumExpr) ( 42 ), NEW(NumExpr) ( 42 ) ) ) );
        CHECK( parse_str( "-42+-42" )->equals( NEW(AddExpr) ( NEW(NumExpr) ( -42 ), NEW(NumExpr) ( -42 ) ) ) );
        CHECK( parse_str( "42*42" )->equals( NEW(AddExpr) ( NEW(NumExpr) ( -42 ), NEW(NumExpr) ( -42 ) ) ) == false );

        CHECK( parse_str( "x+y" )->equals( NEW(AddExpr) ( NEW(VarExpr) ( "x" ), NEW(VarExpr) ( "y" ) ) ) );
        CHECK( parse_str( "X+Y" )->equals( NEW(AddExpr) ( NEW(VarExpr) ( "X" ), NEW(VarExpr) ( "Y" ) ) ) );

        CHECK( parse_str( "  \n\t  42  \n\t  + \n\t  X  \n\t  " )
        ->equals( NEW(AddExpr) ( NEW(NumExpr) ( 42 ), NEW(VarExpr) ( "X" ) ) ) );

        CHECK( parse_str( "x + 42" )->equals( NEW(AddExpr) ( NEW(VarExpr) ( "x" ), NEW(NumExpr) ( 42 ) ) ) );

        CHECK_THROWS_WITH( parse_str( "- 42 + 42" ), "invalid input" );
        CHECK_THROWS_WITH( parse_str( "42 + - 42" ), "invalid input" );
        CHECK_THROWS_WITH( parse_str( "+" ), "invalid input" );
    }

    SECTION( "MultExpr" )
    {
        CHECK( parse_str( "42 * 42" )->equals( NEW(MultExpr) ( NEW(NumExpr) ( 42 ), NEW(NumExpr) ( 42 ) ) ) );
        CHECK( parse_str( "-42 * -42" )->equals( NEW(MultExpr) ( NEW(NumExpr) ( -42 ), NEW(NumExpr) ( -42 ) ) ) );

        CHECK( parse_str( "x * y" )->equals( NEW(MultExpr) ( NEW(VarExpr) ( "x" ), NEW(VarExpr) ( "y" ) ) ) );
        CHECK( parse_str( "X * Y" )->equals( NEW(MultExpr) ( NEW(VarExpr) ( "X" ), NEW(VarExpr) ( "Y" ) ) ) );

        CHECK( parse_str( "x * 42" )->equals( NEW(MultExpr) ( NEW(VarExpr) ( "x" ), NEW(NumExpr) ( 42 ) ) ) );

        CHECK( parse_str( "  \n\t  42  \n\t  *  \n\t  X  \n\t  " )
        ->equals( NEW(MultExpr) ( NEW(NumExpr) ( 42 ), NEW(VarExpr) ( "X" ) ) ) );

        CHECK_THROWS_WITH( parse_str( "- 42 * 42" ), "invalid input" );
        CHECK_THROWS_WITH( parse_str( "42 * - 42" ), "invalid input" );
        CHECK_THROWS_WITH( parse_str( "*" ), "invalid input" );
    }


    SECTION( "LetExpr" )
    {
        CHECK( parse_str( "_let x = 5 _in x + 5" )
                       ->equals( NEW(LetExpr) ( "x", NEW(NumExpr) ( 5 ), NEW(AddExpr) ( NEW(VarExpr) ( "x" ), NEW(NumExpr) ( 5 ) ) ) ) );

        CHECK( parse_str( "_let x = (_let y = 5 _in y+6) _in x+7" )
                       ->equals( NEW(LetExpr) ( "x",
                                          NEW(LetExpr) ( "y", NEW(NumExpr) ( 5 ), NEW(AddExpr) ( NEW(VarExpr) ( "y" ), NEW(NumExpr) ( 6 ) ) ),
                                          NEW(AddExpr) ( NEW(VarExpr) ( "x" ), NEW(NumExpr) ( 7 ) ) ) ) );

        CHECK( parse_str( "_let x = 5 _in (_let y = 3 _in y + _let z = 6 _in z + 8) + x" )
                       ->equals( (  NEW(LetExpr) ( "x", NEW(NumExpr) ( 5 ), NEW(AddExpr) (
                                        NEW(LetExpr) ( "y", NEW(NumExpr) ( 3 ), NEW(AddExpr) ( NEW(VarExpr) ( "y" ),
                                                                             NEW(LetExpr) ( "z", NEW(NumExpr) ( 6 ), NEW(AddExpr) ( NEW(VarExpr) ( "z" ),
                                                                                                                  NEW(NumExpr) ( 8 ) ) ) ) ),
                                    NEW(VarExpr) ( "x" ) ) ) ) ) );

        CHECK( parse_str( "  \n\t  _let  \n\t  x  \n\t  =  5  \n\t  _in  \n\t"
                           "  (  \n\t  _let  \n\t  y  \n\t  =  \n\t  3  \n\t"
                           "  _in  \n\t  y   \n\t  +  \n\t  _let  \n\t  z"
                           "  \n\t  =  \n\t  6  \n\t   _in  \n\t  z  \n\t "
                           " +  \n\t  8  \n\t  )  \n\t  +  \n\t  x  \n\t  " )
                       ->equals( (  NEW(LetExpr) ( "x", NEW(NumExpr) ( 5 ), NEW(AddExpr) (
                                        NEW(LetExpr) ( "y", NEW(NumExpr) ( 3 ), NEW(AddExpr) ( NEW(VarExpr) ( "y" ),
                                                                             NEW(LetExpr) ( "z", NEW(NumExpr) ( 6 ), NEW(AddExpr) ( NEW(VarExpr) ( "z" ),
                                                                                                                  NEW(NumExpr) ( 8 ) ) ) ) ),
                                    NEW(VarExpr) ( "x" ) ) ) ) ) );

        CHECK_THROWS_WITH( parse_str( "_let x = 5 _42" ), "consume mismatch" );
        CHECK_THROWS_WITH( parse_str( "_let x = 5 _in" ), "invalid input" );
        CHECK_THROWS_WITH( parse_str( "_let 3 = 5 _in y + 3" ), "consume mismatch for = \n" );
        CHECK_THROWS_WITH( parse_str( "_let 3 = 5 _in y + " ), "consume mismatch for = \n" );
        CHECK_THROWS_WITH( parse_str( "_let x = 5 _in y + 3" ), "invalid let expression" );
        CHECK_THROWS_WITH( parse_str( "_let x = 5 _in 3" ), "invalid let expression" );

    }

    SECTION( "IfExpr" )
    {
        CHECK( parse_str("_if x==3 _then 42+x _else 84*x")
                       ->equals( NEW(IfExpr) ( NEW(EqExpr) ( NEW(VarExpr) ("x"), NEW(NumExpr) (3) ),
                                         NEW(AddExpr) ( NEW(NumExpr) (42), NEW(VarExpr) ("x") ),
                                         NEW(MultExpr) ( NEW(NumExpr) (84), NEW(VarExpr) ("x") ) ) )
        );

        CHECK( parse_str("  \n\t  _if  \n\t  x  \n\t  ==  \n\t  3  \n\t  _then  \n\t  42  \n\t  +  \n\t  x  "
                          "\n\t  _else  \n\t  84  \n\t  *  \n\t  x  \n\t  ")
                       ->equals( NEW(IfExpr) ( NEW(EqExpr) ( NEW(VarExpr) ("x"), NEW(NumExpr) (3) ),
                                         NEW(AddExpr) ( NEW(NumExpr) (42), NEW(VarExpr) ("x") ),
                                         NEW(MultExpr) ( NEW(NumExpr) (84), NEW(VarExpr) ("x") ) ) )
        );

        PTR(Expr)  leviathan = NEW(LetExpr) ( "A", ( NEW(LetExpr) ( "B", NEW(NumExpr) (1), ( NEW(IfExpr) ( NEW(EqExpr) ( NEW(VarExpr) ("x"), (
                           NEW(IfExpr) ( NEW(EqExpr) ( NEW(VarExpr) ("y"), NEW(NumExpr) (2)), NEW(NumExpr) (3), NEW(NumExpr) (4) ) ) ),
                           NEW(NumExpr) (11), NEW(VarExpr) ("B") ) ) ) ), ( NEW(IfExpr) ( NEW(EqExpr) ( ( NEW(LetExpr) ( "C", NEW(NumExpr) (5),
                           NEW(AddExpr) ( NEW(VarExpr) ("C"), NEW(NumExpr) (6) ) ) ), ( NEW(LetExpr) ( "D", NEW(NumExpr) (7), NEW(AddExpr) ( NEW(VarExpr) ("D"),
                           NEW(NumExpr) (8) ) ) ) ), NEW(NumExpr) (4), ( NEW(LetExpr) ( "E", NEW(NumExpr) (9), NEW(AddExpr) ( NEW(VarExpr) ("E"),
                           NEW(VarExpr) ("A") ) ) ) ) ) );

        CHECK( parse_str(               "_let A = _let B = 1\n"
                                         "         _in  _if   x == (_if   y == 2\n"
                                         "                          _then 3\n"
                                         "                          _else 4)\n"
                                         "              _then 11\n"
                                         "              _else B\n"
                                         "_in  _if   (_let C = 5\n"
                                         "            _in  C + 6) == (_let D = 7\n"
                                         "                            _in  D + 8)\n"
                                         "     _then 4\n"
                                         "     _else _let E = 9\n"
                                         "           _in  E + A"
        )->equals( leviathan ) );

    }

    SECTION( "BoolExpr" )
    {
        CHECK( parse_str( "_true" )->equals( NEW(BoolExpr) ( true ) ) );
        CHECK( parse_str( "_false" )->equals( NEW(BoolExpr) ( false ) ) );
    }

    SECTION( "EqExpr" )
    {
        CHECK( parse_str( "42==42" )->equals( NEW(EqExpr) ( NEW(NumExpr) ( 42 ), NEW(NumExpr) ( 42 ) ) ) );
        CHECK( parse_str( "-42==-42" )->equals( NEW(EqExpr) ( NEW(NumExpr) ( -42 ), NEW(NumExpr) ( -42 ) ) ) );
        CHECK( parse_str( "42*42" )->equals( NEW(EqExpr) ( NEW(NumExpr) ( -42 ), NEW(NumExpr) ( -42 ) ) ) == false );

        CHECK( parse_str( "x==y" )->equals( NEW(EqExpr) ( NEW(VarExpr) ( "x" ), NEW(VarExpr) ( "y" ) ) ) );
        CHECK( parse_str( "X==Y" )->equals( NEW(EqExpr) ( NEW(VarExpr) ( "X" ), NEW(VarExpr) ( "Y" ) ) ) );

        CHECK( parse_str( "  \n\t  42  \n\t  == \n\t  X  \n\t  " )
        ->equals( NEW(EqExpr) ( NEW(NumExpr) ( 42 ), NEW(VarExpr) ( "X" ) ) ) );

        CHECK( parse_str( "x == 42" )->equals( NEW(EqExpr) ( NEW(VarExpr) ( "x" ), NEW(NumExpr) ( 42 ) ) ) );
        CHECK( parse_str( "1+1==2+0" )->interp()->equals( NEW(BoolVal) ( true ) ) );


        CHECK_THROWS_WITH( parse_str( "- 42 == 42" ), "invalid input" );
        CHECK_THROWS_WITH( parse_str( "42 == - 42" ), "invalid input" );
        CHECK_THROWS_WITH( parse_str( "==" ), "invalid input" );
        CHECK_THROWS_WITH( parse_str( "42 = 42" ), "invalid input" );
        CHECK_THROWS_WITH( parse_str( "42 = = 42" ), "invalid input" );
    }
    
    SECTION("FunExpr")
    {
        CHECK( parse_str("_fun (x) x + 2")->equals(NEW(FunExpr) ("x", NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (2)))));
        
        //FunExpr with AddExpr
        CHECK( parse_str("_fun (x)\n"
                         "  2 + x")->equals(NEW(FunExpr) ("x", NEW(AddExpr) (NEW(NumExpr) (2), NEW(VarExpr) ("x")))));
        
        //FunExpr with nested LetExpr
        CHECK( parse_str("_fun (x)\n  x + _let y = 5\n      _in  y + 6")->equals(NEW(FunExpr) ("x", NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(LetExpr) ("y", NEW(NumExpr) (5), NEW(AddExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (6)))))));
        
        //FunExpr nested within AddExpr
        CHECK( parse_str("4 + _fun (x)\n5 * x")->equals(NEW(AddExpr) (NEW(NumExpr) (4), NEW(FunExpr) ("x", NEW(MultExpr) (NEW(NumExpr) (5), NEW(VarExpr) ("x"))))));

        //FunExpr nested within LetExpr
        CHECK( parse_str("_let x = 3\n_in  _fun (y)\ny + x")->equals(NEW(LetExpr) ("x", NEW(NumExpr) (3), NEW(FunExpr) ("y", NEW(AddExpr) (NEW(VarExpr) ("y"), NEW(VarExpr) ("x"))))));
        
        //FunExp nested within FunExpr
        CHECK( parse_str("_fun (x)\n      x * (_fun (y)\n  \n \n 4 + y)")->equals(NEW(FunExpr) ("x", NEW(MultExpr) (NEW(VarExpr) ("x"), NEW(FunExpr) ("y", NEW(AddExpr) (NEW(NumExpr) (4), NEW(VarExpr) ("y")))))));
    }
    
    SECTION("CallExpr")
    {
        //CallExpr with FunExpr and VarExpr
        CHECK( parse_str("(_fun (x)\nx + 4)(x)")->equals(NEW(CallExpr) (NEW(FunExpr) ("x", NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (4))), NEW(VarExpr) ("x"))));
        
        //CallExpr with FunExpr and AddExpr
        CHECK( parse_str("(_fun (x)\nx + 4)(3 + 8)")->equals(NEW(CallExpr) (NEW(FunExpr) ("x", NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (4))), NEW(AddExpr) (NEW(NumExpr) (3), NEW(NumExpr) (8)))));
        
        //CallExpr with FunExpr and MultExpr
        CHECK( parse_str("(_fun (y)\n y + 7)(1 * 5)")->equals(NEW(CallExpr) (NEW(FunExpr) ("y", NEW(AddExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (7))), NEW(MultExpr) (NEW(NumExpr) (1), NEW(NumExpr) (5)))));
        
        //CallExpr with FunExpr and LetExpr
        CHECK( parse_str( "(_fun (y)\n y + 7)(_let x = 9\n   \n  \n _in  x + 4)")->equals(NEW(CallExpr) (NEW(FunExpr) ("y", NEW(AddExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (7))), NEW(LetExpr) ("x", NEW(NumExpr) (9), NEW(AddExpr) (NEW(VarExpr) ("x"), NEW(NumExpr) (4))))));
        
        //CallExpr with FunExpr and NumExpr
        CHECK( parse_str("(_fun (y)\n   \n \n  \n \n \n y + 7)(0)")->equals(NEW(CallExpr) (NEW(FunExpr) ("y", NEW(AddExpr) (NEW(VarExpr) ("y"), NEW(NumExpr) (7))), NEW(NumExpr) (0))));
        
        CHECK(parse_str("_let factrl = _fun (factrl) _fun (x) _if x == 1 _then 1 _else x * factrl(factrl)(x + -1) _in  factrl(factrl)(10)")->interp()->equals( NEW(NumVal) (3628800) ) );
    }

    SECTION( "Test parse_expr with: parentheses" )
    {
        CHECK( parse_str( "(42+42)" )->equals( NEW(AddExpr) ( NEW(NumExpr) ( 42 ), NEW(NumExpr) ( 42 ) ) ) );
        CHECK( parse_str( "((42))" )->equals( NEW(NumExpr) ( 42 ) ) );
        CHECK( parse_str( "  \n\t  (  \n\t  42  \n\t  +  \n\t  42  \n\t  )  \n\t  " )
                       ->equals( NEW(AddExpr) ( NEW(NumExpr) ( 42 ), NEW(NumExpr) ( 42 ) ) ) );

        CHECK_THROWS_WITH( parse_str( "()" ), "invalid input" );
        CHECK_THROWS_WITH( parse_str( "(" ), "invalid input" );
        CHECK_THROWS_WITH( parse_str( "(4" ), "missing close parenthesis" );
        CHECK_THROWS_WITH( parse_str( "(4+2" ), "missing close parenthesis" );
        CHECK_THROWS_WITH( parse_str( "( (4+2)" ), "missing close parenthesis" );
        CHECK_THROWS_WITH( parse_str( "2 * ((4+2)" ), "missing close parenthesis" );
    }

    SECTION( "Test parse_expr with: --interp" )
    {
        CHECK( parse_str( "(3 + 5) * 6 * 1" )
                       ->interp()->equals( NEW(NumVal) ( 48 ) ) );
        CHECK( parse_str( "(7 * 7) * (9 + 2)" )
                       ->interp()->equals( NEW(NumVal) ( 539 ) ) );
        CHECK( parse_str( "_let x = 5 _in x + 5" )
                       ->interp()->equals( NEW(NumVal) ( 10 ) ) );
        CHECK( parse_str( "_let x = (_let y = 5 _in y+6) _in x+7" )
                       ->interp()->equals( NEW(NumVal) ( 18 ) ) );
        CHECK( parse_str( "_let x = 5 _in (_let y = 3 _in y + _let z = 6 _in z + 8) + x" )
                       ->interp()->equals( NEW(NumVal) ( 22 ) ) );

        CHECK( parse_str( "1==2+3" )->interp()->equals( NEW(BoolVal) ( false ) ) );
        CHECK_THROWS_WITH( parse_str( "(1==2)+3" )->interp()->equals( NEW(NumVal) (3) ), "Cannot perform add operation on BoolVal!" );
    }

    SECTION( "Test parse_expr with: --print" )
    {
        CHECK( parse_str( "1 * (2 + 3)" )
                       ->to_string() == "(1*(2+3))" );
        CHECK( parse_str( "(8 * 1) * y" )
                       ->to_string() == "((8*1)*y)" );
        CHECK( parse_str( "(3 + 5) * 6 * 1" )
                       ->to_string() == "((3+5)*(6*1))" );
        CHECK( parse_str( "(7 * 7) * (9 + 2)" )
                       ->to_string() == "((7*7)*(9+2))" );
        CHECK( parse_str( "_let x = (_let y = 5 _in y+6) _in x+7" )
                       ->to_string() == "(_let x=(_let y=5 _in (y+6)) _in (x+7))" );
        CHECK( parse_str( "_let x = 5 _in (_let y = 3 _in y + _let z = 6 _in z + 8) + x" )
                       ->to_string() == "(_let x=5 _in ((_let y=3 _in (y+(_let z=6 _in (z+8))))+x))" );

    }

    SECTION( "Test parse_expr with: --pretty-print" )
    {
        CHECK( parse_str( "1 * (2 + 3)" )
                       ->to_stringPP() == "1 * (2 + 3)" );
        CHECK( parse_str( "(8 * 1) * y" )
                       ->to_stringPP() == "(8 * 1) * y" );
        CHECK( parse_str( "(3 + 5) * 6 * 1" )
                       ->to_stringPP() == "(3 + 5) * 6 * 1" );
        CHECK( parse_str( "(7 * 7) * (9 + 2)" )
                       ->to_stringPP() == "(7 * 7) * (9 + 2)" );
        CHECK( parse_str( "_let x = 5 _in (_let y = 3 _in y + _let z = 6 _in z + 8) + x" )
                       ->to_stringPP() == "_let x = 5\n"
                                               "_in  (_let y = 3\n"
                                               "      _in  y + _let z = 6\n"
                                               "               _in  z + 8) + x" );
    }
}

TEST_CASE( "Comprehensive tests" )
{
    SECTION( "equals()" )
    {
        CHECK( ( NEW(MultExpr) ( NEW(AddExpr) ( NEW(NumExpr) ( 2 ), NEW(NumExpr) ( 2 ) ), NEW(AddExpr) ( NEW(NumExpr) ( 2 ), NEW(NumExpr) ( 2 ) ) ) )
                       ->equals( NEW(MultExpr) ( NEW(AddExpr) ( NEW(NumExpr) ( 2 ), NEW(NumExpr) ( 2 ) ),
                                           NEW(AddExpr) ( NEW(NumExpr) ( 2 ), NEW(NumExpr) ( 2 ) ) ) ) );
        CHECK( ( NEW(MultExpr) ( NEW(AddExpr) ( NEW(NumExpr) ( 0 ), NEW(NumExpr) ( 0 ) ), NEW(AddExpr) ( NEW(NumExpr) ( 2 ), NEW(NumExpr) ( 2 ) ) ) )
                       ->equals( NEW(MultExpr) ( NEW(AddExpr) ( NEW(NumExpr) ( 0 ), NEW(NumExpr) ( 0 ) ),
                                           NEW(AddExpr) ( NEW(NumExpr) ( 2 ), NEW(NumExpr) ( 2 ) ) ) ) );
    }

    SECTION( "subst()" )
    {
        // 3 * (4 * x) + -2 * ( 8 + x )->subst( 3 * ( 4 * (y + 7) ) + -2 * ( 8 + (y + 7) ) )
        CHECK( ( NEW(AddExpr) ( NEW(MultExpr) ( NEW(NumExpr) ( 3 ), NEW(MultExpr) ( NEW(NumExpr) ( 4 ), NEW(VarExpr) ( "x" ) ) ),
                          NEW(MultExpr) ( NEW(NumExpr) ( -2 ), NEW(AddExpr) ( NEW(NumExpr) ( 8 ), NEW(VarExpr) ( "x" ) ) ) ) )->subst( "x", NEW(AddExpr) (
                NEW(VarExpr) ( "y" ), NEW(NumExpr) ( 7 ) ) )->equals(
                NEW(AddExpr) ( NEW(MultExpr) ( NEW(NumExpr) ( 3 ), NEW(MultExpr) ( NEW(NumExpr) ( 4 ), NEW(AddExpr) ( NEW(VarExpr) ( "y" ), NEW(NumExpr) ( 7 ) ) ) ),
                         NEW(MultExpr) ( NEW(NumExpr) ( -2 ),
                                   NEW(AddExpr) ( NEW(NumExpr) ( 8 ), NEW(AddExpr) ( NEW(VarExpr) ( "y" ), NEW(NumExpr) ( 7 ) ) ) ) ) ) );
    }

    SECTION( "parse_expr()" )
    {
        CHECK( parse_str( "1 * (2 + 3)" )
                       ->equals( NEW(MultExpr) ( NEW(NumExpr) ( 1 ), NEW(AddExpr) ( NEW(NumExpr) ( 2 ), NEW(NumExpr) ( 3 ) ) ) ) );
        CHECK( parse_str( "(8 * 1) * y" )
                       ->equals( NEW(MultExpr) ( NEW(MultExpr) ( NEW(NumExpr) ( 8 ), NEW(NumExpr) ( 1 ) ), NEW(VarExpr) ( "y" ) ) ) );
        CHECK( parse_str( "(3 + 5) * 6 * 1" )
                       ->equals( NEW(MultExpr) ( NEW(AddExpr) ( NEW(NumExpr) ( 3 ), NEW(NumExpr) ( 5 ) ),
                                           NEW(MultExpr) ( NEW(NumExpr) ( 6 ), NEW(NumExpr) ( 1 ) ) ) ) );
        CHECK( parse_str( "(7 * 7) * (9 + 2)" )
                       ->equals( NEW(MultExpr) ( NEW(MultExpr) ( NEW(NumExpr) ( 7 ), NEW(NumExpr) ( 7 ) ),
                                           NEW(AddExpr) ( NEW(NumExpr) ( 9 ), NEW(NumExpr) ( 2 ) ) ) ) );
        CHECK( parse_str( "  \n\t  (  \n\t  8  \n\t   *   \n\t  1)  \n\t   *   \n\t  y  \n\t  " )
                       ->equals( NEW(MultExpr) ( NEW(MultExpr) ( NEW(NumExpr) ( 8 ), NEW(NumExpr) ( 1 ) ), NEW(VarExpr) ( "y" ) ) ) );
    }

    SECTION( "The void" )
    {
        PTR(Expr)  leviathan = NEW(LetExpr) ( "A", ( NEW(LetExpr) ( "B", NEW(NumExpr) (1), ( NEW(IfExpr) ( NEW(EqExpr) ( NEW(VarExpr) ("x"), (
                            NEW(IfExpr) ( NEW(EqExpr) ( NEW(VarExpr) ("y"), NEW(NumExpr) (2)), NEW(NumExpr) (3), NEW(NumExpr) (4) ) ) ),
                            NEW(NumExpr) (11), NEW(VarExpr) ("B") ) ) ) ), ( NEW(IfExpr) ( NEW(EqExpr) ( ( NEW(LetExpr) ( "C", NEW(NumExpr) (5),
                            NEW(AddExpr) ( NEW(VarExpr) ("C"), NEW(NumExpr) (6) ) ) ), ( NEW(LetExpr) ( "D", NEW(NumExpr) (7), NEW(AddExpr) ( NEW(VarExpr) ("D"),
                            NEW(NumExpr) (8) ) ) ) ), NEW(NumExpr) (4), ( NEW(LetExpr) ( "E", NEW(NumExpr) (9), NEW(AddExpr) ( NEW(VarExpr) ("E"),
                            NEW(VarExpr) ("A") ) ) ) ) ) );

        CHECK( ( leviathan )

                       ->to_stringPP() ==

               "_let A = _let B = 1\n"
               "         _in  _if   x == _if   y == 2\n"
               "                         _then 3\n"
               "                         _else 4\n"
               "              _then 11\n"
               "              _else B\n"
               "_in  _if   (_let C = 5\n"
               "            _in  C + 6) == _let D = 7\n"
               "                           _in  D + 8\n"
               "     _then 4\n"
               "     _else _let E = 9\n"
               "           _in  E + A"
        );

        // Quintuple-nested Let, with Let as RHS in all nested Lets, and every operation in Expr class as Body
        CHECK( (
                       NEW(LetExpr) ( "add",
                                NEW(LetExpr) ("mult",
                                        NEW(LetExpr) ( "eq",
                                                 NEW(LetExpr) ( "if", NEW(NumExpr) (1), NEW(IfExpr) ( NEW(EqExpr) ( NEW(NumExpr) (2), NEW(NumExpr) (3) ), NEW(NumExpr)  (4), NEW(NumExpr) (5) ) ),
                                                 NEW(EqExpr) ( NEW(NumExpr) (6), NEW(NumExpr) (7) ) ),
                                        NEW(MultExpr) ( NEW(NumExpr) (8), NEW(NumExpr) (9) ) ),
                                NEW(AddExpr) ( NEW(NumExpr) (10), NEW(NumExpr) (11) ) ) )

                       ->to_stringPP() ==

               "_let add = _let mult = _let eq = _let if = 1\n"
               "                                 _in  _if   2 == 3\n"
               "                                      _then 4\n"
               "                                      _else 5\n"
               "                       _in  6 == 7\n"
               "           _in  8 * 9\n"
               "_in  10 + 11"

        );

        // Quadruple-nested Let, with Let as RHS in all nested Lets
        CHECK( (
                       NEW(LetExpr) ( "add",
                                NEW(LetExpr) ("mult",
                                        NEW(LetExpr) ( "eq",
                                                 NEW(LetExpr) ( "if", NEW(NumExpr) (1), NEW(AddExpr) ( NEW(NumExpr)  (4), NEW(NumExpr) (5) ) ),
                                                 NEW(MultExpr) ( NEW(NumExpr) (6), NEW(NumExpr) (7) ) ),
                                        NEW(MultExpr) ( NEW(NumExpr) (8), NEW(NumExpr) (9) ) ),
                                NEW(AddExpr) ( NEW(NumExpr) (10), NEW(NumExpr) (11) ) ) )

                       ->to_stringPP() ==

               "_let add = _let mult = _let eq = _let if = 1\n"
               "                                 _in  4 + 5\n"
               "                       _in  6 * 7\n"
               "           _in  8 * 9\n"
               "_in  10 + 11"

        );

        CHECK( ( NEW(EqExpr) ( NEW(LetExpr) ( "x", NEW(NumExpr) ( 42 ), NEW(VarExpr) ( "x" ) ), NEW(LetExpr) ( "y", NEW(NumExpr) ( 84 ), NEW(VarExpr) ( "y" ) ) ) )

                       ->to_stringPP() == "(_let x = 42\n"
                                               " _in  x) == _let y = 84\n"
                                               "            _in  y" );

        CHECK( ( NEW(MultExpr) ( NEW(LetExpr) ( "x", NEW(NumExpr) ( 42 ), NEW(VarExpr) ( "x" ) ), NEW(LetExpr) ( "y", NEW(NumExpr) ( 84 ), NEW(VarExpr) ( "y" ) ) ) )

                       ->to_stringPP() == "(_let x = 42\n"
                                               " _in  x) * (_let y = 84\n"
                                               "            _in  y)" );

        CHECK( ( NEW(IfExpr) ( NEW(EqExpr) ( NEW(VarExpr) ( "x" ), NEW(NumExpr) ( 1 ) ),
                         NEW(IfExpr) ( NEW(NumExpr) ( 2 ), NEW(NumExpr) ( 3 ), NEW(NumExpr) ( 4 ) ),
                         NEW(IfExpr) ( NEW(NumExpr) ( 5 ), NEW(NumExpr) ( 6 ), NEW(NumExpr) ( 7 ) ) ) )

                       ->to_stringPP() ==

               "_if   x == 1\n"
               "_then _if   2\n"
               "      _then 3\n"
               "      _else 4\n"
               "_else _if   5\n"
               "      _then 6\n"
               "      _else 7"
        );
    }
}


