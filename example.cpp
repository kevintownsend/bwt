#include <vector>
#include <string>
#include <cstring>
#include <cstring>
#include <fstream>
#include "bwt.hpp"

using namespace std;
using namespace townsend::algorithm;

int main(int argc, char* argv[]){
    vector<char> original;
    if(argc == 3){
        if(string(argv[1]) == "-f"){
            original.clear();
            ifstream file(argv[2]);
            while(!file.eof()){
                char c = file.get();
                if(c == '\n') break;
                original.push_back(c);
            }
            file.close();
        }else{
            cerr << "usage: " << argv[0] << " gataca" << endl;
            return 1;
        }
    }else if(argc == 2){
        original.insert(original.begin(),argv[1], argv[1] + strlen(argv[1]));
    }else if(argc == 1){
        char c;
        while((c = getchar()) != -1)
            original.push_back(c);
    }else{
        cerr << "usage: " << argv[0] << " gataca" << endl;
        return 1;
    }
    auto key = bwtEncode(original.begin(), original.end());
    for(auto it = original.begin(); it != original.end(); it++){
        if(it == key)
            cout << "$";
        cout << *it;
    }
    cout << endl;
    bwtDecode(original.begin(), original.end(), key);
    for(auto it = original.begin(); it != original.end(); it++){
        cout << *it;
    }
    cout << endl;
    return 0;
}
