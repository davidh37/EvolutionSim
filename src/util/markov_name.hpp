#pragma once
#include "engine/common.hpp"

namespace markov_name {
    static string TERMINAL_COLOR = "\033[1;30m";
    
    void initialize(std::string file);

    std::string generateWord(int min_length, int max_length);

    void mutateWord(std::string& word);
}

