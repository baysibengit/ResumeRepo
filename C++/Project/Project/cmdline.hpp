/**
* \file cmdline.h
* \brief cmdline class delarations
*/

#include "catch.hpp"

/*! \brief custom enum to interpret  command line arguments
* Can be either nothing, interp, print, and pretty print
*/
typedef enum {

  do_nothing,
  do_interp,
  do_print,
  do_pretty_print

} run_mode_t;


#ifndef HOMEWORK1SMSDSCRIPT_CMDLINE_H
#define HOMEWORK1SMSDSCRIPT_CMDLINE_H


//void use_arguments( int argc, char **argv);
run_mode_t use_arguments( int argc, char **argv);


#endif //HOMEWORK1SMSDSCRIPT_CMDLINE_H

