#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <ranges>
#include <algorithm>

using namespace std;

enum class Direction {
    Up = '^',
    Down = 'v',
    Left = '<',
    Right = '>',
    Activate = 'A'
};

auto generate_optimal_keypad_routes(const map<char, pair<int, int>>& keypad_positions, const vector<pair<int, int>>& oob_positions) {
    map<pair<char, char>, vector<vector<Direction>>> optimal_routes;

    for (auto [prev_key, src] : keypad_positions) {
        for (auto [key, dst] : keypad_positions) {
            using enum Direction;
            vector<vector<Direction>> routes;

            Direction dir_a, dir_b;
            size_t cnt_a = 0, cnt_b = 0;

            cnt_a = abs(src.first - dst.first);
            dir_a = src.first < dst.first ? Down : Up;
            cnt_b = abs(src.second - dst.second);
            dir_b = src.second < dst.second ? Right : Left;
            if (cnt_a == 0)
                swap(cnt_a, cnt_b), swap(dir_a, dir_b);

            vector<Direction> route_permutation(cnt_a + cnt_b);
            fill(begin(route_permutation), begin(route_permutation) + cnt_a, dir_a);
            fill(begin(route_permutation) + cnt_a, end(route_permutation), dir_b);

            sort(begin(route_permutation), end(route_permutation)); // lexicographical min
            do {
                // check that the route doesn't pass any impossible states
                const bool is_oob = find_if(begin(route_permutation), end(route_permutation), [&oob_positions, curr = src](const auto& step) mutable {
                    switch (step) {
                    case Up:
                        curr.first--;
                        break;
                    case Down:
                        curr.first++;
                        break;
                    case Left:
                        curr.second--;
                        break;
                    case Right:
                        curr.second++;
                        break;
                    default:
                        throw runtime_error("invalid route permutation step");
                    }
                    return find(begin(oob_positions), end(oob_positions), curr) != end(oob_positions);
                    }) != end(route_permutation);

                if (is_oob)
                    continue;

                auto route = route_permutation; // copy
                route.push_back(Activate);
                routes.push_back(route);
            } while (next_permutation(begin(route_permutation), end(route_permutation)));
            optimal_routes[{prev_key, key}] = move(routes);
        }
    }

    return optimal_routes;
}


auto search_numeric_routes(const auto& target, const map<pair<char, char>, vector<vector<Direction>>>& poss) {
    vector<vector<Direction>> routes;
    routes.emplace_back(vector<Direction>{});
    char prev_key = 'A';
    for (auto key : target) {
        // add routes from prev_key -> key
        vector<vector<Direction>> new_routes;
        for (auto v : poss.at({ prev_key, key })) {
            for (auto route : routes) {
                route.insert(end(route), begin(v), end(v));
                new_routes.push_back(route);
            }
        }

        swap(new_routes, routes);

        prev_key = key;
    }

    return routes;
}

constexpr size_t max_depth = 25;
map<pair<Direction, Direction>, long> dp[max_depth]; // [n]: {prev_dir, dir} -> minimum number of moves given n robots

long recursive_solve(const vector<Direction>& route, const map<pair<char, char>, vector<vector<Direction>>>& optimal_paths, const size_t depth = max_depth) {
    if (depth == 0) {
        return route.size(); // base case
    }

    Direction prev_dir = Direction::Activate;
    long ans = 0;
    for (auto dir : route) {
        if (!dp[depth - 1].contains({ prev_dir, dir })) {
            dp[depth - 1][{prev_dir, dir}] = ranges::min(
                optimal_paths.at({ static_cast<char>(prev_dir), static_cast<char>(dir) }) | views::transform([&](auto path) {
                    return recursive_solve(path, optimal_paths, depth - 1);
                    })
            );
        }
        ans += dp[depth - 1][{prev_dir, dir}];
        prev_dir = dir;
    }
    return ans;
}

int main() {
    ifstream file("../input");
    string line;

    map<char, pair<int, int>> numeric_keypad_positions;
    numeric_keypad_positions['7'] = { 0,0 };
    numeric_keypad_positions['8'] = { 0,1 };
    numeric_keypad_positions['9'] = { 0,2 };
    numeric_keypad_positions['4'] = { 1,0 };
    numeric_keypad_positions['5'] = { 1,1 };
    numeric_keypad_positions['6'] = { 1,2 };
    numeric_keypad_positions['1'] = { 2,0 };
    numeric_keypad_positions['2'] = { 2,1 };
    numeric_keypad_positions['3'] = { 2,2 };
    // numeric_keypad_positions['X'] = { 3,0 };
    numeric_keypad_positions['0'] = { 3,1 };
    numeric_keypad_positions['A'] = { 3,2 };

    map<char, pair<int, int>> directional_keypad_positions;
    // directional_keypad_positions['X'] = { 0,0 };
    directional_keypad_positions['^'] = { 0,1 };
    directional_keypad_positions['A'] = { 0,2 };
    directional_keypad_positions['<'] = { 1,0 };
    directional_keypad_positions['v'] = { 1,1 };
    directional_keypad_positions['>'] = { 1,2 };

    const auto optimal_numeric_routes = generate_optimal_keypad_routes(numeric_keypad_positions, { {3,0} });
    const auto optimal_directional_routes = generate_optimal_keypad_routes(directional_keypad_positions, { {0,0} });
    long ans = 0;
    while (getline(file, line)) {
        const auto numeric_routes = search_numeric_routes(line, optimal_numeric_routes);

        const long shortest_route_length = ranges::min(
            numeric_routes | views::transform([&](auto& rt) {
                return recursive_solve(rt, optimal_directional_routes);
                })
        );

        ans += stol(line) * shortest_route_length;
    }
    cout << ans << endl;
}