#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <cassert>

using namespace std;

enum Direction {
    Up = '^',
    Down = 'v',
    Left = '<',
    Right = '>',
    Activate = 'A'
};

auto solve(const vector<Direction>& target, map<pair<char, char>, vector<vector<Direction>>>& poss) {
    vector<vector<Direction>> routes;
    routes.emplace_back(vector<Direction>{});  // FIXME: was a bug with {{}}
    char prev_key = 'A';
    for (auto key : target) {
        // add routes from prev_key -> key
        vector<vector<Direction>> new_routes;
        for (auto v : poss[{prev_key, key}]) {
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

auto gen(map<char, pair<int, int>>& poss, const vector<pair<int, int>>& oobs) {
    map<pair<char, char>, vector<vector<Direction>>> ps;

    for (auto [key, dst] : poss) {
        for (auto [prev_key, src] : poss) {
            vector<vector<Direction>> routes;

            size_t a = 0, b = 0;
            Direction A, B;
            if (src == dst) {
                routes.push_back({ Activate });
            }
            else {
                if (src.first != dst.first) {
                    a = abs(src.first - dst.first);
                    A = src.first < dst.first ? Down : Up;

                    b = abs(src.second - dst.second);
                    B = src.second < dst.second ? Right : Left;
                }
                else {
                    a = abs(src.second - dst.second);
                    A = src.second < dst.second ? Right : Left;
                }

                vector<Direction> v;
                for (size_t i = 0; i < a; i++) {
                    v.push_back(A);
                }
                for (size_t i = 0; i < b; i++) {
                    v.push_back(B);
                }

                sort(begin(v), end(v)); // lexicographical min
                do {
                    // check that the route doesn't pass any impossible states
                    auto curr = src;
                    auto route = v;
                    bool oob = false;
                    for (auto step : route) {
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
                            assert(false);
                        }
                        if (find(begin(oobs), end(oobs), curr) != end(oobs)) {
                            oob = true;
                            break;
                        }
                    }
                    if (oob) {
                        // cout << "oob at: " << curr.first << ',' << curr.second << endl;
                        continue;
                    }

                    route.push_back(Activate);
                    routes.push_back(route);
                } while (next_permutation(begin(v), end(v)));
            }
            ps[{prev_key, key}] = move(routes);
        }
    }

    return ps;
}

int main() {
    ifstream file("../input");
    string line;

    map<char, pair<int, int>> poss;
    poss['7'] = { 0,0 };
    poss['8'] = { 0,1 };
    poss['9'] = { 0,2 };
    poss['4'] = { 1,0 };
    poss['5'] = { 1,1 };
    poss['6'] = { 1,2 };
    poss['1'] = { 2,0 };
    poss['2'] = { 2,1 };
    poss['3'] = { 2,2 };
    // poss['7'] = { 3,0 };
    poss['0'] = { 3,1 };
    poss['A'] = { 3,2 };

    map<char, pair<int, int>> poss2;
    // poss2['<'] = { 0,0 };
    poss2['^'] = { 0,1 };
    poss2['A'] = { 0,2 };
    poss2['<'] = { 1,0 };
    poss2['v'] = { 1,1 };
    poss2['>'] = { 1,2 };

    auto ps = gen(poss, { {3,0} });
    auto ps2 = gen(poss2, { {0,0} });
    long ans = 0;
    while (getline(file, line)) {
        // target is line
        vector<Direction> target;
        for (auto ch : line) {
            target.push_back(Direction(ch));
        }

        auto routes = solve(target, ps);

        set<vector<Direction>> routes2;
        for (const auto& route : routes) {
            auto r2 = solve(route, ps2);
            for (auto x : r2) {
                routes2.insert(x);
            }
        }

        set<vector<Direction>> routes3;
        for (const auto& route : routes2) {
            auto r3 = solve(route, ps2);
            for (auto x : r3) {
                routes3.insert(x);
            }
        }

        const auto& me = min_element(begin(routes3), end(routes3), [](const auto& x, const auto& y) {
            return x.size() < y.size();
            });
        auto cpl = stoi(line) * me->size();
        cout << stoi(line) << "," << me->size() << endl;
        ans += cpl;
        // cout << "routes:\n";
        // for (const auto& route : routes3) {
        //     cout << string(begin(route), end(route)) << endl;
        // }

        // break;
    }
    cout << ans << endl;
}