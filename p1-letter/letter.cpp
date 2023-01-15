//  EECS-281 Project 1 Letterman Reboot
//  letter.cpp
//
//  JunCheng An; jamiean
//  Version 4.0 Last modified on 5/4/2018
//
//  Created by JunCheng An on 5/1/18.
//  Copyright © 2018 JunCheng An. All rights reserved.

#include "final_word.h"

int main(int argc, char *argv[]) {
    
    std::ios_base::sync_with_stdio(false);
    
    /*
    // for running w/ xcode
    #ifdef __APPLE__
    if(!freopen("test-9-goo-good-slcm.txt", "r", stdin)) {
        std::cerr << "THERE IS NO INPUT!" << std::endl;
        exit(1);
    }
    //freopen("myoutput.txt", "w", stdout);
    #endif
     */
    
    Letter letter_man;
    
    
    try {
        // if  command line argument error  -> throw arg_error
        //     helper function is called    -> throw HelpFunctioncalled
        //     nothing special              -> throw nothing
        letter_man.read_com(argc, argv);
    
        letter_man.read_file();
    
        // if  begin word = end word        -> throw Begin_end_same
        //     begin word and end word not exist   -> throw arg_error
        //     nothing special              -> throw nothing
        letter_man.check_b_e_validity();
        
        letter_man.find_similar();
        
        letter_man.double_mode_print();
    }
    
    // command line argument error
    catch (arg_error &arg) {
        if (arg.err_type == 1) {
            std::cerr << "Invalid command line arguments input!" << std::endl;
        }
        if (arg.err_type == 2) {
            std::cerr << "Begin or End word does not exist!" << std::endl;
        }
        exit(1);
    }
    catch (HelpFunctionCalled &help) { exit(0);}        // help function called
    catch (Begin_end_same &be) { return 0;}             // begin == end
    
    
    return 0;
    
    
    //letter_man.debug_print_words();
    
    /*
    // test function for similar_checking
    std::string a = "bi";
    std::string b = "i";
    std::cout << letter_man.similar_checking(a, b);
    */

    
    /*
    // test function for find_pos_let
    std::string a = "bi";
    std::string b = "i";
    letter_man.find_pos_let(a, b);
     */
}
