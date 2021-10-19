#include <apt-pkg/acquire-item.h>
#include <apt-pkg/acquire.h>
#include <apt-pkg/configuration.h>
#include <apt-pkg/error.h>
#include <apt-pkg/fileutl.h>
#include <apt-pkg/sourcelist.h>
#include <apt-pkg/strutl.h>
#include <apt-pkg/update.h>

#include <string>
#include "apti18n.h"

bool AcquireUpdateL(pkgAcquire &Fetcher, int const PulseInterval = 0,
		   bool const RunUpdateScripts = true, bool const ListCleanup = true);