#include <string>
#include <lz4frame.h>
#include <string>
#include <vector>
class AptFileSearch
{
  public:
  struct entry
  {
    std::string path;
    std::string pkgname;
  };

private: 
  /*
  * these functions is about decompressing lz4
  */
  static int decompress_file_internal(FILE *f_in, FILE *f_out, LZ4F_dctx *dctx, void *src, size_t srcCapacity, size_t filled, size_t alreadyConsumed, void *dst, size_t dstCapacity);
  static size_t get_block_size(const LZ4F_frameInfo_t *info);
  static int decompress_file_allocDst(FILE *f_in, FILE *f_out, LZ4F_dctx *dctx, void *src, size_t srcCapacity);
  static bool decompress_file(FILE *f_in, FILE *f_out);

  static bool execmd(const std::string cmd, int buffnum = 1024);
  
  static void safe_fwrite(void* buf, size_t eltSize, size_t nbElt, FILE* f);

public:
  AptFileSearch();
  ~AptFileSearch();

  /*
  * dpath, destination path,copy files to this path
  * rpath, resource path, copy files from this path  
  */
  bool cpcontent(const std::string dpath, const std::string rpath = "/var/lib/apt/lists");

  //
  /*decompress the tarball, decompressed file has same filename with the original tarball. 
  * need input and output to supply absolute path
  * input is an file path, output is an directory path
  * if output is not exist, we will build it(but its parrent path must be exist)
  */
  bool decompress_contents(std::string input, std::string output);

  /*
  * Search for the file in contents and get the package name
  * filename: Path of contents file in the system
  * targets: List of files to find
  */
  static std::vector<entry> parse_contents(const std::string &filename, std::vector<std::string> targets);
};
