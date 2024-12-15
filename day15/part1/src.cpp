#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

using namespace std;

enum class Tile {
    Wall,
    Box,
    Empty
};

using Grid = vector<vector<Tile>>;

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
                break;
            case '.':
                row.push_back(Tile::Empty);
                break;
            case 'O':
                row.push_back(Tile::Box);
                break;
            default:
                row.push_back(Tile::Empty);
                robot_y = grid.size(), robot_x = i;
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
        };

    // const auto print = [&]() {
    //     for (int y = 0; y < height; y++) {
    //         for (int x = 0; x < width; x++) {
    //             if (grid[y][x] == Tile::Box) {
    //                 cout << "O";
    //             }
    //             else if (grid[y][x] == Tile::Empty) {
    //                 if (robot_x == x && robot_y == y) {
    //                     cout << "@";
    //                 }
    //                 else {
    //                     cout << ".";
    //                 }
    //             }
    //             else {
    //                 cout << "#";
    //             }
    //         }
    //         cout << endl;
    //     }
    //     cout << endl;
    //     };

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
            if (grid[y][x] == Tile::Box) {
                ans += 100 * y + x;
            }
        }
    }

    cout << ans << endl;
}