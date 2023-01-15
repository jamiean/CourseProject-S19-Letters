//  EECS-281 Project 1 Letterman Reboot
//  word.cpp
//
//  JunCheng An; jamiean
//  Version 4.0 Last modified on 5/4/2018
//
//  Created by JunCheng An on 5/1/18.
//  Copyright © 2018 JunCheng An. All rights reserved.


#include <getopt.h>
#include <algorithm>
#include "final_word.h"


// Effects: read command line arguments, check command errors
//                change the indicators
//          if  command line argument error  -> throw arg_error
//              helper function is called    -> throw HelpFunctioncalled
//              nothing special              -> throw nothing
void Letter::read_com(int argc, char **argv) {
    int command = 0;
    int option_ind = 0;
    option long_option[] = {
        {"stack", no_argument, nullptr, 's'},
        {"queue", no_argument, nullptr, 'q'},
        {"change", no_argument, nullptr, 'c'},
        {"swap", no_argument, nullptr, 'p'},
        {"length", no_argument, nullptr, 'l'},
        {"output", required_argument, nullptr, 'o'},
        {"begin", required_argument, nullptr, 'b'},
        {"end", required_argument, nullptr, 'e'},
        {"help", no_argument, nullptr, 'h'},
        { nullptr, 0, nullptr, '\0' }
    };
    
    while ((command = getopt_long(argc, argv, "b:e:o:sqcplh"
                                 , long_option, &option_ind)) != -1) {
        switch (command) {
            case 'h':
                std::cout << "Helper function should appear here lol!" << '\n';
                throw HelpFunctionCalled();
            case 'b':
                begin = std::string(optarg);
                break;
            case 'e':
                end = std::string(optarg);
                break;
            case 's':
                if (queue_on) throw arg_error(1);
                stack_on = true;
                break;
            case 'q':
                if (stack_on) throw arg_error(1);
                queue_on = true;
                break;
            case 'c':
                change_on = true;
                break;
            case 'p':
                swap_on = true;
                break;
            case 'l':
                length_on = true;
                break;
            case 'o':
                // command line ERROR checking -> only M or W would behind -o
                if (std::string(optarg) == "M") modif_mode = true;
                else if (std::string(optarg) == "W") word_mode = true;
                else throw arg_error(1);
            default:
                break;
        }
    }
    
    // command line ERROR checking
    // 1 -> check at least one of s or q is specified
    // 2 -> check not both s and q on
    // 3 -> check at least one of c, s, l is specified
    // 4 -> check begin and end words exist
    // 5 -> check if length mode off, begin and end must diff in length
    // 6 -> check at least one of m or w is specified
    // 7 -> if non of w or m specified, turn on word mode
    if (!stack_on && !queue_on) throw arg_error(1);
    if (stack_on && queue_on) throw arg_error(1);
    if (!change_on && !swap_on && !length_on) throw arg_error(1);
    if (begin == "" || end == "") throw arg_error(1);
    if (!length_on && (end.length() != begin.length())) throw arg_error(1);
    if (modif_mode && word_mode) throw arg_error(1);
    if (!modif_mode && !word_mode) word_mode = true;
}




