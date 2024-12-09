#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

int main()
{
    ifstream file("../input");
    char ch;
    bool is_file = true;
    typedef unsigned char Size;
    vector<Size> files, spaces;
    while (file >> ch)
    {
        const Size sz = ch - '0';
        if (is_file)
        {
            files.push_back(sz);
        }
        else
        {
            spaces.push_back(sz);
        }

        is_file = !is_file;
    }

    unsigned long ans = 0;

    size_t pos = 0;
    size_t static_idx = 0, space_idx = 0, move_idx = files.size() - 1;
    bool is_space = false;
    while (true)
    {
        if (!is_space)
        {
            if (files[static_idx] == 0)
            {
                is_space = true;
                static_idx++;
            }
            else
            {
                ans += pos * static_idx;
                files[static_idx]--;
                pos++;
            }
        } else {
            if (move_idx <= space_idx) {
                break;
            }
            if (spaces[space_idx] == 0) {
                is_space = false;
                space_idx++;
            } else {
                if (files[move_idx] == 0) {
                    move_idx--;
                } else {
                    ans += pos * move_idx;
                    files[move_idx]--;
                    spaces[space_idx]--;
                    pos++;
                }
            }
        }
    }

    cout << ans << endl;
}