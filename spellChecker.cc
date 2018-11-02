#include <iostream>
#include <fstream>
#include <map>
#include <vector>

using namespace std;

bool compare(string str, vector<string> words){
    bool ret = false;

    for(int i = 0; i<words.size();i++){
        //cout << words[i] << endl;
        if(str.compare(words[i]) == 0){
            cout << words[i] << endl;
            ret = true;
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
    //getline(cin,str);
    bool result = compare(str,words);

    } else {
        cout << "Dictionary not found" << endl;
    }
}
