/**
* \file test_msdscript.cpp
* \brief contains test_msdscript class forward declarations and  implementations
        Performs automated fuzz testing with randomly generated test cases 
* \author Ben Baysinger
*/

#include <stdio.h>
#include <iostream>
#include <time.h>
#include "exec.hpp"

static std::string random_add_expr_string();
static std::string random_mult_expr_string();
static std::string random_variable();
static std::string rand_var ();
static std::string random_expr_string();
static std::string random_num_expr_string();
static std::string random_let_expr_string();
static std::string random_body_of_let(std::string lhs);



int main(int argc, char **argv) {
    srand (static_cast<unsigned int>(clock()));
    
    if ( argc == 2 ) {
        
        const char * const interp_argv[] = { argv[1], "--interp" };
        const char * const print_argv[] = { argv[1], "--print" };
        const char * const pretty_print_argv[] = { argv[1], "--pretty-print" };

        std::cout << "arg: " << argv[1] << std::endl;
        
        for (int i = 0; i < 100; i++) {
            std::string in = random_expr_string();
            std::cout << "Trying"<< in << "\n";
            ExecResult interp_result = exec_program(2, interp_argv, in);
            ExecResult print_result = exec_program(2, print_argv, in);
            ExecResult interp_again_result = exec_program(2, interp_argv, print_result.out);
            
            if ( interp_result.exit_code != 0 ) {
                std::cout << ".EXIT_CODE: " << interp_result.exit_code << std::endl;
                std::cout << ".ERR: " << interp_result.err << std::endl;
                std::cout << ".OUT: " << interp_result.out << std::endl;
            }

            ExecResult pretty_print_result = exec_program(2, pretty_print_argv, in);
            ExecResult interp_again_result2 = exec_program(2, interp_argv, pretty_print_result.out);
            
            if (interp_again_result.out != interp_result.out) {
                throw std::runtime_error("different result for printed");
            }
            if (interp_again_result2.out != interp_result.out) {
                throw std::runtime_error("different result for printed");
            }
        }
    }
    
    if ( argc == 3 ) {
        
        const char * const interp1_argv[] = { argv[1], "--interp" };
        const char * const interp2_argv[] = { argv[2], "--interp" };
        
        const char * const print1_argv[] = { argv[1], "--print" };
        const char * const print2_argv[] = { argv[2], "--print" };
        
        const char * const prettyPrint1_argv[] = { argv[1], "--pretty-print" };
        const char * const prettyPrint2_argv[] = { argv[2], "--pretty-print" };
        
        for (int i = 0; i < 100; i++) {
            
            std::string in = random_expr_string();
            ExecResult interp1_result = exec_program(2, interp1_argv, in);
            ExecResult interp2_result = exec_program(2, interp2_argv, in);
            ExecResult print1_result = exec_program(2, print1_argv, in);
            ExecResult print2_result = exec_program(2, print2_argv, in);
            ExecResult prettyPrint1_result = exec_program(2, prettyPrint1_argv, in);
            ExecResult prettyPrint2_result = exec_program(2, prettyPrint2_argv, in);
            if (interp1_result.out != interp2_result.out){
                std::cout << "Trying " << in << "\n";
                std::cout<<"Interp result: " << interp1_result.out << "\n" << "Tester :       " << interp2_result.out << "\n";
                throw std::runtime_error("different results");

            }
            if (print1_result.out != print2_result.out){
                std::cout << "Trying " << in << "\n";
                std::cout<<"Print result: " << print1_result.out << "\n" << "Tester :      " << print2_result.out << "\n";
                throw std::runtime_error("different results");

            }
            if (prettyPrint1_result.out != prettyPrint2_result.out){
                std::cout << "Trying " << in << "\n";
                std::cout<<"PrettyPrintResult:\n" <<prettyPrint1_result.out << "\n" << "TesterPrettyPrint:\n" << prettyPrint2_result.out << "\n";
                throw std::runtime_error("different results");
            }
        }
    }
return 0;
}

/**
* \brief produces a random var string to test
* \return random VarExpr as string
*/
std::string rand_var () {
    switch (rand() % 3){
        case 0: {
            std::string s = "";
            for ( int i = 0; i < rand()%6; i++){
                s += (rand() % 26 + 'a' );
            }
            return s;
        }
        case 1:
            return "x";
        case 2:
            return "y";
    }
    return "";
}

/**
* \brief produces a random AddExpr string to test. Recurses until variable or number is encountered.
* \return random AddExpr as string
*/
std::string random_add_expr_string() {
   
        return random_expr_string() + "+" + random_expr_string();
}

/**
* \brief produces a random MultExpr string to test. Recurses until variable or number is                 *      encountered.
* \return random MultExpr as string
*/
std::string random_mult_expr_string() {
  
        return random_expr_string() + "*" + random_expr_string();
}

/**
* \brief randomly generates num, var, add, mult, or let expressoin. Recurses until variable or number is                              encountered. Start of recursive chain
* \return random expression as string
*/
std::string random_expr_string() {
    
    std::string errorMessage = "Error in random_expr \n";
    if ( (rand()%10) < 4 ) {
        switch (rand() % 5){
            case 0:
                return random_add_expr_string();
            case 1:
                return random_mult_expr_string();
            case 2:
                return random_let_expr_string();
            case 3:
                return rand_var();
            case 4:
                return '-' + random_num_expr_string();
        }
    }
    return std::to_string(rand());
}

/**
* \brief produces a random LetExpr string to test. Recurses until variable or number is encountered. Binds lhs variable       in body of let expression
* \return random LetExpr as string
*/
std::string random_let_expr_string() {
    
    std::string lhs = rand_var();
    
    return "_let " + lhs + "=" + random_expr_string() + " _in " + random_body_of_let(lhs);//that contains var
}

/**
* \brief produces a random NumExpr string to  test.
* \return random NumExpr as string
*/
std::string random_num_expr_string() {
    
    return std::to_string(rand());
   
}

/**
* \brief produces body of let expression with param lhs contained within
* \param lhs string varaible to be binded to in body of let expression
* \return random expression to act as body of let expression
*/
std::string random_body_of_let(std::string lhs) {
    
    std::string errorMessage = "Error in random_expr \n";
    
    switch (rand() % 4){
        case 0:
            return  lhs + "+" + random_expr_string();
        case 1:
            return lhs + "*" + random_expr_string();
        case 2:
            return random_expr_string() + "+" + lhs;
        case 3:
            return random_expr_string() + "*" + lhs;
    }
    return errorMessage;
    
}


