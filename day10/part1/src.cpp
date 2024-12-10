#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

typedef unsigned char Tile;
typedef vector<vector<Tile>> Grid;
const Grid parse() {
    ifstream file("../input");
    Grid grid;
    string line;
    while (getline(file, line)) {
        vector<Tile> row(line.length());
        transform(begin(line), end(line), begin(row), [](const auto ch) {
            return ch - '0';
            });
        grid.push_back(row);
    }
    return grid;
}

constexpr Tile trail_end = 9, trail_start = 0;
unsigned int count_trails(const Grid& grid, Grid& visited, pair<int, int> pos) {
    const auto [i, j] = pos;
    const Tile curr = grid[i][j];

    visited[i][j] = 1;
    if (curr == trail_end) {
        return 1;
    }

    constexpr int offsets[] = { 1, 0, -1, 0, 1 };
    const auto in_grid = [height = static_cast<int>(grid.size()), width = static_cast<int>(grid[0].size())](int i, int j) -> bool {
        return (i >= 0 && i < height && j >= 0 && j < width);
        };

    unsigned int ans = 0;
    for (auto x = 0; x < 4; x++) {
        auto ii = i + offsets[x], jj = j + offsets[x + 1];
        if (in_grid(ii, jj) && !visited[ii][jj] && grid[ii][jj] == curr + 1) {
            ans += count_trails(grid, visited, { ii, jj });
        }
    }

    return ans;
}

int main() {
    const auto grid = parse();
    const size_t height = grid.size(), width = grid[0].size();
    auto visited = vector<vector<Tile>>(height, vector<Tile>(width));
    const auto reset = [](auto& grid) {
        for (auto& row : grid) {
            fill(begin(row), end(row), 0);
        }
        };
    unsigned long ans = 0;
    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            if (grid[i][j] == trail_start)
            {
                reset(visited);
                ans += count_trails(grid, visited, { i, j });
            }
        }
    }
    cout << ans << endl;
}