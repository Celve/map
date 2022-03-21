#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include "map.hpp"
#include <vector>

using namespace std;

sjtu::map<int, int> test;
map<int, int> temp;
vector<int> v;

int main() {
    for (int i = 0; i < 1000; ++i) {
        int a = rand(), b = rand();
        test[a] = b;
        cout << b << " ";
        v.push_back(a);
    }
    cout << endl;
    for (int i = 0; i < 1000; ++i)
        cout << test[v[i]] << " ";
    return 0;
}
