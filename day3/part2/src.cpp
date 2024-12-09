#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

int main()
{
    ifstream file("../input");
    stringstream buffer;
    buffer << file.rdbuf();

    string line = buffer.str();

    // extract do/nt
    vector<int> do_pos, dont_pos;
    do_pos.push_back(-1);
    dont_pos.push_back(-2);
    size_t pos = 0;
    while ((pos = line.find("do()", pos)) != string::npos)
    {
        do_pos.push_back(pos);
        pos++;
    }
    pos = 0;
    while ((pos = line.find("don't()", pos)) != string::npos)
    {
        dont_pos.push_back(pos);
        pos++;
    }
    size_t do_idx = 0, dont_idx = 0;
    const auto state = [&do_idx, &dont_idx, &do_pos, &dont_pos](size_t pos) -> bool
    {
        while (do_idx + 1 < do_pos.size() && do_pos[do_idx + 1] < (int)pos)
        {
            do_idx++;
        }
        while (dont_idx + 1 < dont_pos.size() && dont_pos[dont_idx + 1] < (int)pos)
        {
            dont_idx++;
        }
        return do_pos[do_idx] > dont_pos[dont_idx];
    };

    long long sum = 0;
    pos = 0;
    const string prefix = "mul(";
    while (true)
    {
        pos = line.find(prefix, pos);
        if (pos == string::npos)
        {
            // goto next line
            break;
        }
        if (!state(pos))
        {
            pos++;
            continue;
        }
        const auto extract_number = [&line](int start_idx) -> int
        {
            int digit_cnt = 0;
            for (size_t i = 0; i < 3 && start_idx + i < line.length(); i++)
            {
                if (isdigit(line[start_idx + i]))
                {
                    digit_cnt++;
                }
                else
                {
                    break;
                }
            }
            return digit_cnt;
        };

        int n1 = extract_number(pos + prefix.length());
        if (n1 == 0)
        {
            pos++;
            continue;
        }
        int num1 = stoi(line.substr(pos + prefix.length(), n1));
        if (pos + prefix.length() + n1 >= line.length() || line[pos + prefix.length() + n1] != ',')
        {
            pos++;
            continue;
        }

        int n2 = extract_number(pos + prefix.length() + n1 + 1);
        if (n2 == 0)
        {
            pos++;
            continue;
        }
        int num2 = stoi(line.substr(pos + prefix.length() + n1 + 1, n2));
        if (pos + prefix.length() + n1 + 1 + n2 >= line.length() || line[pos + prefix.length() + n1 + 1 + n2] != ')')
        {
            pos++;
            continue;
        }
        sum += num1 * num2;
        pos++;
    }
    cout << sum << endl;
}