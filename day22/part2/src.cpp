#include <iostream>
#include <vector>
#include <fstream>
#include <charconv>
#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include <span>

using namespace std;

constexpr long mix(long secret, long value) {
    return secret ^ value;
}

constexpr long prune(long secret) {
    return secret % 16777216;
}

vector<long> evolve(long secret, size_t cycles = 2000) {
    vector<long> secrets = { secret % 10 };
    for (size_t i = 0; i < cycles; i++) {
        secret = prune(mix(secret * 64, secret));
        secret = prune(mix(secret / 32, secret));
        secret = prune(mix(secret * 2048, secret));
        secrets.push_back(secret % 10);
    }
    return secrets;
}

int main() {
    ifstream file("../input");
    string line;

    unordered_map<int32_t, long> mp;
    const auto serialize = [](const span<int>& diffs) {
        if (diffs.size() != 4) {
            throw invalid_argument("wrong diffs length");
        }
        const int32_t id = (10 + diffs[0]) | (10 + diffs[1]) << 8 | (10 + diffs[2]) << 16 | (10 + diffs[3]) << 24;
        return id;
        };

    long ans = -1;
    while (getline(file, line)) {
        long secret;
        if (from_chars(line.data(), line.data() + line.length(), secret).ec != errc{})
            throw runtime_error("Secret is not numeric");

        auto secrets = evolve(secret);
        vector<int> diffs(secrets.size() - 1);
        for (size_t i = 0; i < secrets.size() - 1; i++) {
            diffs[i] = secrets[i + 1] - secrets[i];
        }
        unordered_set<int32_t> seen;
        for (size_t i = 0; i < diffs.size() - 3; i++) {
            const auto id = serialize(span(diffs).subspan(i, 4));
            if (seen.contains(id))
                continue; // we can only take the first occurrence of the 4-sequence

            seen.insert(id);
            if ((mp[id] += secrets[i + 4]) > ans) {
                ans = mp[id];
            }
        }

    }

    cout << ans << endl;
}