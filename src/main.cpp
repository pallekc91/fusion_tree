#include "include/node.h"
#include "include/utils.h"
#include "include/fusion.h"
#include <iostream>
using namespace std;

int main(){
    fusion f;
    f.insert(12);
    f.insert(56);
    f.insert(41);
    f.insert(39);
    f.insert(58);
    f.insert(60);
    f.insert(1);
    f.initialize();
    cout << f.successor(0) << endl;
}