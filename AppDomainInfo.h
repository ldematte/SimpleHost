
#ifndef SH_APPDOMAIN_INFO_H_INCLUDED
#define SH_APPDOMAIN_INFO_H_INCLUDED

#include "Common.h"

//#import "SimpleHostRuntime.tlb" no_namespace named_guids
struct ISimpleHostDomainManager;

struct AppDomainInfo {
   
   AppDomainInfo(DWORD threadId, ISimpleHostDomainManager* manager) 
      : mainThreadId(threadId), appDomainManager(manager) { 
      threadsInAppDomain = 1; // The "main" thread
      bytesInAppDomain = 0;
   }

   DWORD mainThreadId;
   ISimpleHostDomainManager* appDomainManager;
   LONG threadsInAppDomain;
   LONG bytesInAppDomain;
   LONG allocsInAppDomain;
};

#endif //SH_APPDOMAIN_INFO_H_INCLUDED
