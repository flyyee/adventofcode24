#include "solver.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
// #include <cmath>

using namespace std;

using Bit = unsigned char;

struct Program {
    vector<Bit> code;
    Expression* reg_a, * reg_b, * reg_c;
    long ip;
    vector<Expression*> buf;
    Program() : reg_a(nullptr), reg_b(nullptr), reg_c(nullptr), ip(0) {}
};

struct ExploreState {
    size_t max_depth;
    bool done;
};

ExploreState explore_state = {
    .max_depth = 6,
    .done = false,
};

using CallbackFunction = function<void(const vector<Expression*>& out_buf, SolverV2& solver)>;

const void explore(Program program, SolverV2& solver, CallbackFunction callback, size_t depth = 0) {
    if (explore_state.done) {
        return;
    }
    if (depth == explore_state.max_depth) {
        cout << "Reached max depth" << endl;
        return;
    }

    const auto out = [&](auto v) {
        program.buf.push_back(v);
        };

    const auto combo = [&](Bit c) -> Expression* {
        switch (c) {
        case 7:
            assert(false);
        case 4:
            return program.reg_a;
        case 5:
            return program.reg_b;
        case 6:
            return program.reg_c;
        default:
            return solver.constant("const", c);
        }
        assert(false);
        };

    while (program.ip + 1 < long(program.code.size())) {
        // cout << "Step at: " << int(program.code[program.ip]) << endl;
        switch (program.code[program.ip]) {
        case 0:
        {// adv
            auto divisor = solver.exp2(combo(program.code[program.ip + 1]));
            program.reg_a = solver.div(program.reg_a, divisor);
            // a = a / pow(2, combo(program[ip + 1]));
            program.ip += 2;
            break;
        }
        case 1:
        {//bxl
            auto operand = solver.constant("const", program.code[program.ip + 1]);
            program.reg_b = solver.exclusive_or(program.reg_b, operand);
            // b ^= program[ip + 1];
            program.ip += 2;
            break;
        }
        case 2:
        {//bst
            program.reg_b = solver.mod8(combo(program.code[program.ip + 1]));
            // b = combo(program[ip + 1]) % 8;
            program.ip += 2;
            break;
        }
        case 3:
            // jnz
        {
            auto ss = solver.save_state();
            solver.add_constraint(program.reg_a, ConstraintType::Eq, 0);
            program.ip += 2;
            auto buf_sz = program.buf.size();
            explore(program, solver, callback, depth + 1);
            program.buf.resize(buf_sz);
            program.ip -= 2;
            solver.restore_state(move(ss));

            // TODO: implement
            ss = solver.save_state();
            solver.add_constraint(program.reg_a, ConstraintType::Neq, 0);
            const auto saved_ip = program.ip;
            program.ip = program.code[program.ip + 1];
            buf_sz = program.buf.size();
            explore(program, solver, callback, depth + 1);
            program.buf.resize(buf_sz);
            program.ip = saved_ip;
            solver.restore_state(move(ss));

            return;
        }
        case 4:
        {// bxc
            program.reg_b = solver.exclusive_or(program.reg_b, program.reg_c);
            // b = b ^ c;
            program.ip += 2;
            break;
        }
        case 5:
        {// out
            auto res = solver.mod8(combo(program.code[program.ip + 1]));
            out(res);
            // out(combo(program.code[ip + 1]) % 8);
            program.ip += 2;
            break;
        }
        case 6:
        {// bdv
            auto divisor = solver.exp2(combo(program.code[program.ip + 1]));
            program.reg_b = solver.div(program.reg_a, divisor);
            // b = a / pow(2, combo(program[ip + 1]));
            program.ip += 2;
            break;
        }
        case 7:
        {// bdv
            auto divisor = solver.exp2(combo(program.code[program.ip + 1]));
            program.reg_c = solver.div(program.reg_a, divisor);
            // c = a / pow(2, combo(program[ip + 1]));
            program.ip += 2;
            break;
        }
        default:
            assert(false);
        }
    }

    callback(program.buf, solver);

    return;
};

int main() {
    ifstream file("../input");
    string line;
    const auto parse_reg = [&]() -> long {
        getline(file, line);
        auto pos = line.find(": ") + 2;
        return stol(line.substr(pos));
        };

    Program input;
    SolverV2 s;

    input.reg_a = s.symbol("register_a");
    (void)parse_reg(); // ignore value
    input.reg_b = s.constant("register_b", parse_reg());
    input.reg_c = s.constant("register_c", parse_reg());
    getline(file, line); // nl

    getline(file, line);
    line = line.substr(strlen("Program: "));
    for (size_t i = 0; i < line.length();i += 2) {
        input.code.push_back(Bit(line[i] - '0'));
    }

    const auto handler = [&](const vector<Expression*>& out_buffer, SolverV2& s) {
        cout << "Reached end state with out_buffer having " << out_buffer.size() << " elements.\n";
        cout << "Printing conditions...\n";
        for (size_t i = 0; i < out_buffer.size();i++) {
            cout << "Conditions for out_buffer[" << i << "]:\n";
            s.print_detailed(out_buffer[i]);
            cout << endl;
        }
        cout << endl;
        };

    explore(input, s, handler);
}