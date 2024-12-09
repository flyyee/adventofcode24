#include <iostream>
#include <vector>
#include <fstream>
#include <utility>
#include <cassert>
#include <unordered_set>
#include <algorithm>
#include <execution>
#include <optional>
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
    enum class MoveResult
    {
        OutOfGrid,
        Locked,
        Ok
    };
    const auto move_guard = [&](Position &guard, Direction &dir, const optional<Position> obstacle = nullopt) -> MoveResult
    {
        int cnt = 0;
        while (true)
        {
            if (cnt++ >= 4)
            {
                // catch infinite loop
                return MoveResult::Locked;
            }
            const auto idx = static_cast<int>(dir);
            const Position target = {guard.first + offsets[idx], guard.second + offsets[idx + 1]};
            if (!in_grid(target))
            {
                return MoveResult::OutOfGrid;
            }

            if (grid[target.first][target.second] == Tile::Obstacle || (obstacle.has_value() && target == *obstacle))
            {
                dir = static_cast<Direction>((idx + 1) % static_cast<int>(Direction::Count));
                continue;
            }

            guard = target;
            break;
        }

        return MoveResult::Ok;
    };

    typedef pair<Position, Direction> State;
    struct PairHash
    {
        auto operator()(const State &p) const -> long
        {
            const auto &[pos, dir] = p;
            return (static_cast<long>(pos.first) << 32) + pos.second + (static_cast<long>(dir) << 61);
        }
        auto operator()(const Position &pos) const -> long
        {
            return (static_cast<long>(pos.first) << 32) + pos.second;
        }
    };
    const auto start = guard;
    unordered_set<Position, PairHash> route;
    do
    {
        route.insert(guard);
    } while (move_guard(guard, dir) == MoveResult::Ok);
    // obstructions necessarily lie on the guard's original path, so we step through that

    int ans = count_if(execution::par_unseq, begin(route), end(route), [&move_guard, &ans, &start](const auto &step)
                       {
                 Position guard = start;
                 Direction dir = Direction::Up;
                 unordered_set<State, PairHash> seen;
                 do
                 {
                     if (!seen.insert({guard, dir}).second)
                     {
                         return true;
                         break;
                     }
                 } while (move_guard(guard, dir, step) == MoveResult::Ok); 
                 return false; });

    cout << ans << endl;
}