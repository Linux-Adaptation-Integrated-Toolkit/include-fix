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

vector<entry> ParseContents(string filename, char** targets)
{
    fstream file;
    vector<entry> res;
    file.open(filename, ios::in);
    if (!file)
    {
        cout << "文件打开失败:"<<filename<<"\n";
        return res;
    }
    int tarnum =0;
    char *p=targets[0];
    while (p)
    {
        tarnum++;
        p=targets[tarnum];
    }
    
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
    file.close();
    return res;
}
int main(int argc, char **argv)
{

    char* targets[argc - 1];
    for (int i = 2; i < argc; i++)
    {
        targets[i - 2] = argv[i];
    }
    targets[argc-1]=nullptr;
    if(argc<3){
        cout<<"参数过少";
    }
    
    vector<entry> a = ParseContents(argv[1], targets);
    for (entry i : a)
    {
        cout << i.pkgname << ": " << i.path << endl;
    }
    
    return 0;
}
