/* copy from config.h of dpkg source code after
 * ./configure
*/
#ifndef LIBDPKG_VOLATILE_API
  #define LIBDPKG_VOLATILE_API 1
#endif

int searchoutput(struct fsys_namenode *namenode);
int searchfiles(char** argv);