#include "bwt.hpp"
#include <vector>
#include <string>
#include <cstring>

using namespace std;

int main(int argc, char* argv[]){
    if(argc != 2){
        cerr << "usage: " << argv[0] << " gataca" << endl;
        return 1;
    }
    vector<char> original(argv[1], argv[1] + strlen(argv[1]));
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
