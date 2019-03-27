#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <vector>
#include <sys/socket.h>
#include <string>
#include "simpleServer.h"
#include <pthread.h>

#define DEFAULT_DICTIONARY "words"

using namespace std;

//Global Variables
vector<string> words;


//function headers
string compare(string str1, vector<string> words);




int main(int argc, char* argv[]){


    int portNumber = 3207; // default port number
    string dictionary = DEFAULT_DICTIONARY;

    //doing some house keeping over here, checking for port number and dictionary.
    if (argc == 1){
        printf("No port number entered. Switching to default port 3207\n");
    } else {
        portNumber = atoi(argv[1]);
    }

    if (argc < 3) {
        printf("No dictionary name entered, switching to DEFAULT_DICTIONARY\n");
    } else{
        dictionary = argv[2];
    }


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

    cout << "Enter a word\n";
    string input;
    cin >> input;

    cout << compare(input, words);

    /* //testing the vector
    for(int x = 0; x<words.size(); x++){
        cout << words[x] << endl;
    }*/

    
}


string compare(string str1, vector<string> words){
    string str = "";
    for(int i = 0; i<str1.size(); i++){
        if(isalpha(str1[i]))
            str += str1[i];
    }

    string ret = str1 + " Misspelled\n";
    for(int i = 0; i<words.size();i++){
        if(str.compare(words[i]) == 0){
            ret = str1 + "  OK\n";
            break;
        }
    }
    return ret;
}