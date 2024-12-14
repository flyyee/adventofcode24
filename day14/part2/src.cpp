#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

struct Robot {
    long px, py, vx, vy;
};

int main() {
    ifstream file("../input");
    string line;
    constexpr long w = 101, h = 103;
    // constexpr long w = 11, h = 7;
    // constexpr long seconds = 100;

    vector<Robot> robots;
    while (getline(file, line)) {
        Robot r;
        auto x = line.find("p=") + 2;
        auto y = line.find(",", x);
        r.px = stol(line.substr(x, y - x));
        x = y + 1;
        y = line.find(" ", x);
        r.py = stol(line.substr(x, y - x));

        x = line.find("v=") + 2;
        y = line.find(",", x);
        r.vx = stol(line.substr(x, y - x));
        x = y + 1;
        y = line.find(" ", x);
        r.vy = stol(line.substr(x, y - x));

        robots.push_back(r);
    }

    const auto move_robot = [&w, &h](Robot& r) -> void {
        r.px += r.vx + w;
        r.px %= w;

        r.py += r.vy + h;
        r.py %= h;
        };

    // return 4 on fail
    // const auto quadrant = [&w, &h](Robot& r) -> int {
    //     long w_ = w / 2, h_ = h / 2;
    //     if (r.px < w_ && r.py < h_) {
    //         return 0;
    //     }
    //     else if (r.px < w_ && r.py > h_) {
    //         return 1;
    //     }
    //     else if (r.px > w_ && r.py < h_) {
    //         return 2;
    //     }
    //     else if (r.px > w_ && r.py > h_) {
    //         return 3;
    //     }
    //     return 4;
    //     };

    // int quadrants[5] = { 0 };

    const auto display = [&w, &h](const vector<Robot>& robots) {
        vector<vector<char>> screen = vector<vector<char>>(h, vector<char>(w, ' '));
        for (const auto& r : robots) {
            screen[r.py][r.px] = '#';
        }
        for (const auto& row : screen) {
            for (auto x : row) {
                cout << x;
            }
            cout << endl;
        }
        cout << endl;
        };

    long target = 805; // weird
    for (long s = 0; s < 1000000; s++) {
        for (auto& r : robots) {
            move_robot(r);
        }
        cout << s + 1 << " seconds elapsed" << endl;
        if (s == target) {
            display(robots);
            target += 101;
            getchar();
        }
    }
}