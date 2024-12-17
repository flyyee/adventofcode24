#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <cstring>
#include <cmath>
#include <variant>
#include <optional>

using namespace std;

using Bit = unsigned char;
vector<Bit> program;
long a, b, c;
vector<long> buf;
const auto get_buf(long t) {
    buf.clear();
    a = t;
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
        // if (ip == 0) {
        //     cout << "A: " << a << endl;
        //     cout << "B: " << b << endl;
        //     cout << "C: " << c << endl;
        // }
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
    return;
};

const void solve(long inp, size_t curr_len = 1) {
    const auto matches = [&]() -> bool {
        for (size_t i = 0; i < buf.size(); i++) {
            if (buf[i] != program[program.size() - buf.size() + i])
            {
                return false;
            }
        }
        return true;
        };

    if (curr_len == 1) {
        for (long t = 0; t < 8; t++) {
            get_buf(t);
            if (buf.size() == curr_len && matches()) {
                for (long o = 0; o < 8; o++) {
                    solve(t * 8 + o, curr_len + 1);
                }
            }
        }
    }
    else {
        long t = inp;
        get_buf(t);
        if (buf.size() == curr_len && matches()) {
            if (curr_len == program.size()) {
                // win!
                cout << "Win with: " << t << endl;
                return;
            }
            for (long o = 0; o < 8; o++) {
                solve(t * 8 + o, curr_len + 1);
            }
        }
    }
}

int main() {
    ifstream file("../input");
    string line;
    const auto parse_reg = [&]() -> long {
        getline(file, line);
        auto pos = line.find(": ") + 2;
        return stol(line.substr(pos));
        };
    a = parse_reg();
    b = parse_reg();
    c = parse_reg();
    getline(file, line); // nl

    getline(file, line);
    line = line.substr(strlen("Program: "));
    for (size_t i = 0; i < line.length();i += 2) {
        program.push_back(Bit(line[i] - '0'));
    }

    // for (long t = 0; t < 8; t++) {
    //     get_buf(t);
    //     cout << "For attempted a = " << t << ", output is: " << endl;
    //     for (auto i : buf) {
    //         cout << i << ',';
    //     }
    //     cout << endl;
    // }

    solve(0);

    /*
// check quine
        bool good = true;
        if (buf.size() == program.size()) {
            for (size_t i = 0;i < program.size();i++) {
                if (buf[i] != program[i]) {
                    good = false;
                    break;
                }
            }
        }
        else {
            good = false;
        }

        if (good) {
            cout << t << endl;
            break;
        }
        else {
            cout << "For attempted a = " << t << ", output is: " << endl;
            for (auto i : buf) {
                cout << i << ',';
            }
            cout << endl;
        }
    */

    // 100000000000000
}