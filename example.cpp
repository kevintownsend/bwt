#include "bwt.hpp"
#include <vector>
#include <string>

using namespace std;

int main(){
    char buffer[] = "gataca";
    vector<char> original(buffer, buffer + sizeof(buffer) - 1);
    bwtEncode(original.begin(), original.end());
    for(int i = 0; i < original.size(); i++){
        cerr << original[i];
    }
    cerr << endl;
    return 0;
}
