#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <stdexcept>
#include <set>
#include <unordered_set>

using namespace std;

set<string> patterns;

bool solve(const string& design, size_t offset = 0) {
    if (offset == design.length()) {
        return true;
    }
    for (size_t possible_length = 1; possible_length <= design.length() - offset; possible_length++) {
        const auto sub = design.substr(offset, possible_length);
        if (patterns.contains(sub) && solve(design, offset + possible_length)) {
            return true;
        }
    }

    return false;
}

int main() {
    ifstream file("../input");
    string line;
    getline(file, line);
    const string delim = ", ";
    size_t prev_pos = 0, pos;
    while (true) {
        pos = line.find(delim, prev_pos);
        const string pattern = line.substr(prev_pos, pos - prev_pos);
        patterns.insert(pattern);
        if (pos == string::npos)
            break;
        prev_pos = pos + 2;
    }

    getline(file, line); // nl
    long ans = 0;
    while (getline(file, line)) {
        if (solve(line))
            ans++;
    }
    cout << ans << endl;
}