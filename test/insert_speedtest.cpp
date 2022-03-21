#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include "map.hpp"
#include <vector>
#include <ctime>
#include <unordered_map>

using namespace std;

sjtu::map<int, int> test;

int main() {
    clock_t start_time = clock();
    for (int i = 0; i < 1000000; ++i) {
        int a = rand(), b = rand();
        test[a] = b;
    }
    clock_t end_time = clock();
    cout << 1.0 * (end_time - start_time) / CLOCKS_PER_SEC << endl;
}
