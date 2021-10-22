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
vector<entry> ParseContents(const char *filename, char **targets)
{
    fstream file;
    vector<entry> res;
    file.open(filename, ios::in);
    if (!file)
    {
        cout << "文件打开失败。\n";
        return res;
    }
    else
    {
        cout << "正在解析文件...\n";
    }
    int tarnum = sizeof(targets) / sizeof(char *);
    if (!tarnum)
    {
        return res;
    }
    string line;
    int linenum = 0;
    while (getline(file, line))
    {
        linenum++;
        int index;
        index = line.find("\t");
        if (index == line.npos)
        {
            index = line.find(" ");
        }
        string opt_left = line.substr(0, index);
        int lastindex = opt_left.find_last_of("/");
        if (lastindex == opt_left.npos)
            lastindex = -1;
        string filename = opt_left.substr(lastindex + 1, opt_left.length() - lastindex);
        for (int i = 0; i < tarnum; i++)
        {
            if (!filename.compare(targets[i]))
            {
                ///
                index = line.find_last_of("\t");
                int index2 = line.find_last_of(" ");
                if (index == line.npos)
                {
                    index = line.length();
                }
                if(index2 == line.npos)
                {
                    index2 = line.length();
                }
                index = min(index,index2)+1;

                string opt_right = line.substr(index, line.length() - index);
                ///
                lastindex = opt_right.find_last_of("/");
                if (lastindex == opt_right.npos)
                    lastindex = -1;
                string pkgname = opt_right.substr(lastindex + 1, opt_right.length() - lastindex);
                entry a;
                a.path = opt_left;
                a.pkgname = pkgname;
                res.push_back(a);
            }
        }
    }
    //这里可能要对res去重
    // for(int i=0;i<res.size();i++){
    //     for(int j=i+1;j<res.size();j++){
    //         if (res.at(i).pkgname == res.at(j).pkgname)
    //         {
    //             res.erase(res.begin()+j);
    //         }
    //     }
    // }
    file.close();
    return res;
}
int main(int argc, char **argv)
{
    char *targets[argc - 1];
    for (int i = 1; i < argc; i++)
    {
        targets[i - 1] = argv[i];
    }
    vector<entry> a = ParseContents("../Contents-amd64", targets);
    for (entry i : a)
    {
        cout << i.pkgname << ": " << i.path << endl;
    }
    return a.size() != 0;
    // return 0;
}
