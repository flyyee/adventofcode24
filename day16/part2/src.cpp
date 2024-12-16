// 14:00:34
#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <queue>
#include <array>
#include <climits>

using namespace std;

enum class Tile {
    Wall,
    Empty
};

enum Direction {
    North,
    East,
    South,
    West
};
constexpr pair<int, int> offsets[] = {
    {-1, 0},
    {0, 1},
    {1, 0},
    {0, -1}
};

int main() {
    ifstream file("../input");
    string line;
    int start_y = -1, start_x = -1, end_y = -1, end_x = -1;
    vector<vector<Tile>> grid;
    while (getline(file, line))
    {
        vector<Tile> row;
        for (size_t i = 0; i < line.length(); i++) {
            switch (line[i]) {
            case '#':
                row.push_back(Tile::Wall);
                break;
            case '.':
                row.push_back(Tile::Empty);
                break;
            case 'S':
                start_y = grid.size(), start_x = i;
                row.push_back(Tile::Empty);
                break;

            case 'E':
                end_y = grid.size(), end_x = i;
                row.push_back(Tile::Empty);
                break;
            default:
                assert(false);
            }
        }
        grid.push_back(row);
    }

    const int width = static_cast<int>(grid[0].size()), height = static_cast<int>(grid.size());
    vector<vector<array<int, 4>>> best_scores = vector<vector<array<int, 4>>>(height, vector<array<int, 4>>(width, { -1,-1,-1,-1 })); // nesw

    queue<pair<pair<int, int>, Direction>> q;
    // bfs, maintain best
    q.push({ {start_y, start_x } , East });
    best_scores[start_y][start_x][East] = 0;

    while (!q.empty()) {
        // either move, or rotate
        auto [pos, dir] = q.front();
        q.pop();
        int curr_score = best_scores[pos.first][pos.second][dir];

        // move
        const auto [dy, dx] = offsets[dir];
        {
            int yy = pos.first + dy, xx = pos.second + dx;
            if (yy >= 0 && yy < height && xx >= 0 && xx < width) {
                if (grid[yy][xx] == Tile::Empty) {
                    // can move there
                    // if either new tile, or new best score
                    if (best_scores[yy][xx][dir] == -1 || curr_score + 1 < best_scores[yy][xx][dir]) {
                        q.push({ {yy, xx}, dir });
                        best_scores[yy][xx][dir] = curr_score + 1;
                    }
                }
            }
        }

        // rotate
        {
            int yy = pos.first, xx = pos.second;
            dir = Direction((dir + 1) % 4);
            // if either new tile, or new best score
            if (best_scores[yy][xx][dir] == -1 || curr_score + 1000 < best_scores[yy][xx][dir]) {
                q.push({ {yy, xx}, dir });
                best_scores[yy][xx][dir] = curr_score + 1000;
            }

            dir = Direction((dir + 2) % 4);
            if (best_scores[yy][xx][dir] == -1 || curr_score + 1000 < best_scores[yy][xx][dir]) {
                q.push({ {yy, xx}, dir });
                best_scores[yy][xx][dir] = curr_score + 1000;
            }
        }
    }

    int ans = INT_MAX;
    for (auto x : best_scores[end_y][end_x]) {
        if (x == -1)
            continue;
        ans = min(ans, x);
    }

    vector<vector<bool>> good_seat = vector<vector<bool>>(height, vector<bool>(width, false));

    for (int i = 0; i < 4; i++) {
        Direction dir_ = Direction(i);
        if (best_scores[end_y][end_x][dir_] == ans) {

            {
                q.push({ {end_y, end_x } , dir_ });
                good_seat[end_y][end_x] = true;

                while (!q.empty()) {
                    // either move, or rotate
                    auto [pos, dir] = q.front();
                    cout << "Checking: " << pos.first << "," << pos.second << endl;
                    q.pop();
                    int curr_score = best_scores[pos.first][pos.second][dir];

                    // move
                    auto [dy, dx] = offsets[dir];
                    dy *= -1;
                    dx *= -1;
                    {
                        int yy = pos.first + dy, xx = pos.second + dx;
                        if (yy >= 0 && yy < height && xx >= 0 && xx < width) {
                            if (grid[yy][xx] == Tile::Empty) {
                                // can move there
                                // if either new tile, or new best score
                                if (!good_seat[yy][xx] && best_scores[yy][xx][dir] != -1 && curr_score - 1 >= best_scores[yy][xx][dir]) {
                                    q.push({ {yy, xx}, dir });
                                    good_seat[yy][xx] = true;
                                }
                            }
                        }
                    }

                    // rotate
                    {
                        int yy = pos.first, xx = pos.second;
                        dir = Direction((dir + 1) % 4);
                        // if either new tile, or new best score
                        if (best_scores[yy][xx][dir] != -1 && curr_score - 1000 >= best_scores[yy][xx][dir]) {
                            q.push({ {yy, xx}, dir });
                            good_seat[yy][xx] = true;
                        }

                        dir = Direction((dir + 2) % 4);
                        if (best_scores[yy][xx][dir] != -1 && curr_score - 1000 >= best_scores[yy][xx][dir]) {
                            q.push({ {yy, xx}, dir });
                            good_seat[yy][xx] = true;
                        }
                    }
                }
            }
        }
    }

    ans = 0;
    for (auto& row : good_seat) {
        for (auto pos : row) {
            if (pos)
                ans++;
        }
    }

    cout << ans << endl;
}