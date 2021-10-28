#include <string>

bool execmd(const std::string cmd, int buffnum = 1024);

/*
  dpath, destination path,copy files to this path
  rpath, resource path, copy files from this path  
*/
bool cpcontent(const std::string dpath, const std::string rpath="/var/lib/apt/lists");

//decompress the tarball, decompressed file has same filename with the original tarball.
bool decompress_contents(std::string input);