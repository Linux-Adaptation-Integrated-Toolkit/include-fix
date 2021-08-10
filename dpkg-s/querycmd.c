#include "config.h"
#include "compat.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <errno.h>
#include <limits.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <fnmatch.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


#include <dpkg/dpkg.h>
#include <dpkg/dpkg-db.h>
#include <dpkg/pkg-array.h>
#include <dpkg/pkg-spec.h>
#include <dpkg/pkg-format.h>
#include <dpkg/pkg-show.h>
#include <dpkg/string.h>
#include <dpkg/path.h>
#include <dpkg/file.h>
#include <dpkg/pager.h>
#include <dpkg/options.h>
#include <dpkg/db-ctrl.h>
#include <dpkg/db-fsys.h>

#include "main.h"

static int searchoutput(struct fsys_namenode *namenode);
static int searchfiles(const char *const *argv);

int main(int argc, const char *const *argv) {
  dpkg_set_report_piped_mode(_IOFBF);
  dpkg_program_init("dpkg-query");

  dpkg_db_set_dir("/var/lib/dpkg");

  fsys_hash_init();
  const char *const *argval = argv;
  int v = searchfiles(argval);
  printf("%d\n", v);
  dpkg_program_done();

  return 0;
}

static int
searchoutput(struct fsys_namenode *namenode)
{
  struct fsys_node_pkgs_iter *iter;
  struct pkginfo *pkg_owner;
  int found;

  if (namenode->divert) {
    const char *name_from = namenode->divert->camefrom ?
                            namenode->divert->camefrom->name : namenode->name;
    const char *name_to = namenode->divert->useinstead ?
                          namenode->divert->useinstead->name : namenode->name;

    if (namenode->divert->pkgset) {
      printf("diversion by %s from: %s\n",
             namenode->divert->pkgset->name, name_from);
      printf("diversion by %s to: %s\n",
             namenode->divert->pkgset->name, name_to);
    } else {
      printf("local diversion from: %s\n", name_from);
      printf("local diversion to: %s\n", name_to);
    }
  }
  found= 0;

  iter = fsys_node_pkgs_iter_new(namenode);
  while ((pkg_owner = fsys_node_pkgs_iter_next(iter))) {
    if (found)
      fputs(", ", stdout);
    fputs(pkg_name(pkg_owner, pnaw_nonambig), stdout);
    found++;
  }
  fsys_node_pkgs_iter_free(iter);

  if (found) printf(": %s\n",namenode->name);
  return found + (namenode->divert ? 1 : 0);
}

static int
searchfiles(const char *const *argv)
{
  struct fsys_namenode *namenode;
  struct fsys_hash_iter *iter;
  const char *thisarg;
  int found;
  int failures = 0;
  struct varbuf path = VARBUF_INIT;
  static struct varbuf vb;

  if (!*argv)
    badusage(("--search 需要至少一个文件名表达式作为参数"));
//open database
  modstatdb_open(msdbrw_readonly);
  ensure_allinstfiles_available_quiet();
  ensure_diversions();

  while ((thisarg = *argv++) != NULL) {
    found= 0;
//start varbuf
    if (!strchr("*[?/",*thisarg)) {
      varbuf_reset(&vb);
      varbuf_add_char(&vb, '*');
      varbuf_add_str(&vb, thisarg);
      varbuf_add_char(&vb, '*');
      varbuf_end_str(&vb);
      thisarg= vb.buf;
    }
    if (!strpbrk(thisarg, "*[?\\")) {
      /* Trim trailing ‘/’ and ‘/.’ from the argument if it is not
       * a pattern, just a pathname. */
      varbuf_reset(&path);
      varbuf_add_str(&path, thisarg);
      varbuf_end_str(&path);
      varbuf_trunc(&path, path_trim_slash_slashdot(path.buf));

      namenode = fsys_hash_find_node(path.buf, 0);
      found += searchoutput(namenode);
    } else {
      //
      iter = fsys_hash_iter_new();
      while ((namenode = fsys_hash_iter_next(iter)) != NULL) {
        if (fnmatch(thisarg,namenode->name,0)) continue;
        found+= searchoutput(namenode);
      }
      fsys_hash_iter_free(iter);
    }
    if (!found) {
      notice(("没有找到与 %s 相匹配的路径"), thisarg);
      failures++;
      m_output(stderr, ("标准错误"));
    } else {
      m_output(stdout, ("标准输出"));
    }
  }
//close database
  modstatdb_shutdown();
//destroy varbuf
  varbuf_destroy(&path);

  return failures;
}

