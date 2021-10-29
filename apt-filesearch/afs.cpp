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
#include <cassert>
#include <regex>
/*
 * use these library to support decompressing Contents file, we support 6 types.
 * xz,gz,lz4,lzma,zst,bz2
*/
//use this to support decompressing lz4.
//TODO: support other types.
#include <lz4frame.h>
#define LZ4_IN_CHUNK_SIZE  (16*1024)

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
    //decompress_contents("/home/uos/.local/share/wali/filesdbcommunity-packages.deepin.com_deepin_dists_apricot_contrib_Contents-amd64.lz4");
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

/* need input and output to supply absolute path
 * input is an file path, output is an directory path
 * if output is not exist, we will build it(but its parrent path must be exist)
*/
bool decompress_contents(string input, string output) {
    fs::path stri(input);
    fs::path stro(output);
    //get filename, file extension name and filename without extension
    string tar_filename = string(stri.filename());
    string tar_filename_remove_extension = string(tar_filename.substr(0, tar_filename.find_last_of(".")));
    string tar_style = string(tar_filename.substr(tar_filename.find_last_of(".") + 1));
    //to avoid params need char*,but std::string cannot be converted to char*,only to const char*
    //so we also supply char* style of output_file
    if (fs::exists(stro) == false) {
        if(fs::exists(stro.parent_path())  == false) {
            cout << "output's parent path is not exit" << endl;
            return false;
        }
        fs::create_directory(stro);
    }
    string output_file = output + "/" + tar_filename_remove_extension;
    char* output_file_char = new char[output_file.length() + 1];
    strcpy(output_file_char, output_file.c_str());
    //get the file size
    std::uintmax_t filesize = fs::file_size(stri);
    int filesize_int = int(filesize);
    // support below style: {"lz4", "xz", "bz2", "gz", "lzma", "zst"};
    if(tar_style == "lz4") {

    }

    delete[] output_file_char;
    return true;
}

static void safe_fwrite(void* buf, size_t eltSize, size_t nbElt, FILE* f)
{
    size_t const writtenSize = fwrite(buf, eltSize, nbElt, f);
    size_t const expectedSize = eltSize * nbElt;
    if (nbElt>0) assert(expectedSize / nbElt == eltSize);  /* check overflow */
    if (writtenSize < expectedSize) {
        if (ferror(f))  /* note : ferror() must follow fwrite */
            fprintf(stderr, "Write failed \n");
        else
            fprintf(stderr, "Write too short \n");
        exit(1);
    }
}

static int decompress_file_internal(FILE* f_in, FILE* f_out, LZ4F_dctx* dctx, void* src, size_t srcCapacity, size_t filled, size_t alreadyConsumed, void* dst, size_t dstCapacity) {
    int firstChunk = 1;
    size_t ret = 1;

    assert(f_in != NULL); assert(f_out != NULL);
    assert(dctx != NULL);
    assert(src != NULL); assert(srcCapacity > 0); assert(filled <= srcCapacity); assert(alreadyConsumed <= filled);
    assert(dst != NULL); assert(dstCapacity > 0);

    /* Decompression */
    while (ret != 0) {
        /* Load more input */
        size_t readSize = firstChunk ? filled : fread(src, 1, srcCapacity, f_in); firstChunk=0;
        const void* srcPtr = (const char*)src + alreadyConsumed; alreadyConsumed=0;
        const void* const srcEnd = (const char*)srcPtr + readSize;
        if (readSize == 0 || ferror(f_in)) {
            printf("Decompress: not enough input or error reading file\n");
            return 1;
        }

        /* Decompress:
         * Continue while there is more input to read (srcPtr != srcEnd)
         * and the frame isn't over (ret != 0)
         */
        while (srcPtr < srcEnd && ret != 0) {
            /* Any data within dst has been flushed at this stage */
            size_t dstSize = dstCapacity;
            size_t srcSize = (const char*)srcEnd - (const char*)srcPtr;
            ret = LZ4F_decompress(dctx, dst, &dstSize, srcPtr, &srcSize, /* LZ4F_decompressOptions_t */ NULL);
            if (LZ4F_isError(ret)) {
                printf("Decompression error: %s\n", LZ4F_getErrorName(ret));
                return 1;
            }
            /* Flush output */
            if (dstSize != 0) safe_fwrite(dst, 1, dstSize, f_out);
            /* Update input */
            srcPtr = (const char*)srcPtr + srcSize;
        }

        assert(srcPtr <= srcEnd);

        /* Ensure all input data has been consumed.
         * It is valid to have multiple frames in the same file,
         * but this example only supports one frame.
         */
        if (srcPtr < srcEnd) {
            printf("Decompress: Trailing data left in file after frame\n");
            return 1;
        }
    }

    /* Check that there isn't trailing data in the file after the frame.
     * It is valid to have multiple frames in the same file,
     * but this example only supports one frame.
     */
    {   size_t const readSize = fread(src, 1, 1, f_in);
        if (readSize != 0 || !feof(f_in)) {
            printf("Decompress: Trailing data left in file after frame\n");
            return 1;
    }   }

    return 0;
}