// Effects: read the input file
//          put the useful words into dictionary
//          check if end and begin words are in the dictionary
void Letter::read_file(){
    std::string mode = "";
    size_t word_size = 0;             // read word size for reserving vector
    int count = 0;                    // index tracing
    std::cin >> mode >> word_size;
    
    std::string input_line;
    getline(std::cin, input_line);           // trash line
    
    // if length mode is not on, we record the length of begin word
    // ALL the words that not of the same length of begin word
    //               will no be added to the dictionary
    if (!length_on) begin_length = int(begin.length());
    
    // Simple mode
    // Reserve vectors and insert words into the container
    if (mode == "S") {
        words_n_prev.reserve(word_size);
        while (getline(std::cin, input_line)) {
            if (input_line[0] != '/') push_check(input_line, count);
        }
    }
    
    // Complex mode
    // Reserve twice size of the word_size of the vector
    // Read the special marks and do corresponding actions
    // Push the resulting words into dictionary
    if (mode == "C") {
        words_n_prev.reserve(word_size * 2);
        std::string::iterator loop;
        int spec_num = 0;
        
        // find the iterator and index of the first special mark
        while (getline(std::cin, input_line)) {
            if (input_line[0] != '/') {
                loop = input_line.begin();
                while(loop != input_line.end()) {
                    if (*loop == '[') break;
                    if (*loop == '!') break;
                    if (*loop == '?') break;
                    if (*loop == '&') break;
                    spec_num ++;
                    ++ loop;
                }
                
                // NO special marks
                // push the word directly into the dicionary
                if (loop == input_line.end()) {
                    push_check(input_line, count);
                }
                else {
                    switch (*loop) {
                            // "?" Question mark, double the word before it
                            // FIRST change the question mark into the word before it
                            // THEN  Copy the string and then delete the "?" position
                            // FINALLY Push both words into dictionary
                        case '?': {
                            input_line[spec_num] = input_line[spec_num - 1];
                            std::string temp = input_line;
                            input_line.erase(loop);
                            push_check(input_line, count);
                            push_check(temp, count);
                            break;
                        }
                            
                            // "!" Exclamation mark, swap the two words before it
                            // FIRST erase the mark position push one word
                            // THEN  swap two chars and push again
                        case '!': {
                            input_line.erase(loop);
                            push_check(input_line, count);
                            std::swap(input_line[spec_num - 2], input_line[spec_num - 1]);
                            push_check(input_line, count);
                            break;
                        }
                            
                            // "&" AND mark, reverse the whole string
                            // Pop the final "&" mark, push and reverse and push again
                        case '&': {
                            input_line.pop_back();
                            push_check(input_line, count);
                            std::reverse(input_line.begin(), input_line.end());
                            push_check(input_line, count);
                            break;
                        }
                            
                            // "[]" , insert the chars between them
                            // FIRST extract all the chars between '[]', store in vector
                            // THEN  delete all '[]' part
                            // FINALLY put the chars one by one to words and push in dictionary
                        case '[': {
                            std::vector<char> middle;
                            int i = 0;
                            ++ loop;                    // loop -> the first inserted letter
                            while(*loop != ']') {
                                middle.push_back(*loop);
                                ++ loop;
                                i ++;
                            }
                            input_line.erase(loop - i, loop + 1);
                            for (const char &insert: middle) {
                                input_line[spec_num] = insert;
                                push_check(input_line, count);
                            }
                            break;
                        }
                        default:
                            break;
                    }
                }
                spec_num = 0;
                
                /*
                // deleted, previously written, for possible future use
                 if (*loop == '?')
                 if (*loop == '!')
                 if (*loop == '&')
                 if (*loop == '[')
                    int i = 1;
                    while ((loop + i) != input_line.end()) {
                        input_line[spec_num + i] = *(loop + i);
                        i ++;
                    }
                    input_line.resize(spec_num + i);//
                }*/
            
            }
        }
    }
}



// Effects: check if begin word and end word validity
//          if  begin word = end word        -> throw Begin_end_same
//              begin word and end word not exist   -> throw arg_error
//              nothing special              -> throw nothing
void Letter::check_b_e_validity() {
    if (begin_index == UNUSED || end_index == UNUSED) throw arg_error(2);
    if (begin_index == end_index) {
        std::cout << "Words in morph: 1" << '\n'
                  << words_n_prev[begin_index].first << '\n';
        throw Begin_end_same();
    }
}


// Effects: find a similar path from begin words to end words
void Letter::find_similar() {
    
    // DEALING with the first begin word
    st_n_que.push_back(begin_index);
    words_n_prev[begin_index].second = FIRST;
    int current_ind = 0;                        // current word's index
    
    // When deque is empty or end word is found, we stop the loop
    while (!st_n_que.empty() && !found_end) {
        if (stack_on) {
            current_ind = st_n_que.back();
            st_n_que.pop_back();
        }
        if (queue_on) {
            current_ind = st_n_que.front();
            st_n_que.pop_front();
        }
        std::string &current = words_n_prev[current_ind].first;
        int count = 0;                          // for recording the loop word's index
        for (std::pair<std::string, int> &w: words_n_prev) {
            if (w.second == UNUSED && similar_checking(current, w.first)) {
                st_n_que.push_back(count);
                w.second = current_ind;
                count_checked ++;
                
                // if we found the end index
                // END the loop immediately and go to next step
                if (count == end_index) {
                    found_end = true;
                    break;
                }
            }
            count ++;
        }
    }
}






// Effects: Print the output according to the mode chosen
void Letter::double_mode_print() {
    // if the end word do not have previous index
    // then there's no similar route, print no solution message
    if (words_n_prev[end_index].second == UNUSED) {
        std::cout << "No solution, " << count_checked
                  << " words checked." << '\n';
        return;
    }
    
    // use a vector to back track the index to print
    // push index into the vector and reverse to print
    std::vector<int> back_track;
    int temp = end_index;
    while (temp != FIRST) {
        back_track.push_back(temp);
        temp = words_n_prev[temp].second;
    }
    int size = int(back_track.size());
    std::cout << "Words in morph: " << size << '\n';
    
    // W mode. only back track to print words
    if (word_mode) {
        for (int i = size - 1; i >= 0; i--) {
            std::cout << words_n_prev[back_track[i]].first << '\n';
        }
    }
    
    // M mode. Print in a special mode by a helper function
    if (modif_mode) {
        std::cout << words_n_prev[back_track[size - 1]].first << '\n';
        for (int i = size - 1; i >= 1; i--) {
            find_pos_let(words_n_prev[back_track[i]].first,
                         words_n_prev[back_track[i - 1]].first);
        }
    }
}






