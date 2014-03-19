#include "bwt.hpp"
#include <vector>
#include <string>

using namespace std;

int main(){
    char buffer[] = "gataca";
    vector<char> original(buffer, buffer + sizeof(buffer));
    bwtEncode(original.begin(), original.end());

    return 0;
}
