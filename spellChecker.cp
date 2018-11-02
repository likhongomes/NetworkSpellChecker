#include <iostream>
#include <fstream>
#include <map>
#include <vector>

using namespace std;

string compare(string str, vector<string> words){
    string ret = "Misspelled";

    for(int i = 0; i<words.size();i++){
        if(str.compare(words[i]) == 0){
            //cout << words[i] << endl;
            ret = "OK";
        }
    }
    return ret;
}

int main(){

    vector<string> words;

    std::ifstream file("words");
    if (file.is_open()) {
    std::string line;

    //reading and populating data into the linked
    while (getline(file, line)) {

        words.push_back(line.c_str());

    }
    file.close();

    string str = "";
    cout << "Input a word to look up: ";
    cin >> str;
    string result = compare(str,words);
    cout << result << endl;

    } else {
        cout << "Dictionary not found" << endl;
    }
}
