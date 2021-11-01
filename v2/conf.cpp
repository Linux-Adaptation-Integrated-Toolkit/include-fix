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
#include "conf.h"
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

      pkgAcquire Fetcher1(&Stat);
      if (Fetcher1.GetLock(_config->FindDir("Dir::State::Lists")) == false)
         return false;

      // Populate it with the source selection
      if (List->GetIndexes(&Fetcher1) == false)
      return false;

      AcquireUpdateL(Fetcher, 0, true);
   }
   return 0;
}

bool AcquireUpdateL(pkgAcquire &Fetcher, int const PulseInterval,
		   bool const RunUpdateScripts, bool const ListCleanup)
{
   // Run scripts
   if (RunUpdateScripts == true) {
      RunScripts("APT::Update::Pre-Invoke");
   }
   pkgAcquire::RunResult res;
   if(PulseInterval > 0) {
      res = Fetcher.Run(PulseInterval);
   }
   else {
      res = Fetcher.Run();
   }
   bool const errorsWereReported = (res == pkgAcquire::Failed);
   bool Failed = errorsWereReported;
   bool TransientNetworkFailure = false;
   bool AllFailed = true;
   for (pkgAcquire::ItemIterator I = Fetcher.ItemsBegin(); 
	I != Fetcher.ItemsEnd(); ++I)
   {
      switch ((*I)->Status)
      {
         case pkgAcquire::Item::StatDone:
            AllFailed = false;
            continue;
         case pkgAcquire::Item::StatTransientNetworkError:
            TransientNetworkFailure = true;
            break;
         case pkgAcquire::Item::StatIdle:
         case pkgAcquire::Item::StatFetching:
         case pkgAcquire::Item::StatError:
         case pkgAcquire::Item::StatAuthError:
         Failed = true;
         break;
      }

      (*I)->Finished();

      if (errorsWereReported)
	 continue;

      ::URI uri((*I)->DescURI());
      uri.User.clear();
      uri.Password.clear();
      std::string const descUri = std::string(uri);
      // Show an error for non-transient failures, otherwise only warn
      if ((*I)->Status == pkgAcquire::Item::StatTransientNetworkError)
	 _error->Warning(_("Failed to fetch %s  %s"), descUri.c_str(),
			(*I)->ErrorText.c_str());
      else
	 _error->Error(_("Failed to fetch %s  %s"), descUri.c_str(),
	       (*I)->ErrorText.c_str());
   }

   // Clean out any old list files
   // Keep "APT::Get::List-Cleanup" name for compatibility, but
   // this is really a global option for the APT library now
   if (!TransientNetworkFailure && !Failed && ListCleanup == true &&
       (_config->FindB("APT::Get::List-Cleanup",true) == true &&
	_config->FindB("APT::List-Cleanup",true) == true))
   {
      if (Fetcher.Clean(_config->FindDir("Dir::State::lists")) == false ||
	  Fetcher.Clean(_config->FindDir("Dir::State::lists") + "partial/") == false)
	 // something went wrong with the clean
	 return false;
   }

   bool Res = true;

   if (errorsWereReported == true)
      Res = false;
   else if (TransientNetworkFailure == true)
      Res = _error->Warning(_("Some index files failed to download. They have been ignored, or old ones used instead."));
   else if (Failed == true)
      Res = _error->Error(_("Some index files failed to download. They have been ignored, or old ones used instead."));

   // Run the success scripts if all was fine
   if (RunUpdateScripts == true)
   {
      if(AllFailed == false)
	 RunScripts("APT::Update::Post-Invoke-Success");

      // Run the other scripts
      RunScripts("APT::Update::Post-Invoke");
   }
   return Res;
}