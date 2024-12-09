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
    while (getline(file, line))
    {
        const auto mid = ranges::count(line, ',') / 2;
        const auto sv = string_view(line);
        // g++11 so no P2210R2 impl (views::split)
        size_t prev = 0, pos = 0;
        unordered_set<int> seen;
        bool fail = false;
        int mid_num, cnt = 0;
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
            if (cnt++ == mid)
            {
                mid_num = num;
            }
            prev = pos + 1;
        } while (!fail && pos != string::npos);
        if (!fail)
        {
            ans += mid_num;
        }
    }
    cout << ans << endl;
}