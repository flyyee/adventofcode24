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
    constexpr long seconds = 100;

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
    const auto quadrant = [&w, &h](Robot& r) -> int {
        long w_ = w / 2, h_ = h / 2;
        if (r.px < w_ && r.py < h_) {
            return 0;
        }
        else if (r.px < w_ && r.py > h_) {
            return 1;
        }
        else if (r.px > w_ && r.py < h_) {
            return 2;
        }
        else if (r.px > w_ && r.py > h_) {
            return 3;
        }
        return 4;
        };

    int quadrants[5] = { 0 };

    for (long s = 0; s < seconds; s++) {
        for (auto& r : robots) {
            move_robot(r);
        }
    }
    for (auto& r : robots) {
        quadrants[quadrant(r)]++;
    }
    long ans = quadrants[0] * quadrants[1] * quadrants[2] * quadrants[3];
    cout << ans << endl;
}