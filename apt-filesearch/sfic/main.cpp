#include <iostream>
#include <fstream>
#include <vector>
using namespace std;
struct entry
{
    string path;
    string pkgname;
};
// 作用：在文件中逐行查找在数组的第一个元素中包含所需内容的项，并返回结果
vector<entry> ParseContents(const string &filename, vector<string> targets)
{
    fstream file;
    vector<entry> res;
    file.open(filename, ios::in);
    if (!file)
    {
        cout << "文件打开失败:"<<filename<<"\n";
        return res;
    }
    string line;
    while (getline(file, line))
    {
        uint index;
        index = line.find('\t');
        if (int(index) == string::npos)
        {
            index = line.find(' ');
        }
        string opt_left = line.substr(0, index);
        uint lastindex = opt_left.find_last_of('/');
        if (int(lastindex) == string::npos)
            lastindex = -1;
        string file_name = opt_left.substr(lastindex + 1, opt_left.length() - lastindex);
        vector<string>::iterator it;
        for (it = targets.begin();it!=targets.end(); it++)
        {
            if (file_name==*it)
            {
                index = line.find_last_of('\t');
                uint index2 = line.find_last_of(' ');
                if (int(index) == string::npos)
                {
                    index = line.length();
                }
                if(int(index2) == string::npos)
                {
                    index2 = line.length();
                }
                index = min(index,index2)+1;
                string opt_right = line.substr(index, line.length() - index);
                lastindex = opt_right.find_last_of(' ');
                if (int(lastindex) == string::npos)
                    lastindex = -1;
                string pkgname = opt_right.substr(lastindex + 1, opt_right.length() - lastindex);
                entry a;
                a.path = opt_left;
                a.pkgname = pkgname;
                res.push_back(a);
            }
        }
    }
    file.close();
    return res;
}
int main(int argc, char* argv[])
{

    vector<string> targets;
    for(int i=2;i<argc;i++){
        targets.emplace_back(argv[i]);
    }
    vector<entry> a = ParseContents(argv[1], targets);
    for (entry i : a)
    {
        cout << i.pkgname << ": " << i.path << endl;
    }

    return 0;
}
