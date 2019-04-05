#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

vector<string> words;
#define DEFAULT_DICTIONARY "/usr/share/dict/words"

string compare(string str1, vector<string> words){

    string str = "";
    for(int i = 0; i<str1.size(); i++){
        if(isalpha(str1[i]))
            str += str1[i];
    }
    string ret = str + " Misspelled\n";
    for(int i = 0; i<words.size();i++){
        if(str.compare(words[i]) == 0){
            ret = str + " OK\n";
            break;
        }
    }
    return ret;
}

int main(){
    string dictionary = DEFAULT_DICTIONARY;

        //Reading the dictionary and loading it into the queue.
    std::ifstream file(DEFAULT_DICTIONARY);
    if (file.is_open()) {
    std::string line;

        //reading and populating data into the vector
        while (getline(file, line)) {
            words.push_back(line.c_str());
        }
        file.close();
    }

    cout << "Input a word to test the compare function: " << endl;
    string str;
    cin >> str;
    cout << compare(str, words);
}

