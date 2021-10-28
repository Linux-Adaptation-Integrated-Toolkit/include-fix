#include <string>
#include <cstring>
#if __GNUC__ <= 6
    #error your compiler cannot support C++17 standard. we need at least GCC 7
#elif (__GNUC__ > 6) && (__GNUC__ < 11)
    #include <experimental/filesystem>
#else
    #include <filesystem>
#endif
#include <iostream>
#include <fstream>
#include <regex>
#include <lz4.h>
#include "afs.h"

using namespace std;
#if (__GNUC__ > 6) && (__GNUC__ < 11)
    namespace fs = std::experimental::filesystem;
#else
    namespace fs = std::filesystem;
#endif

int main() {
    //string cmd = "pkexec apt update -c " + string(getenv("HOME")) + "/.config/wali/wali.conf 2>/dev/null";
    //execmd(cmd);
    //const string destdir = string(getenv("HOME")) + "/.local/share/wali/filesdb/";
    //cpcontent(destdir);
    decompress_contents("/home/uos/.local/share/wali/filesdbcommunity-packages.deepin.com_deepin_dists_apricot_contrib_Contents-amd64.lz4");
    return 0;
}

bool execmd(const string cmd, int buffnum) {
    int closeResult;
    FILE* cmdout;
    char read_buffer[buffnum];

    memset(read_buffer, 0, 64);
    //string cmd = "pkexec apt update -c wali.conf";
    //check if shell is available
    if(system(nullptr) == 0)
    {
        cout << "shell is not available" << endl;
        return false;
    }
    //execute the command
    cmdout = popen(cmd.c_str(), "r");
    if(cmdout == nullptr)
    {
        cout << "popen error" << endl;
        return false;
    }

    //print the output of popen
    while(fgets(read_buffer, 1024, cmdout) != NULL) {
        fprintf(stdout, "%s", read_buffer);
    }

    //get the return value
    closeResult = pclose(cmdout);
    if(closeResult != 0) {
        return false;
    }

    return true;
}

bool cpcontent(const std::string dpath, const std::string rpath) {
    fs::path strd(dpath);
    fs::path strr(rpath);
    regex contentsr("_Contents-");

    //init
    fs::remove_all(dpath);
    
    //if rpath is not exit, will return false
    if (fs::exists(rpath) == false) {
        cout << "resource directory is not exist" << endl;
        return false;
    }
    //if dpath is not exit, will build it
    // it support build a new path and its parent path.
    if (fs::exists(dpath) == false) {
        cout << "destination path is not exist, now it will be built" << endl;
        while(strd.has_parent_path()) {
            if(fs::exists(strd.parent_path())) {
                break;
            }
            fs::create_directory(strd.parent_path());
            strd = strd.parent_path();
        }
        fs::create_directory(strd);
    }
    // copy files from rpath to dpath
    int calfile = 0;
    for(const auto &file : fs::directory_iterator(rpath)) {
        if(fs::is_directory(file) == false) {
            if(regex_search(string(fs::path(file).filename()), contentsr)) {
                string I = string(fs::path(file).filename());
//                cout << I.substr(I.find_last_of(".")+1) << endl;
                string destaddr = dpath + string(fs::path(file).filename());
                copy_file(fs::path(file), destaddr);
                calfile++;
            }
        }
    }
    //a safe backtracking mechanism，if execmd function get 0 Contents file.
    if (calfile == 0) {
        cout << "maybe apt update can not get Contents file" << endl;
        return false;
    }
    return true;
}

//need input and output to supply absolute path
bool decompress_contents(string input) {
    fs::path stri(input);
    //get filename, file extension name and filename without extension
    string tar_filename = string(stri.filename());
    string tar_filename_remove_extension = string(tar_filename.substr(0, tar_filename.find_last_of(".")));
    string tar_style = string(tar_filename.substr(tar_filename.find_last_of(".") + 1));
    //to avoid params need char*,but std::string cannot be converted to char*,only to const char*
    //so we also supply char* style of output_file
    fs::path current_path = stri.parent_path();
    string output_file = string(current_path) + "/" + tar_filename_remove_extension;
    char* output_file_char = new char[output_file.length() + 1];
    strcpy(output_file_char, output_file.c_str());
    //get the file size
    std::uintmax_t filesize = fs::file_size(stri);
    int filesize_int = int(filesize);
    // support below style: {"lz4", "xz", "bz2", "gz", "lzma", "zst"};
    if(tar_style == "lz4") {
        char* const regen_buffer = new char[4096];
        if (regen_buffer == NULL) {
            cout << "Failed to allocate memory for " << input << endl;
            return false;
        }

        int decompressed_size = LZ4_decompress_safe(input.c_str(), regen_buffer, filesize_int, 4096);
        cout << decompressed_size << endl;
        delete[] regen_buffer;
    }

    delete[] output_file_char;
    return true;
}