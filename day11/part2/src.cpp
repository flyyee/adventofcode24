#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <numeric>

using namespace std;

using Number = long;
using Blinks = size_t;
using Split = pair<Number, Number>;
unordered_map<long, pair<Blinks, Split>> m;

template <size_t N>
constexpr auto generate_round_numbers() -> array<long, N> {
    long v = 1;
    array<long, N> arr;
    for (size_t i = 0; i < N; i++) {
        arr[i] = v;
        v *= 10;
    }
    return arr;
}

constexpr auto round_numbers = generate_round_numbers<18>();

constexpr auto digit_cnt(long x) -> size_t {
    for (size_t n = 18; n > 1; n--) {
        if (x >= round_numbers[n - 1]) {
            return n;
        }
    }
    return 1;
};

constexpr pair<Blinks, Split> calc_blinks_till_split(long n) {
    size_t cnt = 0;
    while (true) {
        cnt++;
        if (n == 0) {
            n = 1;
        }
        else if (digit_cnt(n) % 2 == 0) {
            const auto d = digit_cnt(n);
            const auto split_val = round_numbers[d / 2];
            long x = n / split_val, y = n % split_val;
            return { cnt, {x, y} };
        }
        else {
            n *= 2024;
        }
    }
}

constexpr Number top = 1950139; // ensure that this top is the peak of the descent
constexpr Blinks blinks_max = 75;
array<pair<Blinks, Split>, top + 1> blinks_till_split;
array<array<long, top + 1>, blinks_max + 1> dp; // (blinks_required, input number) -> answer

constexpr long get_max_split() {
    long largest_seen = 0;
    for (long k = 0; k < top; k++) {
        const auto [x, y] = calc_blinks_till_split(k).second;
        largest_seen = max(largest_seen, max(x, y));
    }
    return largest_seen;
}

void init() {
    assert(get_max_split() <= top); // the dp search space is bounded by top
    for (auto& row : dp) {
        row.fill(0);
    }
    for (Number k = 0; k <= top; k++) {
        blinks_till_split[k] = calc_blinks_till_split(k); // memoize using array
    }
    for (Number k = 0; k <= top; k++) {
        const auto blinks_required = blinks_till_split[k].first;
        for (size_t i = 0; i < blinks_required; i++) {
            dp[i][k] = 1;
        }
        dp[blinks_required][k] = 2;
        // blinks_required is at least 1, so dp[0..1][..] is init
    }
    for (Blinks blinks_available = 2; blinks_available <= blinks_max; blinks_available++) {
        for (Number k = 0; k <= top; k++) {
            if (dp[blinks_available][k] == 0) {
                // uninit
                const auto [blinks_required, split] = blinks_till_split[k];
                const auto [x, y] = split;
                dp[blinks_available][k] = dp[blinks_available - blinks_required][x] + dp[blinks_available - blinks_required][y];
            }
        }
    }
}

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

    init();
    const long ans = accumulate(begin(nums), end(nums), 0l, [](auto sum, auto num) {
        return sum + dp[blinks_max][num];
        });
    cout << ans << endl;
}