#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <stack>


using namespace std;

class Solve {
public:
    using Number = long;
    using Blinks = size_t;
    using Split = pair<Number, Number>;
    unordered_map<long, pair<Blinks, Split>> m;
    constexpr auto round_digit(size_t n) -> long {
        long ans = 1;
        for (size_t i = 1; i < n; i++) {
            ans *= 10;
        }
        return ans;
    };
    constexpr auto digit_cnt(long x) -> size_t {
        for (size_t n = 18; n > 1; n--) {
            if (x >= round_digit(n)) {
                return n;
            }
        }
        return 1;
    };

    pair<Blinks, Split> blinks_till_split(long n) {
        if (!m.contains(n)) {
            size_t cnt = 0;
            while (true) {
                cnt++;
                if (n == 0) {
                    n = 1;
                }
                else if (digit_cnt(n) % 2 == 0) {
                    const auto d = digit_cnt(n);
                    const auto split_val = round_digit(d / 2 + 1);
                    long x = n / split_val, y = n % split_val;
                    return { cnt, {x, y} };
                }
                else {
                    n *= 2024;
                }
            }
        }
        return m[n];
    }

    stack<pair<long, size_t>> stk;
    auto stones(long start, size_t blinks) -> long {
        long ans = 1; // current stone
        if (!stk.empty()) {
            throw runtime_error("bruh stk has stuff");
        }
        stk.push({ start, blinks });
        while (!stk.empty()) {
            auto [curr, blinks_left] = stk.top();
            stk.pop();

            const auto [blinks_required, split] = blinks_till_split(curr);
            if (blinks_required > blinks_left) {
                continue;
            }
            blinks_left -= blinks_required;
            ans++;
            stk.push({ split.first, blinks_left });
            stk.push({ split.second, blinks_left });
        }

        return ans;
    }
};

constexpr long top = 1950139; // ensure that this top is the peak of the descent
constexpr long blinks_max = 75;
array<array<long, top + 1>, blinks_max + 1> store;
void init() {
    Solve s{};
    for (auto& row : store) {
        row.fill(0);
    }
    for (long k = 0; k <= top; k++) {
        const auto blinks_required = s.blinks_till_split(k).first;
        for (size_t i = 0; i < blinks_required; i++) {
            store[i][k] = 1;
        }
        store[blinks_required][k] = 2;
    }
    for (long blinks_available = 1; blinks_available <= blinks_max; blinks_available++) {
        cout << blinks_available << endl;
        for (long k = 0; k <= top; k++) {
            if (store[blinks_available][k] == 0) {
                // uninit
                const auto [blinks_required, split] = s.blinks_till_split(k);
                const auto [x, y] = split;
                store[blinks_available][k] = store[blinks_available - blinks_required][x] + store[blinks_available - blinks_required][y];
            }
        }
    }
}

void test() {
    Solve s{};
    long largest_seen = 0;
    for (long k = 0; k < 1950139; k++) {
        const auto [x, y] = s.blinks_till_split(k).second;
        largest_seen = max(largest_seen, max(x, y));
    }
    cout << largest_seen << endl;
    // 2D array of (blinks_left, curr) -> resulting count
}

int main() {
    // test();
    // return 0;
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
    long ans = 0;
    init();
    for (const auto num : nums) {
        ans += store[blinks_max][num];
    }
    cout << ans << endl;

}