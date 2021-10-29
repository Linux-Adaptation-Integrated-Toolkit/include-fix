#include <string>
#include <lz4frame.h>

bool execmd(const std::string cmd, int buffnum = 1024);

/*
  dpath, destination path,copy files to this path
  rpath, resource path, copy files from this path  
*/
bool cpcontent(const std::string dpath, const std::string rpath="/var/lib/apt/lists");

//decompress the tarball, decompressed file has same filename with the original tarball.
bool decompress_contents(std::string input, std::string output);

/*
 * these functions is about decompressing lz4
*/
static int decompress_file_internal(FILE* f_in, FILE* f_out, LZ4F_dctx* dctx, void* src, size_t srcCapacity, size_t filled, size_t alreadyConsumed, void* dst, size_t dstCapacity);
static size_t get_block_size(const LZ4F_frameInfo_t* info);
static int decompress_file_allocDst(FILE* f_in, FILE* f_out, LZ4F_dctx* dctx, void* src, size_t srcCapacity);
bool decompress_file(FILE* f_in, FILE* f_out);