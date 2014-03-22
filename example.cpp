#include "bwt.hpp"
#include <vector>
#include <string>

using namespace std;

int main(){
    char buffer[] = "gataca";
    vector<char> original(buffer, buffer + sizeof(buffer) - 1);
    auto key = bwtEncode(original.begin(), original.end());
    for(int i = 0; i < original.size(); i++){
        cerr << original[i];
    }
    cerr << endl;
    bwtDecode(original.begin(), original.end(), key);
    return 0;
}
