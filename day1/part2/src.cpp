#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <unordered_map>

using namespace std;

int main()
{
    ifstream file("input");
    int first, second;
    vector<int> v1;
    unordered_map<int, int> m;
    while (file >> first >> second)
    {
        v1.push_back(first);
        m[second]++;
    }
    long ans = accumulate(begin(v1), end(v1), 0, [&m](auto sum, auto curr)
                          { return sum + curr * m[curr]; });
    cout << ans << endl;
}