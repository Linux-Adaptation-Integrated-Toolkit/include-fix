/*
   supported at least c++ 11
*/
#include <apt-pkg/acquire-item.h>
#include <apt-pkg/acquire.h>
#include <apt-pkg/cachefile.h>
#include <apt-pkg/cmndline.h>
#include <apt-pkg/configuration.h>
#include <apt-pkg/error.h>
#include <apt-pkg/fileutl.h>
#include <apt-pkg/metaindex.h>
#include <apt-pkg/sourcelist.h>
#include <apt-pkg/update.h>

#include "apt-private/acqprogress.h"
#include "apt-private/private-download.h"
#include "apt-private/private-output.h"
#include "apt-private/private-update.h"
#include "apt-private/private-main.h"
#include "apt-private/private-cachefile.h"

#include <ostream>
#include <string>
#include <regex>

using namespace std;

int main() {
   pkgInitConfig(*_config);
   pkgInitSystem(*_config, _system);
      InitSignals();
   InitOutput();
   CacheFile Cache;

   // Get the source list
   if (Cache.BuildSourceList() == false) {
      _error->Error("can not build source list");
   }
   pkgSourceList *List = Cache.GetSourceList();
   _config->CndSet("Acquire::ForceHash", "md5sum");
   cout << List->begin() << endl;
   // Populate it with the source selection and get all Indexes
   // (GetAll=true)
   aptAcquireWithTextStatus Fetcher;
   if (List->GetIndexes(&Fetcher,true) == false) {
      cout << "can not get source indexes" << endl;
   }
   string compExt = APT::Configuration::getCompressionTypes()[0];
   pkgAcquire::UriIterator I = Fetcher.UriBegin();
   for (; I != Fetcher.UriEnd(); ++I)
   {
      string FileName = flNotDir(I->Owner->DestFile);
      if(compExt.empty() == false && APT::String::Endswith(FileName, compExt)) {
         FileName = FileName.substr(0, FileName.size() - compExt.size() - 1);
         regex r("Contents");
         if (regex_search(I->URI, r) == true) {
//            cout << "\"" << I->URI << "\" \"" << FileName << "\"" << endl;
         }
      }
   }

   if (_config->FindB("APT::Get::Download",true) == true) {
      AcqTextStatus Stat(std::cout, ScreenWidth,_config->FindI("quiet",0));
      ListUpdate(Stat, *List);
   }
   return 0;
}