#include <iostream>
#include <string>
#include <vector>

#include "nbind/api.h"
#include "TlHelp32.h"
#include "psapi.h"
//
class ProcessEntry32
{
  public:
    ProcessEntry32()
    {
      this->size = 0;
      this->processID = 0;
      this->threadCount = 0;
      this->parentProcessID = 0;
      this->basePriority = 0;
      this->exeFileName = "";
    }
    
    unsigned long getSize() { return(size); }
    unsigned long getProcessID() { return(processID); }
    unsigned long getThreadCount() { return(threadCount); }
    unsigned long getParentProcessID() { return(parentProcessID); }
    long getBasePriority() { return(basePriority); }
    std::string getExeFileName() { return(exeFileName); }

    void setSize(unsigned long size) { this->size = size; }
    void setProcessID(unsigned long processID) { this->processID = processID; }
    void setThreadCount(unsigned long threadCount) { this->threadCount = threadCount; }
    void setParentProcessID(unsigned long parentProcessID) { this->parentProcessID = parentProcessID; }
    void setBasePriority(long basePriority) { this->basePriority = basePriority; }
    void setExeFileName(std::string exeFileName) { this->exeFileName = exeFileName; }

    unsigned long size;
    unsigned long processID;
    unsigned long threadCount;
    unsigned long parentProcessID;
    long basePriority;
    std::string exeFileName;
};

//
class ProcessWalker
{
  public:
    static std::vector<ProcessEntry32> p32s;
    
    static std::vector<ProcessEntry32> walkProcess()
    {
      p32s.clear();

      HANDLE hProcessSnap;
      PROCESSENTRY32 pe32;
      hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

      if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return p32s;
      }
      else {
        pe32.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hProcessSnap, &pe32)) 
        {
          ProcessEntry32 t1;
          t1.setSize(pe32.dwSize);
          t1.setProcessID(pe32.th32ProcessID);
          t1.setThreadCount(pe32.cntThreads);
          t1.setParentProcessID(pe32.th32ParentProcessID);
          t1.setBasePriority(pe32.pcPriClassBase);
          t1.setExeFileName(pe32.szExeFile);
          
          p32s.push_back(t1);
          while (Process32Next(hProcessSnap, &pe32)) {
            ProcessEntry32 t2;
            
            t2.setSize(pe32.dwSize);
            t2.setProcessID(pe32.th32ProcessID);
            t2.setThreadCount(pe32.cntThreads);
            t2.setParentProcessID(pe32.th32ParentProcessID);
            t2.setBasePriority(pe32.pcPriClassBase);
            t2.setExeFileName(pe32.szExeFile);
            
            p32s.push_back(t2);
          }
          CloseHandle(hProcessSnap);
        }
      }
      //
      return p32s;
    }
};

std::vector<ProcessEntry32> ProcessWalker::p32s;

#include "nbind/nbind.h"

NBIND_CLASS(ProcessEntry32) {
  construct<>();

  getset(getSize, setSize);
  getset(getProcessID, setProcessID);
  getset(getThreadCount, setThreadCount);
  getset(getParentProcessID, setParentProcessID);
  getset(getBasePriority, setBasePriority);
  getset(getExeFileName, setExeFileName);
}

NBIND_CLASS(ProcessWalker) {
  method(walkProcess);
}
