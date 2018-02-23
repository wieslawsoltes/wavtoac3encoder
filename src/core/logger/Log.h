#pragma once

#include <string>
#include <wchar.h>
#include <memory>
#include <cstdio>
#include "utilities\StringHelper.h"

namespace logger
{
    class ILog
    {
    public:
        ILog() { };
        virtual ~ILog() { }
    public:
        virtual bool Open() = 0;
        virtual void Close() = 0;
        virtual void Log(const std::wstring szMessage, const bool bNewLine = true) = 0;
    };

    class ConsoleLog : public ILog
    {
    public:
        ConsoleLog() : ILog()
        {
        };
        virtual ~ConsoleLog()
        {
            Close();
        }
    public:
        bool Open()
        {
            return true;
        }
        void Close()
        {
        }
        void Log(const std::wstring szMessage, const bool bNewLine = true)
        {
            try
            {
                std::wstring szData = szMessage;
                if (bNewLine)
                    szData += L"\n";
                std::string szAnsi = util::StringHelper::Convert(szData);
                std::fwrite(szAnsi.data(), sizeof(char), szAnsi.size(), stderr);
                std::fflush(stderr);
            }
            catch (...) {}
        }
    };

    class FileLog : public ILog
    {
        std::wstring szPath;
        FILE *fs;
    public:
        FileLog(const std::wstring szPath) : szPath(szPath), fs(nullptr), ILog() { };
        virtual ~FileLog() 
        {
            Close();
        }
    public:
        bool Open()
        {
            try
            {
                errno_t error = _wfopen_s(&fs, this->szPath.c_str(), L"wt, ccs=UTF-8");
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
        void Log(const std::wstring szMessage, const bool bNewLine = true)
        {
            try
            {
                std::wstring szData = szMessage;
                if (bNewLine)
                    szData += L"\n";
                if (fs)
                {
                    std::fwrite(szData.data(), sizeof(wchar_t), szData.size(), fs);
                    std::fflush(fs);
                }
            }
            catch (...) { }
        }
    };
}
