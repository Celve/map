#include <cstdio>
#include "map.hpp"
#include <iostream>
#include <cassert>
#include <string>

clock_t half_time;

class Integer {
public:
    static int counter;
    int val;

    Integer(int val) : val(val) {
        counter++;
    }

    Integer(const Integer &rhs) {
        val = rhs.val;
        counter++;
    }

    Integer& operator = (const Integer &rhs) {
        assert(false);
    }

    ~Integer() {
        counter--;
    }
};

int Integer::counter = 0;

class Compare {
public:
    bool operator () (const Integer &lhs, const Integer &rhs) const {
        return lhs.val < rhs.val;
    }
};

void tester(void) {
    sjtu::map<Integer, std::string, Compare> map;
    for (int i = 0; i < 1000000; ++i) {
        std::string string = "";
        for (int number = i; number; number /= 10) {
            char digit = '0' + number % 10;
            string = digit + string;
        }
        if (i & 1) {
            map[Integer(i)] = string;
            auto result = map.insert(sjtu::pair<Integer, std::string>(Integer(i), string));
            assert(!result.second);
        } else {
            auto result = map.insert(sjtu::pair<Integer, std::string>(Integer(i), string));
            assert(result.second);
        }
    }
    half_time = clock();
    int counter = 0;
    for (int time = 0; time <= 30; time++) {
        sjtu::map<Integer, std::string, Compare>::const_iterator const_iterator;
        const_iterator = map.cbegin();
        while (const_iterator != map.cend()) {
            const Integer integer(const_iterator->first);
            if ((++counter) % 1000 == 0) {
                std::cout << const_iterator->second << " ";
            }
            const_iterator++;
        }
        sjtu::map<Integer, std::string, Compare>::iterator iterator;
        iterator = map.begin();
        while (iterator != map.end()) {
            const Integer integer(iterator->first);
            std::string tmp = iterator->second;
            iterator->second = "SJTU";
            if ((++counter) % 1000 == 0) {
                std::cout << iterator->second << " ";
            }
            iterator->second = tmp;
            iterator++;
        }
        std::cout << map.size() << std::endl;
    }
}

int main(void) {
    clock_t start_time = clock();
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);
    std::cout.tie(0);
    tester();
    std::cout << Integer::counter << std::endl;
    clock_t end_time = clock();
    std::cout << 1.0 * (end_time - start_time) / CLOCKS_PER_SEC << std::endl;
    std::cout << 1.0 * (half_time - start_time) / CLOCKS_PER_SEC << std::endl;
    std::cout << 1.0 * (end_time - half_time) / CLOCKS_PER_SEC << std::endl;
}