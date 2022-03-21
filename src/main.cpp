#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include "map.hpp"
#include <vector>
#include <ctime>
#include <deque>
#include <unordered_map>

using namespace std;

sjtu::map<int, int> test;
map<int, int> temp;
//int a[8] = {1, 6, 4, 2, 5, 8, 7, 3};
vector<int> A, B;
deque<int> C;

int main() {
    clock_t start_time = clock();
    srand(time(nullptr));
    for (int i = 0; i < 1000000; ++i) {
        int a = rand(), b = rand();
        test[a] = b;
        temp[a] = b;
        A.push_back(a);
        B.push_back(b);
        C.push_back(a);
    }
    for (int i = 0; i < 1000000; ++i)
        test.TestDelete(A[i]);
    clock_t end_time = clock();
    cout << 1.0 * (end_time - start_time) / CLOCKS_PER_SEC << endl;
}
