#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

using namespace std;

enum class Tile {
    Wall,
    BoxLeft,
    BoxRight,
    Empty
};

using Grid = vector<vector<Tile>>;

auto try_move(const Grid& grid, int y, int x, int offset_y) -> bool {
    if (grid[y][x] == Tile::Empty) {
        return true;
    }
    if (grid[y][x] == Tile::Wall) {
        return false;
    }
    if (grid[y][x] == Tile::BoxLeft) {
        return try_move(grid, y + offset_y, x, offset_y) && try_move(grid, y + offset_y, x + 1, offset_y);
    }
    else {
        return try_move(grid, y + offset_y, x, offset_y) && try_move(grid, y + offset_y, x - 1, offset_y);
    }
}

auto do_move(Grid& grid, int y, int x, int offset_y) -> void {
    if (grid[y][x] == Tile::Empty) {
        return;
    }
    if (grid[y][x] == Tile::Wall) {
        return;
    }
    if (grid[y][x] == Tile::BoxLeft) {
        do_move(grid, y + offset_y, x, offset_y);
        do_move(grid, y + offset_y, x + 1, offset_y);
        grid[y + offset_y][x] = Tile::BoxLeft;
        grid[y + offset_y][x + 1] = Tile::BoxRight;
        grid[y][x] = Tile::Empty;
        grid[y][x + 1] = Tile::Empty;
    }
    else {
        do_move(grid, y + offset_y, x, offset_y);
        do_move(grid, y + offset_y, x - 1, offset_y);
        grid[y + offset_y][x] = Tile::BoxRight;
        grid[y + offset_y][x - 1] = Tile::BoxLeft;
        grid[y][x] = Tile::Empty;
        grid[y][x - 1] = Tile::Empty;
    }
}

int main() {
    ifstream file("../input");
    string line;
    Grid grid;
    int robot_x, robot_y;
    while (getline(file, line) && line.length() > 0)
    {
        vector<Tile> row;
        for (size_t i = 0; i < line.size(); i++) {
            switch (line[i]) {
            case '#':
                row.push_back(Tile::Wall);
                row.push_back(Tile::Wall);
                break;
            case '.':
                row.push_back(Tile::Empty);
                row.push_back(Tile::Empty);
                break;
            case 'O':
                row.push_back(Tile::BoxLeft);
                row.push_back(Tile::BoxRight);
                break;
            default:
                row.push_back(Tile::Empty);
                row.push_back(Tile::Empty);
                robot_y = grid.size(), robot_x = i * 2;
            }

        }
        grid.push_back(row);
    }

    const int width = static_cast<int>(grid[0].size()), height = static_cast<int>(grid.size());
    const auto perform = [&](const char move) {
        int offset_x, offset_y;
        switch (move) {
        case '^':
            offset_y = -1, offset_x = 0;
            break;
        case '<':
            offset_x = -1, offset_y = 0;
            break;
        case 'v':
            offset_y = 1, offset_x = 0;
            break;
        case '>':
            offset_x = 1, offset_y = 0;
            break;
        default:
            assert(false); // should never reach here!
        }

        // find next empty space
        int curr_x = robot_x + offset_x, curr_y = robot_y + offset_y;
        const auto in_grid = [&](int y, int x) -> bool {
            return (x >= 0 && x < width && y >= 0 && y < height);
            };

        if (offset_y == 0) {
            // side movement can use same logic
            while (in_grid(curr_y, curr_x) && grid[curr_y][curr_x] != Tile::Empty && grid[curr_y][curr_x] != Tile::Wall) {
                curr_y += offset_y;
                curr_x += offset_x;
            }

            if (!in_grid(curr_y, curr_x)) {
                // no move
                return;
            }

            if (grid[curr_y][curr_x] == Tile::Wall) {
                // no move
                return;
            }

            // shift everything starting from the empty space
            while (curr_x != robot_x || curr_y != robot_y) {
                grid[curr_y][curr_x] = grid[curr_y - offset_y][curr_x - offset_x];  // move
                curr_x -= offset_x;
                curr_y -= offset_y;
            }

            robot_x += offset_x;
            robot_y += offset_y;
        }
        else {
            // vertical movement, need to track berth
            if (try_move(grid, curr_y, curr_x, offset_y)) {
                do_move(grid, curr_y, curr_x, offset_y);
                robot_y = curr_y;
                robot_x = curr_x;
            }
        }
        };

    const auto print = [&]() {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (grid[y][x] == Tile::BoxLeft) {
                    cout << "[";
                }
                else if (grid[y][x] == Tile::BoxRight) {
                    cout << "]";
                }
                else if (grid[y][x] == Tile::Empty) {
                    if (robot_x == x && robot_y == y) {
                        cout << "@";
                    }
                    else {
                        cout << ".";
                    }
                }
                else {
                    cout << "#";
                }
            }
            cout << endl;
        }
        cout << endl;
        };

    print();
    while (getline(file, line)) {
        // each line is a sequence of instructions
        for (const auto chr : line) {
            perform(chr);
            // print();
        }
    }

    long ans = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (grid[y][x] == Tile::BoxLeft) {
                ans += 100 * y + x;
            }
        }
    }

    cout << ans << endl;
}