#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <optional>

using namespace std;

int main()
{
    ifstream file("../input");
    string line;
    int safe_cnt = 0;
    while (getline(file, line))
    {
        // new report
        istringstream iss(line);
        vector<int> v;
        int num;
        while (iss >> num)
        {
            v.push_back(num);
        }

        bool safe = true;
        for (int skip = -1; skip < (int)v.size(); skip++)
        {
            int prev = -1;
            int is_inc = -1;
            safe = true;
            for (int i = 0; i < (int)v.size(); i++)
            {
                if (i == skip)
                    continue;
                num = v[i];

                // new level
                if (prev == -1)
                {
                    prev = num;
                    continue;
                }
                if (is_inc == -1)
                {
                    if (num == prev)
                    {
                        safe = false;
                        break;
                    }
                    else if (num > prev)
                    {
                        is_inc = 1;
                    }
                    else
                    {
                        is_inc = 0;
                    }
                }
                if (is_inc && !(num >= prev + 1 && num <= prev + 3))
                {
                    safe = false;
                    break;
                }
                if (!is_inc && !(num <= prev - 1 && num >= prev - 3))
                {
                    safe = false;
                    break;
                }
                prev = num;
            }
            if (safe)
                break;
        }
        safe_cnt += safe ? 1 : 0;
    }
    cout << safe_cnt << endl;
}