#include <iostream>
#include <vector>
#include <fstream>
#include <charconv>

using namespace std;

constexpr long mix(long secret, long value) {
    return secret ^ value;
}

constexpr long prune(long secret) {
    return secret % 16777216;
}

constexpr long evolve(long secret, size_t cycles = 2000) {
    for (size_t i = 0; i < cycles; i++) {
        secret = prune(mix(secret * 64, secret));
        secret = prune(mix(secret / 32, secret));
        secret = prune(mix(secret * 2048, secret));
    }
    return secret;
}

int main() {
    ifstream file("../input");
    string line;

    long ans = 0;
    while (getline(file, line)) {
        long secret;
        if (from_chars(line.data(), line.data() + line.length(), secret).ec != errc{})
            throw runtime_error("Secret is not numeric");

        ans += evolve(secret);
    }

    cout << ans << endl;
}