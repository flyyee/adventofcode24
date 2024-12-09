#include <iostream>
#include <fstream>

using namespace std;

int main()
{
    ifstream file("../input");
    // parse line by line
    string line;
    long long sum = 0;
    while (getline(file, line))
    {
        size_t pos = 0;
        const string prefix = "mul(";
        while (true)
        {
            pos = line.find(prefix, pos);
            if (pos == string::npos)
            {
                // goto next line
                break;
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
    }
    cout << sum << endl;
}