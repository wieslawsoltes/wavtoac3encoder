#pragma once

#include <string>
#include <wchar.h>
#include <memory>
#include <cstdio>

namespace logger
{
    class ILog
    {
    public:
        ILog() { };
        virtual ~ILog() { }
    public:
        virtual bool Open(const std::wstring szPath) = 0;
        virtual void Close() = 0;
        virtual void Log(const std::wstring szMessage) = 0;
    };

    class FileLog : public ILog
    {
        FILE *fs;
    public:
        FileLog() : fs(nullptr), ILog() 
        {
        };
        FileLog(const std::wstring szPath) : fs(nullptr), ILog() 
        {
            Open(szPath);
        };
        virtual ~FileLog() 
        {
            Close();
        }
    public:
        bool Open(const std::wstring szPath)
        {
            try
            {
                errno_t error = _wfopen_s(&fs, szPath.c_str(), L"wt, ccs=UTF-8");
                return (error != 0) ? false : true;
            }
            catch (...)
            {
                return false;
            }
        }
        void Close()
        {
            try
            {
                if (fs)
                    fclose(fs);
            }
            catch (...) { }
        }
        void Log(const std::wstring szMessage)
        {
            try
            {
                std::wstring szData = szMessage + L"\n";
                if (fs)
                {
                    std::fwrite(szData.data(), sizeof(wchar_t), szData.size(), fs);
                    std::fflush(fs);
                }
            }
            catch (...) { }
        }
    };

    extern std::unique_ptr<ILog> Log;
}
