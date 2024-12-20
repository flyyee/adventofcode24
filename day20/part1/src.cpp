#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <set>

using namespace std;

enum class Tile {
    Empty,
    Wall
};

using Grid = vector<vector<Tile>>;

int main() {
    ifstream file("../input");
    string line;
    Grid grid;
    int sx, sy, ex, ey;
    while (getline(file, line)) {
        vector<Tile> row;
        for (size_t i = 0; i < line.length(); i++) {
            if (line[i] == '#') {
                row.push_back(Tile::Wall);
            }
            else {
                if (line[i] == 'S') {
                    sx = grid.size(), sy = row.size();
                }
                else if (line[i] == 'E') {
                    ex = grid.size(), ey = row.size();
                }
                row.push_back(Tile::Empty);
            }
        }
        grid.push_back(row);
        // 1. find shortest paths starting from all non-walls (we can just do bfs from the end point)
        // 2. try cheating at every point along the shortest path from start to end
    }

    // bfs from end point
    queue<pair<int, int>> q;
    vector<vector<int>> shortest = vector<vector<int>>(grid.size(), vector<int>(grid[0].size(), -1));
    size_t curr_len = 1, next_len = 0;
    q.push({ ex, ey });
    shortest[ex][ey] = 0;

    int X = static_cast<int>(grid.size()), Y = static_cast<int>(grid[0].size());

    while (!q.empty()) {
        auto [x, y] = q.front();
        q.pop();

        constexpr int offsets[] = { 0, 1, 0, -1, 0 };
        for (size_t i = 0; i < 4; i++) {
            if (--curr_len == 0) {
                swap(curr_len, next_len);
            }

            int xx = x + offsets[i], yy = y + offsets[i + 1];
            if (xx < 0 || xx >= X || yy < 0 || yy >= Y) {
                continue;
            }
            if (grid[xx][yy] == Tile::Wall || shortest[xx][yy] != -1)
                continue;
            q.push({ xx,yy });
            shortest[xx][yy] = 1 + shortest[x][y];
            next_len++;
        }
    }

    vector<pair<int, int>> route;
    pair<int, int> curr = { sx, sy };
    route.push_back(curr);

    while (curr != pair<int, int>{ex, ey}) {
        constexpr int offsets[] = { 0, 1, 0, -1, 0 };
        for (size_t i = 0; i < 4; i++) {
            int xx = curr.first + offsets[i], yy = curr.second + offsets[i + 1];
            if (xx < 0 || xx >= X || yy < 0 || yy >= Y) {
                continue;
            }
            if (shortest[xx][yy] == shortest[curr.first][curr.second] - 1) {
                curr = { xx, yy };
                route.push_back(curr);
                break;
            }
        }
    }

    long ans = 0;
    long best_time = (long)(route.size()) - 1;
    for (const auto& [x, y] : route) {
        constexpr pair<int, int> offsets[] = { {2,0}, {0,2}, {-2,0}, {0,-2}, {1,1},{-1,1},{1,-1},{-1,1} };
        for (const auto& [dx, dy] : offsets) {
            if (x + dx < 0 || x + dx >= X || y + dy < 0 || y + dy >= Y || grid[x + dx][y + dy] == Tile::Wall) {
                continue;
            }
            int t = shortest[sx][sy] - shortest[x][y] + 2 + shortest[x + dx][y + dy];
            if (t + 100 <= best_time) {
                ans++;
            }
        }

    }
    cout << ans << endl;
}