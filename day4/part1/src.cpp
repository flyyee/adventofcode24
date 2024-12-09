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
    typedef array<pos, 4> pos_set;
    const string word = "XMAS";
    constexpr auto gen_pos = []() constexpr
    {
        array<pos_set, 8> v;
        constexpr int dx[] = {0, 0, 1, 1, 1, -1, -1, -1};
        constexpr int dy[] = {1, -1, 0, 1, -1, 0, 1, -1};
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                v[i][j] = {dx[i] * j, dy[i] * j};
            }
        }
        return v;
    };
    constexpr auto pos_sets = gen_pos();
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