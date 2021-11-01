#include "wali-afs.h"
#include "wali-dpkgs.h"
#include <string>
#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "please input at least one head file" << endl;
        return 0;
    }
    //dpkg -s
    cout << "本地库搜索" << endl;
    searchfiles(argv);
    //apt-file search
    cout << "远程仓库搜索" << endl;
    AptFileSearch afs;
    //string destdir = string(getenv("HOME")) + "/.local/share/wali/filesdb/";
    //afs.cpcontent(destdir);
    //afs.decompress_contents("/home/uos/.local/share/wali/filesdb/community-packages.deepin.com_deepin_dists_apricot_main_Contents-amd64.lz4");
    vector<string> target;
    for(int i = 1; i < argc; ++i) {
        target.push_back(argv[i]);
    }
    vector<AptFileSearch::entry> out = afs.parse_contents("/home/uos/.local/share/wali/filesdb/community-packages.deepin.com_deepin_dists_apricot_main_Contents-amd64", target);
    if (out.size() == 0) {
        cout << "函数执行失败" << endl;
    }
    for (int i = 0; i < out.size(); ++i) {
        cout << out[i].pkgname << ":  " << out[i].path << endl;
    }
    return 0;
}