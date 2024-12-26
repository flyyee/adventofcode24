#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string_view>
#include <array>
#include <set>
#include <span>
using namespace std;

auto intersects(auto x, auto y) {
    set<size_t> result;

    set_intersection(begin(x), end(x), begin(y), end(y),
        inserter(result, result.begin()));

    return result;
};

size_t best_v = 0;
vector<size_t> best;
void recursive_solve(vector<size_t> v, const span<set<size_t>>& adjs, set<size_t> pool) {
    if (best_v < v.size()) {
        best_v = v.size();
        best = v;
    }

    for (auto x : pool) {
        if (x < v.back()) {
            continue;
        }
        auto new_pool = intersects(pool, adjs[x]);
        auto v_new = v;
        v_new.push_back(x);
        recursive_solve(v_new, adjs, new_pool);
    }
}

vector<size_t> solve(const span<set<size_t>>& adjs) {
    for (size_t i = 0; i < adjs.size(); i++) {
        for (auto j : adjs[i]) {
            if (j < i) {
                continue;
            }
            auto res = intersects(adjs[i], adjs[j]);
            vector<size_t> input = { i, j };
            recursive_solve(input, adjs, res);
        }
    }

    return best;
}

int main() {
    ifstream file("../input");
    string line;
    constexpr size_t dsu_size = 3380;  // number of lines in input
    unordered_map<string, size_t> name_to_id;
    array<string, dsu_size> names;
    size_t curr_id = 0;
    const auto get_id = [&](string name) {
        if (!name_to_id.contains(name)) {
            name_to_id[name] = curr_id;
            names[curr_id] = name;
            curr_id++;
        }
        return name_to_id[name];
        };

    array<set<size_t>, dsu_size> adjs;
    fill(begin(adjs), end(adjs), set<size_t>{});
    const auto connect = [&](string a, string b) {
        adjs[get_id(a)].insert(get_id(b));
        adjs[get_id(b)].insert(get_id(a));
        };

    while (getline(file, line)) {
        connect(line.substr(0, 2), line.substr(3, 2));
    }

    auto res = solve(span(adjs).subspan(0, curr_id));
    vector<string> ans(res.size());
    transform(begin(res), end(res), begin(ans), [&](auto idx) {
        return names[idx];
        });
    sort(begin(ans), end(ans));
    cout << ans[0];
    for (size_t i = 1; i < ans.size();i++) {
        cout << ',' << ans[i];
    }
    cout << endl;
}