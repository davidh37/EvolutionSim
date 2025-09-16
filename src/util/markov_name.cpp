#include "util/markov_name.hpp"

#include <fstream>
#include <ctime>


namespace markov_name {

    struct MarkovState {
        char character;
        int occurences[27];
        float transition[27];
    };

    static MarkovState states[26];

    static void createChain(){
        for(int i = 0; i < 26; i++){
            states[i].character = 'a' + i;
            for(int j = 0; j < 26; j++){
                states[i].transition[j] = 1.0f / 27.0f;
                states[i].occurences[j] = 0;
            }
        }
    }

    static void populateChain(std::vector<std::string> &words){
        for(size_t i = 0; i < words.size(); i++){
            std::string &word = words[i];
            bool only_letter = word.find_first_not_of("abcdefghijklmnopqrstuvwxyz") == std::string::npos;
            if(!only_letter){
                std::cout << TERMINAL_COLOR + "[markov_name] ERROR! Invalid token " << word << TERMINAL_CLEAR << std::endl;
                for(size_t j = 0; j < word.size(); j++){
                    std::cout << j << " " << (int)word[j] << std::endl;
                }
                abort();
            }
            assert(only_letter);

            int index = 0;
            int index2 = 0;
            for(size_t j = 0; j < word.size()-1; j++){
                index = word[j] - 'a';
                index2 = word[j+1] - 'a';
                states[index].occurences[index2] += 1;
            }
            states[index2].occurences[26] += 1;
        }

        for(int s = 0; s < 26; s++){
            int total_occurences = 0;
            for(int t = 0; t < 27; t++){
                total_occurences += states[s].occurences[t];
            }
            if(total_occurences == 0){
                continue;
            }
            for(int t = 0; t < 27; t++){
                states[s].transition[t] = states[s].occurences[t] / (float)total_occurences;
            }
            
        }
        for(int s = 0; s < 26; s++){
            for(int t = 1; t < 27; t++){
                states[s].transition[t] = states[s].transition[t] + states[s].transition[t-1];            
            }
        }

        //for(int s = 0; s < 26; s++){
            //std::cout << "state " << (char)(s + 'a') << std::endl;
            //for(int t = 0; t < 27; t++){
                //std::cout << "transition " << (char)(t + 'a') << " = " << states[s].transition[t] << std::endl;
            //}
        //}


    }

    std::string generateWord(int min_length, int max_length){
        int last_char = rand() % 26;
        std::string result = "";
        result += (char)(last_char + 'a');

        for(int i = 0; i < max_length; i++){
            float r = (rand() / (float) RAND_MAX);
            for(int t = 0; t < 27; t++){
                if(r <= states[last_char].transition[t]){
                    if(t == 26){
                        if((int)result.size() < min_length){
                            result = generateWord(min_length, max_length);
                        }
                        return result;
                    }
                    last_char = t;
                    result.push_back((char) last_char + 'a');
                    break;
                }
            }
        }
        return result;
    }

    void mutateWord(std::string& word) {
        word = generateWord(word.size(), word.size());
    }



    void initialize(std::string file){
        std::ifstream inputFile(file);
        if (!inputFile) {
            std::cerr << "Error: Could not open input file." << std::endl;
            abort();
        }
        std::vector<std::string> firstWords;
        std::string line;

        while (std::getline(inputFile, line)) {
            // Find the position of the first space or tab
            size_t pos = line.find_first_of(" \t");

            // If a space or tab was found, extract the first word
            if (pos != std::string::npos) {
                std::string firstWord = line.substr(0, pos);
                firstWords.push_back(firstWord);
            }
        }

        inputFile.close();

        createChain();
        populateChain(firstWords);

        std::cout << TERMINAL_COLOR << "[markov_name] intitialized" << TERMINAL_CLEAR << std::endl;
    }
}