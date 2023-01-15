//  EECS-281 Project 1 Letterman Reboot
//  word.h
//
//  JunCheng An; jamiean
//  Version 4.0 Last modified on 5/4/2018
//
//  Created by JunCheng An on 5/1/18.
//  Copyright © 2018 JunCheng An. All rights reserved.

#ifndef letter_h
#define letter_h

#include <iostream>
#include <deque>
#include <vector>
#include <utility>
#include <string>

// EXCEPTIONS for special case and error checking
class HelpFunctionCalled{};
class Begin_end_same{};
class arg_error{
public:
    arg_error(int type_in): err_type(type_in) {}
    int err_type = 0;
};




class Letter {
public:
    
    // Effects: read command line arguments, check command errors
    //                change the indicators
    //          if  command line argument error  -> throw arg_error
    //              helper function is called    -> throw HelpFunctioncalled
    //              nothing special              -> throw nothing
    void read_com(int argc, char *argv[]);
    
    // Effects: read the input file
    //          put the useful words into dictionary
    //          check if end and begin words are in the dictionary
    void read_file();
    
    // Effects: check if begin word and end word validity
    //          if  begin word = end word        -> throw Begin_end_same
    //              begin word and end word not exist   -> throw arg_error
    //              nothing special              -> throw nothing
    void check_b_e_validity();
    
    // Effects: find a similar path from begin words to end words
    void find_similar();
    
    // Effects: Print the output according to the mode chosen
    void double_mode_print();
    
    
private:
    
    // const variable to indicate if the word is used or found
    const int UNUSED = -253;
    const int FIRST = -1;
    
    // MODE switch on-off indicator
    bool stack_on = false;
    bool queue_on = false;
    bool change_on = false;
    bool swap_on = false;
    bool length_on = false;
    bool word_mode = false;
    bool modif_mode = false;
    
    // variables to record the begin word and end word
    std::string begin = "";
    std::string end = "";
    int begin_index = UNUSED;
    int end_index = UNUSED;
    bool f_begin = false;
    bool f_end = false;
    int begin_length = 0;
    
    // count_checked is for number of check words when no solution
    // found_end     is for ending the loop earlier if found end word
    // special_check is 
    int count_checked = 1;
    bool found_end = false;

    
    // A container for finding similar
    // which is apply to both stack and queue
    std::deque<int> st_n_que;
    
    
    // Vectors of pair, first string and an integer
    // first of the pair is word
    // second of the pair is previous index
    // previous index is initialized to -1
    std::vector<std::pair<std::string, int>> words_n_prev;
    
    

/***********************************************************************
 *                   Helper Function Declaration                       *
************************************************************************/

    
    // Require : str_input is a valid string
    //           pos is a valid integer which is the current index
    // Modifier: word_n_prev
    // Effect  : check if the length mode on and check if the word's worth
    //           push the word into word_n_prev
    //           check begin and end words exists or not
    void push_check(std::string &str_input, int &pos);
    
    
    // Require : lhs and rhs are valid string
    // Modifier: lhs, rhs
    // Effect  : find if the two words are similar
    //           if similar, return true
    //           if not, return false
    bool similar_checking(std::string &lhs, std::string &rhs);
    
    
    // Require : lhs and rhs are two similar valid string
    // Modifier: cout
    // Effect  : will find how lhs change to rhs and print out the change
    //           if change , print: c, pos ,letter
    //           if swap   , print: s, pos
    //           if insert , print: i ,pos ,letter
    //           if delete , print: d ,pos
    void find_pos_let(std::string &lhs, std::string &rhs);
    
    
    // Modifier: cout
    // Effect  : print out the string part of dictionary vector
    //           FOR DEBUGING
    void debug_print_words();
    

    
};



#endif /* letter_h */
