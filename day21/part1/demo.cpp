#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <stdexcept>
#include <variant>
#include <array>
#include <memory>
#include <map>

using namespace std;

enum Direction {
    Up = '^',
    Down = 'v',
    Left = '<',
    Right = '>',
    Activate = 'A'
};

class Keypad {
public:
    virtual void move(Direction) = 0;
};

class NumericKeypad :public Keypad {
    vector<char> logs_, path_;
    array<char[3], 4> vals = { { {'7','8','9'}, {'4','5','6'}, {'1','2','3'}, {'X','0', 'A'} } };
    int y = 3, x = 2;
public:
    void move(Direction dir) {
        switch (dir) {
        case Up:
            y--;
            break;
        case Down:
            y++;
            break;
        case Left:
            x--;
            break;
        case Right:
            x++;
            break;
        case Activate:
            path_.push_back(vals[y][x]);
            break;
        default:
            throw invalid_argument("invalid dir");
        }
        if (x == 0 && y == 3) {
            throw runtime_error("chose X");
        }
        if (x < 0 || x >= 3 || y < 0 || y >= 4) {
            throw runtime_error("OOB xy");
        }
        logs_.push_back(vals[y][x]);
    }

    auto logs() const {
        return logs_;
    }

    auto path() const {
        return path_;
    }
};

class DirectionalKeypad : public Keypad {
    shared_ptr<Keypad> child_;

    vector<char> logs_, path_;
    array<Direction[3], 2> vals = { { {Activate, Up, Activate}, {Left, Down, Right} } };
    int y = 0, x = 2;
public:
    DirectionalKeypad(shared_ptr<Keypad> child) : child_(child) {}

    void move(Direction dir) {
        switch (dir) {
        case Up:
            y--;
            break;
        case Down:
            y++;
            break;
        case Left:
            x--;
            break;
        case Right:
            x++;
            break;
        case Activate:
            child_->move(vals[y][x]);
            path_.push_back(vals[y][x]);
            break;
        default:
            throw invalid_argument("invalid dir");
        }
        if (x == 0 && y == 0) {
            throw runtime_error("chose X");
        }
        if (x < 0 || x >= 3 || y < 0 || y >= 2) {
            throw runtime_error("OOB xy");
        }
        logs_.push_back(vals[y][x]);
    }

    auto logs() const {
        return logs_;
    }

    auto path() const {
        return path_;
    }
};

int main() {
    ifstream file("../input");
    string line;
    map<char, Direction> mp;
    mp['<'] = Left;
    mp['>'] = Right;
    mp['A'] = Activate;
    mp['v'] = Down;
    mp['^'] = Up;

    while (getline(file, line)) {
        // target is line
        auto root = make_shared<NumericKeypad>();
        auto a = make_shared<DirectionalKeypad>(static_pointer_cast<Keypad>(root)),
            b = make_shared<DirectionalKeypad>(static_pointer_cast<Keypad>(a));

        const string command = "<vA<AA>>^AvAA<^A>A<v<A>>^AvA^A<vA>^A<v<A>^A>AAvA^A<v<A>A>^AAAvA<^A>A";
        for (auto ch : command) {
            b->move(mp[ch]);
        }

        {
            cout << "For root:\n";
            auto path = root->path();
            cout << "printing path: ";
            for (auto& p : path) {
                cout << p;
            }
            cout << endl;

            auto logs = root->logs();
            cout << "printing logs: ";
            for (auto& log : logs) {
                cout << log;
            }
            cout << endl;
        }

        {
            cout << "For a:\n";
            auto path = a->path();
            cout << "printing path: ";
            for (auto& p : path) {
                cout << p;
            }
            cout << endl;

            auto logs = a->logs();
            cout << "printing logs: ";
            for (auto& log : logs) {
                cout << log;
            }
            cout << endl;
        }
        {
            cout << "For b:\n";
            auto path = b->path();
            cout << "printing path: ";
            for (auto& p : path) {
                cout << p;
            }
            cout << endl;

            auto logs = b->logs();
            cout << "printing logs: ";
            for (auto& log : logs) {
                cout << log;
            }
            cout << endl;
        }

        break;
    }
}