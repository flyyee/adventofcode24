#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
    ifstream file("input");
    int first, second;
    vector<int> v1, v2;
    while (file >> first >> second)
    {
        v1.push_back(first);
        v2.push_back(second);
    }
    sort(begin(v1), end(v1));
    sort(begin(v2), end(v2));
    long ans = 0;
    for (size_t i = 0; i < v1.size(); i++)
    {
        ans += abs(v1[i] - v2[i]);
    }
    cout << ans << endl;
}