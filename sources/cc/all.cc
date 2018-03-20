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
      this->usageCount = 0;
      this->processID = 0;
      this->moduleID = 0;
      this->threadCount = 0;
      this->parentProcessID = 0;
      this->basePriority = 0;
      this->flags = 0;
      this->exeFileName = "";
      this->exeFullPath = "";
      this->processVMUsage = 0;
    }
    
    unsigned long getSize() { return(size); }
    unsigned long getUsageCount() { return(usageCount); }
    unsigned long getProcessID() { return(processID); }
    unsigned long getModuleID() { return(moduleID); }
    unsigned long getThreadCount() { return(threadCount); }
    unsigned long getParentProcessID() { return(parentProcessID); }
    long getBasePriority() { return(basePriority); }
    unsigned long getFlags() { return(flags); }
    std::string getExeFileName() { return(exeFileName); }
    std::string getExeFullPath() { return(exeFullPath); }
    unsigned long getProcessVMUsage(){ return(processVMUsage); }
    unsigned long getProcessRAMUsage(){ return(processRAMUsage); }

    void setSize(unsigned long size) { this->size = size; }
    void setUsageCount(unsigned long usageCount) { this->usageCount = usageCount; }
    void setProcessID(unsigned long processID) { this->processID = processID; }
    void setModuleID(unsigned long moduleID) { this->moduleID = moduleID; }
    void setThreadCount(unsigned long threadCount) { this->threadCount = threadCount; }
    void setParentProcessID(unsigned long parentProcessID) { this->parentProcessID = parentProcessID; }
    void setBasePriority(long basePriority) { this->basePriority = basePriority; }
    void setFlags(unsigned long flags) { this->flags = flags; }
    void setExeFileName(std::string exeFileName) { this->exeFileName = exeFileName; }
    void setExeFullPath(std::string exeFullPath) { this->exeFullPath = exeFullPath; }
    void setProcessVMUsage(unsigned long processVMUsage){ this->processVMUsage = processVMUsage; }
    void setProcessRAMUsage(unsigned long processRAMUsage){ this->processRAMUsage = processRAMUsage; }

    unsigned long size;
    unsigned long usageCount;
    unsigned long processID;
    unsigned long moduleID;
    unsigned long threadCount;
    unsigned long parentProcessID;
    long basePriority;
    unsigned long flags;
    std::string exeFileName;
    std::string exeFullPath;
    unsigned long processVMUsage;
    unsigned long processRAMUsage;
};

//
class ProcessWalker
{
  public:
    static std::vector<ProcessEntry32> p32s;

    static unsigned long getProcessVMUsage(unsigned long procId)
    {
      PROCESS_MEMORY_COUNTERS_EX pmc;
      GetProcessMemoryInfo(OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, procId), reinterpret_cast<PPROCESS_MEMORY_COUNTERS>(&pmc), sizeof(pmc));
      SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
      return (virtualMemUsedByMe);
    }

    static unsigned long getProcessRAMUsage(unsigned long procId)
    {
      PROCESS_MEMORY_COUNTERS_EX pmc;
      GetProcessMemoryInfo(OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, procId), reinterpret_cast<PPROCESS_MEMORY_COUNTERS>(&pmc), sizeof(pmc));
      SIZE_T physMemUsedByMe  = pmc.WorkingSetSize;
      return (physMemUsedByMe );
    }

    static std::string GetProcessFullPath(unsigned long procId)
    {
      HANDLE processHandle = NULL;
      TCHAR filename[MAX_PATH];

      processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, procId);
      if (processHandle != NULL) {
        if (GetModuleFileNameEx(processHandle, NULL, filename, MAX_PATH) == 0) {
          //std::cout << "Failed to get module filename." << std::endl;
          return std::string("");
        }
        else {
          //std::cout << "Module filename is: " << filename << std::endl;
          return std::string(filename);
        }
        CloseHandle(processHandle);
      }
      else {
        //std::cout << "Failed to open process." << std::endl;
        return std::string("");
      }
      return std::string(filename);
    }

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
          t1.setUsageCount(pe32.cntUsage);
          t1.setProcessID(pe32.th32ProcessID);
          t1.setModuleID(pe32.th32ModuleID);
          t1.setThreadCount(pe32.cntThreads);
          t1.setParentProcessID(pe32.th32ParentProcessID);
          t1.setBasePriority(pe32.pcPriClassBase);
          t1.setFlags(pe32.dwFlags);
          t1.setExeFileName(pe32.szExeFile);
          t1.setExeFullPath(GetProcessFullPath(pe32.th32ProcessID));
          t1.setProcessVMUsage(getProcessVMUsage(pe32.th32ProcessID));
          t1.setProcessRAMUsage(getProcessRAMUsage(pe32.th32ProcessID));
          
          p32s.push_back(t1);
          while (Process32Next(hProcessSnap, &pe32)) {
            ProcessEntry32 t2;
            
            t2.setSize(pe32.dwSize);
            t2.setUsageCount(pe32.cntUsage);
            t2.setProcessID(pe32.th32ProcessID);
            t2.setModuleID(pe32.th32ModuleID);
            t2.setThreadCount(pe32.cntThreads);
            t2.setParentProcessID(pe32.th32ParentProcessID);
            t2.setBasePriority(pe32.pcPriClassBase);
            t2.setFlags(pe32.dwFlags);
            t2.setExeFileName(pe32.szExeFile);
            t2.setExeFullPath(GetProcessFullPath(pe32.th32ProcessID));
            t2.setProcessVMUsage(getProcessVMUsage(pe32.th32ProcessID));
            t2.setProcessRAMUsage(getProcessRAMUsage(pe32.th32ProcessID));
            
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

//
class Coord {

public:

  Coord(signed int x = 0, signed int y = 0) : x(x), y(y) {}
  explicit Coord(const Coord *other) : x(other->x), y(other->y) {}

  void toJS(nbind::cbOutput output) {
    output(x, y);
  }

  signed int getX() { std::cout << "Get X\n"; return(x); }
  signed int getY() { std::cout << "Get Y\n"; return(y); }

  void setX(signed int x) { this->x = x; }
  void setY(signed int y) { this->y = y; }

  signed int x, y;

};

#include "nbind/nbind.h"

NBIND_CLASS(Coord) {
  construct<>();
  construct<const Coord *>();
  construct<signed int, signed int>();

  getset(getX, setX);
  getset(getY, setY);
}

NBIND_CLASS(ProcessEntry32) {
  construct<>();

  getset(getSize, setSize);
  getset(getUsageCount, setUsageCount);
  getset(getProcessID, setProcessID);
  getset(getModuleID, setModuleID);
  getset(getThreadCount, setThreadCount);
  getset(getParentProcessID, setParentProcessID);
  getset(getBasePriority, setBasePriority);
  getset(getFlags, setFlags);
  getset(getExeFileName, setExeFileName);
  getset(getExeFullPath, setExeFullPath);
  getset(getProcessVMUsage, setProcessVMUsage);
  getset(getProcessRAMUsage, setProcessRAMUsage);
}

NBIND_CLASS(ProcessWalker) {
  method(walkProcess);
  method(getProcessVMUsage);
  method(getProcessRAMUsage);
}
