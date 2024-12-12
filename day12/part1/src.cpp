#include <iostream>
#include <fstream>
#include <vector>
#include <stack>

using namespace std;

using Tile = char;
using Grid = vector<vector<Tile>>;

Grid parse() {
    ifstream file("../input");
    Grid grid;
    string line;
    while (getline(file, line)) {
        vector<Tile> row{};
        copy(begin(line), end(line), back_inserter(row));
        grid.push_back(row);
    }
    return grid;
}

using Area = int;
using Perimeter = int;
pair<Area, Perimeter> traverse(Grid& grid, pair<int, int> pos) {
    Area area = 0;
    Perimeter perimeter = 0;
    constexpr int offsets[] = { 1 , 0, -1, 0, 1 };
    stack<pair<int, int>> targets;
    constexpr auto mark = [](Tile tile) -> Tile {
        return tile | (1 << 7);
        };
    constexpr auto unmark = [](Tile tile) -> Tile {
        return tile & ~(1 << 7);
        };
    constexpr auto is_marked = [](Tile tile) -> bool {
        return tile >> 7;
        };
    const int h = static_cast<int>(grid.size()), w = static_cast<int>(grid[0].size());
    if (!is_marked(grid[pos.first][pos.second])) {
        targets.push(pos);
        grid[pos.first][pos.second] = mark(grid[pos.first][pos.second]);
    }

    while (!targets.empty()) {
        const auto curr = targets.top();
        targets.pop();
        // check adjacents
        Perimeter open_sides = 4;
        for (size_t i = 0; i < 4; i++) {
            pair<int, int> adj = { curr.first + offsets[i], curr.second + offsets[i + 1] };
            if (adj.first >= 0 && adj.first < h && adj.second >= 0 && adj.second < w) {
                // in grid
                if (unmark(grid[adj.first][adj.second]) == unmark(grid[curr.first][curr.second])) {
                    // same type
                    open_sides--;
                    if (!is_marked(grid[adj.first][adj.second])) {
                        // add target
                        targets.push(adj);
                        grid[adj.first][adj.second] = mark(grid[adj.first][adj.second]);
                    }
                }
            }
        }
        // add contribution
        perimeter += open_sides;
        area++;
    }

    return { area, perimeter };
}

int main() {
    auto grid = parse();
    // search from every position, marking visited nodes accordingly
    const int h = static_cast<int>(grid.size()), w = static_cast<int>(grid[0].size());
    long cost = 0;
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            const auto [area, perimeter] = traverse(grid, { i, j });
            cost += area * perimeter;
        }
    }

    cout << cost << endl;
}