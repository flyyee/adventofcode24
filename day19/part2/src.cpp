#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <stdexcept>
#include <unordered_set>
#include <string_view>
#include <unordered_map>

using namespace std;

unordered_set<string_view> patterns;
unordered_map<string_view, long> memo;

long solve(string_view design, size_t offset = 0) {
    long ans = 0;
    if (memo.contains(design.substr(offset)))
        return memo[design.substr(offset)];

    if (offset == design.length()) {
        return memo[design.substr(offset)] = 1;
    }

    for (size_t possible_length = 1; possible_length <= design.length() - offset; possible_length++) {
        const auto sub = design.substr(offset, possible_length);
        if (patterns.contains(sub))
            ans += solve(design, offset + possible_length);
    }

    return memo[design.substr(offset)] = ans;
}

int main() {
    ifstream file("../input");
    string line;
    getline(file, line);
    const string delim = ", ";
    size_t prev_pos = 0, pos;
    vector<string> ps;
    string line_ = line;
    string_view line__(line_);
    while (true) {
        pos = line__.find(delim, prev_pos);
        patterns.insert(line__.substr(prev_pos, pos - prev_pos));
        if (pos == string::npos)
            break;
        prev_pos = pos + 2;
    }

    getline(file, line); // nl
    long ans = 0;
    while (getline(file, line)) {
        ans += solve(string_view(line));
    }
    cout << ans << endl;
}