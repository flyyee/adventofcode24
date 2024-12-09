#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <charconv>
#include <cassert>
#include <algorithm>
#include <ranges>

using namespace std;
int main()
{
    ifstream file("../input");
    unordered_map<int, vector<int>> rules;
    string line;
    while (getline(file, line) && line.length() > 1)
    {
        const auto pos = line.find("|");
        assert(pos != string::npos);
        const auto sv = string_view(line);
        const auto first = sv.substr(0, pos), second = sv.substr(pos + 1);
        int i_first, i_second;
        auto out = from_chars(first.data(), first.data() + first.size(), i_first);
        assert(out.ec == errc{});
        out = from_chars(second.data(), second.data() + second.size(), i_second);
        assert(out.ec == errc{});
        rules[i_first].push_back(i_second);
    }
    int ans = 0;
    const auto is_fail = [&rules](string_view sv) -> bool
    {
        // g++11 so no P2210R2 impl (views::split)
        size_t prev = 0, pos = 0;
        unordered_set<int> seen;
        bool fail = false;
        do
        {
            pos = sv.find(',', prev);
            const auto sub = sv.substr(prev, pos - prev);
            int num;
            auto out = from_chars(sub.data(), sub.data() + sub.size(), num);
            assert(out.ec == errc{});
            for (const auto target : rules[num])
            {
                if (seen.contains(target))
                {
                    fail = true;
                    break;
                }
            }
            seen.insert(num);
            prev = pos + 1;
        } while (!fail && pos != string::npos);
        return fail;
    };
    while (getline(file, line))
    {
        const auto sv = string_view(line);
        if (!is_fail(sv))
        {
            continue;
        }

        // g++11 so no P2210R2 impl (views::split)
        size_t prev = 0, pos = 0;
        unordered_set<int> nums;
        vector<pair<int, int>> ranks;
        do
        {
            pos = sv.find(',', prev);
            const auto sub = sv.substr(prev, pos - prev);
            int num;
            auto out = from_chars(sub.data(), sub.data() + sub.size(), num);
            assert(out.ec == errc{});
            nums.insert(num);
            prev = pos + 1;
        } while (pos != string::npos);
        ranks.reserve(nums.size());
        for (const auto num : nums)
        {
            int after = count_if(begin(rules[num]), end(rules[num]), [&nums](const auto x)
                                 { return nums.contains(x); });
            ranks.push_back({num, after});
        }
        sort(begin(ranks), end(ranks), [](const auto &x, const auto &y)
             { return x.second > y.second; }); // symmetry lol
        ans += ranks[ranks.size() / 2].first;
    }
    cout << ans << endl;
}