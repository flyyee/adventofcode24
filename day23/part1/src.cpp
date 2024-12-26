#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string_view>
#include <array>
#include <set>
using namespace std;

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

    const auto intersects = [](auto x, auto y) {
        set<size_t> result;

        set_intersection(begin(x), end(x), begin(y), end(y),
            inserter(result, result.begin()));

        return result;
        // return !result.empty();
        };

    long cnt = 0;
    for (size_t i = 0; i < dsu_size; i++) {
        for (auto j : adjs[i]) {
            if (j < i) {
                continue;
            }
            for (auto k : intersects(adjs[i], adjs[j])) {
                if (k < j) {
                    continue;
                }
                if (names[i].starts_with('t') || names[j].starts_with('t') || names[k].starts_with('t'))
                    cnt++;
            }

        }
    }

    cout << cnt << endl;
}