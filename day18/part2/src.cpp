#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <queue>
#include <set>
using namespace std;

enum class Tile {
    Empty,
    Corrupt
};

int main() {
    ifstream file("../input");
    string line;

    constexpr int n = 70;
    constexpr long w = n + 1, h = w;
    constexpr size_t byte_limit = 1024;
    constexpr pair<int, int> target = { n, n };
    size_t byte_cnt = 0;

    vector<vector<Tile>> grid = vector<vector<Tile>>(h, vector<Tile>(w, Tile::Empty));
    while (getline(file, line)) {
        auto pos = line.find(',');
        auto x = stoi(line), y = stoi(line.substr(pos + 1));
        grid[y][x] = Tile::Corrupt;
        if (++byte_cnt == byte_limit) {
            break;
        }
    }

    pair<int, int> last_byte_added;
    do {
        // bfs
        queue<pair<int, int>> coords;
        set<pair<int, int>> visited;
        coords.push({ 0, 0 });
        visited.insert({ 0,0 });
        size_t curr_len = 1, next_len = 0;
        long curr_steps = 0;

        while (!coords.empty()) {
            if (curr_len == 0) {
                swap(next_len, curr_len);
                curr_steps++;
            }

            auto& curr = coords.front();
            if (curr == target) {
                break;
            }
            constexpr int offsets[] = { 1, 0, -1, 0, 1 };
            for (size_t i = 0; i < 4; i++) {
                auto x = curr.first + offsets[i], y = curr.second + offsets[i + 1];
                if (x < 0 || x >= h || y < 0 || y >= w) {
                    continue;
                }
                if (grid[x][y] == Tile::Corrupt) {
                    continue;
                }
                if (visited.contains({ x, y })) {
                    continue;
                }
                // cout << x << ',' << y << endl;
                visited.insert({ x, y });
                coords.push({ x, y });
                next_len++;
            }

            coords.pop();
            curr_len--;
        }

        if (coords.empty()) {
            cout << "failed to find route\n";
            cout << last_byte_added.second << ',' << last_byte_added.first << endl;
            break;
        }

        {
            getline(file, line);
            auto pos = line.find(',');
            auto x = stoi(line), y = stoi(line.substr(pos + 1));
            grid[y][x] = Tile::Corrupt;
            if (++byte_cnt == byte_limit) {
                break;
            }
            last_byte_added = { y, x };
        }
    } while (true);
}