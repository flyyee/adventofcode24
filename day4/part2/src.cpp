#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <array>
#include <algorithm>
#include <ranges>
using namespace std;

int main()
{
    ifstream file("../input");
    stringstream ss;
    ss << file.rdbuf();
    const auto contents = ss.str();

    int row_len = contents.find('\n') + 1; // number of chars in a row + nl
    int row_cnt = contents.length() / row_len + (contents.back() == '\n' ? 0 : 1);
    const auto check = [row_cnt, row_len, &contents](int i, int j, char c) -> bool
    {
        return (i >= 0 && i < row_cnt && j >= 0 && j < row_len - 1 && contents[i * row_len + j] == c);
    };

    typedef pair<int, int> pos;
    typedef array<pos, 5> pos_set;
    const string word = "MASMS";
    constexpr auto gen = []()
    {
        array<pos_set, 4> arr;
        constexpr pos first[3] = {{-1, -1}, {0, 0}, {1, 1}};
        constexpr pos second[2] = {{-1, 1}, {1, -1}};
        arr[0] = {first[0], first[1], first[2], second[0], second[1]};
        arr[1] = {first[0], first[1], first[2], second[1], second[0]};
        arr[2] = {first[2], first[1], first[0], second[0], second[1]};
        arr[3] = {first[2], first[1], first[0], second[1], second[0]};
        return arr;
    };
    constexpr auto pos_sets = gen();
    int cnt = 0;
    for (int i = 0; i < row_cnt; i++)
    {
        for (int j = 0; j < row_len - 1; j++)
        {
            cnt += count_if(begin(pos_sets), end(pos_sets), [&](const auto &ps)
                            { return ranges::all_of(views::iota(0, static_cast<int>(word.length())), [&](int k)
                                                    { 
                                    const auto [di, dj] = ps[k];
                                    return check(i + di, j + dj, word[k]); }); });
        }
    }
    cout << cnt << endl;
}