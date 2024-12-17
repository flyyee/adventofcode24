#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <cstring>
#include <cmath>
#include <variant>
#include <optional>

using namespace std;

int main() {
    ifstream file("../input");
    string line;
    const auto parse_reg = [&]() -> long {
        getline(file, line);
        auto pos = line.find(": ") + 2;
        return stol(line.substr(pos));
        };
    long a = parse_reg();
    long b = parse_reg();
    long c = parse_reg();
    getline(file, line); // nl
    using Bit = unsigned char;
    vector<Bit> program;

    getline(file, line);
    line = line.substr(strlen("Program: "));
    for (size_t i = 0; i < line.length();i += 2) {
        program.push_back(Bit(line[i] - '0'));
    }

    vector<long> buf;
    const auto out = [&](long v) {
        buf.push_back(v);
        };

    const auto combo = [&](Bit c) -> long {
        switch (c) {
        case 7:
            assert(false);
        case 4:
            return a;
        case 5:
            return b;
        case 6:
            return c;
        default:
            return long(c);
        }
        assert(false);
        };

    long ip = 0;
    while (ip + 1 < long(program.size())) {
        // cout << "step: " << int(program[ip]) << endl;
        switch (program[ip]) {
        case 0:
            // adv
            a = a / pow(2, combo(program[ip + 1]));
            ip += 2;
            break;
        case 1:
            //bxl
            b ^= program[ip + 1];
            ip += 2;
            break;
        case 2:
            //bst
            b = combo(program[ip + 1]) % 8;
            ip += 2;
            break;
        case 3:
            // jnz
            if (a == 0) {
                ip += 2;
                break;
            }
            ip = program[ip + 1];
            break;
        case 4:
            // bxc
            b = b ^ c;
            ip += 2;
            break;
        case 5:
            // out
            out(combo(program[ip + 1]) % 8);
            ip += 2;
            break;
        case 6:
            // bdv
            b = a / pow(2, combo(program[ip + 1]));
            ip += 2;
            break;
        case 7:
            // bdv
            c = a / pow(2, combo(program[ip + 1]));
            ip += 2;
            break;
        default:
            assert(false);
        }
    }

    for (auto x : buf) {
        cout << x << ",";
    }
    cout << endl;
}