static size_t get_block_size(const LZ4F_frameInfo_t* info) {
    switch (info->blockSizeID) {
        case LZ4F_default:
        case LZ4F_max64KB:  return 1 << 16;
        case LZ4F_max256KB: return 1 << 18;
        case LZ4F_max1MB:   return 1 << 20;
        case LZ4F_max4MB:   return 1 << 22;
        default:
            printf("Impossible with expected frame specification (<=v1.6.1)\n");
            exit(1);
    }
}

static int decompress_file_allocDst(FILE* f_in, FILE* f_out, LZ4F_dctx* dctx, void* src, size_t srcCapacity) {
    assert(f_in != NULL); assert(f_out != NULL);
    assert(dctx != NULL);
    assert(src != NULL);
    assert(srcCapacity >= LZ4F_HEADER_SIZE_MAX);  /* ensure LZ4F_getFrameInfo() can read enough data */

    /* Read Frame header */
    size_t const readSize = fread(src, 1, srcCapacity, f_in);
    if (readSize == 0 || ferror(f_in)) {
        printf("Decompress: not enough input or error reading file\n");
        return 1;
    }

    LZ4F_frameInfo_t info;
    size_t consumedSize = readSize;
    {   size_t const fires = LZ4F_getFrameInfo(dctx, &info, src, &consumedSize);
        if (LZ4F_isError(fires)) {
            printf("LZ4F_getFrameInfo error: %s\n", LZ4F_getErrorName(fires));
            return 1;
    }   }

    /* Allocating enough space for an entire block isn't necessary for
     * correctness, but it allows some memcpy's to be elided.
     */
    size_t const dstCapacity = get_block_size(&info);
    void* const dst = malloc(dstCapacity);
    if (!dst) { perror("decompress_file(dst)"); return 1; }

    int const decompressionResult = decompress_file_internal(
                        f_in, f_out,
                        dctx,
                        src, srcCapacity, readSize-consumedSize, consumedSize,
                        dst, dstCapacity);

    free(dst);
    return decompressionResult;
}
bool decompress_file(FILE* f_in, FILE* f_out) {
    assert(f_in != NULL); 
    assert(f_out != NULL);

    /* Resource allocation */
    void* const src = malloc(LZ4_IN_CHUNK_SIZE);
    if (!src) { 
        perror("decompress_file(src)"); return 1; 
    }

    LZ4F_dctx* dctx;
    {
        size_t const dctxStatus = LZ4F_createDecompressionContext(&dctx, LZ4F_VERSION);
        if (LZ4F_isError(dctxStatus)) {
            printf("LZ4F_dctx creation error: %s\n", LZ4F_getErrorName(dctxStatus));
        }   
    }

    int const result = !dctx ? 1 /* error */ : decompress_file_allocDst(f_in, f_out, dctx, src, LZ4_IN_CHUNK_SIZE);

    free(src);
    LZ4F_freeDecompressionContext(dctx);   /* note : free works on NULL */
    /* @result : 1==error, 0==success */
    if(result == 1) {
        return false;
    }
    return true;
}
