#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

int main() {
    ifstream file("../input");
    string line;
    getline(file, line); // input is short so just read the entire thing at once
    vector<long> nums;
    size_t start = 0, delim = line.find(' ', start);
    while (true) {
        nums.emplace_back(stoi(line.substr(start, delim)));
        if (delim == line.length()) {
            break;
        }
        start = delim + 1;
        delim = line.find(' ', start);
        delim = (delim == string::npos) ? line.length() : delim;
    }
    constexpr auto digit_cnt = [](long n) -> size_t {
        return to_string(n).length();
        };
    const auto stones = [digit_cnt](long start, size_t blinks) -> long {
        vector<long> l;
        l.emplace_back(start);
        while (blinks--) {
            const auto n = l.size();
            for (size_t i = 0; i < n; i++) {
                auto& curr = l[i];
                if (curr == 0) {
                    curr = 1;
                }
                else if (digit_cnt(curr) % 2 == 0) {
                    const auto s = to_string(curr);
                    long x = stoi(s.substr(0, s.length() / 2)), y = stoi(s.substr(s.length() / 2));
                    curr = x;
                    l.push_back(y); // exploit independence of arrangement
                }
                else {
                    curr *= 2024;
                }
            }
        }
        return l.size();
        };
    long ans = 0;
    for (const auto num : nums) {
        ans += stones(num, 25);
    }
    cout << ans << endl;
}