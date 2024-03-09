/**
* \file cmdline.cpp
* \brief contains cmdline class implemenations
      purpose of class is to sets arguments that can follow after ./<program>
      gives description of what the arguments are and what they do.
* \author Ben Baysinger
*/

#define CATCH_CONFIG_RUNNER
#include "cmdline.hpp"
#include <iostream>




/**
* \brief sets aruments that can follow after ./<program>
 * --help shows what arguments are allowed
 * --test runs catach tests
 * --interp returns the operative value of what expression is passed
 * --print returns a string value of what expression is passed
 * --pretty-print returns a string value of what expression is passed
* \param argc numbeer of arguments
* \param argv array storing arguments ran
* \returns enum type of argument passed
*/
run_mode_t use_arguments( int argc, char **argv) {
    
    bool hasSeen = false;
    run_mode_t mode = do_nothing;

    for( int i = 1; i < argc; i++ ) {
        if (std::strcmp(argv[i], "--help") ==0) {
            std::cout <<"Arguments allowed are:\n " << "--help: shows allowed arguments with explanation of what they do\n"
            << " --test: checks if test has passed and runs tests \n"
            << " --interp: returns the operative value of what expression is passed\n"
            << " --print: returns a string value of what expression is passed\n"
            << " --pretty-print: returns a string value of what expression is passed\n";
            exit(0);
        }
        else if ( std::strcmp(argv[i], "--test") ==0) {

            if (!hasSeen) {
                if ( Catch::Session().run(1, argv) != 0) {
                    exit(1);
                }
                hasSeen = true;
            }
            else {
                std::cerr << "Error test has already been seen \n";
                exit(1);
            }
        }
        else if ( std::strcmp(argv[i], "--interp") == 0) {
            mode = do_interp;
        }
        else if (std::strcmp(argv[i], "--print") == 0 ) {
            mode = do_print;
        }
        else if (std::strcmp(argv[i], "--pretty-print") == 0 ) {
            mode = do_pretty_print;
        }
        
    
        else{
            std::cerr<<"Invalid argument \n";
            exit(1);
        }
    
    }
    return mode;
}
