#include <iostream>
#include <vector>
#include <fstream>
#include <utility>
#include <cassert>
#include <unordered_set>
using namespace std;

enum class Tile
{
    Obstacle,
    Empty,
};
typedef vector<vector<Tile>> Grid;
typedef pair<int, int> Position;

pair<Grid, Position> parse_input()
{
    ifstream file("../input");
    Grid grid{};
    string line;
    Position guard;
    int cnt = 0;
    while (getline(file, line))
    {
        vector<Tile> row(line.length());
        transform(begin(line), end(line), begin(row), [&](const auto &c)
                  {
                      switch (c)
                      {
                      case '^':
                          guard = {cnt, static_cast<int>(&c - &line[0])};
                          [[fallthrough]];
                      case '.':
                          return Tile::Empty;
                      case '#':
                          return Tile::Obstacle;
                      }
                      assert(false); // invalid char
                  });
        grid.push_back(row);
        cnt++;
    }
    return {grid, guard};
}

int main()
{
    auto [grid, guard] = parse_input();
    const int width = grid[0].size(), height = grid.size();
    const auto in_grid = [width, height](const Position &guard) -> bool
    {
        return (guard.first >= 0 && guard.first < height && guard.second >= 0 && guard.second < width);
    };
    enum class Direction
    {
        Up,
        Right,
        Down,
        Left,
        Count
    };
    constexpr int offsets[] = {-1, 0, 1, 0, -1};
    Direction dir = Direction::Up;
    const auto move_guard = [&]() -> bool
    {
        int cnt = 0;
        while (true)
        {
            assert(cnt++ < 4); // catch infinite loop
            const auto idx = static_cast<int>(dir);
            const Position target = {guard.first + offsets[idx], guard.second + offsets[idx + 1]};
            if (!in_grid(target))
            {
                return false;
            }

            if (grid[target.first][target.second] == Tile::Obstacle)
            {
                dir = static_cast<Direction>((idx + 1) % static_cast<int>(Direction::Count));
                continue;
            }

            guard = target;
            break;
        }

        return true;
    };

    struct PairHash
    {
        auto operator()(const Position &p) const -> long
        {
            return (static_cast<long>(p.first) << 32) + p.second;
        }
    };
    unordered_set<Position, PairHash> visited;
    do
    {
        visited.insert(guard);
    } while (move_guard());
    cout << visited.size() << endl;
}