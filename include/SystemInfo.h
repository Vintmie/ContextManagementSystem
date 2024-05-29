#include <iostream>
#include <windows.h>
#include <vector>

class SystemInfo
{
public:
    static DWORD GetLogicalProcessorCount()
    {
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        return sysInfo.dwNumberOfProcessors;
    }

    static DWORD GetPhysicalCoreCount()
    {
        DWORD coreCount = 0;
        DWORD bufferSize = 0;
        GetLogicalProcessorInformationEx(RelationProcessorCore, nullptr, &bufferSize);

        std::vector<char> buffer(bufferSize);
        if (GetLogicalProcessorInformationEx(
                RelationProcessorCore, reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer.data()), &bufferSize))
        {
            auto ptr = buffer.data();
            while (ptr < buffer.data() + bufferSize)
            {
                PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX info = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(ptr);
                if (info->Relationship == RelationProcessorCore)
                {
                    coreCount++;
                }
                ptr += info->Size;
            }
        }
        return coreCount;
    }

    static DWORD GetMaxThreads()
    {
        DWORD logicalProcessors = GetLogicalProcessorCount();
        DWORD physicalCores = GetPhysicalCoreCount();
        return logicalProcessors > 0 ? logicalProcessors : physicalCores;
    }
};
