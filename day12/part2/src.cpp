#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <optional>

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

enum SideBit {
    Down = 1,
    Left = 2,
    Top = 4,
    Right = 8
};
const SideBit side_bits[] = { SideBit::Down, SideBit::Left, SideBit::Top, SideBit::Right };
constexpr int offsets[] = { 1 , 0, -1, 0, 1 };
constexpr int all_side_bits = SideBit::Top | SideBit::Left | SideBit::Down | SideBit::Right;

struct Side {
    SideBit side_;
    int static_position;
    int start, end;

    Side(pair<int, int> pos, SideBit side_bit) {
        side_ = side_bit;
        if (side_bit == Top || side_bit == Down) {
            static_position = pos.first;
            start = end = pos.second;
        }
        else {
            static_position = pos.second;
            start = end = pos.first;
        }
    }

    [[nodiscard]] bool extend(pair<int, int> pos, SideBit side_bit) noexcept {
        // (pos, side_bit) defines which side of a position is part of the perimeter
        if (side_bit != side_) {
            return false;
        }
        if (side_bit == Top || side_bit == Down) {
            // check if pos is directionally adjacent to the side
            if (static_position == pos.first) {
                if (pos.second == start - 1) {
                    start--;
                    return true;
                }
                if (pos.second == end + 1) {
                    end++;
                    return true;
                }
            }
        }
        else {
            // check if pos is directionally adjacent to the side
            if (static_position == pos.second) {
                if (pos.first == start - 1) {
                    start--;
                    return true;
                }
                if (pos.first == end + 1) {
                    end++;
                    return true;
                }
            }
        }
        return false;
    }

    void merge(const Side& other) {
        if (other.side_ != side_ || other.static_position != static_position) {
            throw invalid_argument("Other is not mergeable (trivial)");
        }
        if (min(end, other.end) < max(start, other.start)) {
            throw invalid_argument("Other is not mergeable (not contiguous)");
        }
        start = min(start, other.start);
        end = max(end, other.end);
    }
};

using Area = int;
using SideCount = int;
pair<Area, SideCount> traverse(Grid& grid, pair<int, int> pos) {
    Area area = 0;
    vector<Side> sides; // track sides of the current shape

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

    const auto extend_sides = [&sides](pair<int, int> pos, SideBit side_bit) -> void {
        optional<size_t> x = nullopt;
        for (size_t i = 0; i < sides.size(); i++) {
            // try to extend existing side
            auto& side = sides[i];
            if (side.extend(pos, side_bit)) {
                if (!x) {
                    x = i;
                }
                else {
                    // the parameter pos may lead to (at most) 2 sides being joined
                    // merge *x and i
                    sides[*x].merge(sides[i]);
                    // move/pop
                    swap(sides[i], sides.back());
                    sides.pop_back();
                    return;
                }
            }
        }
        if (!x) // could not existing side, so create new one
            sides.emplace_back(Side(pos, side_bit));
        };

    while (!targets.empty()) {
        const auto curr = targets.top();
        targets.pop();
        // check adjacents
        int open_sides = all_side_bits; // bits
        for (size_t i = 0; i < 4; i++) {
            pair<int, int> adj = { curr.first + offsets[i], curr.second + offsets[i + 1] };
            if (adj.first >= 0 && adj.first < h && adj.second >= 0 && adj.second < w) {
                // in grid
                if (unmark(grid[adj.first][adj.second]) == unmark(grid[curr.first][curr.second])) {
                    // same type
                    open_sides &= ~side_bits[i]; // this corresponds to offsets
                    if (!is_marked(grid[adj.first][adj.second])) {
                        // add target
                        targets.push(adj);
                        grid[adj.first][adj.second] = mark(grid[adj.first][adj.second]);
                    }
                }
            }
        }
        // add contribution
        area++;
        for (int i = 0; i < 4; i++) {
            if (open_sides & side_bits[i]) {
                extend_sides(curr, side_bits[i]);
            }
        }
    }

    return { area, sides.size() };
}

int main() {
    auto grid = parse();
    // search from every position, marking visited nodes accordingly
    const int h = static_cast<int>(grid.size()), w = static_cast<int>(grid[0].size());
    long cost = 0;
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            const auto [area, side_count] = traverse(grid, { i, j });
            cost += area * side_count;
        }
    }

    cout << cost << endl;
}