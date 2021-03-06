
#ifndef CONTEXT_H_INCLUDED
#define CONTEXT_H_INCLUDED

#include "Common.h"

#include <map>
#include <list>

#import "SimpleHostRuntime.tlb" no_namespace named_guids

//using namespace SimpleHostRuntime;

#include "AppDomainInfo.h"

const int MAX_THREAD_PER_DOMAIN = 10;
const int MAX_ALLOCS_PER_DOMAIN = 1000;
const int MAX_BYTES_PER_DOMAIN = 10 * 1024 * 1024; // 10 MB

struct MemoryInfo {
   DWORD appDomainId;
   DWORD dwBytes;
};

class HostContext: public IHostContext {
private:
   volatile LONG m_cRef;

   LPCRITICAL_SECTION domainMapCrst;
   std::map<DWORD, AppDomainInfo> appDomains;

#ifdef TRACK_THREAD_RELATIONSHIP
   std::map<DWORD, DWORD> childThreadToParent;
#endif //TRACK_THREAD_RELATIONSHIP

   std::map<DWORD, DWORD> threadAppDomain;
   std::map<void*, MemoryInfo> memoryAppDomain;

   volatile unsigned long numZombieDomains;

   ISimpleHostDomainManager* defaultDomainManager;
   DWORD defaultDomainId;

   ICLRRuntimeHost* runtimeHost;

   // Our "windows-style" message queue
   std::list<HostEvent> messageQueue;
   LPCRITICAL_SECTION messageQueueCrst;
   HANDLE hMessageEvent;

public:
   HostContext(ICLRRuntimeHost* runtimeHost);
   virtual ~HostContext();

   // IUnknown functions
   STDMETHODIMP_(DWORD) AddRef();
   STDMETHODIMP_(DWORD) Release();
   STDMETHODIMP QueryInterface(const IID &riid, void **ppvObject);

   // IHostContext functions
   virtual STDMETHODIMP raw_GetThreadCount(
      /*[in]*/ long appDomainId,
      /*[out,retval]*/ long * pRetVal);

   virtual STDMETHODIMP raw_GetMemoryUsage(
      /*[in]*/ long appDomainId,
      /*[out,retval]*/ long * pRetVal);

   virtual STDMETHODIMP raw_GetNumberOfZombies(
      /*[out,retval]*/ long * pRetVal);

   virtual STDMETHODIMP raw_ResetCountersForAppDomain(/*[in]*/long appDomainId);

   virtual STDMETHODIMP raw_UnloadDomain(/*[in]*/long appDomainId);

   virtual STDMETHODIMP raw_GetLastMessage(/*[in]*/ long dwMilliseconds,  /*[out]*/ HostEvent* hostEvent,  /*[out,retval]*/ VARIANT_BOOL* eventPresent);

   void PostHostMessage(long eventType, long appDomainId, long managedThreadId);

   void OnDomainUnload(DWORD domainId);
   void OnDomainRudeUnload();
   void OnDomainCreate(DWORD domainId, DWORD dwCurrentThreadId, ISimpleHostDomainManager* domainManager);
   ISimpleHostDomainManager* GetDomainManagerForDefaultDomain();

   // Notifies that the managed code "got hold" (created, got from a pool) of a new thread   
   bool OnThreadAcquiring(DWORD dwParentThreadId);
   bool OnThreadAcquire(DWORD dwParentThreadId, DWORD dwNewThreadId);
   bool OnThreadRelease(DWORD dwThreadId);

   bool OnMemoryAcquiring(DWORD dwThreadId, LONG bytes);
   void OnMemoryAcquire(DWORD dwThreadId, LONG bytes, PVOID address);
   int OnMemoryRelease(PVOID address);

   bool IsSnippetThread(DWORD nativeThreadId);
  
   static HRESULT HostWait(HANDLE hWait, DWORD dwMilliseconds, DWORD dwOption);
   static HRESULT Sleep(DWORD dwMilliseconds, DWORD dwOption);
   static HRESULT HRESULTFromWaitResult(DWORD dwWaitResult);

   // Check the "status" of a Snippet-AppDomain
   // TODO: consider using ICLRAppDomainResourceMonitor
   // http://msdn.microsoft.com/en-us/library/vstudio/dd627196%28v=vs.100%29.aspx

};

#endif //CONTEXT_H_INCLUDED