/***********************************************************************
 *                   BELOW ARE THE HELPER FUNCTIONS                    *
************************************************************************/


// Require : str_input is a valid string
//           pos is a valid integer which is the current index
// Modifier: word_n_prev
// Effect  : check if the length mode on and check if the word's worth
//           push the word into word_n_prev
//           check begin and end words exists or not
void Letter::push_check(std::string &str_input, int &pos){
    if (!length_on && int(str_input.length()) != begin_length) return;
    words_n_prev.push_back(std::make_pair(str_input, UNUSED));
    if (!f_begin && str_input == begin) {
        begin_index = pos;
        f_begin = true;
    }
    if (!f_end && str_input == end) {
        end_index = pos;
        f_end = true;
    }
    pos ++;
}




// Require : lhs and rhs are valid string
// Modifier: lhs, rhs
// Effect  : find if the two words are similar
//           if similar, return true
//           if not, return false
bool Letter::similar_checking(std::string &lhs, std::string &rhs) {
    int length_l = int(lhs.length());
    int length_r = int(rhs.length());
    
    // two words are of same lenth
    // then only swap and change mode would be useful
    if (length_l == length_r) {
        if (!swap_on && !change_on) return false;
        else {
            int f_diff = 0;
            while (lhs[f_diff] == rhs[f_diff]) f_diff ++;
            
            // if there's a second diff
            // then we can't use change mode to go between words
            if (change_on) {
                int second_diff = f_diff + 1;
                while (second_diff != length_r &&
                       lhs[second_diff] == rhs[second_diff]) second_diff ++;
                if (length_l == second_diff) return true;
            }
            
            // only enter swap if change mode do not work or can't similar by c
            // if first diff is the last word and enter swap, then not similar
            // FIRST we check if the first and second diff satisfy swap
            // THEN  we check if the following chars are same
            // if any of them is violated, return false
            if (swap_on) {
                if (f_diff == length_l - 1) return false;
                if (lhs[f_diff] == rhs[f_diff + 1] && rhs[f_diff] == lhs[f_diff + 1]) {
                    f_diff += 2;
                    while (f_diff != length_l) {
                        if (rhs[f_diff] != lhs[f_diff]) return false;
                        f_diff ++;
                    }
                    return true;
                }
            }
        }
        return false;
    }
    
    
    // two words are of different length
    // only length mode would be useful
    else {
        if (!length_on) return false;
        else {
            int f_diff = 0;
            
            // FIRST find the first difference
            // THEN add the longer word's index by 1
            // FINALLY check if corresponding index are same after the addition
            // if same, then similar. Same logic for both case
            if (length_l - length_r == 1) {
                while (f_diff != length_r &&
                       lhs[f_diff] == rhs[f_diff]) f_diff ++;
                while (f_diff != length_r &&
                       lhs[f_diff + 1] == rhs[f_diff]) f_diff ++;
                if (f_diff == length_r) return true;
            }
            if (length_r - length_l == 1) {
                while (f_diff != length_l && lhs[f_diff] == rhs[f_diff]) f_diff ++;
                while (f_diff != length_l && lhs[f_diff] == rhs[f_diff + 1]) f_diff ++;
                if (f_diff == length_l) return true;
            }
        }
        return false;
    }
    return false;
}





// Require : lhs and rhs are two similar valid string
// Modifier: cout
// Effect  : will find how lhs change to rhs and print out the change
//           if change , print c,pos,letter
//           if swap   , print s,pos
//           if insert , print i,pos,letter
//           if delete , print d,pos
void Letter::find_pos_let(std::string &lhs, std::string &rhs) {
    int length_l = int(lhs.length());
    int length_r = int(rhs.length());
    int f_diff = 0;
    
    // if they are of same length, then only change or swap
    if (length_l == length_r) {
        while (lhs[f_diff] == rhs[f_diff]) f_diff ++;
        
        // if first difference is the last OR
        // the one after the first difference is the same
        // then it must be change, and output change stats
        if (f_diff == length_r || lhs[f_diff + 1] == rhs[f_diff + 1]) {
            std::cout << "c," << f_diff << "," << rhs[f_diff] << '\n';
        }
        // otherwise it can only be swap
        else std::cout << "s," << f_diff << '\n';
    }
    
    // original word is longer, delete case
    else if (length_l > length_r) {
        while (f_diff != length_r && lhs[f_diff] == rhs[f_diff]) f_diff ++;
        std::cout << "d," << f_diff << '\n';
    }
    // only case left is insert
    else {
        while (f_diff != length_l && lhs[f_diff] == rhs[f_diff]) f_diff ++;
        std::cout << "i," << f_diff << "," << rhs[f_diff] << '\n';
    }
}




// Modifier: cout
// Effect  : print out the string part of pair vector
//           FOR DEBUGING
void Letter::debug_print_words() {
    for (const std::pair<std::string, int> &w: words_n_prev) {
        std::cout << w.first << '\n';
    }
}
