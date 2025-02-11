/**
* \mainpage MSDScript
* \author Ben Baysinger
* \date 01-16-2024
*/


#include "cmdline.hpp"
#include "parse.hpp"


int main( int argc, char **argv ) {
    
    try {
        
        run_mode_t mode = use_arguments(argc,argv);
        switch (mode){
            case do_nothing:
                break;
            case do_interp:
                executeInterp();
                break;
            case do_print:
                executePrint();
                break;
            case do_pretty_print:
                executePrettyPrint();
                break;
        }
        
        return 0;
    } catch (std::runtime_error exn) {
        std::cerr << exn.what() << "\n";
        return 1;
      }
}


