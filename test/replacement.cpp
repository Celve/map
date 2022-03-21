#include <cstdio>
#include <cstring>
#include <iostream>
#include "../src/map.hpp"

using namespace std;

sjtu::map<int, int> test;
int v[100];

int main() {
    for (int i = 0; i < 1000; ++i) {
        int a = rand() % 100, b = rand() % 100;
        test[a] = b;
        v[a] = b;
    }
    for (int i = 0; i < 100; ++i)
        cout << test[i] << " ";
    cout << endl;
    for (int i = 0; i < 100; ++i)
        cout << test[i] << " ";
    cout << endl;
    return 0;
}
